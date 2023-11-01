#pragma once

#define STRICT
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

// Agility SDK needs to be included before Windows 10 SDK - according to Microsoft.
#include "ThirdParty/microsoft.direct3d.d3d12.1.711.3-preview/build/native/include/d3d12.h"
#include <dxgi1_6.h>

#include <windows.h>
#include <wrl/client.h> // for ComPtr

#include <vector>
#include <string>
#include <exception>
#include <format>
#include <stdexcept>

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
