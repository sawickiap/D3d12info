#include "pch.h"
#include "Utils.h"

static const wchar_t* const PROGRAM_VERSION = L"0.0.2-development";

static const int PROGRAM_EXIT_SUCCESS            = 0;
static const int PROGRAM_EXIT_ERROR_INIT         = -1;
static const int PROGRAM_EXIT_ERROR_COMMAND_LINE = -2;
static const int PROGRAM_EXIT_ERROR_EXCEPTION    = -3;

//#define AUTO_LINK_DX12    // use this on everything before Win10
#if defined(AUTO_LINK_DX12)

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

#else // #if defined(AUTO_LINK_DX12)

typedef HRESULT (WINAPI* PFN_DXGI_CREATE_FACTORY1)(REFIID riid, _COM_Outptr_ void **);

HMODULE g_DxgiLibrary = nullptr;
HMODULE g_Dx12Library = nullptr;

const wchar_t* DYN_LIB_DXGI = L"dxgi.dll";
const wchar_t* DYN_LIB_DX12 = L"d3d12.dll";

PFN_DXGI_CREATE_FACTORY1 g_CreateDXGIFactory1;
PFN_D3D12_CREATE_DEVICE g_D3D12CreateDevice;

#if defined(_DEBUG)
PFN_D3D12_GET_DEBUG_INTERFACE g_D3D12GetDebugInterface;
#endif

#endif // #if defined(AUTO_LINK_DX12)


static const wchar_t* D3D12_SHADER_MIN_PRECISION_SUPPORT_NAMES[] = {
    L"D3D12_SHADER_MIN_PRECISION_SUPPORT_NONE",
    L"D3D12_SHADER_MIN_PRECISION_SUPPORT_10_BIT",
    L"D3D12_SHADER_MIN_PRECISION_SUPPORT_16_BIT",
};
static const uint32_t D3D12_SHADER_MIN_PRECISION_SUPPORT_VALUES[] = {
    D3D12_SHADER_MIN_PRECISION_SUPPORT_NONE,
    D3D12_SHADER_MIN_PRECISION_SUPPORT_10_BIT,
    D3D12_SHADER_MIN_PRECISION_SUPPORT_16_BIT,
};

static const wchar_t* D3D12_TILED_RESOURCES_TIER_NAMES[] = {
    L"D3D12_TILED_RESOURCES_TIER_NOT_SUPPORTED",
    L"D3D12_TILED_RESOURCES_TIER_1",
    L"D3D12_TILED_RESOURCES_TIER_2",
    L"D3D12_TILED_RESOURCES_TIER_3",
    //L"D3D12_TILED_RESOURCES_TIER_4", // TODO
};
static const uint32_t D3D12_TILED_RESOURCES_TIER_VALUES[] = {
    D3D12_TILED_RESOURCES_TIER_NOT_SUPPORTED,
    D3D12_TILED_RESOURCES_TIER_1,
    D3D12_TILED_RESOURCES_TIER_2,
    D3D12_TILED_RESOURCES_TIER_3,
    //D3D12_TILED_RESOURCES_TIER_4, // TODO
};

static const wchar_t* D3D12_RESOURCE_BINDING_TIER_NAMES[] = {
    L"D3D12_RESOURCE_BINDING_TIER_1",
    L"D3D12_RESOURCE_BINDING_TIER_2",
    L"D3D12_RESOURCE_BINDING_TIER_3",
};
static const uint32_t D3D12_RESOURCE_BINDING_TIER_VALUES[] = {
    D3D12_RESOURCE_BINDING_TIER_1,
    D3D12_RESOURCE_BINDING_TIER_2,
    D3D12_RESOURCE_BINDING_TIER_3,
};

static const wchar_t* D3D12_CONSERVATIVE_RASTERIZATION_TIER_NAMES[] = {
    L"D3D12_CONSERVATIVE_RASTERIZATION_TIER_NOT_SUPPORTED",
    L"D3D12_CONSERVATIVE_RASTERIZATION_TIER_1",
    L"D3D12_CONSERVATIVE_RASTERIZATION_TIER_2",
    L"D3D12_CONSERVATIVE_RASTERIZATION_TIER_3",
};
static const uint32_t D3D12_CONSERVATIVE_RASTERIZATION_TIER_VALUES[] = {
    D3D12_CONSERVATIVE_RASTERIZATION_TIER_NOT_SUPPORTED,
    D3D12_CONSERVATIVE_RASTERIZATION_TIER_1,
    D3D12_CONSERVATIVE_RASTERIZATION_TIER_2,
    D3D12_CONSERVATIVE_RASTERIZATION_TIER_3,
};

static const wchar_t* D3D12_CROSS_NODE_SHARING_TIER_NAMES[] = {
    L"D3D12_CROSS_NODE_SHARING_TIER_NOT_SUPPORTED",
    L"D3D12_CROSS_NODE_SHARING_TIER_1_EMULATED",
    L"D3D12_CROSS_NODE_SHARING_TIER_1",
    L"D3D12_CROSS_NODE_SHARING_TIER_2",
    //L"D3D12_CROSS_NODE_SHARING_TIER_3",
};
static const uint32_t D3D12_CROSS_NODE_SHARING_TIER_VALUES[] = {
    D3D12_CROSS_NODE_SHARING_TIER_NOT_SUPPORTED,
    D3D12_CROSS_NODE_SHARING_TIER_1_EMULATED,
    D3D12_CROSS_NODE_SHARING_TIER_1,
    D3D12_CROSS_NODE_SHARING_TIER_2,
    //D3D12_CROSS_NODE_SHARING_TIER_3, // TODO
};

static const wchar_t* D3D12_RESOURCE_HEAP_TIER_NAMES[] = {
    L"D3D12_RESOURCE_HEAP_TIER_1",
    L"D3D12_RESOURCE_HEAP_TIER_2",
};
static const uint32_t D3D12_RESOURCE_HEAP_TIER_VALUES[] = {
    D3D12_RESOURCE_HEAP_TIER_1,
    D3D12_RESOURCE_HEAP_TIER_2,
};

static const wchar_t* DXGI_ADAPTER_FLAG_NAMES[] = {
   L"DXGI_ADAPTER_FLAG_NONE",
   L"DXGI_ADAPTER_FLAG_REMOTE",
   L"DXGI_ADAPTER_FLAG_SOFTWARE",
};
static const uint32_t DXGI_ADAPTER_FLAG_VALUES[] = {
   DXGI_ADAPTER_FLAG_NONE,
   DXGI_ADAPTER_FLAG_REMOTE,
   DXGI_ADAPTER_FLAG_SOFTWARE,
};

static const wchar_t* D3D_FEATURE_LEVEL_NAMES[] = {
    L"D3D_FEATURE_LEVEL_9_1",
    L"D3D_FEATURE_LEVEL_9_2",
    L"D3D_FEATURE_LEVEL_9_3",
    L"D3D_FEATURE_LEVEL_10_0",
    L"D3D_FEATURE_LEVEL_10_1",
    L"D3D_FEATURE_LEVEL_11_0",
    L"D3D_FEATURE_LEVEL_11_1",
    L"D3D_FEATURE_LEVEL_12_0",
    L"D3D_FEATURE_LEVEL_12_1",
};
static const uint32_t D3D_FEATURE_LEVEL_VALUES[] = {
    D3D_FEATURE_LEVEL_9_1,
    D3D_FEATURE_LEVEL_9_2,
    D3D_FEATURE_LEVEL_9_3,
    D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_12_0,
    D3D_FEATURE_LEVEL_12_1,
};

static const wchar_t* D3D_SHADER_MODEL_NAMES[] = {
    L"D3D_SHADER_MODEL_5_1",
    L"D3D_SHADER_MODEL_6_0",
    L"D3D_SHADER_MODEL_6_1",
    L"D3D_SHADER_MODEL_6_2",
    L"D3D_SHADER_MODEL_6_3",
    L"D3D_SHADER_MODEL_6_4",
};
static const uint32_t D3D_SHADER_MODEL_VALUES[] = {
    D3D_SHADER_MODEL_5_1,
    D3D_SHADER_MODEL_6_0,
    D3D_SHADER_MODEL_6_1,
    //D3D_SHADER_MODEL_6_2, // TODO
    //D3D_SHADER_MODEL_6_3,
    //D3D_SHADER_MODEL_6_4,
};

static const wchar_t* D3D_ROOT_SIGNATURE_VERSION_NAMES[] = {
    L"D3D_ROOT_SIGNATURE_VERSION_1",
    L"D3D_ROOT_SIGNATURE_VERSION_1_0",
    L"D3D_ROOT_SIGNATURE_VERSION_1_1",
};
static const uint32_t D3D_ROOT_SIGNATURE_VERSION_VALUES[] = {
    D3D_ROOT_SIGNATURE_VERSION_1,
    D3D_ROOT_SIGNATURE_VERSION_1_0,
    D3D_ROOT_SIGNATURE_VERSION_1_1,
};

static const wchar_t* D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER_NAMES[] = {
    L"D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER_NOT_SUPPORTED",
    L"D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER_1",
    L"D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER_2",
};
static const uint32_t D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER_VALUES[] = {
    D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER_NOT_SUPPORTED,
    D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER_1,
    D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER_2,
};

static const wchar_t* D3D12_SHADER_CACHE_SUPPORT_FLAGS_NAMES[] = {
    L"D3D12_SHADER_CACHE_SUPPORT_NONE",
    L"D3D12_SHADER_CACHE_SUPPORT_SINGLE_PSO",
    L"D3D12_SHADER_CACHE_SUPPORT_LIBRARY",
    L"D3D12_SHADER_CACHE_SUPPORT_AUTOMATIC_INPROC_CACHE",
    L"D3D12_SHADER_CACHE_SUPPORT_AUTOMATIC_DISK_CACHE",
};
static const uint32_t D3D12_SHADER_CACHE_SUPPORT_FLAGS_VALUES[] = {
    D3D12_SHADER_CACHE_SUPPORT_NONE,
    D3D12_SHADER_CACHE_SUPPORT_SINGLE_PSO,
    D3D12_SHADER_CACHE_SUPPORT_LIBRARY,
    D3D12_SHADER_CACHE_SUPPORT_AUTOMATIC_INPROC_CACHE,
    D3D12_SHADER_CACHE_SUPPORT_AUTOMATIC_DISK_CACHE,
};

static const wchar_t* D3D12_COMMAND_LIST_SUPPORT_FLAGS_NAMES[] = {
    L"D3D12_COMMAND_LIST_SUPPORT_FLAG_NONE",
    L"D3D12_COMMAND_LIST_SUPPORT_FLAG_DIRECT",
    L"D3D12_COMMAND_LIST_SUPPORT_FLAG_BUNDLE",
    L"D3D12_COMMAND_LIST_SUPPORT_FLAG_COMPUTE",
    L"D3D12_COMMAND_LIST_SUPPORT_FLAG_COPY",
    L"D3D12_COMMAND_LIST_SUPPORT_FLAG_VIDEO_DECODE",
    L"D3D12_COMMAND_LIST_SUPPORT_FLAG_VIDEO_PROCESS",
};
static const uint32_t D3D12_COMMAND_LIST_SUPPORT_FLAGS_VALUES[] = {
    D3D12_COMMAND_LIST_SUPPORT_FLAG_NONE,
    D3D12_COMMAND_LIST_SUPPORT_FLAG_DIRECT,
    D3D12_COMMAND_LIST_SUPPORT_FLAG_BUNDLE,
    D3D12_COMMAND_LIST_SUPPORT_FLAG_COMPUTE,
    D3D12_COMMAND_LIST_SUPPORT_FLAG_COPY,
    D3D12_COMMAND_LIST_SUPPORT_FLAG_VIDEO_DECODE,
    D3D12_COMMAND_LIST_SUPPORT_FLAG_VIDEO_PROCESS,
};

static const wchar_t* D3D12_VIEW_INSTANCING_TIER_NAMES[] = {
    L"D3D12_VIEW_INSTANCING_TIER_NOT_SUPPORTED",
    L"D3D12_VIEW_INSTANCING_TIER_1",
    L"D3D12_VIEW_INSTANCING_TIER_2",
    L"D3D12_VIEW_INSTANCING_TIER_3",
};
static const uint32_t D3D12_VIEW_INSTANCING_TIER_VALUES[] = {
    D3D12_VIEW_INSTANCING_TIER_NOT_SUPPORTED,
    D3D12_VIEW_INSTANCING_TIER_1,
    D3D12_VIEW_INSTANCING_TIER_2,
    D3D12_VIEW_INSTANCING_TIER_3,
};

static const wchar_t* D3D12_SHARED_RESOURCE_COMPATIBILITY_TIER_NAMES[] = {
    L"D3D12_SHARED_RESOURCE_COMPATIBILITY_TIER_0",
    L"D3D12_SHARED_RESOURCE_COMPATIBILITY_TIER_1",
};
static const uint32_t D3D12_SHARED_RESOURCE_COMPATIBILITY_TIER_VALUES[] = {
    D3D12_SHARED_RESOURCE_COMPATIBILITY_TIER_0,
    D3D12_SHARED_RESOURCE_COMPATIBILITY_TIER_1,
};

static const wchar_t* D3D12_RENDER_PASS_TIER_NAMES[] = {
    L"D3D12_RENDER_PASS_TIER_0",
    L"D3D12_RENDER_PASS_TIER_1",
    L"D3D12_RENDER_PASS_TIER_2",
};
static const uint32_t D3D12_RENDER_PASS_TIER_VALUES[] = {
    D3D12_RENDER_PASS_TIER_0,
    D3D12_RENDER_PASS_TIER_1,
    D3D12_RENDER_PASS_TIER_2,
};

static const wchar_t* D3D12_RAYTRACING_TIER_NAMES[] = {
    L"D3D12_RAYTRACING_TIER_NOT_SUPPORTED",
    L"D3D12_RAYTRACING_TIER_1_0",
};
static const uint32_t D3D12_RAYTRACING_TIER_VALUES[] = {
    D3D12_RAYTRACING_TIER_NOT_SUPPORTED,
    D3D12_RAYTRACING_TIER_1_0,
};

static const wchar_t* D3D12_CPU_PAGE_PROPERTY_NAMES[] = {
    L"D3D12_CPU_PAGE_PROPERTY_UNKNOWN",
    L"D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE",
    L"D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE",
    L"D3D12_CPU_PAGE_PROPERTY_WRITE_BACK",
};
static const uint32_t D3D12_CPU_PAGE_PROPERTY_VALUES[] = {
    D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
    D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE,
    D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE,
    D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
};

static const wchar_t* D3D12_MEMORY_POOL_NAMES[] = {
    L"D3D12_MEMORY_POOL_UNKNOWN",
    L"D3D12_MEMORY_POOL_L0",
    L"D3D12_MEMORY_POOL_L1",
};
static const uint32_t D3D12_MEMORY_POOL_VALUES[] = {
    D3D12_MEMORY_POOL_UNKNOWN,
    D3D12_MEMORY_POOL_L0,
    D3D12_MEMORY_POOL_L1,
};

static uint32_t g_Indent;
static uint32_t g_ArrayIndex = UINT32_MAX;

static void PrintIndent()
{
    static const wchar_t* maxIndentStr = L"                                                                ";
    const uint32_t offset = 64u - (g_Indent * 4u);
    assert(offset < wcslen(maxIndentStr));
    wprintf(L"%s", maxIndentStr + offset);
}
static void BeginArray()
{
    g_ArrayIndex = 0;
}
static void EndArray()
{
    g_ArrayIndex = UINT32_MAX;
}
static void StepArray()
{
    ++g_ArrayIndex;
}

static void PrintName(const wchar_t* name)
{
    if(g_ArrayIndex != UINT32_MAX)
    {
        wprintf(L"%s[%u]", name, g_ArrayIndex);
    }
    else
    {
        wprintf(L"%s", name);
    }
}

static void Print_BOOL(const wchar_t* name, BOOL value)
{
    PrintIndent();
    PrintName(name);
    wprintf(L" = %s\n", value ? L"TRUE" : L"FALSE");
}
static void Print_uint32(const wchar_t* name, uint32_t value)
{
    PrintIndent();
    PrintName(name);
    wprintf(L" = %u\n", value);
}
static void Print_uint64(const wchar_t* name, uint64_t value)
{
    PrintIndent();
    PrintName(name);
    wprintf(L" = %llu\n", value);
}
static void Print_size(const wchar_t* name, size_t value)
{
    PrintIndent();
    PrintName(name);
    wprintf(L" = %zu\n", value);
}
static void Print_hex32(const wchar_t* name, uint32_t value)
{
    PrintIndent();
    PrintName(name);
    wprintf(L" = 0x%X\n", value);
}
static void Print_string(const wchar_t* name, const wchar_t* value)
{
    PrintIndent();
    PrintName(name);
    wprintf(L" = %s\n", value);
}
static void Print_LUID(const wchar_t* name, LUID value)
{
    PrintIndent();
    PrintName(name);
    wprintf(L" = %08X-%08X\n", (uint32_t)value.HighPart, (uint32_t)value.LowPart);
}

static void PrintStructBegin(const wchar_t* name)
{
    PrintIndent();
    PrintName(name);
    wprintf(L":\n");
    ++g_Indent;
}

static void PrintStructEnd()
{
    --g_Indent;
}

static void PrintEnum(const wchar_t* name, uint32_t value,
    const wchar_t* const* names, const uint32_t* values, size_t valueCount)
{
    PrintIndent();
    PrintName(name);
    for(size_t i = 0; i < valueCount; ++i)
    {
        if(value == values[i])
        {
            wprintf(L" = %s (0x%X)\n", names[i], value);
            return;
        }
    }
    wprintf(L" = 0x%X\n", value);
}

static void PrintFlags(const wchar_t* name, uint32_t value,
    const wchar_t* const* flagNames, const uint32_t* flagValues,
    size_t flagCount)
{
    PrintIndent();
    PrintName(name);
    wprintf(L" = 0x%X\n", value);

    ++g_Indent;
    size_t zeroFlagIndex = SIZE_MAX;
    for(size_t i = 0; i < flagCount; ++i)
    {
        if(flagValues[i] == 0)
        {
            zeroFlagIndex = i;
        }
        else
        {
            if((value & flagValues[i]) != 0)
            {
                PrintIndent();
                wprintf(L"%s\n", flagNames[i]);
            }
        }
    }
    if(value == 0 && zeroFlagIndex != SIZE_MAX)
    {
        PrintIndent();
        wprintf(L"%s\n", flagNames[zeroFlagIndex]);
    }
    --g_Indent;
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS(const D3D12_FEATURE_DATA_D3D12_OPTIONS& options)
{
    Print_BOOL(  L"DoublePrecisionFloatShaderOps       ", options.DoublePrecisionFloatShaderOps);
    Print_BOOL(  L"OutputMergerLogicOp                 ", options.OutputMergerLogicOp);
    PrintEnum(   L"MinPrecisionSupport                 ", options.MinPrecisionSupport, D3D12_SHADER_MIN_PRECISION_SUPPORT_NAMES, D3D12_SHADER_MIN_PRECISION_SUPPORT_VALUES, _countof(D3D12_SHADER_MIN_PRECISION_SUPPORT_VALUES));
    PrintEnum(   L"TiledResourcesTier                  ", options.TiledResourcesTier, D3D12_TILED_RESOURCES_TIER_NAMES, D3D12_TILED_RESOURCES_TIER_VALUES, _countof(D3D12_TILED_RESOURCES_TIER_VALUES));
    PrintEnum(   L"ResourceBindingTier                 ", options.ResourceBindingTier, D3D12_RESOURCE_BINDING_TIER_NAMES, D3D12_RESOURCE_BINDING_TIER_VALUES, _countof(D3D12_RESOURCE_BINDING_TIER_VALUES));
    Print_BOOL(  L"PSSpecifiedStencilRefSupported      ", options.PSSpecifiedStencilRefSupported);
    Print_BOOL(  L"TypedUAVLoadAdditionalFormats       ", options.TypedUAVLoadAdditionalFormats);
    Print_BOOL(  L"ROVsSupported                       ", options.ROVsSupported);
    PrintEnum(   L"ConservativeRasterizationTier       ", options.ConservativeRasterizationTier, D3D12_CONSERVATIVE_RASTERIZATION_TIER_NAMES, D3D12_CONSERVATIVE_RASTERIZATION_TIER_VALUES, _countof(D3D12_CONSERVATIVE_RASTERIZATION_TIER_VALUES));
    Print_uint32(L"MaxGPUVirtualAddressBitsPerResource ", options.MaxGPUVirtualAddressBitsPerResource);
    Print_BOOL(  L"StandardSwizzle64KBSupported        ", options.StandardSwizzle64KBSupported);
    PrintEnum(   L"CrossNodeSharingTier                ", options.CrossNodeSharingTier, D3D12_CROSS_NODE_SHARING_TIER_NAMES, D3D12_CROSS_NODE_SHARING_TIER_VALUES, _countof(D3D12_CROSS_NODE_SHARING_TIER_VALUES));
    Print_BOOL(  L"CrossAdapterRowMajorTextureSupported", options.CrossAdapterRowMajorTextureSupported);
    Print_BOOL(  L"VPAndRTArrayIndexFromAnyShaderFeedingRasterizerSupportedWithoutGSEmulation", options.VPAndRTArrayIndexFromAnyShaderFeedingRasterizerSupportedWithoutGSEmulation);
    PrintEnum(   L"ResourceHeapTier                    ", options.ResourceHeapTier, D3D12_RESOURCE_HEAP_TIER_NAMES, D3D12_RESOURCE_HEAP_TIER_VALUES, _countof(D3D12_RESOURCE_HEAP_TIER_VALUES));
}

static void Print_D3D12_FEATURE_DATA_ARCHITECTURE(const D3D12_FEATURE_DATA_ARCHITECTURE& architecture)
{
    Print_uint32(L"NodeIndex        ", architecture.NodeIndex);
    Print_BOOL  (L"TileBasedRenderer", architecture.TileBasedRenderer);
    Print_BOOL  (L"UMA              ", architecture.UMA);
    Print_BOOL  (L"CacheCoherentUMA ", architecture.CacheCoherentUMA);
}

static void Print_D3D12_FEATURE_DATA_ARCHITECTURE1(const D3D12_FEATURE_DATA_ARCHITECTURE1& architecture1)
{
    Print_uint32(L"NodeIndex        ", architecture1.NodeIndex);
    Print_BOOL  (L"TileBasedRenderer", architecture1.TileBasedRenderer);
    Print_BOOL  (L"UMA              ", architecture1.UMA);
    Print_BOOL  (L"CacheCoherentUMA ", architecture1.CacheCoherentUMA);
    Print_BOOL  (L"IsolatedMMU      ", architecture1.IsolatedMMU);
}

static void Print_D3D12_FEATURE_DATA_FEATURE_LEVELS(const D3D12_FEATURE_DATA_FEATURE_LEVELS& featureLevels)
{
    Print_uint32(L"NumFeatureLevels", featureLevels.NumFeatureLevels);
    BeginArray();
    for(uint32_t i = 0; i < featureLevels.NumFeatureLevels; ++i)
    {
        PrintEnum(L"pFeatureLevelsRequested", featureLevels.pFeatureLevelsRequested[i], D3D_FEATURE_LEVEL_NAMES, D3D_FEATURE_LEVEL_VALUES, _countof(D3D_FEATURE_LEVEL_VALUES));
        StepArray();
    }
    EndArray();
    PrintEnum(L"MaxSupportedFeatureLevel", featureLevels.MaxSupportedFeatureLevel, D3D_FEATURE_LEVEL_NAMES, D3D_FEATURE_LEVEL_VALUES, _countof(D3D_FEATURE_LEVEL_VALUES));
}

static void Print_D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT(const D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT& virtualAddressSupport)
{
    Print_uint32(L"MaxGPUVirtualAddressBitsPerResource", virtualAddressSupport.MaxGPUVirtualAddressBitsPerResource);
    Print_uint32(L"MaxGPUVirtualAddressBitsPerProcess ", virtualAddressSupport.MaxGPUVirtualAddressBitsPerProcess);
}

static void Print_D3D12_FEATURE_DATA_SHADER_MODEL(const D3D12_FEATURE_DATA_SHADER_MODEL& shaderModel)
{
    PrintEnum(L"HighestShaderModel", shaderModel.HighestShaderModel, D3D_SHADER_MODEL_NAMES, D3D_SHADER_MODEL_VALUES, _countof(D3D_SHADER_MODEL_VALUES));
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS1(const D3D12_FEATURE_DATA_D3D12_OPTIONS1& options1)
{
    Print_BOOL  (L"WaveOps                      ", options1.WaveOps);
    Print_uint32(L"WaveLaneCountMin             ", options1.WaveLaneCountMin);
    Print_uint32(L"WaveLaneCountMax             ", options1.WaveLaneCountMax);
    Print_uint32(L"TotalLaneCount               ", options1.TotalLaneCount);
    Print_BOOL  (L"ExpandedComputeResourceStates", options1.ExpandedComputeResourceStates);
    Print_BOOL  (L"Int64ShaderOps               ", options1.Int64ShaderOps);
}

static void Print_D3D12_FEATURE_DATA_ROOT_SIGNATURE(const D3D12_FEATURE_DATA_ROOT_SIGNATURE& rootSignature)
{
    PrintEnum(L"HighestVersion", rootSignature.HighestVersion, D3D_ROOT_SIGNATURE_VERSION_NAMES, D3D_ROOT_SIGNATURE_VERSION_VALUES, _countof(D3D_ROOT_SIGNATURE_VERSION_VALUES));
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS2(const D3D12_FEATURE_DATA_D3D12_OPTIONS2& options2)
{
    Print_BOOL(L"DepthBoundsTestSupported       ", options2.DepthBoundsTestSupported);
    PrintEnum (L"ProgrammableSamplePositionsTier", options2.ProgrammableSamplePositionsTier, D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER_NAMES, D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER_VALUES, _countof(D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER_VALUES));
}

static void Print_D3D12_FEATURE_DATA_SHADER_CACHE(const D3D12_FEATURE_DATA_SHADER_CACHE& shaderCache)
{
    PrintFlags(L"SupportFlags", shaderCache.SupportFlags, D3D12_SHADER_CACHE_SUPPORT_FLAGS_NAMES, D3D12_SHADER_CACHE_SUPPORT_FLAGS_VALUES, _countof(D3D12_SHADER_CACHE_SUPPORT_FLAGS_VALUES));
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS3(const D3D12_FEATURE_DATA_D3D12_OPTIONS3& options3)
{
    Print_BOOL(L"CopyQueueTimestampQueriesSupported", options3.CopyQueueTimestampQueriesSupported);
    Print_BOOL(L"CastingFullyTypedFormatSupported  ", options3.CastingFullyTypedFormatSupported);
    PrintFlags(L"WriteBufferImmediateSupportFlags  ", options3.WriteBufferImmediateSupportFlags, D3D12_COMMAND_LIST_SUPPORT_FLAGS_NAMES, D3D12_COMMAND_LIST_SUPPORT_FLAGS_VALUES, _countof(D3D12_COMMAND_LIST_SUPPORT_FLAGS_VALUES));
    PrintEnum (L"ViewInstancingTier                ", options3.ViewInstancingTier, D3D12_VIEW_INSTANCING_TIER_NAMES, D3D12_VIEW_INSTANCING_TIER_VALUES, _countof(D3D12_VIEW_INSTANCING_TIER_VALUES));
    Print_BOOL(L"BarycentricsSupported             ", options3.BarycentricsSupported);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS4(const D3D12_FEATURE_DATA_D3D12_OPTIONS4& options4)
{
    Print_BOOL(L"MSAA64KBAlignedTextureSupported", options4.MSAA64KBAlignedTextureSupported);
    PrintEnum(L"SharedResourceCompatibilityTier", options4.SharedResourceCompatibilityTier, D3D12_SHARED_RESOURCE_COMPATIBILITY_TIER_NAMES, D3D12_SHARED_RESOURCE_COMPATIBILITY_TIER_VALUES, _countof(D3D12_SHARED_RESOURCE_COMPATIBILITY_TIER_VALUES));
    Print_BOOL(L"Native16BitShaderOpsSupported", options4.Native16BitShaderOpsSupported);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS5(const D3D12_FEATURE_DATA_D3D12_OPTIONS5& options5)
{
    Print_BOOL(L"SRVOnlyTiledResourceTier3", options5.SRVOnlyTiledResourceTier3);
    PrintEnum(L"RenderPassesTier", options5.RenderPassesTier, D3D12_RENDER_PASS_TIER_NAMES, D3D12_RENDER_PASS_TIER_VALUES, _countof(D3D12_RENDER_PASS_TIER_VALUES));
    PrintEnum(L"RaytracingTier", options5.RaytracingTier, D3D12_RAYTRACING_TIER_NAMES, D3D12_RAYTRACING_TIER_VALUES, _countof(D3D12_RAYTRACING_TIER_VALUES));
}

static void Print_D3D12_FEATURE_DATA_EXISTING_HEAPS(const D3D12_FEATURE_DATA_EXISTING_HEAPS& existingHeaps)
{
    Print_BOOL(L"Supported", existingHeaps.Supported);
}

static void Print_DXGI_QUERY_VIDEO_MEMORY_INFO(const DXGI_QUERY_VIDEO_MEMORY_INFO& videoMemoryInfo)
{
    Print_uint64(L"Budget", videoMemoryInfo.Budget);
    Print_uint64(L"CurrentUsage", videoMemoryInfo.CurrentUsage);
    Print_uint64(L"AvailableForReservation", videoMemoryInfo.AvailableForReservation);
    Print_uint64(L"CurrentReservation", videoMemoryInfo.CurrentReservation);
}

static void Print_D3D12_HEAP_PROPERTIES(const D3D12_HEAP_PROPERTIES& heapProperties)
{
    // heapProperties.Type intentionally ignored.
    PrintEnum(L"CPUPageProperty", heapProperties.CPUPageProperty, D3D12_CPU_PAGE_PROPERTY_NAMES, D3D12_CPU_PAGE_PROPERTY_VALUES, _countof(D3D12_CPU_PAGE_PROPERTY_VALUES));
    PrintEnum(L"MemoryPoolPreference", heapProperties.MemoryPoolPreference, D3D12_MEMORY_POOL_NAMES, D3D12_MEMORY_POOL_VALUES, _countof(D3D12_MEMORY_POOL_VALUES));
    Print_hex32(L"CreationNodeMask", heapProperties.CreationNodeMask);
    Print_hex32(L"VisibleNodeMask", heapProperties.VisibleNodeMask);
}

static void PrintInfoAdapter(IDXGIAdapter1* adapter1)
{
    assert(adapter1 != nullptr);

    ++g_Indent;

    DXGI_ADAPTER_DESC1 desc = {};
    CHECK_HR( adapter1->GetDesc1(&desc) );

    Print_string(L"Description          ", desc.Description);
    Print_hex32 (L"VendorId             ", desc.VendorId);
    Print_hex32 (L"DeviceId             ", desc.DeviceId);
    Print_hex32 (L"SubSysId             ", desc.SubSysId);
    Print_hex32 (L"Revision             ", desc.Revision);
    Print_size  (L"DedicatedVideoMemory ", desc.DedicatedVideoMemory);
    Print_size  (L"DedicatedSystemMemory", desc.DedicatedSystemMemory);
    Print_size  (L"SharedSystemMemory   ", desc.SharedSystemMemory);
    Print_LUID  (L"AdapterLuid          ", desc.AdapterLuid);
    PrintFlags  (L"Flags                ", desc.Flags, DXGI_ADAPTER_FLAG_NAMES, DXGI_ADAPTER_FLAG_VALUES, _countof(DXGI_ADAPTER_FLAG_VALUES));
    wprintf(L"\n");

    HRESULT hr;

    IDXGIAdapter3* adapter3 = nullptr;
    hr = adapter1->QueryInterface<IDXGIAdapter3>(&adapter3);
    if(SUCCEEDED(hr))
    {
        assert(adapter3 != nullptr);

        for(uint32_t memorySegmentGroup = 0; memorySegmentGroup < 2; ++memorySegmentGroup)
        {
            DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo = {};
            CHECK_HR( adapter3->QueryVideoMemoryInfo(0, (DXGI_MEMORY_SEGMENT_GROUP)memorySegmentGroup, &videoMemoryInfo) );

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

    wprintf(L"\n");
}

static void PrintDeviceDetails(IDXGIAdapter1* adapter1)
{
    HRESULT hr;
    ID3D12Device* device = nullptr;
#if defined(AUTO_LINK_DX12)
    CHECK_HR( ::D3D12CreateDevice(adapter1, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)) );
#else
    CHECK_HR( g_D3D12CreateDevice(adapter1, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)) );
#endif
    if (device != nullptr)
    {
        wprintf(L"\n");
        wprintf(L"D3D12_FEATURE_DATA_D3D12_OPTIONS:\n");
        wprintf(L"=================================\n");
        D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
        CHECK_HR( device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options)) );
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS(options);

        D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT gpuVirtualAddressSupport = {};
        hr = device->CheckFeatureSupport(D3D12_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT, &gpuVirtualAddressSupport, sizeof(gpuVirtualAddressSupport));
        if(SUCCEEDED(hr))
        {
            wprintf(L"\n");
            wprintf(L"D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT:\n");
            wprintf(L"===============================================\n");
            Print_D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT(gpuVirtualAddressSupport);
        }

        wprintf(L"\n");
        wprintf(L"D3D12_FEATURE_DATA_SHADER_MODEL:\n");
        wprintf(L"================================\n");
        D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = {};
        shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_1;
        CHECK_HR( device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel)) );
        Print_D3D12_FEATURE_DATA_SHADER_MODEL(shaderModel);

        wprintf(L"\n");
        wprintf(L"D3D12_FEATURE_DATA_D3D12_OPTIONS1:\n");
        wprintf(L"==================================\n");
        D3D12_FEATURE_DATA_D3D12_OPTIONS1 options1 = {};
        CHECK_HR( device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS1, &options1, sizeof(options1)) );
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS1(options1);

        D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignature = {};
        rootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
        hr = device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rootSignature, sizeof(rootSignature));
        if(SUCCEEDED(hr))
        {
            wprintf(L"\n");
            wprintf(L"D3D12_FEATURE_DATA_ROOT_SIGNATURE:\n");
            wprintf(L"==================================\n");
            Print_D3D12_FEATURE_DATA_ROOT_SIGNATURE(rootSignature);
        }

        D3D12_FEATURE_DATA_ARCHITECTURE1 architecture1 = {};
        hr = device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE1, &architecture1, sizeof(architecture1));
        if(SUCCEEDED(hr))
        {
            wprintf(L"\n");
            wprintf(L"D3D12_FEATURE_DATA_ARCHITECTURE1:\n");
            wprintf(L"=================================\n");
            Print_D3D12_FEATURE_DATA_ARCHITECTURE1(architecture1);
        }
        else
        {
            wprintf(L"\n");
            wprintf(L"D3D12_FEATURE_DATA_ARCHITECTURE:\n");
            wprintf(L"================================\n");
            D3D12_FEATURE_DATA_ARCHITECTURE architecture = {};
            CHECK_HR( device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE, &architecture, sizeof(architecture)) );
            Print_D3D12_FEATURE_DATA_ARCHITECTURE(architecture);
        }

        static const D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_12_1,
            D3D_FEATURE_LEVEL_12_0,
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };

        D3D12_FEATURE_DATA_FEATURE_LEVELS levels =
        {
            _countof(featureLevels), featureLevels, D3D_FEATURE_LEVEL_11_0
        };

        hr = device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &levels, sizeof(levels));
        if(SUCCEEDED(hr))
        {
            wprintf(L"\n");
            wprintf(L"D3D12_FEATURE_DATA_FEATURE_LEVELS:\n");
            wprintf(L"==================================\n");
            Print_D3D12_FEATURE_DATA_FEATURE_LEVELS(levels);
        }

        D3D12_FEATURE_DATA_D3D12_OPTIONS2 options2 = {};
        hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS2, &options2, sizeof(options2));
        if(SUCCEEDED(hr))
        {
            wprintf(L"\n");
            wprintf(L"D3D12_FEATURE_DATA_D3D12_OPTIONS2:\n");
            wprintf(L"==================================\n");
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS2(options2);
        }

        D3D12_FEATURE_DATA_SHADER_CACHE shaderCache = {};
        hr = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_CACHE, &shaderCache, sizeof(shaderCache));
        if(SUCCEEDED(hr))
        {
            wprintf(L"\n");
            wprintf(L"D3D12_FEATURE_DATA_SHADER_CACHE:\n");
            wprintf(L"================================\n");
            Print_D3D12_FEATURE_DATA_SHADER_CACHE(shaderCache);
        }

        /*
        D3D12_FEATURE_DATA_COMMAND_QUEUE_PRIORITY commandQueuePriority = {};
        hr = device->CheckFeatureSupport(D3D12_FEATURE_COMMAND_QUEUE_PRIORITY, &commandQueuePriority, sizeof(commandQueuePriority));
        if(SUCCEEDED(hr))
        {
            wprintf(L"\n");
            wprintf(L"D3D12_FEATURE_DATA_COMMAND_QUEUE_PRIORITY:\n");
            wprintf(L"==========================================\n");
            Print_D3D12_FEATURE_DATA_COMMAND_QUEUE_PRIORITY(commandQueuePriority);
        }
        */
        
        D3D12_FEATURE_DATA_D3D12_OPTIONS3 options3 = {};
        hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS3, &options3, sizeof(options3));
        if(SUCCEEDED(hr))
        {
            wprintf(L"\n");
            wprintf(L"D3D12_FEATURE_DATA_D3D12_OPTIONS3:\n");
            wprintf(L"==================================\n");
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS3(options3);
        }

        D3D12_FEATURE_DATA_EXISTING_HEAPS existingHeaps = {};
        hr = device->CheckFeatureSupport(D3D12_FEATURE_EXISTING_HEAPS, &existingHeaps, sizeof(existingHeaps));
        if(SUCCEEDED(hr))
        {
            wprintf(L"\n");
            wprintf(L"D3D12_FEATURE_DATA_EXISTING_HEAPS:\n");
            wprintf(L"==================================\n");
            Print_D3D12_FEATURE_DATA_EXISTING_HEAPS(existingHeaps);
        }

        wprintf(L"\n");
        wprintf(L"D3D12_HEAP_PROPERTIES:\n");
        wprintf(L"======================\n");
        for(uint32_t heapType = D3D12_HEAP_TYPE_DEFAULT; heapType <= D3D12_HEAP_TYPE_READBACK; ++heapType)
        {
            switch(heapType)
            {
            case D3D12_HEAP_TYPE_DEFAULT:
                PrintStructBegin(L"D3D12_HEAP_PROPERTIES[D3D12_HEAP_TYPE_DEFAULT]");
                break;
            case D3D12_HEAP_TYPE_UPLOAD:
                PrintStructBegin(L"D3D12_HEAP_PROPERTIES[D3D12_HEAP_TYPE_UPLOAD]");
                break;
            case D3D12_HEAP_TYPE_READBACK:
                PrintStructBegin(L"D3D12_HEAP_PROPERTIES[D3D12_HEAP_TYPE_READBACK]");
                break;
            default:
                assert(0);
            }

            const D3D12_HEAP_PROPERTIES heapProperties = device->GetCustomHeapProperties(0, (D3D12_HEAP_TYPE)heapType);
            Print_D3D12_HEAP_PROPERTIES(heapProperties);

            PrintStructEnd();
        }

        D3D12_FEATURE_DATA_D3D12_OPTIONS4 options4 = {};
        hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS4, &options4, sizeof(options4));
        if(SUCCEEDED(hr))
        {
            wprintf(L"\n");
            wprintf(L"D3D12_FEATURE_DATA_D3D12_OPTIONS4:\n");
            wprintf(L"==================================\n");
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS4(options4);
        }

        D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {};
        hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5));
        if(SUCCEEDED(hr))
        {
            wprintf(L"\n");
            wprintf(L"D3D12_FEATURE_DATA_D3D12_OPTIONS5:\n");
            wprintf(L"==================================\n");
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS5(options5);
        }

        wprintf(L"\n");

        SAFE_RELEASE(device);
    }

    --g_Indent;
}

#if !defined(AUTO_LINK_DX12)

static int LoadLibraries()
{
    g_DxgiLibrary = ::LoadLibraryEx(DYN_LIB_DXGI, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_DxgiLibrary)
    {
        wprintf(L"could not load %s\n", DYN_LIB_DXGI);
        return -1;
    }

    g_Dx12Library = ::LoadLibraryEx(DYN_LIB_DX12, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_Dx12Library)
    {
        wprintf(L"could not load %s\n", DYN_LIB_DX12);
        return -1;
    }

    g_CreateDXGIFactory1 = reinterpret_cast<PFN_DXGI_CREATE_FACTORY1>(::GetProcAddress(g_DxgiLibrary, "CreateDXGIFactory1"));
    if (!g_CreateDXGIFactory1)
    {
        return -1;
    }

    g_D3D12CreateDevice = reinterpret_cast<PFN_D3D12_CREATE_DEVICE>(::GetProcAddress(g_Dx12Library, "D3D12CreateDevice"));
    if (!g_D3D12CreateDevice)
    {
        return -1;
    }

#if defined(_DEBUG)
    g_D3D12GetDebugInterface = reinterpret_cast<PFN_D3D12_GET_DEBUG_INTERFACE>(::GetProcAddress(g_Dx12Library, "D3D12GetDebugInterface"));
    if (!g_D3D12GetDebugInterface)
    {
        return -1;
    }
#endif

    return 0;
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

ID3D12Debug* EnableDebugLayer()
{
    HRESULT hr;
    ID3D12Debug* debugController = nullptr;

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
    wprintf(L"  -a --Adapter=<Index> Print details of adapter at specified index (instead of default).\n");
}

int wmain2(int argc, wchar_t** argv)
{
    wprintf(L"============================\n");
    wprintf(L"D3D12INFO %s\n", PROGRAM_VERSION);
    wprintf(L"Built: %hs, %hs\n", __DATE__, __TIME__);
    wprintf(L"============================\n");
    wprintf(L"\n");

#if !defined(AUTO_LINK_DX12)
    if (LoadLibraries() != 0)
    {
        wprintf(L"could not load DXGI & DX12 libraries\n");
        return PROGRAM_EXIT_ERROR_INIT;
    }
#endif

    UINT requestedAdapterIndex = UINT32_MAX;

    CmdLineParser cmdLineParser(argc, argv);

    enum CMD_LINE_PARAM
    {
        CMD_LINE_OPT_VERSION,
        CMD_LINE_OPT_HELP,
        CMD_LINE_OPT_ADAPTER,
    };

    cmdLineParser.RegisterOpt(CMD_LINE_OPT_VERSION, L"Version", false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_VERSION, L'v', false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_HELP,    L"Help", false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_HELP,    L'h', false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_ADAPTER, L"Adapter", true);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_ADAPTER, L'a', true);

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
            case CMD_LINE_OPT_ADAPTER:
                requestedAdapterIndex = _wtoi(cmdLineParser.GetParameter().c_str());
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

#if defined(_DEBUG)
    ID3D12Debug* debugController = EnableDebugLayer();
#endif

    IDXGIFactory4* dxgiFactory = nullptr;
#if defined(AUTO_LINK_DX12)
    CHECK_HR( ::CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) );
#else
    CHECK_HR( g_CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) );
#endif
    assert(dxgiFactory != nullptr);

    IDXGIAdapter1* requestedAdapter = nullptr;
    IDXGIAdapter1* currAdapter1 = nullptr;
    UINT currAdapterIndex = 0;
    while(dxgiFactory->EnumAdapters1(currAdapterIndex, &currAdapter1) != DXGI_ERROR_NOT_FOUND)
    {
        wprintf(L"DXGI Adapter %u:\n", currAdapterIndex);
        wprintf(L"===============\n");

        PrintInfoAdapter(currAdapter1);

        // No explicit adapter requested: Choose first non-software and non-remote.
        if(requestedAdapterIndex == UINT32_MAX)
        {
            DXGI_ADAPTER_DESC1 desc = {};
            currAdapter1->GetDesc1(&desc);
            if(desc.Flags == 0)
            {
                requestedAdapterIndex = 0;
            }
        }

        if(requestedAdapterIndex == currAdapterIndex)
        {
            requestedAdapter = currAdapter1;
            currAdapter1 = nullptr;
        }
        else
        {
            SAFE_RELEASE(currAdapter1);
        }

        ++currAdapterIndex;
    }

    if(requestedAdapter)
    {
        wprintf(L"Adapter %u chosen to show D3D12 device details.\n", requestedAdapterIndex);
        PrintDeviceDetails(requestedAdapter);
        SAFE_RELEASE(requestedAdapter);
    }
    else
    {
        wprintf(L"No valid adapter chosen to show D3D12 device details.\n");
    }

    SAFE_RELEASE(dxgiFactory);

#if defined(_DEBUG)
    SAFE_RELEASE(debugController);
#endif

#if !defined(AUTO_LINK_DX12)
    UnloadLibraries();
#endif

    return PROGRAM_EXIT_SUCCESS;
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
