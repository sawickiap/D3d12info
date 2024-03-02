/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2024 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.

Most of the code in this file is a derivative work of Intel GPU Detect library:
https://github.com/GameTechDev/gpudetect
License: Apache 2.0
See the original library in directory: Src\ThirdParty\gpudetect
*/
#include "IntelData.hpp"
#include "Printing.hpp"
#include "Utils.hpp"
#include "Json.hpp"
#include "Enums.hpp"

// Macro set by Cmake.
#if USE_INTEL_GPUDETECT

#include <d3d11.h>
#ifdef _WIN32_WINNT_WIN10
#include <d3d11_3.h>
#endif
#include <winreg.h>
#include <tchar.h>

// Don't forget to update to the date of the last commit when grabbing a new version!
static const wchar_t* INTEL_GPU_DETECT_COMPILED_VERSION = L"2023-07-18";

struct CUSTOM_FILE
{
	const char* m_Ptr = nullptr;
};
static CUSTOM_FILE g_CustomFile;

void custom_fopen_s(CUSTOM_FILE** _Stream, const char* _FileName, const char* _Mode)
{
	const HRSRC resourceHandle1 = FindResource(NULL, L"INTEL_GPUDETECT_INTELGFX_CFG", L"TEXT_FILE");
	assert(resourceHandle1);
	HGLOBAL resourceHandle2 = LoadResource(NULL, resourceHandle1);
	assert(resourceHandle2);
	const void* const resourceData = LockResource(resourceHandle2);
	assert(resourceData);
	g_CustomFile.m_Ptr = (const char*)resourceData;
	*_Stream = &g_CustomFile;
}

void custom_fclose(CUSTOM_FILE* _Stream)
{
	_Stream->m_Ptr = nullptr;
}

char* custom_fgets(char* _Buffer, int _MaxCount, CUSTOM_FILE* _Stream)
{
	assert(_MaxCount > 0);
	assert(_Stream->m_Ptr);
	int i = 0;
	for(; i + 1 < _MaxCount && *_Stream->m_Ptr != '\0'; ++i)
	{
		_Buffer[i] = *(_Stream->m_Ptr++);
		if(_Buffer[i] == '\n')
		{
			++i;
			break;
		}
	}
	_Buffer[i] = '\0';
	return i ? _Buffer : nullptr;
}

// #############################################################################
// Intel GPU Detect "DeviceId.h"

/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017-2018 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

namespace GPUDetect
{
	enum INTEL_GPU_ARCHITECTURE
	{
		IGFX_UNKNOWN = 0x00,

		IGFX_SANDYBRIDGE = 0x0c,
		IGFX_IVYBRIDGE,
		IGFX_HASWELL,
		IGFX_VALLEYVIEW,
		IGFX_BROADWELL,
		IGFX_CHERRYVIEW,
		IGFX_SKYLAKE,
		IGFX_KABYLAKE,
		IGFX_COFFEELAKE,
		IGFX_WILLOWVIEW,
		IGFX_BROXTON,
		IGFX_GEMINILAKE,

		IGFX_CANNONLAKE = 0x1a,

		IGFX_ICELAKE = 0x1c,
		IGFX_ICELAKE_LP,

		IGFX_LAKEFIELD = 0x1e,

		IGFX_TIGERLAKE_LP = 0x21,

		IGFX_ROCKETLAKE = 0x23,

		IGFX_ADL = 0x24,
		IGFX_ADL_LP = 0X25,

		// Alchemist 
		DGFX_ACM = 1270,

		IGFX_DG1 = 0x4ba,

		IGFX_MAX_PRODUCT,

		// Architectures with no enum value
		IGFX_WHISKEYLAKE,
		IGFX_COMETLAKE,

	};

	/*******************************************************************************
	 * getIntelGPUArchitecture
	 *
	 *      Returns the architecture of an Intel GPU by parsing the device id.  It
	 *      assumes that it is indeed an Intel GPU device ID (i.e., that VendorID
	 *      was INTEL_VENDOR_ID).
	 *
	 *      You cannot generally compare device IDs to compare architectures; for
	 *      example, a newer architecture may have an lower deviceID.
	 *
	 ******************************************************************************/
	INTEL_GPU_ARCHITECTURE GetIntelGPUArchitecture(unsigned int deviceId);

	/*******************************************************************************
	 * getIntelGPUArchitectureString
	 *
	 *     Convert A INTEL_GPU_ARCHITECTURE to a string.
	 *
	 ******************************************************************************/
	char const* GetIntelGPUArchitectureString(INTEL_GPU_ARCHITECTURE arch);
}

// #############################################################################
// Intel GPU Detect "DeviceId.cpp"

////////////////////////////////////////////////////////////////////////////////
// Copyright 2017-2018 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
////////////////////////////////////////////////////////////////////////////////

namespace GPUDetect
{

	char const* GetIntelGPUArchitectureString( INTEL_GPU_ARCHITECTURE arch )
	{
		switch( arch )
		{
		case IGFX_SANDYBRIDGE:   return "Sandy Bridge";
		case IGFX_IVYBRIDGE:     return "Ivy Bridge";
		case IGFX_HASWELL:       return "Haswell";
		case IGFX_VALLEYVIEW:    return "ValleyView";
		case IGFX_BROADWELL:     return "Broadwell";
		case IGFX_CHERRYVIEW:    return "Cherryview";
		case IGFX_SKYLAKE:       return "Skylake";
		case IGFX_KABYLAKE:      return "Kabylake";
		case IGFX_COFFEELAKE:    return "Coffeelake";
		case IGFX_WILLOWVIEW:    return "Willowview";
		case IGFX_BROXTON:       return "Broxton";
		case IGFX_GEMINILAKE:    return "Geminilake";

		case IGFX_CANNONLAKE:    return "Cannonlake";

		case IGFX_ICELAKE:       return "Icelake";
		case IGFX_ICELAKE_LP:    return "Icelake LP";
		case IGFX_LAKEFIELD:     return "Lakefield";

		case IGFX_TIGERLAKE_LP:  return "Tigerlake LP";

		case IGFX_ROCKETLAKE:	 return "Rocketlake";

		case IGFX_DG1:           return "DG1";

		case IGFX_ADL:			 return "Alderlake";

		case DGFX_ACM:			return "Alchemist";

		// Architectures with no unique enum value, but that still can be determined from DeviceID
		case IGFX_WHISKEYLAKE:   return "Whiskeylake";
		case IGFX_COMETLAKE:	 return "Cometlake";

		case IGFX_UNKNOWN:
		case IGFX_MAX_PRODUCT:
		default:
			assert(false);
			return "Unknown";
		}
	}

	INTEL_GPU_ARCHITECTURE GetIntelGPUArchitecture( unsigned int deviceId )
	{
		const unsigned int idhi = deviceId & 0xFF00;
		const unsigned int idlo = deviceId & 0x00FF;

		if( idhi == 0x0100 )
		{
			if( ( idlo & 0xFFF0 ) == 0x0050 || ( idlo & 0xFFF0 ) == 0x0060 )
			{
				return IGFX_IVYBRIDGE;
			}
			return IGFX_SANDYBRIDGE;
		}

		if( idhi == 0x0400 || idhi == 0x0A00 || idhi == 0x0D00 || idhi == 0x0C00 )
		{
			return IGFX_HASWELL;
		}

		if( idhi == 0x1600 || idhi == 0x0B00 )
		{
			return IGFX_BROADWELL;
		}

		if( idhi == 0x1900 || idhi == 0x0900 )
		{
			return IGFX_SKYLAKE;
		}

		if( idhi == 0x5900 )
		{
			return IGFX_KABYLAKE;
		}

		if( idhi == 0x3100 )
		{
			return IGFX_GEMINILAKE;
		}

		if( idhi == 0x5A00 )
		{
			return IGFX_CANNONLAKE;
		}

		if( idhi == 0x3E00 )
		{
			if( idlo == 0x00A0 || idlo == 0x00A1 )
			{
				return IGFX_WHISKEYLAKE;
			}
			return IGFX_COFFEELAKE;
		}

		if ( idhi == 0x8A00 )
		{
			return IGFX_ICELAKE_LP;
		}

		if( idhi == 0x9A00 )
		{
			return IGFX_TIGERLAKE_LP;
		}

		if( idhi == 0x4900 )
		{
			return IGFX_DG1;
		}

		if (idhi == 0x4C00)
		{
			return IGFX_ROCKETLAKE;
		}

		if (idhi == 0x9B00)
		{
			return IGFX_COMETLAKE;
		}

		if (idhi == 0x4600)
		{
			return IGFX_ADL;
		}

		if (idhi == 0x5600)
		{
			return DGFX_ACM;
		}

		assert(false);
		return IGFX_UNKNOWN;
	}

}

// #############################################################################
// Intel GPU Detect "ID3D10Extensions.h"

//--------------------------------------------------------------------------------------
// Copyright 2011,2012,2013 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.
//--------------------------------------------------------------------------------------

namespace ID3D10
{

	/*****************************************************************************\
	CONST: EXTENSION_INTERFACE_VERSION
	PURPOSE: Version of this header file
	\*****************************************************************************/
	const UINT EXTENSION_INTERFACE_VERSION_1_0 = 0x00010000;
	const UINT EXTENSION_INTERFACE_VERSION = EXTENSION_INTERFACE_VERSION_1_0;

	/*****************************************************************************\
	CONST: CAPS_EXTENSION_KEY
	PURPOSE: KEY to pass to UMD
	\*****************************************************************************/
	const char CAPS_EXTENSION_KEY[ 16 ] = {
		'I','N','T','C',
		'E','X','T','N',
		'C','A','P','S',
		'F','U','N','C' };

	/*****************************************************************************\
	TYPEDEF: PFND3D10UMDEXT_CHECKEXTENSIONSUPPORT
	PURPOSE: Function pointer for shader flag extensions
	\*****************************************************************************/
	typedef BOOL( APIENTRY* PFND3D10UMDEXT_CHECKEXTENSIONSUPPORT )( UINT );

	/*****************************************************************************\
	STRUCT: EXTENSION_BASE
	PURPOSE: Base data structure for extension initialization data
	\*****************************************************************************/
	struct EXTENSION_BASE
	{
		// Input:
		char    Key[ 16 ];                // CAPS_EXTENSION_KEY
		UINT    ApplicationVersion;     // EXTENSION_INTERFACE_VERSION
	};

	/*****************************************************************************\
	STRUCT: CAPS_EXTENSION_1_0
	PURPOSE: Caps data structure
	\*****************************************************************************/
	struct CAPS_EXTENSION_1_0 : EXTENSION_BASE
	{
		// Output:
		UINT    DriverVersion;          // EXTENSION_INTERFACE_VERSION
		UINT    DriverBuildNumber;      // BUILD_NUMBER
	};

	typedef CAPS_EXTENSION_1_0  CAPS_EXTENSION;

#ifndef D3D10_UMD
	/*****************************************************************************\
	FUNCTION: GetExtensionCaps
	PURPOSE: Gets extension caps table from Intel graphics driver
	\*****************************************************************************/
	inline HRESULT GetExtensionCaps(
		ID3D11Device* pd3dDevice,
		CAPS_EXTENSION* pCaps )
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory( &desc, sizeof( desc ) );
		desc.ByteWidth = sizeof( CAPS_EXTENSION );
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = pCaps;
		initData.SysMemPitch = sizeof( CAPS_EXTENSION );
		initData.SysMemSlicePitch = 0;

		ZeroMemory( pCaps, sizeof( CAPS_EXTENSION ) );
		memcpy( pCaps->Key, CAPS_EXTENSION_KEY,
				sizeof( pCaps->Key ) );
		pCaps->ApplicationVersion = EXTENSION_INTERFACE_VERSION;

		ID3D11Buffer* pBuffer = NULL;
		HRESULT result = pd3dDevice->CreateBuffer(
			&desc,
			&initData,
			&pBuffer );

		if( pBuffer )
			pBuffer->Release();

		if( S_OK == result )
		{
			result = ( pCaps->ApplicationVersion <= pCaps->DriverVersion ) ? S_OK : S_FALSE;
		}
		return result;
	};
#endif

	/*****************************************************************************\
	CONST: RESOURCE_EXTENSION_KEY
	PURPOSE: KEY to pass to UMD
	\*****************************************************************************/
	const char RESOURCE_EXTENSION_KEY[ 16 ] = {
		'I','N','T','C',
		'E','X','T','N',
		'R','E','S','O',
		'U','R','C','E' };

	/*****************************************************************************\
	ENUM: RESOURCE_EXTENSION_TYPE
	PURPOSE: Enumeration of supported resource extensions
	\*****************************************************************************/
	enum RESOURCE_EXTENSION_TYPE
	{
		RESOURCE_EXTENSION_RESERVED = 0,

		// Version 1_0
		RESOURCE_EXTENSION_DIRECT_ACCESS = 1,
	};

	/*****************************************************************************\
	ENUM: RESOURCE_EXTENSION_FLAGS
	PURPOSE: Enumeration for extra information
	\*****************************************************************************/
	enum RESOURCE_EXTENSION_FLAGS
	{
		RESOURCE_EXTENSION_DIRECT_ACCESS_LINEAR_ALLOCATION = 0x1,
	};

	/*****************************************************************************\
	STRUCT: RESOURCE_EXTENSION_1_0
	PURPOSE: Resource extension interface structure
	\*****************************************************************************/
	struct RESOURCE_EXTENSION_1_0 : EXTENSION_BASE
	{
		// Input:

		// Enumeration of the extension
		UINT    Type;       // RESOURCE_EXTENSION_TYPE

		// Extension data
		UINT    Data[ 16 ];
	};

	typedef RESOURCE_EXTENSION_1_0  RESOURCE_EXTENSION;

	/*****************************************************************************\
	STRUCT: RESOURCE_DIRECT_ACCESS_MAP_DATA
	PURPOSE: Direct Access Resource extension Map structure
	\*****************************************************************************/
	struct RESOURCE_DIRECT_ACCESS_MAP_DATA
	{
		void*   pBaseAddress;
		UINT    XOffset;
		UINT    YOffset;

		UINT    TileFormat;
		UINT    Pitch;
		UINT    Size;
	};

#ifndef D3D10_UMD
	/*****************************************************************************\
	FUNCTION: SetResouceExtension
	PURPOSE: Resource extension interface
	\*****************************************************************************/
	inline HRESULT SetResouceExtension(
		ID3D11Device* pd3dDevice,
		const RESOURCE_EXTENSION* pExtnDesc )
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory( &desc, sizeof( desc ) );
		desc.ByteWidth = sizeof( RESOURCE_EXTENSION );
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory( &initData, sizeof( initData ) );
		initData.pSysMem = pExtnDesc;
		initData.SysMemPitch = sizeof( RESOURCE_EXTENSION );
		initData.SysMemSlicePitch = 0;

		ID3D11Buffer* pBuffer = NULL;
		HRESULT result = pd3dDevice->CreateBuffer(
			&desc,
			&initData,
			&pBuffer );

		if( pBuffer )
			pBuffer->Release();

		return result;
	}

	/*****************************************************************************\
	FUNCTION: SetDirectAccessResouceExtension
	PURPOSE: Direct Access Resource extension interface
	\*****************************************************************************/
	inline HRESULT SetDirectAccessResouceExtension(
		ID3D11Device* pd3dDevice,
		const UINT flags )
	{
		RESOURCE_EXTENSION extnDesc;
		ZeroMemory( &extnDesc, sizeof( extnDesc ) );
		memcpy( &extnDesc.Key[ 0 ], RESOURCE_EXTENSION_KEY,
				sizeof( extnDesc.Key ) );
		extnDesc.ApplicationVersion = EXTENSION_INTERFACE_VERSION;
		extnDesc.Type = RESOURCE_EXTENSION_DIRECT_ACCESS;
		extnDesc.Data[ 0 ] = flags;

		return SetResouceExtension( pd3dDevice, &extnDesc );
	}
#endif

	/*****************************************************************************\
	CONST: STATE_EXTENSION_KEY
	PURPOSE: KEY to pass to UMD
	\*****************************************************************************/
	const char STATE_EXTENSION_KEY[ 16 ] = {
		'I','N','T','C',
		'E','X','T','N',
		'S','T','A','T',
		'E','O','B','J' };

	/*****************************************************************************\
	ENUM: STATE_EXTENSION_TYPE
	PURPOSE: Enumeration of supported state extensions
	\*****************************************************************************/
	enum STATE_EXTENSION_TYPE
	{
		STATE_EXTENSION_RESERVED = 0,

		// Version 1_0
	};

	/*****************************************************************************\
	STRUCT: STATE_EXTENSION_1_0
	PURPOSE: UMD extension interface structure
	\*****************************************************************************/
	struct STATE_EXTENSION_1_0 : EXTENSION_BASE
	{
		// Input:

		// Enumeration of the extension
		UINT    Type;       // STATE_EXTENSION_TYPE

		// Extension data
		UINT    Data[ 16 ];
	};

	typedef STATE_EXTENSION_1_0 STATE_EXTENSION;

#ifndef D3D10_UMD
	/*****************************************************************************\
	FUNCTION: SetStateExtension
	PURPOSE: State extension interface
	\*****************************************************************************/
	inline HRESULT SetStateExtension(
		ID3D11Device* pd3dDevice,
		const STATE_EXTENSION* pExtnDesc )
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory( &desc, sizeof( desc ) );
		desc.ByteWidth = sizeof( STATE_EXTENSION );
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory( &initData, sizeof( initData ) );
		initData.pSysMem = pExtnDesc;
		initData.SysMemPitch = sizeof( STATE_EXTENSION );
		initData.SysMemSlicePitch = 0;

		ID3D11Buffer* pBuffer = NULL;
		HRESULT result = pd3dDevice->CreateBuffer(
			&desc,
			&initData,
			&pBuffer );

		if( pBuffer )
			pBuffer->Release();

		return result;
	}
#endif

	/*****************************************************************************\
	ENUM: SHADER_EXTENSION_TYPE
	PURPOSE: Enumeration of supported shader extensions
	\*****************************************************************************/
	enum SHADER_EXTENSION_TYPE
	{
		SHADER_EXTENSION_RESERVED = 0,

		// Version 1_0
		SHADER_EXTENSION_PIXEL_SHADER_ORDERING = 1,
	};

} // namespace ID3D10

// #############################################################################
// Intel GPU Detect "GPUDetect.h"

////////////////////////////////////////////////////////////////////////////////
// Copyright 2017-2020 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
////////////////////////////////////////////////////////////////////////////////

// forward decls
struct IDXGIAdapter;
struct ID3D11Device;


// Error return codes for GPUDetect
// These codes are set up so that (ERROR_CODE % GENERAL_ERROR_CODE) == 0 if
// ERROR_CODE is a subset of GENERAL_ERROR_CODE.
// e.g. GPUDETECT_ERROR_DXGI_LOAD % GPUDETECT_ERROR_DXGI_LOAD == 0
#define GPUDETECT_ERROR_GENERIC                 1

/// General DXGI Errors
#define GPUDETECT_ERROR_GENERAL_DXGI            2
#define GPUDETECT_ERROR_DXGI_LOAD               GPUDETECT_ERROR_GENERAL_DXGI * 3
#define GPUDETECT_ERROR_DXGI_ADAPTER_CREATION   GPUDETECT_ERROR_GENERAL_DXGI * 5
#define GPUDETECT_ERROR_DXGI_FACTORY_CREATION   GPUDETECT_ERROR_GENERAL_DXGI * 7
#define GPUDETECT_ERROR_DXGI_DEVICE_CREATION    GPUDETECT_ERROR_GENERAL_DXGI * 11
#define GPUDETECT_ERROR_DXGI_GET_ADAPTER_DESC   GPUDETECT_ERROR_GENERAL_DXGI * 13

/// DXGI Counter Errors
#define GPUDETECT_ERROR_GENERAL_DXGI_COUNTER    17
#define GPUDETECT_ERROR_DXGI_BAD_COUNTER        GPUDETECT_ERROR_GENERAL_DXGI_COUNTER * 19
#define GPUDETECT_ERROR_DXGI_COUNTER_CREATION   GPUDETECT_ERROR_GENERAL_DXGI_COUNTER * 23
#define GPUDETECT_ERROR_DXGI_COUNTER_GET_DATA   GPUDETECT_ERROR_GENERAL_DXGI_COUNTER * 29

/// Windows Registry Errors
#define GPUDETECT_ERROR_REG_GENERAL_FAILURE     31
#define GPUDETECT_ERROR_REG_NO_D3D_KEY          GPUDETECT_ERROR_REG_GENERAL_FAILURE * 37 // A DirectX key was not found in the registry in the expected location
#define GPUDETECT_ERROR_REG_MISSING_DRIVER_INFO GPUDETECT_ERROR_REG_GENERAL_FAILURE * 41 // Driver info is missing from the registry

/// Precondition Errors
#define GPUDETECT_ERROR_BAD_DATA                47

#define GPUDETECT_ERROR_NOT_SUPPORTED           55


namespace GPUDetect
{
	// Define settings to reflect Fidelity abstraction levels you need
	enum PresetLevel
	{
		NotCompatible,  // Found GPU is not compatible with the app
		Low,
		Medium,
		MediumPlus,
		High,
		Undefined  // No predefined setting found in cfg file.
				   // Use a default level for unknown video cards.
	};

	// An enum that identifies the generation of Graphics
	enum IntelGraphicsGeneration
	{
		INTEL_GFX_GEN_UNKNOWN = 0,
		INTEL_GFX_GEN6,
		INTEL_GFX_GEN7,
		INTEL_GFX_GEN7_5,
		INTEL_GFX_GEN8,
		INTEL_GFX_GEN9,
		INTEL_GFX_GEN9_5,
		INTEL_GFX_GEN10,
		INTEL_GFX_GEN11,
		INTEL_GFX_GEN12,
		INTEL_DGFX_ACM
	};

	struct GPUData
	{
		/////////////////////////
		// DX11 Extension Data //
		/////////////////////////

		/*******************************************************************************
		 * dxAdapterAvailability
		 *
		 *     Is true if Intel driver extension data is populated.
		 *     If this value is false, all other extension data will be null.
		 *
		 *     This value is initialized by the InitExtensionInfo function.
		 *
		 ******************************************************************************/
		bool dxAdapterAvailability;

		/*******************************************************************************
		 * vendorID
		 *
		 *     The vendorID of the GPU.
		 *
		 *     This value is initialized by the InitExtensionInfo function.
		 *
		 ******************************************************************************/
		unsigned int vendorID;

		/*******************************************************************************
		 * deviceID
		 *
		 *     The DeviceID of the GPU.
		 *
		 *     This value is initialized by the InitExtensionInfo function.
		 *
		 ******************************************************************************/
		unsigned int deviceID;

		/*******************************************************************************
		 * adapterLUID
		 *
		 *     The LUID of the d3d adapter.
		 *
		 *     This value is initialized by the InitExtensionInfo function.
		 *
		 ******************************************************************************/
		LUID adapterLUID;

		/*******************************************************************************
		* architectureCounter
		*
		*     The architecture of the GPU.
		*
		*     This value is initialized by the InitExtensionInfo function.
		*
		******************************************************************************/
		INTEL_GPU_ARCHITECTURE architecture;

		/*******************************************************************************
		 * isUMAArchitecture
		 *
		 *     Is true if the GPU uses a uniform memory access architecture.
		 *     On GPUs with a Unified Memory Architecture (UMA) like Intel integrated
		 *     GPUs, the CPU system memory is also used for graphics and there is no
		 *     dedicated VRAM.  Any memory reported as "dedicated" is really a small
		 *     pool of system memory reserved by the BIOS for internal use. All normal
		 *     application allocations (buffers, textures, etc.) are allocated from
		 *     general system "shared" memory.  For this reason, do not use the
		 *     dedicated memory size as an indication of UMA GPU capability (either
		 *     performance, nor memory capacity).
		 *
		 *     This value is initialized by the InitExtensionInfo function.
		 *
		 ******************************************************************************/
		bool isUMAArchitecture;

		/*******************************************************************************
		 * videoMemory
		 *
		 *     The amount of Video memory in bytes.
		 *
		 *     This value is initialized by the InitExtensionInfo function.
		 *
		 ******************************************************************************/
		uint64_t videoMemory;

		/*******************************************************************************
		 * description
		 *
		 *     The driver-provided description of the GPU.
		 *
		 *     This value is initialized by the InitExtensionInfo function.
		 *
		 ******************************************************************************/
		WCHAR description[ _countof( DXGI_ADAPTER_DESC::Description ) ];

		/*******************************************************************************
		 * extensionVersion
		 *
		 *     Version number for D3D driver extensions.
		 *
		 *     This value is initialized by the InitExtensionInfo function.
		 *
		 ******************************************************************************/
		unsigned int extensionVersion;

		/*******************************************************************************
		 * intelExtensionAvailability
		 *
		 *     True if Intel driver extensions are available on the GPU.
		 *
		 *     This value is initialized by the InitExtensionInfo function.
		 *
		 ******************************************************************************/
		bool intelExtensionAvailability;

		/////////////////////////////////
		// DX11 Hardware Counters Data //
		/////////////////////////////////

		/*******************************************************************************
		 * counterAvailability
		 *
		 *     Returns true if Intel driver extensions are available to gather data from.
		 *     If this value is false, all other extension data will be null.
		 *
		 *     This value is initialized by the InitCounterInfo function.
		 *
		 ******************************************************************************/
		bool counterAvailability;

		/*******************************************************************************
		 * maxFrequency
		 *
		 *     Returns the maximum frequency of the GPU in MHz.
		 *
		 *     This value is initialized by the InitCounterInfo function.
		 *
		 ******************************************************************************/
		unsigned int maxFrequency;

		/*******************************************************************************
		 * minFrequency
		 *
		 *     Returns the minimum frequency of the GPU in MHz.
		 *
		 *     This value is initialized by the InitCounterInfo function.
		 *
		 ******************************************************************************/
		unsigned int minFrequency;

		/// Advanced counter info

		/*******************************************************************************
		 * advancedCounterDataAvailability
		 *
		 *     Returns true if advanced counter data is available from this GPU.
		 *     Older Intel products only provide the maximum and minimum GPU frequency
		 *     via the hardware counters.
		 *
		 *     This value is initialized by the InitCounterInfo function.
		 *
		 ******************************************************************************/
		bool advancedCounterDataAvailability;

		/*******************************************************************************
		 * euCount
		 *
		 *     Returns the number of execution units (EUs) on the GPU.
		 *
		 *     This value is initialized by the InitCounterInfo function.
		 *
		 ******************************************************************************/
		unsigned int euCount;

		/*******************************************************************************
		 * packageTDP
		 *
		 *     Returns the thermal design power (TDP) of the GPU in watts.
		 *
		 *     This value is initialized by the InitCounterInfo function.
		 *
		 ******************************************************************************/
		unsigned int packageTDP;

		/*******************************************************************************
		 * maxFillRate
		 *
		 *     Returns the maximum fill rate of the GPU in pixels/clock.
		 *
		 *     This value is initialized by the InitCounterInfo function.
		 *
		 ******************************************************************************/
		unsigned int maxFillRate;

		///////////////////////
		// D3D Registry Data //
		///////////////////////

		/*******************************************************************************
		* dxDriverVersion
		*
		*     The version number for the adapter's driver. This is stored in a 4 part
		*     version number that should be displayed in the format: "0.1.2.4"
		*
		*     This value is initialized by the InitDxDriverVersion function.
		*
		******************************************************************************/
		unsigned int dxDriverVersion[ 4 ];

		/*******************************************************************************
		 * d3dRegistryDataAvailability
		 *
		 *     Is true if d3d registry data is populated. If this value is true and
		 *     vendorID == INTEL_VENDOR_ID, then the driverInfo and dxDriverVersion 
		 *     fields will be populated. If this value is false, all other registry data
		 *     will be null.
		 *
		 *     This value is initialized by the InitDxDriverVersion function.
		 *
		 ******************************************************************************/
		bool d3dRegistryDataAvailability;

		struct DriverVersionInfo
		{
			/*******************************************************************************
			 * driverReleaseRevision
			 *
			 *     The release revision for this device's driver. This is the third
			 *     section of the driver version number as illustrated by the 'X's below:
			 *     00.00.XXX.0000
			 *
			 *     This value is initialized by the InitDxDriverVersion function.
			 *
			 ******************************************************************************/
			unsigned int driverReleaseRevision;

			/*******************************************************************************
			 * buildNumber
			 *
			 *     The build number for this device's driver. This is the last
			 *     section of the driver version number as shown below:
			 *     00.00.000.XXXX
			 *
			 *     For example, for the driver version 12.34.56.7890, buildNumber
			 *     would be 7890.
			 *
			 *     This value is initialized by the InitDxDriverVersion function.
			 *
			 ******************************************************************************/
			unsigned int driverBuildNumber;
		} driverInfo;
	};

	/*******************************************************************************
	 * InitAll
	 *
	 *     Loads all available info from this GPUDetect into gpuData. Returns
	 *     EXIT_SUCCESS if no error was encountered, otherwise returns an error code.
	 *
	 *     gpuData
	 *         The struct in which the information will be stored.
	 *
	 *     adapterIndex
	 *         The index of the adapter to get the information from.
	 *
	 ******************************************************************************/
	int InitAll( GPUData* const gpuData, int adapterIndex );

	/*******************************************************************************
	 * InitAll
	 *
	 *     Loads all available info from this GPUDetect into gpuData. Returns
	 *     EXIT_SUCCESS if no error was encountered, otherwise returns an error code.
	 *
	 *     gpuData
	 *         The struct in which the information will be stored.
	 *
	 *     adapter
	 *         A pointer to the adapter to draw info from.
	 *
	 *	   device
	 *         A pointer to the device to draw info from.
	 *
	 ******************************************************************************/
	int InitAll( GPUData* const gpuData, IDXGIAdapter* adapter, ID3D11Device* device );

	/*******************************************************************************
	 * InitExtensionInfo
	 *
	 *     Loads available info from the DX11 extension interface. Returns
	 *     EXIT_SUCCESS if no error was encountered, otherwise returns an error code.
	 *
	 *     gpuData
	 *         The struct in which the information will be stored.
	 *
	 *     adapterIndex
	 *         The index of the adapter to get the information from.
	 *
	 ******************************************************************************/
	int InitExtensionInfo( GPUData* const gpuData, int adapterIndex );

	/*******************************************************************************
	 * InitExtensionInfo
	 *
	 *     Loads available info from the DX11 extension interface. Returns
	 *     EXIT_SUCCESS if no error was encountered, otherwise returns an error code.
	 *
	 *     gpuData
	 *         The struct in which the information will be stored.
	 *
	 *     adapter
	 *         A pointer to the adapter to draw info from.
	 *
	 *	   device
	 *         A pointer to the device to draw info from.
	 *
	 ******************************************************************************/
	int InitExtensionInfo( GPUData* const gpuData, IDXGIAdapter* adapter, ID3D11Device* device );

	/*******************************************************************************
	 * InitCounterInfo
	 *
	 *     Loads available info from DX11 hardware counters. Returns EXIT_SUCCESS
	 *     if no error was encountered, otherwise returns an error code. Requires
	 *     that InitExtensionInfo be run on gpuData before this is called.
	 *
	 *     gpuData
	 *         The struct in which the information will be stored.
	 *
	 *     adapterIndex
	 *         The index of the adapter to get the information from.
	 *
	 ******************************************************************************/
	int InitCounterInfo( GPUData* const gpuData, int adapterIndex );

	/*******************************************************************************
	 * InitCounterInfo
	 *
	 *     Intel exposes additional information through the DX driver
	 *     that can be obtained querying a special counter. This loads the
	 *     available info. It returns EXIT_SUCCESS if no error was encountered,
	 *     otherwise returns an error code.
	 *
	 *      Requires that InitExtensionInfo be run on gpuData before this is called.
	 *
	 *     gpuData
	 *         The struct in which the information will be stored.
	 *
	 *	   device
	 *         A pointer to the device to draw info from.
	 *
	 ******************************************************************************/
	int InitCounterInfo( GPUData* const gpuData, ID3D11Device* device );

	/*******************************************************************************
	 * GetDefaultFidelityPreset
	 *
	 *     Function to find the default fidelity preset level, based on the type of
	 *     graphics adapter present.
	 *
	 *     The guidelines for graphics preset levels for Intel devices is a generic
	 *     one based on our observations with various contemporary games. You would
	 *     have to change it if your game already plays well on the older hardware
	 *     even at high settings.
	 *
	 *     Presets for Intel are expected in a file named "IntelGFX.cfg". This
	 *     method can also be easily modified to also read similar .cfg files
	 *     detailing presets for other manufacturers.
	 *
	 *     gpuData
	 *         The data for the GPU in question.
	 *
	 ******************************************************************************/
	PresetLevel GetDefaultFidelityPreset( const GPUData* const gpuData );

	/*******************************************************************************
	 * InitDxDriverVersion
	 *
	 *     Loads the DX driver version for the given adapter from the windows
	 *     registry. Returns EXIT_SUCCESS if no error was encountered, otherwise
	 *     returns an error code. Requires that InitExtensionInfo be run on gpuData
	 *     before this is called.
	 *
	 *     gpuData
	 *         The struct in which the information will be stored.
	 *
	 ******************************************************************************/
	int InitDxDriverVersion( GPUData* const gpuData );

	/*******************************************************************************
	 * GetDriverVersionAsCString
	 *
	 *     Stores the driver version as a string in the 00.00.000.0000 format.
	 *
	 *     gpuData
	 *         The struct that contains the driver version.
	 *
	 *     outBuffer
	 *         Buffer to store the resulting c string.
	 *
	 *     outBufferSize
	 *         Size of buffer to store the resulting c string.
	 *
	 ******************************************************************************/
	void GetDriverVersionAsCString( const GPUData* const gpuData, char* const outBuffer, size_t outBufferSize );

	/*******************************************************************************
	 * GetIntelGraphicsGeneration
	 *
	 *     Returns the generation of Intel(tm) Graphics, given the architecture.
	 *
	 *     architecture
	 *         The architecture to identify.
	 *
	 ******************************************************************************/
	IntelGraphicsGeneration GetIntelGraphicsGeneration( INTEL_GPU_ARCHITECTURE architecture );

	/*******************************************************************************
	 * GetIntelGraphicsGenerationString
	 *
	 *     Returns the generation of Intel(tm) Graphics, given the architecture.
	 *
	 *     generation
	 *         The generation to identify.
	 *
	 ******************************************************************************/
	const char * GetIntelGraphicsGenerationString( const IntelGraphicsGeneration genneration );

	/*******************************************************************************
	 * InitAdapter
	 *
	 *     Initializes a adapter of the given index. Returns EXIT_SUCCESS if no
	 *     error was encountered, otherwise returns an error code.
	 *
	 *     adapter
	 *         The address of the pointer that this function will point to
	 *         the created adapter.
	 *
	 *     adapterIndex
	 *         If adapterIndex >= 0 this will be used as as the index of the
	 *         adapter to create.
	 *
	 ******************************************************************************/
	int InitAdapter( IDXGIAdapter** adapter, int adapterIndex );

	/*******************************************************************************
	 * InitDevice
	 *
	 *     Initializes a DX11 device of the given index. Returns EXIT_SUCCESS if no
	 *     error was encountered, otherwise returns an error code.
	 *
	 *     adapter
	 *         The adapter to create the device from.
	 *
	 *     device
	 *         The address of the pointer that this function will point to the
	 *         created device.
	 *
	 ******************************************************************************/
	int InitDevice( IDXGIAdapter* adapter, ID3D11Device** device );

}

// #############################################################################
// Intel GPU Detect "GPUDetect.cpp"

////////////////////////////////////////////////////////////////////////////////
// Copyright 2017-2020 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
////////////////////////////////////////////////////////////////////////////////

// These should only be needed for reading data from the counter
struct IntelDeviceInfo1
{
	DWORD GPUMaxFreq;
	DWORD GPUMinFreq;
};
static_assert( sizeof(IntelDeviceInfo1) == 8, "struct size mismatch" );

struct IntelDeviceInfo2 : public IntelDeviceInfo1
{
	DWORD GPUArchitecture;   // should match INTEL_GPU_ARCHITECTURE
	DWORD EUCount;
	DWORD PackageTDP;
	DWORD MaxFillRate;
};
static_assert( sizeof(IntelDeviceInfo2) == 24, "struct size mismatch" );


namespace GPUDetect
{

// Returns EXIT_SUCCESS if successfully initialized
int GetIntelDeviceInfo( IntelDeviceInfo2* deviceInfo, ID3D11Device* device );


int InitAll( GPUData* const gpuData, int adapterIndex )
{
	if( gpuData == nullptr || adapterIndex < 0 )
	{
		return GPUDETECT_ERROR_BAD_DATA;
	}

	IDXGIAdapter* adapter = nullptr;
	int returnCode = InitAdapter( &adapter, adapterIndex );
	if( returnCode != EXIT_SUCCESS )
	{
		return returnCode;
	}

	ID3D11Device* device = nullptr;
	returnCode = InitDevice( adapter, &device );
	if( returnCode != EXIT_SUCCESS )
	{
		adapter->Release();
		return returnCode;
	}

	returnCode = InitAll( gpuData, adapter, device );

	adapter->Release();
	device->Release();
	return returnCode;
}

int InitAll( GPUData* const gpuData, IDXGIAdapter* adapter, ID3D11Device* device )
{
	int returnCode = InitExtensionInfo( gpuData, adapter, device );
	if( returnCode != EXIT_SUCCESS )
	{
		return returnCode;
	}

	returnCode = InitDxDriverVersion( gpuData );
	if( returnCode != EXIT_SUCCESS )
	{
		return returnCode;
	}

	returnCode = InitCounterInfo( gpuData, device );

	return returnCode;
}

int InitExtensionInfo( GPUData* const gpuData, int adapterIndex )
{
	if( gpuData == nullptr || adapterIndex < 0 )
	{
		return GPUDETECT_ERROR_BAD_DATA;
	}

	IDXGIAdapter* adapter = nullptr;
	int returnCode = InitAdapter( &adapter, adapterIndex );
	if( returnCode != EXIT_SUCCESS )
	{
		return returnCode;
	}

	ID3D11Device* device = nullptr;
	returnCode = InitDevice( adapter, &device );
	if( returnCode != EXIT_SUCCESS )
	{
		adapter->Release();
		return returnCode;
	}

	returnCode = InitExtensionInfo( gpuData, adapter, device );

	adapter->Release();
	device->Release();
	return returnCode;
}

int InitExtensionInfo( GPUData* const gpuData, IDXGIAdapter* adapter, ID3D11Device* device )
{
	if ( gpuData == nullptr || adapter == nullptr || device == nullptr )
	{
		return GPUDETECT_ERROR_BAD_DATA;
	}

	// basic DXGI information
	DXGI_ADAPTER_DESC AdapterDesc = {};
	if( FAILED( adapter->GetDesc( &AdapterDesc ) ) )
	{
		return GPUDETECT_ERROR_DXGI_ADAPTER_CREATION;
	}

	gpuData->dxAdapterAvailability = true;

	gpuData->vendorID = AdapterDesc.VendorId;
	gpuData->deviceID = AdapterDesc.DeviceId;
	gpuData->adapterLUID = AdapterDesc.AdapterLuid;

	wcscpy_s( gpuData->description, _countof( GPUData::description ), AdapterDesc.Description );

	if( AdapterDesc.VendorId == INTEL_VENDOR_ID && AdapterDesc.DedicatedVideoMemory <= 512 * 1024 * 1024 )
	{
		gpuData->isUMAArchitecture = true;
	}

#ifdef _WIN32_WINNT_WIN10
	ID3D11Device3* pDevice3 = nullptr;
	if( SUCCEEDED( device->QueryInterface( __uuidof( ID3D11Device3 ), (void**) &pDevice3 ) ) )
	{
		D3D11_FEATURE_DATA_D3D11_OPTIONS2 FeatureData = {};
		if( SUCCEEDED( pDevice3->CheckFeatureSupport( D3D11_FEATURE_D3D11_OPTIONS2, &FeatureData, sizeof( FeatureData ) ) ) )
		{
			gpuData->isUMAArchitecture = FeatureData.UnifiedMemoryArchitecture == TRUE;
		}
		pDevice3->Release();
	}
#endif // _WIN32_WINNT_WIN10

	if( gpuData->isUMAArchitecture )
	{
		gpuData->videoMemory = AdapterDesc.SharedSystemMemory;
	}
	else
	{
		gpuData->videoMemory = AdapterDesc.DedicatedVideoMemory;
	}

	// Intel specific information
	if( AdapterDesc.VendorId == INTEL_VENDOR_ID )
	{
		gpuData->architecture = GetIntelGPUArchitecture(gpuData->deviceID);

		ID3D10::CAPS_EXTENSION intelExtCaps = {};
		if (S_OK == GetExtensionCaps(device, &intelExtCaps))
		{
			gpuData->extensionVersion = intelExtCaps.DriverVersion;
			gpuData->intelExtensionAvailability = (gpuData->extensionVersion >= ID3D10::EXTENSION_INTERFACE_VERSION_1_0);
		}
	}

	return EXIT_SUCCESS;
}

int InitCounterInfo( GPUData* const gpuData, int adapterIndex )
{
	if( gpuData == nullptr || adapterIndex < 0 )
	{
		return GPUDETECT_ERROR_BAD_DATA;
	}

	IDXGIAdapter* adapter = nullptr;
	int returnCode = InitAdapter( &adapter, adapterIndex );
	if( returnCode != EXIT_SUCCESS )
	{
		return returnCode;
	}

	ID3D11Device* device = nullptr;
	returnCode = InitDevice( adapter, &device );
	if( returnCode != EXIT_SUCCESS )
	{
		adapter->Release();
		return returnCode;
	}

	returnCode = InitCounterInfo( gpuData, device );

	adapter->Release();
	return returnCode;
}

int InitCounterInfo( GPUData* const gpuData, ID3D11Device* device )
{
	if( gpuData == nullptr || device == nullptr || gpuData->vendorID == 0 || gpuData->deviceID == 0 )
	{
		return GPUDETECT_ERROR_BAD_DATA;
	}

	//
	// In DirectX, Intel exposes additional information through the driver that can be obtained
	// querying a special DX counter
	//
	gpuData->counterAvailability = gpuData->vendorID == INTEL_VENDOR_ID;
	if( !gpuData->counterAvailability )
	{
		return GPUDETECT_ERROR_NOT_SUPPORTED;
	}

	IntelDeviceInfo2 info = {};
	const int deviceInfoReturnCode = GetIntelDeviceInfo( &info, device );
	if (deviceInfoReturnCode != EXIT_SUCCESS)
	{
		return deviceInfoReturnCode;
	}
	else
	{
		gpuData->maxFrequency = info.GPUMaxFreq;
		gpuData->minFrequency = info.GPUMinFreq;

		//
		// Older versions of the IntelDeviceInfo query only return
		// GPUMaxFreq and GPUMinFreq, all other members will be zero.
		//
		if (info.GPUArchitecture != IGFX_UNKNOWN)
		{
			gpuData->advancedCounterDataAvailability = true;

			gpuData->architecture = (INTEL_GPU_ARCHITECTURE)info.GPUArchitecture;
			assert(gpuData->architecture == GetIntelGPUArchitecture(gpuData->deviceID));

			gpuData->euCount = info.EUCount;
			gpuData->packageTDP = info.PackageTDP;
			gpuData->maxFillRate = info.MaxFillRate;
		}
	}

	return EXIT_SUCCESS;
}

PresetLevel GetDefaultFidelityPreset( const GPUData* const gpuData )
{
	// Return if prerequisite info is not met
	if( !gpuData->dxAdapterAvailability )
	{
		return PresetLevel::Undefined;
	}

	//
	// Look for a config file that qualifies devices from any vendor
	// The code here looks for a file with one line per recognized graphics
	// device in the following format:
	//
	// VendorIDHex, DeviceIDHex, CapabilityEnum      ;Commented name of card
	//

	const char* cfgFileName = nullptr;

	switch( gpuData->vendorID )
	{
	case INTEL_VENDOR_ID:
		cfgFileName = "IntelGfx.cfg";
		break;

		// Add other cases in this fashion to allow for additional cfg files
		//case SOME_VENDOR_ID:
		//    cfgFileName =  "OtherBrandGfx.cfg";
		//    break;

	default:
		return PresetLevel::Undefined;;
	}

	PresetLevel presets = Undefined;

	CUSTOM_FILE* fp = nullptr;
	custom_fopen_s( &fp, cfgFileName, "r" );

	if( fp )
	{
		char line[ 100 ];

		//
		// read one line at a time till EOF
		//
		while( custom_fgets( line, _countof( line ), fp ) )
		{
			//
			// Parse and remove the comment part of any line
			//
			unsigned int i = 0;
			for( ; i < _countof( line ) - 1 && line[ i ] && line[ i ] != ';'; i++ )
			{}
			line[ i ] = '\0';

			//
			// Try to extract GPUVendorId, GPUDeviceId and recommended Default Preset Level
			//
			char* context = nullptr;
			const char* const szVendorId = strtok_s( line, ",\n", &context );
			const char* const szDeviceId = strtok_s( nullptr, ",\n", &context );
			const char* const szPresetLevel = strtok_s( nullptr, ",\n", &context );

			if( ( szVendorId == nullptr ) ||
				( szDeviceId == nullptr ) ||
				( szPresetLevel == nullptr ) )
			{
				continue;  // blank or improper line in cfg file - skip to next line
			}

			unsigned int vId = 0;
			int rv = sscanf_s( szVendorId, "%x", &vId );
			assert( rv == 1 );

			unsigned int dId = 0;
			rv = sscanf_s( szDeviceId, "%x", &dId );
			assert( rv == 1 );

			//
			// If current graphics device is found in the cfg file, use the
			// pre-configured default Graphics Presets setting.
			//
			if( ( vId == gpuData->vendorID ) && ( dId == gpuData->deviceID ) )
			{
				char s[ 10 ] = {};
				sscanf_s( szPresetLevel, "%s", s, (unsigned int) _countof( s ) );

				if( !_stricmp( s, "Low" ) )
					presets = Low;
				else if( !_stricmp( s, "Medium" ) )
					presets = Medium;
				else if( !_stricmp( s, "Medium+" ) )
					presets = MediumPlus;
				else if( !_stricmp( s, "High" ) )
					presets = High;
				else
					presets = NotCompatible;

				break;
			}
		}

		custom_fclose( fp );
	}
	else
	{
		fprintf( stderr, "Error: %s not found! Fallback to default presets.\n", cfgFileName );
	}

	//
	// If the current graphics device was not listed in any of the config
	// files, or if config file not found, use Low settings as default.
	// This should be changed to reflect the desired behavior for unknown
	// graphics devices.
	//
	/*
	if( presets == Undefined )
	{
		presets = Low;
	}
	*/

	return presets;
}

int InitDxDriverVersion( GPUData* const gpuData )
{
	if( gpuData == nullptr || !( gpuData->dxAdapterAvailability == true ) )
	{
		return GPUDETECT_ERROR_BAD_DATA;
	}

	if( gpuData->adapterLUID.HighPart == 0 && gpuData->adapterLUID.LowPart == 0 )
	{
		// This should not happen with an active/current adapter.
		// But the registry can contain old leftover driver entries with LUID == 0.
		return GPUDETECT_ERROR_BAD_DATA;
	}

	// Fetch registry data
	HKEY dxKeyHandle = nullptr;
	DWORD numOfAdapters = 0;

	LSTATUS returnCode = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\DirectX"), 0, KEY_READ, &dxKeyHandle );

	if( returnCode != ERROR_SUCCESS )
	{
		return GPUDETECT_ERROR_REG_NO_D3D_KEY;
	}

	// Find all subkeys

	DWORD subKeyMaxLength = 0;

	returnCode = ::RegQueryInfoKey(
		dxKeyHandle,
		nullptr,
		nullptr,
		nullptr,
		&numOfAdapters,
		&subKeyMaxLength,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr
	);

	if( returnCode != ERROR_SUCCESS )
	{
		return GPUDETECT_ERROR_REG_GENERAL_FAILURE;
	}

	subKeyMaxLength += 1; // include the null character

	uint64_t driverVersionRaw = 0;

	bool foundSubkey = false;
	TCHAR* subKeyName = new TCHAR[subKeyMaxLength];

	for( DWORD i = 0; i < numOfAdapters; ++i )
	{
		DWORD subKeyLength = subKeyMaxLength;

		returnCode = ::RegEnumKeyEx(
			dxKeyHandle,
			i,
			subKeyName,
			&subKeyLength,
			nullptr,
			nullptr,
			nullptr,
			nullptr
		);

		if( returnCode == ERROR_SUCCESS )
		{
			LUID adapterLUID = {};
			DWORD qwordSize = sizeof( uint64_t );

			returnCode = ::RegGetValue(
				dxKeyHandle,
				subKeyName,
				_T("AdapterLuid"),
				RRF_RT_QWORD,
				nullptr,
				&adapterLUID,
				&qwordSize
			);

			if( returnCode == ERROR_SUCCESS // If we were able to retrieve the registry values
				&& adapterLUID.HighPart == gpuData->adapterLUID.HighPart && adapterLUID.LowPart == gpuData->adapterLUID.LowPart ) // and if the vendor ID and device ID match
			{
				// We have our registry key! Let's get the driver version num now

				returnCode = ::RegGetValue(
					dxKeyHandle,
					subKeyName,
					_T("DriverVersion"),
					RRF_RT_QWORD,
					nullptr,
					&driverVersionRaw,
					&qwordSize
				);

				if( returnCode == ERROR_SUCCESS )
				{
					foundSubkey = true;
					break;
				}
			}
		}
	}

	returnCode = ::RegCloseKey( dxKeyHandle );
	assert( returnCode == ERROR_SUCCESS );
	delete[] subKeyName;

	if( !foundSubkey )
	{
		return GPUDETECT_ERROR_REG_MISSING_DRIVER_INFO;
	}

	// Now that we have our driver version as a DWORD, let's process that into something readable
	gpuData->dxDriverVersion[ 0 ] = (unsigned int) ( ( driverVersionRaw & 0xFFFF000000000000 ) >> 16 * 3 );
	gpuData->dxDriverVersion[ 1 ] = (unsigned int) ( ( driverVersionRaw & 0x0000FFFF00000000 ) >> 16 * 2 );
	gpuData->dxDriverVersion[ 2 ] = (unsigned int) ( ( driverVersionRaw & 0x00000000FFFF0000 ) >> 16 * 1 );
	gpuData->dxDriverVersion[ 3 ] = (unsigned int) ( ( driverVersionRaw & 0x000000000000FFFF ) );

	gpuData->driverInfo.driverReleaseRevision = gpuData->dxDriverVersion[2];
	gpuData->driverInfo.driverBuildNumber = gpuData->dxDriverVersion[3];

	gpuData->d3dRegistryDataAvailability = true;

	return EXIT_SUCCESS;
}

void GetDriverVersionAsCString( const GPUData* const gpuData, char* const outBuffer, size_t outBufferSize )
{
	// let's assume 4 digits max per segment
	const size_t kMaxBufferSize = (4 * 4) + 3;
	if( gpuData != nullptr && outBuffer != nullptr && outBufferSize <= kMaxBufferSize)
	{
		sprintf_s( outBuffer, outBufferSize, "%u.%u.%u.%u", gpuData->dxDriverVersion[ 0 ], gpuData->dxDriverVersion[ 1 ], gpuData->dxDriverVersion[ 2 ], gpuData->dxDriverVersion[ 3 ] );
	}
}

GPUDetect::IntelGraphicsGeneration GetIntelGraphicsGeneration( INTEL_GPU_ARCHITECTURE architecture )
{
	switch( architecture )
	{
		case IGFX_SANDYBRIDGE:
			return INTEL_GFX_GEN6;

		case IGFX_IVYBRIDGE:
		case IGFX_VALLEYVIEW:
			return INTEL_GFX_GEN7;

		case IGFX_HASWELL:
			return INTEL_GFX_GEN7_5;

		case IGFX_BROADWELL:
		case IGFX_CHERRYVIEW:
			return INTEL_GFX_GEN8;

		case IGFX_SKYLAKE:
			return INTEL_GFX_GEN9;

		case IGFX_GEMINILAKE:
		case IGFX_KABYLAKE:
		case IGFX_WHISKEYLAKE:
		case IGFX_COFFEELAKE:
		case IGFX_COMETLAKE:
			return INTEL_GFX_GEN9_5;

		case IGFX_CANNONLAKE:
			return INTEL_GFX_GEN10;

		case IGFX_LAKEFIELD:
		case IGFX_ICELAKE:
		case IGFX_ICELAKE_LP:
			return INTEL_GFX_GEN11;

		case IGFX_TIGERLAKE_LP:
		case IGFX_DG1:
		case IGFX_ROCKETLAKE:
		case IGFX_ADL: 
			return INTEL_GFX_GEN12;

		case DGFX_ACM:
			return INTEL_DGFX_ACM;

		default:
			return INTEL_GFX_GEN_UNKNOWN;
	}
}

const char * GetIntelGraphicsGenerationString( const IntelGraphicsGeneration generation)
{
	switch( generation )
	{
		case INTEL_GFX_GEN6:   return "Gen6";
		case INTEL_GFX_GEN7:   return "Gen7";
		case INTEL_GFX_GEN7_5: return "Gen7.5";
		case INTEL_GFX_GEN8:   return "Gen8";
		case INTEL_GFX_GEN9:   return "Gen9";
		case INTEL_GFX_GEN9_5: return "Gen9.5";
		case INTEL_GFX_GEN10:  return "Gen10";
		case INTEL_GFX_GEN11:  return "Gen11";
		case INTEL_GFX_GEN12:  return "Gen12 / Xe";
		case INTEL_DGFX_ACM:   return "Xe High Performance Graphics";

		case INTEL_GFX_GEN_UNKNOWN:
		default:               return "Unkown";
	}
}

int InitAdapter( IDXGIAdapter** adapter, int adapterIndex )
{
	if( adapter == nullptr || adapterIndex < 0 )
	{
		return GPUDETECT_ERROR_BAD_DATA;
	}

	//
	// We are relying on DXGI (supported on Windows Vista and later) to query
	// the adapter, so fail if it is not available.
	//
	// DXGIFactory1 is required by Windows Store Apps so try that first.
	//
	const HMODULE hDXGI = ::LoadLibrary( _T("dxgi.dll") );
	if( hDXGI == nullptr )
	{
		return GPUDETECT_ERROR_DXGI_LOAD;
	}

	typedef HRESULT( WINAPI*LPCREATEDXGIFACTORY )( REFIID riid, void** ppFactory );

	LPCREATEDXGIFACTORY pCreateDXGIFactory = (LPCREATEDXGIFACTORY) ::GetProcAddress( hDXGI, "CreateDXGIFactory1" );
	if( pCreateDXGIFactory == nullptr )
	{
		pCreateDXGIFactory = (LPCREATEDXGIFACTORY) ::GetProcAddress( hDXGI, "CreateDXGIFactory" );
		if( pCreateDXGIFactory == nullptr )
		{
			::FreeLibrary( hDXGI );
			return GPUDETECT_ERROR_DXGI_FACTORY_CREATION;
		}
	}

	//
	// We have the CreateDXGIFactory function so use it to actually create the factory and enumerate
	// through the adapters. Here, we are specifically looking for the Intel gfx adapter.
	//
	IDXGIFactory* pFactory = nullptr;
	if( FAILED( pCreateDXGIFactory( __uuidof( IDXGIFactory ), (void**) ( &pFactory ) ) ) )
	{
		::FreeLibrary( hDXGI );
		return GPUDETECT_ERROR_DXGI_FACTORY_CREATION;
	}

	if( FAILED( pFactory->EnumAdapters( adapterIndex, (IDXGIAdapter**) adapter ) ) )
	{
		pFactory->Release();
		::FreeLibrary( hDXGI );
		return GPUDETECT_ERROR_DXGI_ADAPTER_CREATION;
	}

	pFactory->Release();
	::FreeLibrary( hDXGI );
	return EXIT_SUCCESS;
}

int InitDevice( IDXGIAdapter* adapter, ID3D11Device** device )
{
	if ( device == nullptr )
	{
		return GPUDETECT_ERROR_BAD_DATA;
	}

	if( FAILED( ::D3D11CreateDevice( adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, device, nullptr, nullptr ) ) )
	{
		return GPUDETECT_ERROR_DXGI_DEVICE_CREATION;
	}

	return EXIT_SUCCESS;
}

int GetIntelDeviceInfo( IntelDeviceInfo2* deviceInfo, ID3D11Device* device )
{
	assert( deviceInfo != nullptr );
	assert( device != nullptr );

	//
	// Grab the device context from the device.
	//
	ID3D11DeviceContext* deviceContext = nullptr;
	device->GetImmediateContext( &deviceContext );

	//
	// Query the device to find the number of device dependent counters.
	//
	D3D11_COUNTER_INFO counterInfo = {};
	device->CheckCounterInfo( &counterInfo );
	if( counterInfo.LastDeviceDependentCounter == 0 )
	{
		deviceContext->Release();
		return GPUDETECT_ERROR_DXGI_BAD_COUNTER;
	}

	//
	// Search for the "Intel Device Information" counter and, if found, parse
	// it's description to determine the supported version.
	//
	D3D11_COUNTER_DESC counterDesc = {};
	int intelDeviceInfoVersion = 0;
	bool intelDeviceInfo = false;

	for( int i = D3D11_COUNTER_DEVICE_DEPENDENT_0; i <= counterInfo.LastDeviceDependentCounter; ++i )
	{
		counterDesc.Counter = static_cast<D3D11_COUNTER>( i );

		D3D11_COUNTER_TYPE counterType = {};
		UINT uiSlotsRequired = 0;
		UINT uiNameLength = 0;
		UINT uiUnitsLength = 0;
		UINT uiDescLength = 0;

		if( FAILED( device->CheckCounter( &counterDesc, &counterType, &uiSlotsRequired, nullptr, &uiNameLength, nullptr, &uiUnitsLength, nullptr, &uiDescLength ) ) )
		{
			continue;
		}

		LPSTR sName = new char[ uiNameLength ];
		LPSTR sUnits = new char[ uiUnitsLength ];
		LPSTR sDesc = new char[ uiDescLength ];

		intelDeviceInfo =
			SUCCEEDED( device->CheckCounter( &counterDesc, &counterType, &uiSlotsRequired, sName, &uiNameLength, sUnits, &uiUnitsLength, sDesc, &uiDescLength ) ) &&
			( strcmp( sName, "Intel Device Information" ) == 0 );

		if( intelDeviceInfo )
		{
			sscanf_s( sDesc, "Version %d", &intelDeviceInfoVersion );
		}

		delete[] sName;
		delete[] sUnits;
		delete[] sDesc;

		if( intelDeviceInfo )
		{
			break;
		}
	}

	//
	// Create the information counter, and query it to get the data. GetData()
	// returns a pointer to the data, not the actual data.
	//
	ID3D11Counter* counter = nullptr;
	if( !intelDeviceInfo || FAILED( device->CreateCounter( &counterDesc, &counter ) ) )
	{
		deviceContext->Release();
		return GPUDETECT_ERROR_DXGI_COUNTER_CREATION;
	}

	deviceContext->Begin( counter );
	deviceContext->End( counter );

	uintptr_t dataAddress = 0;
	if( deviceContext->GetData( counter, reinterpret_cast<void*>(&dataAddress), sizeof( dataAddress ), 0 ) != S_OK )
	{
		counter->Release();
		deviceContext->Release();
		return GPUDETECT_ERROR_DXGI_COUNTER_GET_DATA;
	}

	//
	// Copy the information into the user's structure
	//
	assert( intelDeviceInfoVersion == 1 || intelDeviceInfoVersion == 2 );
	const size_t infoSize = intelDeviceInfoVersion == 1
		? sizeof( IntelDeviceInfo1 )
		: sizeof( IntelDeviceInfo2 );
	assert( infoSize <= sizeof( *deviceInfo ) );
	memset( deviceInfo, 0, sizeof( *deviceInfo ) );
	memcpy( deviceInfo, reinterpret_cast<const void*>( dataAddress ), infoSize );

	counter->Release();
	deviceContext->Release();
	return EXIT_SUCCESS;
}

}

// #############################################################################
// Code specific to D3d12info

namespace GPUDetect
{
ENUM_BEGIN(PresetLevel)
	ENUM_ITEM(NotCompatible)
	ENUM_ITEM(Low)
	ENUM_ITEM(Medium)
	ENUM_ITEM(MediumPlus)
	ENUM_ITEM(High)
	ENUM_ITEM(Undefined)
ENUM_END(PresetLevel)
}

namespace IntelData
{

void PrintStaticParams()
{
	Print_string(L"Intel GPU Detect compiled version", INTEL_GPU_DETECT_COMPILED_VERSION);
}

void PrintAdapterData(IDXGIAdapter* adapter)
{
	ComPtr<ID3D11Device> device;
	int r = GPUDetect::InitDevice(adapter, &device);
	if(r != EXIT_SUCCESS)
		return;

	GPUDetect::GPUData gpuData = {};
	r = GPUDetect::InitExtensionInfo(&gpuData, adapter, device.Get());
	if(r != EXIT_SUCCESS)
		return;

	ScopedStructRegion region(L"Intel GPUDetect::GPUData");
	PrintVendorId(L"VendorId", gpuData.vendorID);
	Print_hex32(L"deviceID", gpuData.deviceID);
	Print_BOOL(L"isUMAArchitecture", gpuData.isUMAArchitecture ? TRUE : FALSE);
	Print_size(L"videoMemory", gpuData.videoMemory);
	Print_string(L"description", gpuData.description);
	Print_hex32(L"extensionVersion", gpuData.extensionVersion);
	Print_BOOL(L"intelExtensionAvailability", gpuData.intelExtensionAvailability ? TRUE : FALSE);

	r = GPUDetect::InitDxDriverVersion(&gpuData);
	if(r == EXIT_SUCCESS && gpuData.d3dRegistryDataAvailability)
	{
		char driverVersionStr[19] = {};
		GPUDetect::GetDriverVersionAsCString(&gpuData, driverVersionStr, _countof(driverVersionStr));
		Print_string(L"dxDriverVersion", StrToWstr(driverVersionStr, CP_ACP).c_str());
		Print_uint32(L"driverInfo.driverReleaseRevision", gpuData.driverInfo.driverReleaseRevision);
		Print_uint32(L"driverInfo.driverBuildNumber", gpuData.driverInfo.driverBuildNumber);
	}

	if(gpuData.vendorID == GPUDetect::INTEL_VENDOR_ID)
	{
		const GPUDetect::PresetLevel presetLevel = GPUDetect::GetDefaultFidelityPreset(&gpuData);
		PrintEnum(L"DefaultFidelityPreset", (uint32_t)presetLevel, GPUDetect::Enum_PresetLevel);

		r = GPUDetect::InitCounterInfo(&gpuData, device.Get());
		if(r == EXIT_SUCCESS)
		{
			string architectureStr = GPUDetect::GetIntelGPUArchitectureString(gpuData.architecture);
			if(architectureStr == "Unknown")
				architectureStr = std::format("Unknown ({})", (uint32_t)gpuData.architecture);
			Print_string(L"GPUArchitecture", StrToWstr(architectureStr.c_str(), CP_ACP).c_str());

			const GPUDetect::IntelGraphicsGeneration generation =
				GPUDetect::GetIntelGraphicsGeneration(gpuData.architecture);
			string generationStr = GPUDetect::GetIntelGraphicsGenerationString(generation);
			if(generationStr == "Unknown")
				generationStr = std::format("Unknown ({})", (uint32_t)generation);
			Print_string(L"GraphicsGeneration", StrToWstr(generationStr.c_str(), CP_ACP).c_str());

			if(gpuData.advancedCounterDataAvailability)
			{
				Print_uint32(L"euCount", gpuData.euCount);
				Print_uint32(L"packageTDP", gpuData.packageTDP, L"W");
				Print_uint32(L"maxFillRate", gpuData.maxFillRate, L"pixels/clock");
			}

			Print_uint32(L"maxFrequency", gpuData.maxFrequency, L"MHz");
			Print_uint32(L"minFrequency", gpuData.minFrequency, L"MHz");
		}
	}
}

} // namespace IntelData

#endif // #if USE_INTEL_GPUDETECT
