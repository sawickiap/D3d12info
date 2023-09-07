#include "pch.hpp"
#include "AgsData.hpp"
#include "Printing.hpp"
#include "Utils.hpp"
#include "Json.hpp"
#include "Enums.hpp"

// Macro set by Cmake.
#if USE_AGS

#include <amd_ags.h>

////////////////////////////////////////////////////////////////////////////////
// PRIVATE

static const EnumItem Enum_AGSDeviceInfo_AsicFamily[] = {
	ENUM_ITEM(AGSDeviceInfo::AsicFamily_Unknown)
	ENUM_ITEM(AGSDeviceInfo::AsicFamily_PreGCN )
	ENUM_ITEM(AGSDeviceInfo::AsicFamily_GCN1   )
	ENUM_ITEM(AGSDeviceInfo::AsicFamily_GCN2   )
	ENUM_ITEM(AGSDeviceInfo::AsicFamily_GCN3   )
	ENUM_ITEM(AGSDeviceInfo::AsicFamily_GCN4   )
	ENUM_ITEM(AGSDeviceInfo::AsicFamily_Vega   )
	ENUM_ITEM(AGSDeviceInfo::AsicFamily_RDNA   )
	ENUM_ITEM(AGSDeviceInfo::AsicFamily_RDNA2  )
	ENUM_ITEM(AGSDeviceInfo::AsicFamily_RDNA3  )
	ENUM_ITEM(AGSDeviceInfo::AsicFamily_Count  )
	{ NULL, UINT32_MAX } };
static EnumRegistration g_Enum_AGSDeviceInfo_AsicFamily_Registration(L"AGSDeviceInfo_AsicFamily", Enum_AGSDeviceInfo_AsicFamily);

static AGSContext* g_AgsContext;
static AGSGPUInfo g_GpuInfo;
static bool g_DeviceCreatedWithAgs = false;

static bool FindDevice(const AGS_Initialize_RAII::DeviceId& id, int& outIndex)
{
	bool found = false;
	for(int i = 0; i < g_GpuInfo.numDevices; ++i)
	{
		if(g_GpuInfo.devices[i].vendorId == id.vendorId &&
			g_GpuInfo.devices[i].deviceId == id.deviceId &&
			g_GpuInfo.devices[i].revisionId == id.revisionId)
		{
			if(found)
				// Multiple identical devices found.
				return false;
			else
			{
				outIndex = i;
				found = true;
			}
		}
	}
	return found;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC

void AGS_Initialize_RAII::PrintStaticParams()
{
	Print_string(L"AMD_AGS_VERSION", std::format(L"{}.{}.{}",
		AMD_AGS_VERSION_MAJOR, AMD_AGS_VERSION_MINOR, AMD_AGS_VERSION_PATCH).c_str());
	Print_hex32(L"agsGetVersionNumber", (uint32_t)agsGetVersionNumber());
}

AGS_Initialize_RAII::AGS_Initialize_RAII()
{
	AGSConfiguration config = {};
	if(agsInitialize(
		AGS_CURRENT_VERSION,
		nullptr, // config
		&g_AgsContext, &g_GpuInfo) == AGS_SUCCESS)
	{
		m_Initialized = true;
	}
}

AGS_Initialize_RAII::~AGS_Initialize_RAII()
{
	if(m_Initialized)
		agsDeInitialize(g_AgsContext);
}

void AGS_Initialize_RAII::PrintData()
{
	assert(m_Initialized);

	PrintStructBegin(L"AGSGPUInfo");
	Print_string(L"driverVersion", StrToWstr(g_GpuInfo.driverVersion, CP_ACP).c_str());
	Print_string(L"radeonSoftwareVersion", StrToWstr(g_GpuInfo.radeonSoftwareVersion, CP_ACP).c_str());
	PrintStructEnd();
}

void AGS_Initialize_RAII::PrintAgsDeviceData(const DeviceId& id)
{
	assert(IsInitialized());

	int deviceIndex = -1;
	if(!FindDevice(id, deviceIndex))
		return;
	const AGSDeviceInfo& device = g_GpuInfo.devices[deviceIndex];

	PrintStructBegin(L"AGSDeviceInfo");
	Print_string(L"adapterString", StrToWstr(device.adapterString, CP_ACP).c_str());
	PrintEnum(L"asicFamily", device.asicFamily, Enum_AGSDeviceInfo_AsicFamily);
	Print_BOOL(L"isAPU", device.isAPU);
	Print_BOOL(L"isExternal", device.isExternal);
	Print_hex32(L"vendorId", (uint32_t)device.vendorId);
	Print_hex32(L"deviceId", (uint32_t)device.deviceId);
	Print_hex32(L"revisionId", (uint32_t)device.revisionId);
	Print_int32(L"numCUs", device.numCUs);
	Print_int32(L"numWGPs", device.numWGPs);
	Print_int32(L"numROPs", device.numROPs);
	Print_int32(L"coreClock", device.coreClock, L"MHz");
	Print_int32(L"memoryClock", device.memoryClock, L"MHz");
	Print_int32(L"memoryBandwidth", device.memoryBandwidth, L"MB/s");
	Print_float(L"teraFlops", device.teraFlops, L"TFLOPS");
	Print_size(L"localMemoryInBytes", device.localMemoryInBytes);
	Print_size(L"sharedMemoryInBytes", device.sharedMemoryInBytes);
	PrintStructEnd();
}

ComPtr<ID3D12Device> AGS_Initialize_RAII::CreateDeviceAndPrintData(IDXGIAdapter* adapter, D3D_FEATURE_LEVEL featureLevel)
{
	assert(IsInitialized());

	const AGSDX12DeviceCreationParams creationParams = {
		.pAdapter = adapter,
		.iid = __uuidof(ID3D12Device),
		.FeatureLevel = featureLevel};
	const AGSDX12ExtensionParams extensionParams = {
		.pAppName = PROGRAM_NAME,
		.pEngineName = PROGRAM_NAME,
		.appVersion = PROGRAM_VERSION_NUMBER,
		.engineVersion = PROGRAM_VERSION_NUMBER,
		.uavSlot = 0};
	AGSDX12ReturnedParams returnedParams = {};
	if(agsDriverExtensionsDX12_CreateDevice(g_AgsContext, &creationParams, &extensionParams, &returnedParams) == AGS_SUCCESS)
	{
		ComPtr<ID3D12Device> device{returnedParams.pDevice};
		g_DeviceCreatedWithAgs = true;

		PrintStructBegin(L"AGSDX12ReturnedParams::ExtensionsSupported");
		Print_BOOL(L"intrinsics16", returnedParams.extensionsSupported.intrinsics16);
		Print_BOOL(L"intrinsics17", returnedParams.extensionsSupported.intrinsics17);
		Print_BOOL(L"userMarkers", returnedParams.extensionsSupported.userMarkers);
		Print_BOOL(L"appRegistration", returnedParams.extensionsSupported.appRegistration);
		Print_BOOL(L"UAVBindSlot", returnedParams.extensionsSupported.UAVBindSlot);
		Print_BOOL(L"intrinsics19", returnedParams.extensionsSupported.intrinsics19);
		Print_BOOL(L"baseVertex", returnedParams.extensionsSupported.baseVertex);
		Print_BOOL(L"baseInstance", returnedParams.extensionsSupported.baseInstance);
		Print_BOOL(L"getWaveSize", returnedParams.extensionsSupported.getWaveSize);
		Print_BOOL(L"floatConversion", returnedParams.extensionsSupported.floatConversion);
		Print_BOOL(L"readLaneAt", returnedParams.extensionsSupported.readLaneAt);
		Print_BOOL(L"rayHitToken", returnedParams.extensionsSupported.rayHitToken);
		Print_BOOL(L"shaderClock", returnedParams.extensionsSupported.shaderClock);
		PrintStructEnd();

		return device;
	}
	else
		return {};
}

void AGS_Initialize_RAII::DestroyDevice(ComPtr<ID3D12Device>&& device)
{
	if(IsInitialized() && g_DeviceCreatedWithAgs)
	{
		ID3D12Device* rawDevice = device.Detach();
		agsDriverExtensionsDX12_DestroyDevice(g_AgsContext, rawDevice, nullptr);
	}
}

#endif // #if USE_AGS
