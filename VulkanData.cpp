#include "pch.hpp"
#include "VulkanData.hpp"
#include "Printing.hpp"
#include "Utils.hpp"
#include "Json.hpp"
#include "Enums.hpp"

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
	VkPhysicalDeviceVulkan11Properties vulkan11Properties;
	VkPhysicalDeviceVulkan12Properties vulkan12Properties;
};
static std::vector<PhysicalDevicePropertySet> g_PhysicalDeviceProperties;

////////////////////////////////////////////////////////////////////////////////
// PUBLIC

void Vulkan_Initialize_RAII::PrintStaticParams()
{
	Print_string(L"Vulkan SDK version", VULKAN_SDK_VERSION);
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
		g_PhysicalDeviceProperties[i].properties2 = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
			.pNext = &g_PhysicalDeviceProperties[i].vulkan11Properties};
		g_PhysicalDeviceProperties[i].vulkan11Properties = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES};
		if(g_ApiVersion >= VK_API_VERSION_1_2)
		{
			g_PhysicalDeviceProperties[i].vulkan11Properties.pNext = &g_PhysicalDeviceProperties[i].vulkan12Properties;
			g_PhysicalDeviceProperties[i].vulkan12Properties = {
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES};
		}
		g_vkGetPhysicalDeviceProperties2(g_PhysicalDevices[i], &g_PhysicalDeviceProperties[i].properties2);
	}

	m_Initialized = true;
}

Vulkan_Initialize_RAII::~Vulkan_Initialize_RAII()
{
	if(g_vkInstance)
		g_vkDestroyInstance(g_vkInstance, nullptr);
}

void Vulkan_Initialize_RAII::PrintData()
{
	assert(IsInitialized());

	PrintStructBegin(L"Vulkan SDK");
	Print_string(L"Used apiVersion", std::format(L"{}.{}.{}",
		VK_API_VERSION_MAJOR(g_ApiVersion), VK_API_VERSION_MINOR(g_ApiVersion), VK_API_VERSION_PATCH(g_ApiVersion)).c_str());
	PrintStructEnd();

	const VkPhysicalDeviceProperties& props = g_PhysicalDeviceProperties[0].properties2.properties;//TODO
	PrintStructBegin(L"VkPhysicalDeviceProperties");
	Print_string(L"apiVersion", std::format(L"{}.{}.{}",
		VK_API_VERSION_MAJOR(props.apiVersion), VK_API_VERSION_MINOR(props.apiVersion), VK_API_VERSION_PATCH(props.apiVersion)).c_str());
	Print_uint32(L"driverVersion", props.driverVersion);
	Print_hex32(L"vendorID", props.vendorID);
	Print_hex32(L"deviceID", props.deviceID);
	PrintEnum(L"deviceType", props.deviceType, Enum_VkPhysicalDeviceType);
	Print_string(L"deviceName", StrToWstr(props.deviceName, CP_UTF8).c_str());
	PrintStructEnd();

	const VkPhysicalDeviceVulkan11Properties& vulkan11Props = g_PhysicalDeviceProperties[0].vulkan11Properties;//TODO
	PrintStructBegin(L"VkPhysicalDeviceVulkan11Properties");
	PrintHexBytes(L"deviceUUID", vulkan11Props.deviceUUID, VK_UUID_SIZE);
	PrintHexBytes(L"driverUUID", vulkan11Props.driverUUID, VK_UUID_SIZE);
	PrintStructEnd();

	if(g_ApiVersion >= VK_API_VERSION_1_2)
	{
		const VkPhysicalDeviceVulkan12Properties& vulkan12Props = g_PhysicalDeviceProperties[0].vulkan12Properties;//TODO
		PrintStructBegin(L"VkPhysicalDeviceVulkan12Properties");
		PrintEnum(L"driverID", vulkan12Props.driverID, Enum_VkDriverId);
		Print_string(L"driverName", StrToWstr(vulkan12Props.driverName, CP_UTF8).c_str());
		Print_string(L"driverInfo", StrToWstr(vulkan12Props.driverInfo, CP_UTF8).c_str());
		PrintStructEnd();
	}
}

#endif // #if USE_VULKAN
