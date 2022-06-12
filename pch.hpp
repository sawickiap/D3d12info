#pragma once

#define STRICT
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>
#include <wrl/client.h> // for ComPtr

#include <d3d12.h>
#include <dxgi1_6.h>

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

using std::wstring;
using Microsoft::WRL::ComPtr;

#define CHECK_HR(expr)		do { HRESULT hr__ = (expr); if(FAILED(hr__)) assert(0 && #expr); } while(false)
