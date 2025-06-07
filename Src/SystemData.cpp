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

////////////////////////////////////////////////////////////////////////////////
// PRIVATE

////////////////////////////////////////////////////////////////////////////////
// PUBLIC

void PrintOsVersionInfo()
{
    ReportScopeObject scope(L"OS Info");
    ReportFormatter& formatter = ReportFormatter::GetInstance();
    HMODULE m = GetModuleHandle(L"ntdll.dll");
    if(!m)
    {
        formatter.AddFieldString(L"Windows version", L"Unknown");
        return;
    }

    typedef int32_t(WINAPI * RtlGetVersionFunc)(OSVERSIONINFOEX*);
    RtlGetVersionFunc RtlGetVersion = (RtlGetVersionFunc)GetProcAddress(m, "RtlGetVersion");
    if(!RtlGetVersion)
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

    if(uint64_t physicallyInstalledSystemMemory = 0;
        GetPhysicallyInstalledSystemMemory(&physicallyInstalledSystemMemory))
        ReportFormatter::GetInstance().AddFieldSizeKilobytes(
            L"GetPhysicallyInstalledSystemMemory", physicallyInstalledSystemMemory);

    if(MEMORYSTATUSEX memStatEx = { sizeof(MEMORYSTATUSEX) }; GlobalMemoryStatusEx(&memStatEx))
    {
        ReportFormatter& formatter = ReportFormatter::GetInstance();
        formatter.AddFieldSize(L"MEMORYSTATUSEX::ullTotalPhys", memStatEx.ullTotalPhys);
        formatter.AddFieldSize(L"MEMORYSTATUSEX::ullTotalPageFile", memStatEx.ullTotalPageFile);
        formatter.AddFieldSize(L"MEMORYSTATUSEX::ullTotalVirtual", memStatEx.ullTotalVirtual);
    }
}
