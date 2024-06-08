/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2024 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#pragma once

#define STRICT
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
// For constants like CLSID_D3D12DSRDeviceFactory to be initialized with a value,
// to avoid linker error like "LNK2001: unresolved external symbol CLSID_D3D12DSRDeviceFactory".
#define INITGUID

// Agility SDK needs to be included before Windows 10 SDK - according to Microsoft.
#ifdef USE_PREVIEW_AGILITY_SDK
	#include "ThirdParty/microsoft.direct3d.d3d12.1.714.0-preview/build/native/include/d3d12.h"
	#include "ThirdParty/microsoft.direct3d.d3d12.1.714.0-preview/build/native/include/directsr.h"
#else
	#include "ThirdParty/microsoft.direct3d.d3d12.1.614.0/build/native/include/d3d12.h"
#endif

#include <dxgi1_6.h>

#include <windows.h>
#include <Objbase.h> // For StringFromGUID2
#include <wrl/client.h> // for ComPtr

#include <vector>
#include <array>
#include <set>
#include <string>
#include <exception>
#include <format>
#include <stdexcept>
#include <algorithm>
#include <cwctype>

#include <cstdint>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cwchar>

using std::string;
using std::wstring;
using Microsoft::WRL::ComPtr;

#define CHECK_HR(expr)		do { HRESULT hr__ = (expr); if(FAILED(hr__)) { \
		throw std::runtime_error(std::format("{} returned 0x{:08X}", #expr, (uint32_t)hr__)); \
	} } while(false)

static const int PROGRAM_EXIT_SUCCESS            = 0;
static const int PROGRAM_EXIT_ERROR_INIT         = -1;
static const int PROGRAM_EXIT_ERROR_COMMAND_LINE = -2;
static const int PROGRAM_EXIT_ERROR_EXCEPTION    = -3;
static const int PROGRAM_EXIT_ERROR_SEH_EXCEPTION = -4;
static const int PROGRAM_EXIT_ERROR_D3D12        = -5;
