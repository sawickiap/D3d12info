/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2025 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#include "AmdDeviceInfoData.hpp"
#include "Printing.hpp"
#include "Utils.hpp"
#include "Json.hpp"
#include "Enums.hpp"

// Macro set by Cmake.
#if USE_AMD_DEVICE_INFO

////////////////////////////////////////////////////////////////////////////////
// PRIVATE

#include "ThirdParty/AMD_device_info/DeviceInfo.h"

// Don't forget to update to the date of the last commit when grabbing a new version!
static const wchar_t* AMD_DEVICE_INFO_COMPILED_VERSION = L"2024-09-24";

ENUM_BEGIN(GDT_HW_ASIC_TYPE)
	ENUM_ITEM(GDT_ASIC_TYPE_NONE)
	ENUM_ITEM(GDT_TAHITI_PRO)
	ENUM_ITEM(GDT_TAHITI_XT)
	ENUM_ITEM(GDT_PITCAIRN_PRO)
	ENUM_ITEM(GDT_PITCAIRN_XT)
	ENUM_ITEM(GDT_CAPEVERDE_PRO)
	ENUM_ITEM(GDT_CAPEVERDE_XT)
	ENUM_ITEM(GDT_OLAND)
	ENUM_ITEM(GDT_HAINAN)
	ENUM_ITEM(GDT_BONAIRE)
	ENUM_ITEM(GDT_HAWAII)
	ENUM_ITEM(GDT_KALINDI)
	ENUM_ITEM(GDT_SPECTRE)
	ENUM_ITEM(GDT_SPECTRE_SL)
	ENUM_ITEM(GDT_SPECTRE_LITE)
	ENUM_ITEM(GDT_SPOOKY)
	ENUM_ITEM(GDT_ICELAND)
	ENUM_ITEM(GDT_TONGA)
	ENUM_ITEM(GDT_CARRIZO)
	ENUM_ITEM(GDT_CARRIZO_EMB)
	ENUM_ITEM(GDT_FIJI)
	ENUM_ITEM(GDT_STONEY)
	ENUM_ITEM(GDT_ELLESMERE)
	ENUM_ITEM(GDT_BAFFIN)
	ENUM_ITEM(GDT_GFX8_0_4)
	ENUM_ITEM(GDT_VEGAM1)
	ENUM_ITEM(GDT_VEGAM2)
	ENUM_ITEM(GDT_GFX9_0_0)
	ENUM_ITEM(GDT_GFX9_0_2)
	ENUM_ITEM(GDT_GFX9_0_4)
	ENUM_ITEM(GDT_GFX9_0_6)
	ENUM_ITEM(GDT_GFX9_0_9)
	ENUM_ITEM(GDT_GFX9_0_A)
	ENUM_ITEM(GDT_GFX9_0_C)
	ENUM_ITEM(GDT_GFX9_4_2)
	ENUM_ITEM(GDT_GFX10_1_0)
	ENUM_ITEM(GDT_GFX10_1_0_XL)
	ENUM_ITEM(GDT_GFX10_1_2)
	ENUM_ITEM(GDT_GFX10_1_1)
	ENUM_ITEM(GDT_GFX10_3_0)
	ENUM_ITEM(GDT_GFX10_3_1)
	ENUM_ITEM(GDT_GFX10_3_2)
	ENUM_ITEM(GDT_GFX10_3_3)
	ENUM_ITEM(GDT_GFX10_3_4)
	ENUM_ITEM(GDT_GFX10_3_5)
	ENUM_ITEM(GDT_GFX11_0_0)
	ENUM_ITEM(GDT_GFX11_0_1)
	ENUM_ITEM(GDT_GFX11_0_2)
	ENUM_ITEM(GDT_GFX11_0_3)
	ENUM_ITEM(GDT_GFX11_0_3B)
	ENUM_ITEM(GDT_GFX11_5_0)
	ENUM_ITEM(GDT_LAST)
ENUM_END(GDT_HW_ASIC_TYPE)

ENUM_BEGIN(GDT_HW_GENERATION)
	ENUM_ITEM(GDT_HW_GENERATION_NONE)
	ENUM_ITEM(GDT_HW_GENERATION_NVIDIA)
	ENUM_ITEM(GDT_HW_GENERATION_INTEL)
	ENUM_ITEM(GDT_HW_GENERATION_SOUTHERNISLAND)
	ENUM_ITEM(GDT_HW_GENERATION_FIRST_AMD)
	ENUM_ITEM(GDT_HW_GENERATION_SEAISLAND)
	ENUM_ITEM(GDT_HW_GENERATION_VOLCANICISLAND)
	ENUM_ITEM(GDT_HW_GENERATION_GFX9)
	ENUM_ITEM(GDT_HW_GENERATION_GFX10)
	ENUM_ITEM(GDT_HW_GENERATION_GFX103)
	ENUM_ITEM(GDT_HW_GENERATION_GFX11)
	ENUM_ITEM(GDT_HW_GENERATION_CDNA)
	ENUM_ITEM(GDT_HW_GENERATION_CDNA2)
	ENUM_ITEM(GDT_HW_GENERATION_CDNA3)
	ENUM_ITEM(GDT_HW_GENERATION_LAST)
ENUM_END(GDT_HW_GENERATION)

static const GDT_GfxCardInfo* FindCardInfo(const AmdDeviceInfo_Initialize_RAII::DeviceId& id)
{
	for(size_t i = 0; i < gs_cardInfoSize; ++i)
	{
		const GDT_GfxCardInfo& info = gs_cardInfo[i];
		if(info.m_deviceID == id.deviceId &&
			(info.m_revID == id.revisionId || info.m_revID == REVISION_ID_ANY))
		{
			return &info;
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC

void AmdDeviceInfo_Initialize_RAII::PrintStaticParams()
{
    Print_string(L"AMD device_info compiled version", AMD_DEVICE_INFO_COMPILED_VERSION);
}

void AmdDeviceInfo_Initialize_RAII::PrintDeviceData(const DeviceId& id)
{
	const GDT_GfxCardInfo* const cardInfo = FindCardInfo(id);
	if(!cardInfo)
		return;

	{
		ScopedStructRegion region{L"AMD GDT_GfxCardInfo"};
		PrintEnum(L"asicType", cardInfo->m_asicType, Enum_GDT_HW_ASIC_TYPE);
		PrintEnum(L"generation", cardInfo->m_generation, Enum_GDT_HW_GENERATION);
		Print_BOOL(L"APU", cardInfo->m_bAPU ? TRUE : FALSE);
		Print_string(L"CALName", StrToWstr(cardInfo->m_szCALName, CP_UTF8).c_str());
        Print_string(L"MarketingName", StrToWstr(cardInfo->m_szMarketingName, CP_UTF8).c_str());
	}

	if(cardInfo->m_asicType >= 0 && cardInfo->m_asicType < gs_deviceInfoSize)
	{
		const GDT_DeviceInfo& devInfo = gs_deviceInfo[cardInfo->m_asicType];
		if(devInfo.m_deviceInfoValid)
		{
			ScopedStructRegion region(L"AMD GDT_DeviceInfo");
			Print_uint64(L"NumShaderEngines", devInfo.m_nNumShaderEngines); // Number of shader engines.
			Print_uint64(L"MaxWavePerSIMD", devInfo.m_nMaxWavePerSIMD); // Number of wave slots per SIMD.
			Print_uint64(L"ClocksPrim", devInfo.m_suClocksPrim); // Number of clocks it takes to process a primitive.
			Print_uint64(L"NumSQMaxCounters", devInfo.m_nNumSQMaxCounters); // Max number of SQ counters.
			Print_uint64(L"NumPrimPipes", devInfo.m_nNumPrimPipes); // Number of primitive pipes.
			Print_uint64(L"WaveSize", devInfo.m_nWaveSize); // Wavefront size.
			Print_uint64(L"NumSHPerSE", devInfo.m_nNumSHPerSE); // Number of shader array per Shader Engine.
			Print_uint64(L"NumCUPerSH", devInfo.m_nNumCUPerSH); // Number of compute unit per Shader Array.
			Print_uint64(L"NumSIMDPerCU", devInfo.m_nNumSIMDPerCU); // Number of SIMDs per Compute unit.
		}
	}
}

#endif // #if USE_AMD_DEVICE_INFO
