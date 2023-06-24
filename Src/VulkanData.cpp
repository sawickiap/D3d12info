#include "pch.hpp"
#include "VulkanData.hpp"
#include "Printing.hpp"
#include "Utils.hpp"
#include "Json.hpp"
#include "Enums.hpp"

// Macro set by Cmake.
#if USE_VULKAN

#define VK_NO_PROTOTYPES 1
#include <vulkan/vulkan.h>

static const wchar_t* const VULKAN_SDK_VERSION = L"1.3.216.0";

////////////////////////////////////////////////////////////////////////////////
// PRIVATE

ENUM_BEGIN(VkDriverId)
    ENUM_ITEM(VK_DRIVER_ID_AMD_PROPRIETARY)
    ENUM_ITEM(VK_DRIVER_ID_AMD_OPEN_SOURCE)
    ENUM_ITEM(VK_DRIVER_ID_MESA_RADV)
    ENUM_ITEM(VK_DRIVER_ID_NVIDIA_PROPRIETARY)
    ENUM_ITEM(VK_DRIVER_ID_INTEL_PROPRIETARY_WINDOWS)
    ENUM_ITEM(VK_DRIVER_ID_INTEL_OPEN_SOURCE_MESA)
    ENUM_ITEM(VK_DRIVER_ID_IMAGINATION_PROPRIETARY)
    ENUM_ITEM(VK_DRIVER_ID_QUALCOMM_PROPRIETARY)
    ENUM_ITEM(VK_DRIVER_ID_ARM_PROPRIETARY)
    ENUM_ITEM(VK_DRIVER_ID_GOOGLE_SWIFTSHADER)
    ENUM_ITEM(VK_DRIVER_ID_GGP_PROPRIETARY)
    ENUM_ITEM(VK_DRIVER_ID_BROADCOM_PROPRIETARY)
    ENUM_ITEM(VK_DRIVER_ID_MESA_LLVMPIPE)
    ENUM_ITEM(VK_DRIVER_ID_MOLTENVK)
    ENUM_ITEM(VK_DRIVER_ID_COREAVI_PROPRIETARY)
    ENUM_ITEM(VK_DRIVER_ID_JUICE_PROPRIETARY)
    ENUM_ITEM(VK_DRIVER_ID_VERISILICON_PROPRIETARY)
    ENUM_ITEM(VK_DRIVER_ID_MESA_TURNIP)
    ENUM_ITEM(VK_DRIVER_ID_MESA_V3DV)
    ENUM_ITEM(VK_DRIVER_ID_MESA_PANVK)
    ENUM_ITEM(VK_DRIVER_ID_SAMSUNG_PROPRIETARY)
    ENUM_ITEM(VK_DRIVER_ID_MESA_VENUS)
ENUM_END(VkDriverId)

ENUM_BEGIN(VkPhysicalDeviceType)
    ENUM_ITEM(VK_PHYSICAL_DEVICE_TYPE_OTHER)
    ENUM_ITEM(VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
    ENUM_ITEM(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    ENUM_ITEM(VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
    ENUM_ITEM(VK_PHYSICAL_DEVICE_TYPE_CPU)
ENUM_END(VkPhysicalDeviceType)

static HMODULE g_VulkanModule;

static PFN_vkGetInstanceProcAddr g_vkGetInstanceProcAddr;
static PFN_vkCreateInstance g_vkCreateInstance;
static PFN_vkDestroyInstance g_vkDestroyInstance;
static PFN_vkEnumeratePhysicalDevices g_vkEnumeratePhysicalDevices;
static PFN_vkGetPhysicalDeviceProperties2 g_vkGetPhysicalDeviceProperties2;

static VkInstance g_vkInstance;
static uint32_t g_ApiVersion;
static std::vector<VkPhysicalDevice> g_PhysicalDevices;

struct PhysicalDevicePropertySet
{
	VkPhysicalDeviceProperties2 properties2;
	VkPhysicalDeviceVulkan12Properties vulkan12Properties;
	VkPhysicalDeviceIDProperties IDProperties;
};
static std::vector<PhysicalDevicePropertySet> g_PhysicalDeviceProperties;

template<typename DstStruct, typename SrcStruct>
static inline void AddToPnextChain(DstStruct* dst, SrcStruct* src)
{
	src->pNext = dst->pNext;
	dst->pNext = src;
}

static bool FindPhysicalDevice(const DXGI_ADAPTER_DESC& adapterDesc, size_t& outIndex)
{
	outIndex = SIZE_MAX;
	// Match LUID
	for(size_t i = 0; i < g_PhysicalDeviceProperties.size(); ++i)
	{
		if(g_PhysicalDeviceProperties[i].IDProperties.deviceLUIDValid &&
			memcmp(g_PhysicalDeviceProperties[i].IDProperties.deviceLUID, &adapterDesc.AdapterLuid, VK_LUID_SIZE) == 0)
		{
			outIndex = i;
			return true;
		}
	}
	// Match VendorID and DeviceID
	for(size_t i = 0; i < g_PhysicalDeviceProperties.size(); ++i)
	{
		if(g_PhysicalDeviceProperties[i].properties2.properties.vendorID == adapterDesc.VendorId &&
			g_PhysicalDeviceProperties[i].properties2.properties.deviceID == adapterDesc.DeviceId)
		{
			if(outIndex != SIZE_MAX)
				// Multiple matches found.
				return false;
			outIndex = i;
		}
	}
	return outIndex != SIZE_MAX;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC

void Vulkan_Initialize_RAII::PrintStaticParams()
{
	// Empty.
}

Vulkan_Initialize_RAII::Vulkan_Initialize_RAII()
{
	g_VulkanModule = LoadLibrary(L"vulkan-1.dll");
	if(!g_VulkanModule)
		return;
	
	g_vkGetInstanceProcAddr = PFN_vkGetInstanceProcAddr(GetProcAddress(g_VulkanModule, "vkGetInstanceProcAddr"));
	g_vkCreateInstance = PFN_vkCreateInstance(GetProcAddress(g_VulkanModule, "vkCreateInstance"));
	g_vkDestroyInstance = PFN_vkDestroyInstance(GetProcAddress(g_VulkanModule, "vkDestroyInstance"));
	g_vkEnumeratePhysicalDevices = PFN_vkEnumeratePhysicalDevices(GetProcAddress(g_VulkanModule, "vkEnumeratePhysicalDevices"));
	g_vkGetPhysicalDeviceProperties2 = PFN_vkGetPhysicalDeviceProperties2(GetProcAddress(g_VulkanModule, "vkGetPhysicalDeviceProperties2"));
	
	if(!g_vkGetInstanceProcAddr || !g_vkCreateInstance || !g_vkDestroyInstance || !g_vkEnumeratePhysicalDevices ||
		!g_vkGetPhysicalDeviceProperties2)
		return;

	VkApplicationInfo appInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = PROGRAM_NAME_ANSI,
		.applicationVersion = PROGRAM_VERSION_NUMBER,
		.pEngineName = PROGRAM_NAME_ANSI,
		.engineVersion = PROGRAM_VERSION_NUMBER,
		.apiVersion = VK_API_VERSION_1_2};
	const VkInstanceCreateInfo instanceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo};
	if(g_vkCreateInstance(&instanceCreateInfo, nullptr, &g_vkInstance) != VK_SUCCESS)
	{
		appInfo.apiVersion = VK_API_VERSION_1_1;
		if(g_vkCreateInstance(&instanceCreateInfo, nullptr, &g_vkInstance) != VK_SUCCESS)
			return;
	}
	g_ApiVersion = appInfo.apiVersion;

	uint32_t physDeviceCount = 0;
	if(g_vkEnumeratePhysicalDevices(g_vkInstance, &physDeviceCount, nullptr) != VK_SUCCESS)
		return;
	if(physDeviceCount)
	{
		g_PhysicalDevices.resize(physDeviceCount);
		if(g_vkEnumeratePhysicalDevices(g_vkInstance, &physDeviceCount, g_PhysicalDevices.data()) != VK_SUCCESS)
			return;
	}

	g_PhysicalDeviceProperties.resize(physDeviceCount);
	for(uint32_t i = 0; i < physDeviceCount; ++i)
	{
		g_PhysicalDeviceProperties[i].properties2 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
		g_PhysicalDeviceProperties[i].IDProperties = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES};
		g_PhysicalDeviceProperties[i].vulkan12Properties = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES};
		AddToPnextChain(&g_PhysicalDeviceProperties[i].properties2, &g_PhysicalDeviceProperties[i].IDProperties);
		if(g_ApiVersion >= VK_API_VERSION_1_2)
			AddToPnextChain(&g_PhysicalDeviceProperties[i].properties2, &g_PhysicalDeviceProperties[i].vulkan12Properties);
		g_vkGetPhysicalDeviceProperties2(g_PhysicalDevices[i], &g_PhysicalDeviceProperties[i].properties2);
	}

	m_Initialized = true;
}

Vulkan_Initialize_RAII::~Vulkan_Initialize_RAII()
{
	if(g_vkInstance)
		g_vkDestroyInstance(g_vkInstance, nullptr);
}

void Vulkan_Initialize_RAII::PrintData(const DXGI_ADAPTER_DESC& adapterDesc)
{
	assert(IsInitialized());

	size_t physDevIndex = SIZE_MAX;
	if(!FindPhysicalDevice(adapterDesc, physDevIndex))
		return;
	const PhysicalDevicePropertySet& propSet = g_PhysicalDeviceProperties[physDevIndex];

	{
		const VkPhysicalDeviceProperties& props = propSet.properties2.properties;
		PrintStructBegin(L"VkPhysicalDeviceProperties");
		Print_string(L"apiVersion", std::format(L"{}.{}.{}",
			VK_API_VERSION_MAJOR(props.apiVersion), VK_API_VERSION_MINOR(props.apiVersion), VK_API_VERSION_PATCH(props.apiVersion)).c_str());
		Print_uint32(L"driverVersion", props.driverVersion);
		Print_hex32(L"vendorID", props.vendorID);
		Print_hex32(L"deviceID", props.deviceID);
		PrintEnum(L"deviceType", props.deviceType, Enum_VkPhysicalDeviceType);
		Print_string(L"deviceName", StrToWstr(props.deviceName, CP_UTF8).c_str());
		PrintStructEnd();
	}

	{
		const VkPhysicalDeviceIDProperties& IDProps = propSet.IDProperties;
		PrintStructBegin(L"VkPhysicalDeviceIDProperties");
		PrintHexBytes(L"deviceUUID", IDProps.deviceUUID, VK_UUID_SIZE);
		PrintHexBytes(L"driverUUID", IDProps.driverUUID, VK_UUID_SIZE);
		if(IDProps.deviceLUIDValid)
			PrintHexBytes(L"deviceLUID", IDProps.deviceLUID, VK_LUID_SIZE);
		PrintStructEnd();
	}

	if(g_ApiVersion >= VK_API_VERSION_1_2)
	{
		const VkPhysicalDeviceVulkan12Properties& vulkan12Props = propSet.vulkan12Properties;
		PrintStructBegin(L"VkPhysicalDeviceVulkan12Properties");
		PrintEnum(L"driverID", vulkan12Props.driverID, Enum_VkDriverId);
		Print_string(L"driverName", StrToWstr(vulkan12Props.driverName, CP_UTF8).c_str());
		Print_string(L"driverInfo", StrToWstr(vulkan12Props.driverInfo, CP_UTF8).c_str());
		PrintStructEnd();
	}
}

#endif // #if USE_VULKAN
