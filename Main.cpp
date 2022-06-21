#include "pch.hpp"
#include "NvApiData.hpp"
#include "Utils.hpp"
#include "Enums.hpp"
#include "Json.hpp"
#include "Printing.hpp"

// For Direct3D 12 Agility SDK
extern "C" {
    __declspec(dllexport) extern const UINT D3D12SDKVersion = 700;
    __declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\";
}

static const wchar_t* const PROGRAM_VERSION = L"0.0.2-development";

static const int PROGRAM_EXIT_SUCCESS            = 0;
static const int PROGRAM_EXIT_ERROR_INIT         = -1;
static const int PROGRAM_EXIT_ERROR_COMMAND_LINE = -2;
static const int PROGRAM_EXIT_ERROR_EXCEPTION    = -3;
static const int PROGRAM_EXIT_ERROR_NO_ADAPTER   = -4;
static const int PROGRAM_EXIT_ERROR_D3D12        = -5;

//#define AUTO_LINK_DX12    // use this on everything before Win10
#if defined(AUTO_LINK_DX12)

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

#else // #if defined(AUTO_LINK_DX12)

typedef HRESULT (WINAPI* PFN_DXGI_CREATE_FACTORY1)(REFIID riid, _COM_Outptr_ void **);

HMODULE g_DxgiLibrary = nullptr;
HMODULE g_Dx12Library = nullptr;

static const D3D_ROOT_SIGNATURE_VERSION HIGHEST_ROOT_SIGNATURE_VERSION = D3D_ROOT_SIGNATURE_VERSION_1_1;
static const D3D_FEATURE_LEVEL FEATURE_LEVELS_ARRAY[] =
{
    D3D_FEATURE_LEVEL_12_2,
    D3D_FEATURE_LEVEL_12_1,
    D3D_FEATURE_LEVEL_12_0,
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
};
static const D3D_FEATURE_LEVEL HIGHEST_FEATURE_LEVEL = D3D_FEATURE_LEVEL_12_2;

const wchar_t* DYN_LIB_DXGI = L"dxgi.dll";
const wchar_t* DYN_LIB_DX12 = L"d3d12.dll";

PFN_DXGI_CREATE_FACTORY1 g_CreateDXGIFactory1;
PFN_D3D12_CREATE_DEVICE g_D3D12CreateDevice;

#if defined(_DEBUG)
PFN_D3D12_GET_DEBUG_INTERFACE g_D3D12GetDebugInterface;
#endif

#endif // #if defined(AUTO_LINK_DX12)

static bool g_ListAdapters = false;
static bool g_PrintFormats = false;
static bool g_PrintEnums = false;

static wstring LuidToStr(LUID value)
{
    wchar_t s[64];
    swprintf_s(s, L"%08X-%08X", (uint32_t)value.HighPart, (uint32_t)value.LowPart);
    return wstring{s};
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS(const D3D12_FEATURE_DATA_D3D12_OPTIONS& options)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_D3D12_OPTIONS");
    Print_BOOL(  L"DoublePrecisionFloatShaderOps", options.DoublePrecisionFloatShaderOps);
    Print_BOOL(  L"OutputMergerLogicOp", options.OutputMergerLogicOp);
    PrintEnum(   L"MinPrecisionSupport", options.MinPrecisionSupport, Enum_D3D12_SHADER_MIN_PRECISION_SUPPORT);
    PrintEnum(   L"TiledResourcesTier", options.TiledResourcesTier, Enum_D3D12_TILED_RESOURCES_TIER);
    PrintEnum(   L"ResourceBindingTier", options.ResourceBindingTier, Enum_D3D12_RESOURCE_BINDING_TIER);
    Print_BOOL(  L"PSSpecifiedStencilRefSupported", options.PSSpecifiedStencilRefSupported);
    Print_BOOL(  L"TypedUAVLoadAdditionalFormats", options.TypedUAVLoadAdditionalFormats);
    Print_BOOL(  L"ROVsSupported", options.ROVsSupported);
    PrintEnum(   L"ConservativeRasterizationTier", options.ConservativeRasterizationTier, Enum_D3D12_CONSERVATIVE_RASTERIZATION_TIER);
    Print_uint32(L"MaxGPUVirtualAddressBitsPerResource", options.MaxGPUVirtualAddressBitsPerResource);
    Print_BOOL(  L"StandardSwizzle64KBSupported", options.StandardSwizzle64KBSupported);
    PrintEnum(   L"CrossNodeSharingTier", options.CrossNodeSharingTier, Enum_D3D12_CROSS_NODE_SHARING_TIER);
    Print_BOOL(  L"CrossAdapterRowMajorTextureSupported", options.CrossAdapterRowMajorTextureSupported);
    Print_BOOL(  L"VPAndRTArrayIndexFromAnyShaderFeedingRasterizerSupportedWithoutGSEmulation", options.VPAndRTArrayIndexFromAnyShaderFeedingRasterizerSupportedWithoutGSEmulation);
    PrintEnum(   L"ResourceHeapTier", options.ResourceHeapTier, Enum_D3D12_RESOURCE_HEAP_TIER);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_ARCHITECTURE(const D3D12_FEATURE_DATA_ARCHITECTURE& architecture)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_ARCHITECTURE");
    Print_uint32(L"NodeIndex", architecture.NodeIndex);
    Print_BOOL  (L"TileBasedRenderer", architecture.TileBasedRenderer);
    Print_BOOL  (L"UMA", architecture.UMA);
    Print_BOOL  (L"CacheCoherentUMA", architecture.CacheCoherentUMA);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_ARCHITECTURE1(const D3D12_FEATURE_DATA_ARCHITECTURE1& architecture1)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_ARCHITECTURE1");
    Print_uint32(L"NodeIndex", architecture1.NodeIndex);
    Print_BOOL  (L"TileBasedRenderer", architecture1.TileBasedRenderer);
    Print_BOOL  (L"UMA", architecture1.UMA);
    Print_BOOL  (L"CacheCoherentUMA", architecture1.CacheCoherentUMA);
    Print_BOOL  (L"IsolatedMMU", architecture1.IsolatedMMU);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_FEATURE_LEVELS(const D3D12_FEATURE_DATA_FEATURE_LEVELS& featureLevels)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_FEATURE_LEVELS");
    Print_uint32(L"NumFeatureLevels", featureLevels.NumFeatureLevels);
#if 0//TODO
    BeginArray();
    for(uint32_t i = 0; i < featureLevels.NumFeatureLevels; ++i)
    {
        PrintEnum(L"pFeatureLevelsRequested", featureLevels.pFeatureLevelsRequested[i], Enum_D3D_FEATURE_LEVEL);
        StepArray();
    }
    EndArray();
#endif
    PrintEnum(L"MaxSupportedFeatureLevel", featureLevels.MaxSupportedFeatureLevel, Enum_D3D_FEATURE_LEVEL);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT(const D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT& virtualAddressSupport)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT");
    Print_uint32(L"MaxGPUVirtualAddressBitsPerResource", virtualAddressSupport.MaxGPUVirtualAddressBitsPerResource);
    Print_uint32(L"MaxGPUVirtualAddressBitsPerProcess", virtualAddressSupport.MaxGPUVirtualAddressBitsPerProcess);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_SHADER_MODEL(const D3D12_FEATURE_DATA_SHADER_MODEL& shaderModel)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_SHADER_MODEL");
    PrintEnum(L"HighestShaderModel", shaderModel.HighestShaderModel, Enum_D3D_SHADER_MODEL);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS1(const D3D12_FEATURE_DATA_D3D12_OPTIONS1& options1)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_D3D12_OPTIONS1");
    Print_BOOL  (L"WaveOps", options1.WaveOps);
    Print_uint32(L"WaveLaneCountMin", options1.WaveLaneCountMin);
    Print_uint32(L"WaveLaneCountMax", options1.WaveLaneCountMax);
    Print_uint32(L"TotalLaneCount", options1.TotalLaneCount);
    Print_BOOL  (L"ExpandedComputeResourceStates", options1.ExpandedComputeResourceStates);
    Print_BOOL  (L"Int64ShaderOps", options1.Int64ShaderOps);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_ROOT_SIGNATURE(const D3D12_FEATURE_DATA_ROOT_SIGNATURE& rootSignature)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_ROOT_SIGNATURE");
    PrintEnum(L"HighestVersion", rootSignature.HighestVersion, Enum_D3D_ROOT_SIGNATURE_VERSION);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS2(const D3D12_FEATURE_DATA_D3D12_OPTIONS2& options2)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_D3D12_OPTIONS2");
    Print_BOOL(L"DepthBoundsTestSupported", options2.DepthBoundsTestSupported);
    PrintEnum (L"ProgrammableSamplePositionsTier", options2.ProgrammableSamplePositionsTier, Enum_D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_SHADER_CACHE(const D3D12_FEATURE_DATA_SHADER_CACHE& shaderCache)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_SHADER_CACHE");
    PrintFlags(L"SupportFlags", shaderCache.SupportFlags, Enum_D3D12_SHADER_CACHE_SUPPORT_FLAGS);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS3(const D3D12_FEATURE_DATA_D3D12_OPTIONS3& options3)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_D3D12_OPTIONS3");
    Print_BOOL(L"CopyQueueTimestampQueriesSupported", options3.CopyQueueTimestampQueriesSupported);
    Print_BOOL(L"CastingFullyTypedFormatSupported", options3.CastingFullyTypedFormatSupported);
    PrintFlags(L"WriteBufferImmediateSupportFlags", options3.WriteBufferImmediateSupportFlags, Enum_D3D12_COMMAND_LIST_SUPPORT_FLAGS);
    PrintEnum (L"ViewInstancingTier", options3.ViewInstancingTier, Enum_D3D12_VIEW_INSTANCING_TIER);
    Print_BOOL(L"BarycentricsSupported", options3.BarycentricsSupported);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS4(const D3D12_FEATURE_DATA_D3D12_OPTIONS4& options4)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_D3D12_OPTIONS4");
    Print_BOOL(L"MSAA64KBAlignedTextureSupported", options4.MSAA64KBAlignedTextureSupported);
    PrintEnum(L"SharedResourceCompatibilityTier", options4.SharedResourceCompatibilityTier, Enum_D3D12_SHARED_RESOURCE_COMPATIBILITY_TIER);
    Print_BOOL(L"Native16BitShaderOpsSupported", options4.Native16BitShaderOpsSupported);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS5(const D3D12_FEATURE_DATA_D3D12_OPTIONS5& options5)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_D3D12_OPTIONS5");
    Print_BOOL(L"SRVOnlyTiledResourceTier3", options5.SRVOnlyTiledResourceTier3);
    PrintEnum(L"RenderPassesTier", options5.RenderPassesTier, Enum_D3D12_RENDER_PASS_TIER);
    PrintEnum(L"RaytracingTier", options5.RaytracingTier, Enum_D3D12_RAYTRACING_TIER);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS6(const D3D12_FEATURE_DATA_D3D12_OPTIONS6& o)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_D3D12_OPTIONS6");
    Print_BOOL(L"AdditionalShadingRatesSupported", o.AdditionalShadingRatesSupported);
    Print_BOOL(L"PerPrimitiveShadingRateSupportedWithViewportIndexing", o.PerPrimitiveShadingRateSupportedWithViewportIndexing);
    PrintEnum(L"VariableShadingRateTier", o.VariableShadingRateTier, Enum_D3D12_VARIABLE_SHADING_RATE_TIER);
    Print_uint32(L"ShadingRateImageTileSize", o.ShadingRateImageTileSize);
    Print_BOOL(L"BackgroundProcessingSupported", o.BackgroundProcessingSupported);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS7(const D3D12_FEATURE_DATA_D3D12_OPTIONS7& o)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_D3D12_OPTIONS7");
    PrintEnum(L"MeshShaderTier", o.MeshShaderTier, Enum_D3D12_MESH_SHADER_TIER);
    PrintEnum(L"SamplerFeedbackTier", o.SamplerFeedbackTier, Enum_D3D12_SAMPLER_FEEDBACK_TIER);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS8(const D3D12_FEATURE_DATA_D3D12_OPTIONS8& o)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_D3D12_OPTIONS8");
    Print_BOOL(L"UnalignedBlockTexturesSupported", o.UnalignedBlockTexturesSupported);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS9(const D3D12_FEATURE_DATA_D3D12_OPTIONS9& o)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_D3D12_OPTIONS9");
    Print_BOOL(L"MeshShaderPipelineStatsSupported", o.MeshShaderPipelineStatsSupported);
    Print_BOOL(L"MeshShaderSupportsFullRangeRenderTargetArrayIndex", o.MeshShaderSupportsFullRangeRenderTargetArrayIndex);
    Print_BOOL(L"AtomicInt64OnTypedResourceSupported", o.AtomicInt64OnTypedResourceSupported);
    Print_BOOL(L"AtomicInt64OnGroupSharedSupported", o.AtomicInt64OnGroupSharedSupported);
    Print_BOOL(L"DerivativesInMeshAndAmplificationShadersSupported", o.DerivativesInMeshAndAmplificationShadersSupported);
    PrintEnum(L"WaveMMATier", o.WaveMMATier, Enum_D3D12_WAVE_MMA_TIER);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS10(const D3D12_FEATURE_DATA_D3D12_OPTIONS10& o)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_D3D12_OPTIONS10");
    Print_BOOL(L"VariableRateShadingSumCombinerSupported", o.VariableRateShadingSumCombinerSupported);
    Print_BOOL(L"MeshShaderPerPrimitiveShadingRateSupported", o.MeshShaderPerPrimitiveShadingRateSupported);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS11(const D3D12_FEATURE_DATA_D3D12_OPTIONS11& o)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_D3D12_OPTIONS11");
    Print_BOOL(L"AtomicInt64OnDescriptorHeapResourceSupported", o.AtomicInt64OnDescriptorHeapResourceSupported);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS12(const D3D12_FEATURE_DATA_D3D12_OPTIONS12& o)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_D3D12_OPTIONS12");
    PrintEnum(L"MSPrimitivesPipelineStatisticIncludesCulledPrimitives", o.MSPrimitivesPipelineStatisticIncludesCulledPrimitives, Enum_D3D12_TRI_STATE);
    Print_BOOL(L"EnhancedBarriersSupported", o.EnhancedBarriersSupported);
    PrintStructEnd();
}

static void Print_D3D12_FEATURE_DATA_EXISTING_HEAPS(const D3D12_FEATURE_DATA_EXISTING_HEAPS& existingHeaps)
{
    PrintStructBegin(L"D3D12_FEATURE_DATA_EXISTING_HEAPS");
    Print_BOOL(L"Supported", existingHeaps.Supported);
    PrintStructEnd();
}

static void Print_DXGI_QUERY_VIDEO_MEMORY_INFO(const DXGI_QUERY_VIDEO_MEMORY_INFO& videoMemoryInfo)
{
    // Not printing videoMemoryInfo.CurrentUsage, videoMemoryInfo.CurrentReservation.
    Print_size(L"Budget", videoMemoryInfo.Budget);
    Print_size(L"AvailableForReservation", videoMemoryInfo.AvailableForReservation);
}

static wstring MakeBuildDateTime()
{
    wchar_t s[128];
    swprintf_s(s, L"%hs, %hs", __DATE__, __TIME__);
    return wstring{s};
}

static void PrintHeader_Text()
{
    wprintf(L"============================\n");
    wprintf(L"D3D12INFO %s\n", PROGRAM_VERSION);
    wprintf(L"Built: %s\n", MakeBuildDateTime().c_str());
    wprintf(L"============================\n");
    PrintEmptyLine();
}

static void PrintHeader_Json()
{
    Json::WriteString(L"Header");
    Json::BeginObject();

    Json::WriteString(L"Program");
    Json::WriteString(L"D3D12INFO");
    Json::WriteString(L"Version");
    Json::WriteString(PROGRAM_VERSION);
    Json::WriteString(L"Built");
    Json::WriteString(MakeBuildDateTime());

    Json::EndObject();
}

static void PrintHeaderData()
{
    if(g_UseJson)
        PrintHeader_Json();
    else
        PrintHeader_Text();
}

static wstring MakeCurrentDate()
{
    std::time_t time = std::time(nullptr);
    std::tm time2;
    localtime_s(&time2, &time);
    wchar_t dateStr[32];
    std::wcsftime(dateStr, _countof(dateStr), L"%Y-%m-%d", &time2);
    return wstring{dateStr};
}

static void PrintGeneralParams(NvAPI_Inititalize_RAII* nvAPI)
{
    Print_string(L"Current date", MakeCurrentDate().c_str());
    Print_uint32(L"D3D12SDKVersion", uint32_t(D3D12SDKVersion));

#if USE_NVAPI
    if(nvAPI->IsInitialized())
        nvAPI->PrintGeneralParams();
#endif
}

static void PrintGeneralData(NvAPI_Inititalize_RAII* nvAPI)
{
    if(g_UseJson)
    {
        Json::WriteString(L"General");
        Json::BeginObject();
        PrintGeneralParams(nvAPI);
        Json::EndObject();
    }
    else
    {
        PrintHeader(L"General", 0);
        ++g_Indent;
        PrintGeneralParams(nvAPI);
        --g_Indent;
        PrintEmptyLine();
    }
}

static void PrintEnums_Json()
{
    Json::WriteString(L"Enums");
    Json::BeginObject();

    EnumCollection& enumCollection = EnumCollection::GetInstance();
    for(const auto it : enumCollection.m_Enums)
    {
        Json::WriteString(it.first.data(), it.first.length());
        Json::BeginObject();

        for(const EnumItem* item = it.second; item->m_Name != nullptr; ++item)
        {
            Json::WriteString(item->m_Name);
            Json::WriteNumber(item->m_Value);
        }

        Json::EndObject();
    }

    Json::EndObject();
}

static void PrintEnums_Text()
{
    PrintHeader(L"Enums", 0);
    PrintEmptyLine();

    EnumCollection& enumCollection = EnumCollection::GetInstance();
    for(const auto it : enumCollection.m_Enums)
    {
        PrintHeader(it.first.c_str(), 1);

        for(const EnumItem* item = it.second; item->m_Name != nullptr; ++item)
            Print_uint32(item->m_Name, item->m_Value);

        PrintEmptyLine();
    }
}

static void PrintOsVersionInfo()
{
    HMODULE m = GetModuleHandle(L"ntdll.dll");
    if(!m)
        return;
    typedef int32_t(WINAPI* RtlGetVersionFunc)(OSVERSIONINFOEX*);
    RtlGetVersionFunc RtlGetVersion = (RtlGetVersionFunc)GetProcAddress(m, "RtlGetVersion");
    if(!RtlGetVersion)
        return;
    OSVERSIONINFOEX osVersionInfo = {sizeof(osVersionInfo)};
    // Documentation says it always returns success.
    RtlGetVersion(&osVersionInfo);

    PrintStructBegin(L"OSVERSIONINFOEX");
    Print_uint32(L"dwMajorVersion", osVersionInfo.dwMajorVersion);
    Print_uint32(L"dwMinorVersion", osVersionInfo.dwMinorVersion);
    Print_uint32(L"dwBuildNumber", osVersionInfo.dwBuildNumber);
    Print_uint32(L"dwPlatformId", osVersionInfo.dwPlatformId);
    Print_string(L"szCSDVersion", osVersionInfo.szCSDVersion);
    Print_uint32(L"wServicePackMajor", uint32_t(osVersionInfo.wServicePackMajor));
    Print_uint32(L"wServicePackMinor", uint32_t(osVersionInfo.wServicePackMinor));
    PrintFlags(L"wSuiteMask", uint32_t(osVersionInfo.wSuiteMask), Enum_VER_SUITE);
    PrintEnum(L"wProductType", uint32_t(osVersionInfo.wProductType), Enum_VER_SUITE);
    PrintStructEnd();
}

static void PrintEnumsData()
{
    if(g_UseJson)
        PrintEnums_Json();
    else
        PrintEnums_Text();
}

static void PrintAdapterDesc1(const DXGI_ADAPTER_DESC1& desc1)
{
    PrintStructBegin(L"DXGI_ADAPTER_DESC1");
    Print_string(L"Description", desc1.Description);
    PrintEnum   (L"VendorId", desc1.VendorId, Enum_VendorId);
    Print_hex32 (L"DeviceId", desc1.DeviceId);
    Print_hex32 (L"SubSysId", desc1.SubSysId);
    Print_hex32 (L"Revision", desc1.Revision);
    Print_size  (L"DedicatedVideoMemory", desc1.DedicatedVideoMemory);
    Print_size  (L"DedicatedSystemMemory", desc1.DedicatedSystemMemory);
    Print_size  (L"SharedSystemMemory", desc1.SharedSystemMemory);
    Print_string(L"AdapterLuid", LuidToStr(desc1.AdapterLuid).c_str());
    PrintFlags  (L"Flags", desc1.Flags, Enum_DXGI_ADAPTER_FLAG);
    PrintStructEnd();
}

static void PrintAdapterDesc2(const DXGI_ADAPTER_DESC2& desc2)
{
    PrintStructBegin(L"DXGI_ADAPTER2_DESC2");
    PrintEnum(L"GraphicsPreemptionGranularity", desc2.GraphicsPreemptionGranularity, Enum_DXGI_GRAPHICS_PREEMPTION_GRANULARITY);
    PrintEnum(L"ComputePreemptionGranularity", desc2.ComputePreemptionGranularity, Enum_DXGI_COMPUTE_PREEMPTION_GRANULARITY);
    PrintStructEnd();
}

#if 0
static void PrintAdapterDesc3(const DXGI_ADAPTER_DESC3& desc3)
{
    PrintHeader(L"DXGI_ADAPTER2_DESC3", 1);
    ++g_Indent;
    // There are no members, makes no sense to print it.
    --g_Indent;
    PrintEmptyLine();
}
#endif

static void PrintAdapterData(IDXGIAdapter1* adapter1)
{
    assert(adapter1 != nullptr);

    DXGI_ADAPTER_DESC1 desc1 = {};
    if(SUCCEEDED(adapter1->GetDesc1(&desc1)))
        PrintAdapterDesc1(desc1);

    {
        ComPtr<IDXGIAdapter2> adapter2;
        if(SUCCEEDED(adapter1->QueryInterface(IID_PPV_ARGS(&adapter2))))
        {
            DXGI_ADAPTER_DESC2 desc2;
            if(SUCCEEDED(adapter2->GetDesc2(&desc2)))
                PrintAdapterDesc2(desc2);
        }
    }

    {
        ComPtr<IDXGIAdapter3> adapter3;
        if(SUCCEEDED(adapter1->QueryInterface<IDXGIAdapter3>(&adapter3)))
        {
            for(uint32_t memorySegmentGroup = 0; memorySegmentGroup < 2; ++memorySegmentGroup)
            {
                DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo = {};
                if(SUCCEEDED(adapter3->QueryVideoMemoryInfo(0, (DXGI_MEMORY_SEGMENT_GROUP)memorySegmentGroup, &videoMemoryInfo)))
                {
                    switch(memorySegmentGroup)
                    {
                    case 0:
                        PrintStructBegin(L"DXGI_QUERY_VIDEO_MEMORY_INFO[DXGI_MEMORY_SEGMENT_GROUP_LOCAL]");
                        break;
                    case 1:
                        PrintStructBegin(L"DXGI_QUERY_VIDEO_MEMORY_INFO[DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL]");
                        break;
                    default:
                        assert(0);
                    }
                    Print_DXGI_QUERY_VIDEO_MEMORY_INFO(videoMemoryInfo);
                    PrintStructEnd();
                }
            }
        }
    }

#if 0
    {
        ComPtr<IDXGIAdapter4> adapter4;
        if(SUCCEEDED(adapter4->QueryInterface(IID_PPV_ARGS(&adapter4))))
        {
            DXGI_ADAPTER_DESC3 desc3;
            if(SUCCEEDED(adapter4->GetDesc2(&desc3)))
                PrintAdapterDesc3(desc3);
        }
    }
#endif
}

static void PrintFormatInformation(ID3D12Device* device)
{
    if(g_UseJson)
    {
        Json::WriteString(L"Formats");
        Json::BeginObject();
    }
    else
        PrintHeader(L"Formats", 1);

    D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport = {};
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels = {};
    D3D12_FEATURE_DATA_FORMAT_INFO formatInfo = {};
    for(size_t formatIndex = 0; Enum_DXGI_FORMAT[formatIndex].m_Name != nullptr; ++formatIndex)
    {
        const DXGI_FORMAT format = (DXGI_FORMAT)Enum_DXGI_FORMAT[formatIndex].m_Value;

        if(g_UseJson)
        {
            Json::WriteString(std::format(L"{}", formatIndex));
            Json::BeginObject();
        }
        else
        {
            PrintIndent();
            wprintf(L"%s:\n", Enum_DXGI_FORMAT[formatIndex].m_Name);
            ++g_Indent;
        }

        formatSupport.Format = format;
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, UINT(sizeof formatSupport))))
        {
            PrintFlags(L"Support1", formatSupport.Support1, Enum_D3D12_FORMAT_SUPPORT1);
            PrintFlags(L"Support2", formatSupport.Support1, Enum_D3D12_FORMAT_SUPPORT2);
        }

        {
            if(g_UseJson)
            {
                Json::WriteString(L"MultisampleQualityLevels");
                Json::BeginObject();
            }
            msQualityLevels.Format = format;
            for(msQualityLevels.SampleCount = 1; ; msQualityLevels.SampleCount *= 2)
            {
                if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, UINT(sizeof msQualityLevels))) &&
                    msQualityLevels.NumQualityLevels > 0)
                {
                    if(g_UseJson)
                    {
                        Json::WriteString(std::format(L"{}", msQualityLevels.SampleCount));
                        Json::BeginObject();

                        Json::WriteString(L"NumQualityLevels");
                        Json::WriteNumber(msQualityLevels.NumQualityLevels);
                        Json::WriteString(L"Flags");
                        Json::WriteNumber(uint32_t(msQualityLevels.Flags));

                        Json::EndObject();
                    }
                    else
                    {
                        PrintIndent();
                        wprintf(L"SampleCount = %u: NumQualityLevels = %u", msQualityLevels.SampleCount, msQualityLevels.NumQualityLevels);
                        if((msQualityLevels.Flags & D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_TILED_RESOURCE) != 0)
                            wprintf(L" D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_TILED_RESOURCE\n");
                        else
                            wprintf(L"\n");
                    }
                }
                else
                    break;
            }
            if(g_UseJson)
                Json::EndObject();
        }

        formatInfo.Format = format;
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_INFO, &formatInfo, UINT(sizeof formatInfo))))
        {
            Print_uint32(L"PlaneCount", formatInfo.PlaneCount);
        }

        if(g_UseJson)
            Json::EndObject();
        else
            --g_Indent;
    }
    
    if(g_UseJson)
        Json::EndObject();
    else
        PrintEmptyLine();
}

static int PrintDeviceDetails(IDXGIAdapter1* adapter1, NvAPI_Inititalize_RAII* nvAPI)
{
    ComPtr<ID3D12Device> device;
#if defined(AUTO_LINK_DX12)
    CHECK_HR( ::D3D12CreateDevice(adapter1, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device)) );
#else
    CHECK_HR( g_D3D12CreateDevice(adapter1, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device)) );
#endif
    if (!device)
        return PROGRAM_EXIT_ERROR_D3D12;
     
    {
        D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options))))
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS(options);
    }

    {
        D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT gpuVirtualAddressSupport = {};
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT, &gpuVirtualAddressSupport, sizeof(gpuVirtualAddressSupport))))
            Print_D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT(gpuVirtualAddressSupport);
    }

    /*
    Microsoft documentation says:

    ID3D12Device::CheckFeatureSupport returns E_INVALIDARG if HighestShaderModel
    isn't known by the current runtime. For that reason, we recommend that you call
    this in a loop with decreasing shader models to determine the highest supported
    shader model.
    */
    {
        D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = {};
        for(size_t enumItemIndex = _countof(Enum_D3D_SHADER_MODEL) - 1; enumItemIndex--; )
        {
            shaderModel.HighestShaderModel = D3D_SHADER_MODEL(Enum_D3D_SHADER_MODEL[enumItemIndex].m_Value);
            if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel))))
            {
                Print_D3D12_FEATURE_DATA_SHADER_MODEL(shaderModel);
                break;
            }
        }
    }

    {
        D3D12_FEATURE_DATA_D3D12_OPTIONS1 options1 = {};
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS1, &options1, sizeof(options1))))
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS1(options1);
    }

    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignature = {};
        rootSignature.HighestVersion = HIGHEST_ROOT_SIGNATURE_VERSION;
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rootSignature, sizeof(rootSignature))))
            Print_D3D12_FEATURE_DATA_ROOT_SIGNATURE(rootSignature);
    }

    {
        D3D12_FEATURE_DATA_ARCHITECTURE1 architecture1 = {};
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE1, &architecture1, sizeof(architecture1))))
            Print_D3D12_FEATURE_DATA_ARCHITECTURE1(architecture1);
        else
        {
            D3D12_FEATURE_DATA_ARCHITECTURE architecture = {};
            if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE, &architecture, sizeof(architecture))))
                Print_D3D12_FEATURE_DATA_ARCHITECTURE(architecture);
        }
    }

    {
        D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevels =
        {
            _countof(FEATURE_LEVELS_ARRAY), FEATURE_LEVELS_ARRAY, HIGHEST_FEATURE_LEVEL
        };
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevels, sizeof(featureLevels))))
            Print_D3D12_FEATURE_DATA_FEATURE_LEVELS(featureLevels);
    }

    {
        D3D12_FEATURE_DATA_D3D12_OPTIONS2 options2 = {};
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS2, &options2, sizeof(options2))))
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS2(options2);
    }

    {
        D3D12_FEATURE_DATA_SHADER_CACHE shaderCache = {};
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_SHADER_CACHE, &shaderCache, sizeof(shaderCache))))
            Print_D3D12_FEATURE_DATA_SHADER_CACHE(shaderCache);
    }

    /*
    D3D12_FEATURE_DATA_COMMAND_QUEUE_PRIORITY commandQueuePriority = {};
    // There are IN parameters - need to fill in commandQueuePriority.CommandListType, Priority...
    hr = device->CheckFeatureSupport(D3D12_FEATURE_COMMAND_QUEUE_PRIORITY, &commandQueuePriority, sizeof(commandQueuePriority));
    if(SUCCEEDED(hr))
    {
        PrintStructBegin(L"D3D12_FEATURE_DATA_COMMAND_QUEUE_PRIORITY");
        Print_D3D12_FEATURE_DATA_COMMAND_QUEUE_PRIORITY(commandQueuePriority);
        PrintStructEnd();
    }
    */
        
    {
        D3D12_FEATURE_DATA_D3D12_OPTIONS3 options3 = {};
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS3, &options3, sizeof(options3))))
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS3(options3);
    }

    {
        D3D12_FEATURE_DATA_EXISTING_HEAPS existingHeaps = {};
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_EXISTING_HEAPS, &existingHeaps, sizeof(existingHeaps))))
            Print_D3D12_FEATURE_DATA_EXISTING_HEAPS(existingHeaps);
    }

    {
        D3D12_FEATURE_DATA_D3D12_OPTIONS4 options4 = {};
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS4, &options4, sizeof(options4))))
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS4(options4);
    }

    {
        D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {};
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5))))
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS5(options5);
    }

    {
        D3D12_FEATURE_DATA_D3D12_OPTIONS6 options6 = {};
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &options6, sizeof(options6))))
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS6(options6);
    }

    {
        D3D12_FEATURE_DATA_D3D12_OPTIONS7 options7 = {};
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &options7, sizeof(options7))))
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS7(options7);
    }

    {
        D3D12_FEATURE_DATA_D3D12_OPTIONS8 options8 = {};
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS8, &options8, sizeof(options8))))
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS8(options8);
    }

    {
        D3D12_FEATURE_DATA_D3D12_OPTIONS9 options9 = {};
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS9, &options9, sizeof(options9))))
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS9(options9);
    }

    {
        D3D12_FEATURE_DATA_D3D12_OPTIONS10 options10 = {};
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS10, &options10, sizeof(options10))))
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS10(options10);
    }

    {
        D3D12_FEATURE_DATA_D3D12_OPTIONS11 options11 = {};
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS11, &options11, sizeof(options11))))
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS11(options11);
    }

    {
        D3D12_FEATURE_DATA_D3D12_OPTIONS12 options12 = {};
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS12, &options12, sizeof(options12))))
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS12(options12);
    }

#if USE_NVAPI
    if(nvAPI && nvAPI->IsInitialized())
    {
        nvAPI->PrintD3d12DeviceData(device.Get());

        DXGI_ADAPTER_DESC1 desc1 = {};
        if(SUCCEEDED(adapter1->GetDesc1(&desc1)))
            nvAPI->PrintPhysicalGpuData(desc1.AdapterLuid);
    }
#endif

    if(g_PrintFormats)
        PrintFormatInformation(device.Get());

    return PROGRAM_EXIT_SUCCESS;
}

#if !defined(AUTO_LINK_DX12)

static bool LoadLibraries()
{
    g_DxgiLibrary = ::LoadLibraryEx(DYN_LIB_DXGI, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_DxgiLibrary)
    {
        wprintf(L"could not load %s\n", DYN_LIB_DXGI);
        return false;
    }

    g_Dx12Library = ::LoadLibraryEx(DYN_LIB_DX12, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_Dx12Library)
    {
        wprintf(L"could not load %s\n", DYN_LIB_DX12);
        return false;
    }

    g_CreateDXGIFactory1 = reinterpret_cast<PFN_DXGI_CREATE_FACTORY1>(::GetProcAddress(g_DxgiLibrary, "CreateDXGIFactory1"));
    if (!g_CreateDXGIFactory1)
    {
        return false;
    }

    g_D3D12CreateDevice = reinterpret_cast<PFN_D3D12_CREATE_DEVICE>(::GetProcAddress(g_Dx12Library, "D3D12CreateDevice"));
    if (!g_D3D12CreateDevice)
    {
        return false;
    }

#if defined(_DEBUG)
    g_D3D12GetDebugInterface = reinterpret_cast<PFN_D3D12_GET_DEBUG_INTERFACE>(::GetProcAddress(g_Dx12Library, "D3D12GetDebugInterface"));
    if (!g_D3D12GetDebugInterface)
    {
        return false;
    }
#endif

    return true;
}

static void UnloadLibraries()
{
    g_CreateDXGIFactory1 = nullptr;
    g_D3D12CreateDevice = nullptr;

#if defined(_DEBUG)
    g_D3D12GetDebugInterface = nullptr;
#endif

    BOOL rc;

    rc = ::FreeLibrary(g_DxgiLibrary);
    assert(rc);
    g_DxgiLibrary = nullptr;

    rc = ::FreeLibrary(g_Dx12Library);
    assert(rc);
    g_Dx12Library = nullptr;
}

#endif

#if defined(_DEBUG)

ComPtr<ID3D12Debug> EnableDebugLayer()
{
    HRESULT hr;
    ComPtr<ID3D12Debug> debugController = nullptr;

#if defined(AUTO_LINK_DX12)
    hr = ::D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
#else
    hr = g_D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
#endif

    if (SUCCEEDED(hr))
    {
        assert(debugController != nullptr);
        debugController->EnableDebugLayer();
    }

    return debugController;
}

#endif // #if defined(_DEBUG)

static void PrintCommandLineSyntax()
{
    wprintf(L"Options:\n");
    wprintf(L"  -v --Version         Only print program version information.\n");
    wprintf(L"  -h --Help            Only print this help (command line syntax).\n");
    wprintf(L"  -l --List            Only print the list of all adapters.\n");
    wprintf(L"  -a --Adapter=<Index> Print details of adapter at specified index (default is the first hardware adapter).\n");
    wprintf(L"  -j --JSON            Print output in JSON format instead of human-friendly text.\n");
    wprintf(L"  -f --Formats         Include information about DXGI format capabilities.\n");
    wprintf(L"  -e --Enums           Include information about all known enums and their values.\n");
}

int wmain2(int argc, wchar_t** argv)
{
    UINT adapterIndex = UINT32_MAX;

    CmdLineParser cmdLineParser(argc, argv);

    enum CMD_LINE_PARAM
    {
        CMD_LINE_OPT_VERSION,
        CMD_LINE_OPT_HELP,
        CMD_LINE_OPT_LIST,
        CMD_LINE_OPT_ADAPTER,
        CMD_LINE_OPT_JSON,
        CMD_LINE_OPT_FORMATS,
        CMD_LINE_OPT_ENUMS,
    };

    cmdLineParser.RegisterOpt(CMD_LINE_OPT_VERSION, L"Version", false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_VERSION, L'v', false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_HELP,    L"Help", false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_HELP,    L'h', false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_LIST,    L"List", false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_LIST,    L'l', false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_ADAPTER, L"Adapter", true);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_ADAPTER, L'a', true);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_JSON,    L"JSON", false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_JSON,    L'j', false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_FORMATS, L"Formats", false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_FORMATS, L'f', false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_ENUMS,   L"Enums", false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_ENUMS,   L'e', false);

    CmdLineParser::RESULT cmdLineResult;
    while((cmdLineResult = cmdLineParser.ReadNext()) != CmdLineParser::RESULT_END)
    {
        switch(cmdLineResult)
        {
        case CmdLineParser::RESULT_ERROR:
        case CmdLineParser::RESULT_PARAMETER:
            PrintCommandLineSyntax();
            return PROGRAM_EXIT_ERROR_COMMAND_LINE;
        case CmdLineParser::RESULT_OPT:
            switch(cmdLineParser.GetOptId())
            {
            case CMD_LINE_OPT_VERSION:
                return PROGRAM_EXIT_SUCCESS;
            case CMD_LINE_OPT_HELP:
                PrintCommandLineSyntax();
                return PROGRAM_EXIT_SUCCESS;
            case CMD_LINE_OPT_LIST:
                g_ListAdapters = true;
                break;
            case CMD_LINE_OPT_ADAPTER:
                adapterIndex = _wtoi(cmdLineParser.GetParameter().c_str());
                break;
            case CMD_LINE_OPT_JSON:
                g_UseJson = true;
                break;
            case CMD_LINE_OPT_FORMATS:
                g_PrintFormats = true;
                break;
            case CMD_LINE_OPT_ENUMS:
                g_PrintEnums = true;
                break;
            default:
                PrintCommandLineSyntax();
                return PROGRAM_EXIT_ERROR_COMMAND_LINE;
            }
            break;
        default:
            assert(0);
        }
    }

    if(g_UseJson)
        Json::Begin();

    PrintHeaderData();

#if !defined(AUTO_LINK_DX12)
    if (!LoadLibraries())
    {
        wprintf(L"ERROR: Could not load DXGI & D3D12 libraries.\n");
        return PROGRAM_EXIT_ERROR_INIT;
    }
#endif

#if USE_NVAPI
    NvAPI_Inititalize_RAII nvApiInitializeObj;
    NvAPI_Inititalize_RAII* nvApiObjPtr = &nvApiInitializeObj;
#else
    NvAPI_Inititalize_RAII* nvApiObjPtr = nullptr;
#endif

    PrintGeneralData(nvApiObjPtr);

#if USE_NVAPI
    if(nvApiInitializeObj.IsInitialized())
        nvApiInitializeObj.PrintData();
#endif

    PrintOsVersionInfo();

    if(g_PrintEnums)
        PrintEnumsData();

    int programResult = PROGRAM_EXIT_SUCCESS;

    // Scope for COM objects.
    {
#if defined(_DEBUG)
        ComPtr<ID3D12Debug> debugController = EnableDebugLayer();
#endif

        ComPtr<IDXGIFactory4> dxgiFactory = nullptr;
#if defined(AUTO_LINK_DX12)
        CHECK_HR( ::CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) );
#else
        CHECK_HR( g_CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) );
#endif
        assert(dxgiFactory != nullptr);

        if(g_ListAdapters)
        {
            if(g_UseJson)
            {
                Json::WriteString(L"Adapters");
                Json::BeginArray();
            }

            ComPtr<IDXGIAdapter1> currAdapter;
            UINT currAdapterIndex = 0;
            while(dxgiFactory->EnumAdapters1(currAdapterIndex, &currAdapter) != DXGI_ERROR_NOT_FOUND)
            {
                if(g_UseJson)
                    Json::BeginObject();
                else
                {
                    PrintHeader(std::format(L"DXGI Adapter {}", currAdapterIndex).c_str(), 0);
                    PrintEmptyLine();
                }

                PrintAdapterData(currAdapter.Get());

                if(g_UseJson)
                    Json::EndObject();

                currAdapter.Reset();
                ++currAdapterIndex;
            }

            if(g_UseJson)
                Json::EndArray();
        }
        else
        {
            ComPtr<IDXGIAdapter1> adapter;
            if(adapterIndex != UINT32_MAX)
                dxgiFactory->EnumAdapters1(adapterIndex, &adapter);
            else
            {
                // No explicit adapter requested: Choose first non-software and non-remote.
                adapterIndex = 0;
                DXGI_ADAPTER_DESC1 desc = {};
                while(dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
                {
                    adapter->GetDesc1(&desc);
                    if(desc.Flags == 0)
                        break;
                    adapter.Reset();
                    ++adapterIndex;
                }
            }

            wstring adapterStr = std::format(L"DXGI Adapter {}", adapterIndex);
            if(adapter)
            {
                if(g_UseJson)
                {
                    Json::WriteString(std::move(adapterStr));
                    Json::BeginObject();
                }
                else
                {
                    PrintHeader(adapterStr.c_str(), 0);
                    PrintEmptyLine();
                }

                PrintAdapterData(adapter.Get());
                
                programResult = PrintDeviceDetails(adapter.Get(), nvApiObjPtr);

                if(g_UseJson)
                    Json::EndObject();
            }
            else
            {
                programResult = PROGRAM_EXIT_ERROR_NO_ADAPTER;
                wprintf(L"ERROR: No valid adapter chosen to show D3D12 device details.\n");
            }
        }
    }

#if !defined(AUTO_LINK_DX12)
    UnloadLibraries();
#endif

    if(g_UseJson && programResult == PROGRAM_EXIT_SUCCESS)
    {
        wstring json = Json::End();
        wprintf(L"%.*s", (int)json.length(), json.data());
    }

    return programResult;
}

int wmain(int argc, wchar_t** argv)
{
    try
    {
        return wmain2(argc, argv);
    }
    catch(const std::exception& ex)
    {
        wprintf(L"ERROR: %hs\n", ex.what());
        return PROGRAM_EXIT_ERROR_EXCEPTION;
    }
    catch(...)
    {
        wprintf(L"UNKNOWN ERROR.\n");
        return PROGRAM_EXIT_ERROR_EXCEPTION;
    }
}
