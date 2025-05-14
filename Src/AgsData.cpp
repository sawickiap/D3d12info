/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2025 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#include "AgsData.hpp"

#include "Enums.hpp"
#include "ReportFormatter/ReportFormatter.hpp"
#include "Utils.hpp"

// Macro set by Cmake.
#if USE_AGS

#include <amd_ags.h>

////////////////////////////////////////////////////////////////////////////////
// PRIVATE

ENUM_BEGIN(AGSDeviceInfo_AsicFamily)
    ENUM_ITEM(AGSDeviceInfo::AsicFamily_Unknown)
    ENUM_ITEM(AGSDeviceInfo::AsicFamily_PreGCN)
    ENUM_ITEM(AGSDeviceInfo::AsicFamily_GCN1)
    ENUM_ITEM(AGSDeviceInfo::AsicFamily_GCN2)
    ENUM_ITEM(AGSDeviceInfo::AsicFamily_GCN3)
    ENUM_ITEM(AGSDeviceInfo::AsicFamily_GCN4)
    ENUM_ITEM(AGSDeviceInfo::AsicFamily_Vega)
    ENUM_ITEM(AGSDeviceInfo::AsicFamily_RDNA)
    ENUM_ITEM(AGSDeviceInfo::AsicFamily_RDNA2)
    ENUM_ITEM(AGSDeviceInfo::AsicFamily_RDNA3)
    ENUM_ITEM(AGSDeviceInfo::AsicFamily_Count)
ENUM_END(AGSDeviceInfo_AsicFamily)

static AGSContext* g_AgsContext;
static AGSGPUInfo g_GpuInfo;
static bool g_DeviceCreatedWithAgs = false;

static bool FindDevice(const AGS_Initialize_RAII::DeviceId& id, int& outIndex)
{
    bool found = false;
    for(int i = 0; i < g_GpuInfo.numDevices; ++i)
    {
        if(g_GpuInfo.devices[i].vendorId == id.vendorId && g_GpuInfo.devices[i].deviceId == id.deviceId &&
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
    ReportFormatter::GetInstance().AddFieldString(L"AMD_AGS_VERSION",
        std::format(L"{}.{}.{}", AMD_AGS_VERSION_MAJOR, AMD_AGS_VERSION_MINOR, AMD_AGS_VERSION_PATCH).c_str());

    const uint32_t version = (uint32_t)agsGetVersionNumber();
    ReportFormatter::GetInstance().AddFieldAMDVersion(L"agsGetVersionNumber", version);
}

AGS_Initialize_RAII::AGS_Initialize_RAII()
{
    AGSConfiguration config = {};
    if(agsInitialize(AGS_CURRENT_VERSION,
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

    if(IsStrEmpty(g_GpuInfo.driverVersion) && IsStrEmpty(g_GpuInfo.radeonSoftwareVersion))
        return;

    ReportScopeObject region(L"AGSGPUInfo");
    ReportFormatter::GetInstance().AddFieldString(L"driverVersion", StrToWstr(g_GpuInfo.driverVersion, CP_ACP).c_str());
    ReportFormatter::GetInstance().AddFieldString(
        L"radeonSoftwareVersion", StrToWstr(g_GpuInfo.radeonSoftwareVersion, CP_ACP).c_str());
}

void AGS_Initialize_RAII::PrintAgsDeviceData(const DeviceId& id)
{
    assert(IsInitialized());

    int deviceIndex = -1;
    if(!FindDevice(id, deviceIndex))
        return;
    const AGSDeviceInfo& device = g_GpuInfo.devices[deviceIndex];

    ReportScopeObject region(L"AGSDeviceInfo");
    ReportFormatter& formatter = ReportFormatter::GetInstance();
    formatter.AddFieldString(L"adapterString", StrToWstr(device.adapterString, CP_ACP).c_str());
    formatter.AddFieldEnum(L"asicFamily", device.asicFamily, Enum_AGSDeviceInfo_AsicFamily);
    formatter.AddFieldBool(L"isAPU", device.isAPU);
    formatter.AddFieldBool(L"isExternal", device.isExternal);
    formatter.AddFieldVendorId(L"vendorId", (uint32_t)device.vendorId);
    formatter.AddFieldHex32(L"deviceId", (uint32_t)device.deviceId);
    formatter.AddFieldHex32(L"revisionId", (uint32_t)device.revisionId);
    formatter.AddFieldInt32(L"numCUs", device.numCUs);
    formatter.AddFieldInt32(L"numWGPs", device.numWGPs);
    formatter.AddFieldInt32(L"numROPs", device.numROPs);
    formatter.AddFieldInt32(L"coreClock", device.coreClock, L"MHz");
    formatter.AddFieldInt32(L"memoryClock", device.memoryClock, L"MHz");
    formatter.AddFieldInt32(L"memoryBandwidth", device.memoryBandwidth, L"MB/s");
    formatter.AddFieldFloat(L"teraFlops", device.teraFlops, L"TFLOPS");
    formatter.AddFieldSize(L"localMemoryInBytes", device.localMemoryInBytes);
    formatter.AddFieldSize(L"sharedMemoryInBytes", device.sharedMemoryInBytes);
}

ComPtr<ID3D12Device> AGS_Initialize_RAII::CreateDeviceAndPrintData(
    IDXGIAdapter* adapter, D3D_FEATURE_LEVEL featureLevel)
{
    assert(IsInitialized());

    const AGSDX12DeviceCreationParams creationParams = {
        .pAdapter = adapter, .iid = __uuidof(ID3D12Device), .FeatureLevel = featureLevel
    };
    const AGSDX12ExtensionParams extensionParams = { .pAppName = PROGRAM_NAME,
        .pEngineName = PROGRAM_NAME,
        .appVersion = PROGRAM_VERSION_NUMBER,
        .engineVersion = PROGRAM_VERSION_NUMBER,
        .uavSlot = 0 };
    AGSDX12ReturnedParams returnedParams = {};
    if(agsDriverExtensionsDX12_CreateDevice(g_AgsContext, &creationParams, &extensionParams, &returnedParams) ==
        AGS_SUCCESS)
    {
        ComPtr<ID3D12Device> device{ returnedParams.pDevice };
        g_DeviceCreatedWithAgs = true;

        ReportScopeObject region(L"AGSDX12ReturnedParams::ExtensionsSupported");
        ReportFormatter& formatter = ReportFormatter::GetInstance();
        formatter.AddFieldBool(L"intrinsics16", returnedParams.extensionsSupported.intrinsics16);
        formatter.AddFieldBool(L"intrinsics17", returnedParams.extensionsSupported.intrinsics17);
        formatter.AddFieldBool(L"userMarkers", returnedParams.extensionsSupported.userMarkers);
        formatter.AddFieldBool(L"appRegistration", returnedParams.extensionsSupported.appRegistration);
        formatter.AddFieldBool(L"UAVBindSlot", returnedParams.extensionsSupported.UAVBindSlot);
        formatter.AddFieldBool(L"intrinsics19", returnedParams.extensionsSupported.intrinsics19);
        formatter.AddFieldBool(L"baseVertex", returnedParams.extensionsSupported.baseVertex);
        formatter.AddFieldBool(L"baseInstance", returnedParams.extensionsSupported.baseInstance);
        formatter.AddFieldBool(L"getWaveSize", returnedParams.extensionsSupported.getWaveSize);
        formatter.AddFieldBool(L"floatConversion", returnedParams.extensionsSupported.floatConversion);
        formatter.AddFieldBool(L"readLaneAt", returnedParams.extensionsSupported.readLaneAt);
        formatter.AddFieldBool(L"rayHitToken", returnedParams.extensionsSupported.rayHitToken);
        formatter.AddFieldBool(L"shaderClock", returnedParams.extensionsSupported.shaderClock);

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
