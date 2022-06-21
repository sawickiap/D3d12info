#include "pch.hpp"
#include "NvApiData.hpp"
#include "Printing.hpp"
#include "Enums.hpp"

#if USE_NVAPI

// Needed for some elements of NvAPI
#include <d3d11.h>

#include <nvapi.h>
#pragma comment(lib, "nvapi64.lib")

////////////////////////////////////////////////////////////////////////////////
// PRIVATE

static wstring NvShortStringToStr(NvAPI_ShortString str)
{
    wchar_t w[NVAPI_SHORT_STRING_MAX];
    swprintf_s(w, L"%hs", str);
    return wstring{w};
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC

NvAPI_Inititalize_RAII::NvAPI_Inititalize_RAII()
{
    m_Initialized = NvAPI_Initialize() == NVAPI_OK;
}

NvAPI_Inititalize_RAII::~NvAPI_Inititalize_RAII()
{
    if(m_Initialized)
        NvAPI_Unload();
}

void NvAPI_Inititalize_RAII::PrintGeneralParams()
{
    NvAPI_ShortString nvShortString;
    if(NvAPI_GetInterfaceVersionString(nvShortString) == NVAPI_OK)
        Print_string(L"NvAPI_GetInterfaceVersionString", NvShortStringToStr(nvShortString).c_str());
}

void NvAPI_Inititalize_RAII::PrintData()
{
    NvU32 pDriverVersion = UINT32_MAX;
    NvAPI_ShortString szBuildBranchString = {};
    if(NvAPI_SYS_GetDriverAndBranchVersion(&pDriverVersion, szBuildBranchString) == NVAPI_OK)
    {
        PrintStructBegin(L"NvAPI_SYS_GetDriverAndBranchVersion");
        Print_uint32(L"pDriverVersion", pDriverVersion);
        Print_string(L"szBuildBranchString", NvShortStringToStr(szBuildBranchString).c_str());
        PrintStructEnd();
    }
}

void NvAPI_Inititalize_RAII::PrintDeviceData(ID3D12Device* device)
{
    PrintStructBegin(L"NvAPI");

#if 0 // Doesn't work.
    NV_D3D1x_GRAPHICS_CAPS caps = {};
    if(NvAPI_D3D1x_GetGraphicsCapabilities(device, NV_D3D1x_GRAPHICS_CAPS_VER, &caps) == NVAPI_OK)
    {
        Print_BOOL(L"NV_D3D1x_GRAPHICS_CAPS::bExclusiveScissorRectsSupported", caps.bExclusiveScissorRectsSupported != 0);
        Print_BOOL(L"NV_D3D1x_GRAPHICS_CAPS::bVariablePixelRateShadingSupported", caps.bVariablePixelRateShadingSupported != 0);
    }
#endif

    wstring s;
    for(const EnumItem* ei = Enum_NV_EXTN_OP; ei->m_Name != nullptr; ++ei)
    {
        bool supported = false;
        if(NvAPI_D3D12_IsNvShaderExtnOpCodeSupported(device, ei->m_Value, &supported) == NVAPI_OK)
        {
            s = std::format(L"NvAPI_D3D12_IsNvShaderExtnOpCodeSupported({})", ei->m_Name);
            Print_BOOL(s.c_str(), supported);
        }
    }

    PrintStructEnd();
}

#endif // #if USE_NVAPI
