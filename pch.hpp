#pragma once

#define STRICT
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

// Agility SDK needs to be included before Windows 10 SDK - according to Microsoft.
#include "ThirdParty/microsoft.direct3d.d3d12.1.706.3-preview/build/native/include/d3d12.h"
#include <dxgi1_6.h>

#include <windows.h>
#include <wrl/client.h> // for ComPtr

#include <vector>
#include <string>
#include <exception>
#include <format>

#include <cstdint>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cwchar>

using std::string;
using std::wstring;
using Microsoft::WRL::ComPtr;

#define CHECK_HR(expr)		do { HRESULT hr__ = (expr); if(FAILED(hr__)) assert(0 && #expr); } while(false)
