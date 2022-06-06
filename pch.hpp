#pragma once

#define STRICT
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>
#include <atlbase.h> // for CComPtr

#include <d3d12.h>
#include <dxgi1_6.h>

#include <vector>
#include <string>
#include <exception>

#include <cstdint>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>

using std::wstring;

#define CHECK_HR(expr)		do { HRESULT hr__ = (expr); if(FAILED(hr__)) assert(0 && #expr); } while(false)

#define SAFE_RELEASE(x)		do { if(x) { (x)->Release(); (x) = nullptr; } } while(false)
