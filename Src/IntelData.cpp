/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2024 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.

Most of the code in this file is a derivative work of Intel GPU Detect library:
https://github.com/GameTechDev/gpudetect
License: Apache 2.0
See the original library in directory: Src\ThirdParty\gpudetect
*/
#include "IntelData.hpp"

#include "Enums.hpp"
#include "Printer.hpp"
#include "ReportFormatter/ReportFormatter.hpp"
#include "Utils.hpp"

// Macro set by Cmake.
#if USE_INTEL_GPUDETECT

#include <d3d11.h>
#ifdef _WIN32_WINNT_WIN10
#include <d3d11_3.h>
#endif
#include <tchar.h>
#include <winreg.h>

// Don't forget to update to the date of the last commit when grabbing a new version!
static const wchar_t* INTEL_GPU_DETECT_COMPILED_VERSION = L"2023-07-18";

struct CUSTOM_FILE
{
    const char* m_Ptr = nullptr;
};
static CUSTOM_FILE g_CustomFile;

void custom_fopen_s(CUSTOM_FILE** _Stream, const char* _FileName, const char* _Mode)
{
    const HRSRC resourceHandle1 = FindResource(NULL, L"INTEL_GPUDETECT_INTELGFX_CFG", L"TEXT_FILE");
    assert(resourceHandle1);
    HGLOBAL resourceHandle2 = LoadResource(NULL, resourceHandle1);
    assert(resourceHandle2);
    const void* const resourceData = LockResource(resourceHandle2);
    assert(resourceData);
    g_CustomFile.m_Ptr = (const char*)resourceData;
    *_Stream = &g_CustomFile;
}

void custom_fclose(CUSTOM_FILE* _Stream)
{
    _Stream->m_Ptr = nullptr;
}

char* custom_fgets(char* _Buffer, int _MaxCount, CUSTOM_FILE* _Stream)
{
    assert(_MaxCount > 0);
    assert(_Stream->m_Ptr);
    int i = 0;
    for(; i + 1 < _MaxCount && *_Stream->m_Ptr != '\0'; ++i)
    {
        _Buffer[i] = *(_Stream->m_Ptr++);
        if(_Buffer[i] == '\n')
        {
            ++i;
            break;
        }
    }
    _Buffer[i] = '\0';
    return i ? _Buffer : nullptr;
}

#include "ThirdParty/gpudetect/DeviceId.h"
#include "ThirdParty/gpudetect/DeviceId.cpp"
#include "ThirdParty/gpudetect/ID3D10Extensions.h"
#include "ThirdParty/gpudetect/GPUDetect.h"
#include "ThirdParty/gpudetect/GPUDetect.cpp"

namespace GPUDetect
{
    ENUM_BEGIN(PresetLevel)
        ENUM_ITEM(NotCompatible)
        ENUM_ITEM(Low)
        ENUM_ITEM(Medium)
        ENUM_ITEM(MediumPlus)
        ENUM_ITEM(High)
        ENUM_ITEM(Undefined)
    ENUM_END(PresetLevel)
} // namespace GPUDetect

namespace IntelData
{

    void PrintStaticParams()
    {
        ReportFormatter::GetInstance().AddFieldString(
            L"Intel GPU Detect compiled version", INTEL_GPU_DETECT_COMPILED_VERSION);
    }

    void PrintAdapterData(IDXGIAdapter* adapter)
    {
        ComPtr<ID3D11Device> device;
        int r = GPUDetect::InitDevice(adapter, &device);
        if(r != EXIT_SUCCESS)
            return;

        GPUDetect::GPUData gpuData = {};
        r = GPUDetect::InitExtensionInfo(&gpuData, adapter, device.Get());
        if(r != EXIT_SUCCESS)
            return;

        ReportScopeObject region(L"Intel GPUDetect::GPUData");
        ReportFormatter& formatter = ReportFormatter::GetInstance();
        formatter.AddFieldVendorId(L"VendorId", gpuData.vendorID);
        formatter.AddFieldHex32(L"deviceID", gpuData.deviceID);
        formatter.AddFieldBool(L"isUMAArchitecture", gpuData.isUMAArchitecture ? TRUE : FALSE);
        formatter.AddFieldSize(L"videoMemory", gpuData.videoMemory);
        formatter.AddFieldString(L"description", gpuData.description);
        formatter.AddFieldHex32(L"extensionVersion", gpuData.extensionVersion);
        formatter.AddFieldBool(L"intelExtensionAvailability", gpuData.intelExtensionAvailability ? TRUE : FALSE);

        r = GPUDetect::InitDxDriverVersion(&gpuData);
        if(r == EXIT_SUCCESS && gpuData.d3dRegistryDataAvailability)
        {
            char driverVersionStr[19] = {};
            GPUDetect::GetDriverVersionAsCString(&gpuData, driverVersionStr, _countof(driverVersionStr));
            formatter.AddFieldString(L"dxDriverVersion", StrToWstr(driverVersionStr, CP_ACP).c_str());
            formatter.AddFieldUint32(L"driverInfo.driverReleaseRevision", gpuData.driverInfo.driverReleaseRevision);
            formatter.AddFieldUint32(L"driverInfo.driverBuildNumber", gpuData.driverInfo.driverBuildNumber);
        }

        if(gpuData.vendorID == GPUDetect::INTEL_VENDOR_ID)
        {
            const GPUDetect::PresetLevel presetLevel = GPUDetect::GetDefaultFidelityPreset(&gpuData);
            formatter.AddFieldEnum(L"DefaultFidelityPreset", (uint32_t)presetLevel, GPUDetect::Enum_PresetLevel);

            r = GPUDetect::InitCounterInfo(&gpuData, device.Get());
            if(r == EXIT_SUCCESS)
            {
                string architectureStr = GPUDetect::GetIntelGPUArchitectureString(gpuData.architecture);
                if(architectureStr == "Unknown")
                    architectureStr = std::format("Unknown ({})", (uint32_t)gpuData.architecture);
                formatter.AddFieldString(L"GPUArchitecture", StrToWstr(architectureStr.c_str(), CP_ACP).c_str());

                const GPUDetect::IntelGraphicsGeneration generation =
                    GPUDetect::GetIntelGraphicsGeneration(gpuData.architecture);
                string generationStr = GPUDetect::GetIntelGraphicsGenerationString(generation);
                if(generationStr == "Unknown")
                    generationStr = std::format("Unknown ({})", (uint32_t)generation);
                formatter.AddFieldString(L"GraphicsGeneration", StrToWstr(generationStr.c_str(), CP_ACP).c_str());

                if(gpuData.advancedCounterDataAvailability)
                {
                    formatter.AddFieldUint32(L"euCount", gpuData.euCount);
                    formatter.AddFieldUint32(L"packageTDP", gpuData.packageTDP, L"W");
                    formatter.AddFieldUint32(L"maxFillRate", gpuData.maxFillRate, L"pixels/clock");
                }

                formatter.AddFieldUint32(L"maxFrequency", gpuData.maxFrequency, L"MHz");
                formatter.AddFieldUint32(L"minFrequency", gpuData.minFrequency, L"MHz");
            }
        }
    }

} // namespace IntelData

#endif // #if USE_INTEL_GPUDETECT
