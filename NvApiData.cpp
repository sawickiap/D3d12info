#include "pch.hpp"
#include "NvApiData.hpp"
#include "Printing.hpp"

#if USE_NVAPI

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

#endif // #if USE_NVAPI
