/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2025 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#include "SystemData.hpp"
#include "Enums.hpp"
#include "ReportFormatter/ReportFormatter.hpp"
#include "Utils.hpp"

#ifdef USE_PSAPI
    #include <psapi.h>
    #include <VersionHelpers.h> // Needed??
#endif

////////////////////////////////////////////////////////////////////////////////
// PRIVATE

#ifdef USE_PSAPI

static void PrintModule(HANDLE currProcess, HMODULE module)
{
    constexpr size_t MAX_PATH_LEN = 4096;
    std::vector<WCHAR> path(MAX_PATH_LEN);
    if(!GetModuleFileNameEx(currProcess, module, path.data(), MAX_PATH_LEN))
        return;
    wprintf(L"%s\n", path.data());

    DWORD handle = 0;
    DWORD size = GetFileVersionInfoSize(path.data(), &handle);
    if (size == 0)
    {
        wprintf(L"  GetFileVersionInfoSize failed.\n");
        return;
    }
    std::vector<BYTE> versionData(size);
    if(!GetFileVersionInfo(path.data(), handle, size, versionData.data()))
    {
        wprintf(L"  GetFileVersionInfo failed.\n");
        return;
    }

    // Option 1
    {
        VS_FIXEDFILEINFO* fileInfo = nullptr;
        UINT len = 0;
        auto r4 = VerQueryValue(versionData.data(), L"\\", (void**)&fileInfo, &len);
        if (!r4)
        {
            wprintf(L"  Option 1: VerQueryValue failed.\n");
        }
        else if (fileInfo == NULL || len == 0)
        {
            wprintf(L"  Option 1: VerQueryValue returned empty data.\n");
        }
        else
        {
            WORD major = HIWORD(fileInfo->dwFileVersionMS);
            WORD minor = LOWORD(fileInfo->dwFileVersionMS);
            WORD build = HIWORD(fileInfo->dwFileVersionLS);
            WORD revision = LOWORD(fileInfo->dwFileVersionLS);
            wprintf(L"  Option 1: %u.%u.%u.%u\n", major, minor, build, revision);
        }
    }

    // Option 2
    {
        struct LANGANDCODEPAGE
        {
            WORD wLanguage;
            WORD wCodePage;
        } *lpTranslate = nullptr;
        UINT len = 0;
        if (!VerQueryValue(versionData.data(), L"\\VarFileInfo\\Translation", (void**)&lpTranslate, &len))
        {
            wprintf(L"  Option 2: VerQueryValue (1) failed.\n");
        }
        else if(lpTranslate == NULL || len == 0)
        {
            wprintf(L"  Option 2: VerQueryValue (1) returned empty data.\n");
        }
        else
        {
            WCHAR subBlock[256];
            swprintf(subBlock, 256, L"\\StringFileInfo\\%04x%04x\\FileVersion",
                lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);

            LPVOID lpBuffer = nullptr;
            UINT sizeBuffer = 0;
            if (!VerQueryValue(versionData.data(), subBlock, &lpBuffer, &sizeBuffer))
            {
                wprintf(L"  Option 2: VerQueryValue (2) failed.\n");
            }
            else if (lpBuffer == NULL || sizeBuffer == 0)
            {
                wprintf(L"  Option 2: VerQueryValue (2) returned empty data.\n");
            }
            else
            {
                wprintf(L"  Option 2: %s\n", (WCHAR*)lpBuffer);
            }
        }
    }
}

#endif // #ifdef USE_PSAPI

////////////////////////////////////////////////////////////////////////////////
// PUBLIC

void PrintOsVersionInfo()
{
    ReportScopeObject scope(L"OS Info");
    ReportFormatter& formatter = ReportFormatter::GetInstance();
    HMODULE m = GetModuleHandle(L"ntdll.dll");
    if (!m)
    {
        formatter.AddFieldString(L"Windows version", L"Unknown");
        return;
    }

    typedef int32_t(WINAPI* RtlGetVersionFunc)(OSVERSIONINFOEX*);
    RtlGetVersionFunc RtlGetVersion = (RtlGetVersionFunc)GetProcAddress(m, "RtlGetVersion");
    if (!RtlGetVersion)
    {
        formatter.AddFieldString(L"Windows version", L"Unknown");
        return;
    }

    OSVERSIONINFOEX osVersionInfo = { sizeof(osVersionInfo) };
    // Documentation says it always returns success.
    RtlGetVersion(&osVersionInfo);

    formatter.AddFieldString(L"Windows version", std::format(L"{}.{}.{}", osVersionInfo.dwMajorVersion,
        osVersionInfo.dwMinorVersion, osVersionInfo.dwBuildNumber));
}

void PrintSystemMemoryInfo()
{
    ReportScopeObject scope(L"System memory");

    if (uint64_t physicallyInstalledSystemMemory = 0;
        GetPhysicallyInstalledSystemMemory(&physicallyInstalledSystemMemory))
        ReportFormatter::GetInstance().AddFieldSizeKilobytes(
            L"GetPhysicallyInstalledSystemMemory", physicallyInstalledSystemMemory);

    if (MEMORYSTATUSEX memStatEx = { sizeof(MEMORYSTATUSEX) }; GlobalMemoryStatusEx(&memStatEx))
    {
        ReportFormatter& formatter = ReportFormatter::GetInstance();
        formatter.AddFieldSize(L"MEMORYSTATUSEX::ullTotalPhys", memStatEx.ullTotalPhys);
        formatter.AddFieldSize(L"MEMORYSTATUSEX::ullTotalPageFile", memStatEx.ullTotalPageFile);
        formatter.AddFieldSize(L"MEMORYSTATUSEX::ullTotalVirtual", memStatEx.ullTotalVirtual);
    }
}

void PrintModules()
{
#ifdef USE_PSAPI
    wprintf(L"\n");
    
    const HANDLE currProcess = GetCurrentProcess();
    assert(currProcess);
    
    constexpr size_t MAX_MODULE_COUNT = 1024;
    std::vector<HMODULE> modules(MAX_MODULE_COUNT);
    DWORD neededBytes = (DWORD)(MAX_MODULE_COUNT * sizeof(HMODULE));
    if(!EnumProcessModules(currProcess, modules.data(), neededBytes, &neededBytes))
        return;
    const size_t moduleCount = neededBytes / sizeof(HMODULE);
    for (size_t moduleIndex = 0; moduleIndex < moduleCount; ++moduleIndex)
    {
        PrintModule(currProcess, modules[moduleIndex]);
    }
#endif // #ifdef USE_PSAPI
}
