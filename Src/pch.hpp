/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2025 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#pragma once

#define STRICT
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

// Agility SDK needs to be included before Windows 10 SDK - according to Microsoft.
#ifdef USE_PREVIEW_AGILITY_SDK
#include "ThirdParty/microsoft.direct3d.d3d12.1.717.1-preview/build/native/include/d3d12.h"
#else
#include "ThirdParty/microsoft.direct3d.d3d12.1.618.1/build/native/include/d3d12.h"
#endif

#include <Objbase.h> // for StringFromGUID2
#include <dxgi1_6.h>
#include <windows.h>
#include <wrl/client.h> // for ComPtr
#include <initguid.h> // for DEFINE_GUID

#include <algorithm>
#include <array>
#include <exception>
#include <format>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <cwctype>

using Microsoft::WRL::ComPtr;
using std::string;
using std::wstring;

#define CHECK_HR(expr)		do { HRESULT hr__ = (expr); if(FAILED(hr__)) { \
		throw std::runtime_error(std::format("{} returned 0x{:08X}", #expr, (uint32_t)hr__)); \
	} } while(false)

static const int PROGRAM_EXIT_SUCCESS = 0;
static const int PROGRAM_EXIT_ERROR_INIT = -1;
static const int PROGRAM_EXIT_ERROR_COMMAND_LINE = -2;
static const int PROGRAM_EXIT_ERROR_EXCEPTION = -3;
static const int PROGRAM_EXIT_ERROR_SEH_EXCEPTION = -4;
static const int PROGRAM_EXIT_ERROR_D3D12 = -5;
