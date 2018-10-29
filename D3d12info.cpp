#define STRICT
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <cassert>
#include <cstdint>
#include <cwchar>

#define CHECK_HR(expr)		do { HRESULT hr__ = (expr); if(FAILED(hr__)) assert(0 && #expr); } while(false)
#define SAFE_RELEASE(x)		do { if(x) { (x)->Release(); (x) = nullptr; } } while(false)


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
    wprintf(L"%s", L"                                                                " + (64 - g_Indent * 4));
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

int main()
{
    wprintf(L"============================\n");
    wprintf(L"D3D12INFO\n");
	wprintf(L"built: %hs, %hs\n", __DATE__, __TIME__);
    wprintf(L"============================\n");
    wprintf(L"\n");

    IDXGIFactory4* dxgiFactory = nullptr;
    CHECK_HR( ::CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) );

    wprintf(L"DXGI Adapters:\n");
    wprintf(L"==============\n");
    wprintf(L"\n");

    IDXGIAdapter1* adapter1 = nullptr;
    UINT adapterIndex = 0;
    while(dxgiFactory->EnumAdapters1(adapterIndex, &adapter1) != DXGI_ERROR_NOT_FOUND)
    {
        wprintf(L"Adapter %u:\n", adapterIndex);
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

        IDXGIAdapter3* adapter3 = nullptr;
        if(SUCCEEDED(adapter1->QueryInterface<IDXGIAdapter3>(&adapter3)))
        {
			assert(adapter3 != nullptr);

            for(uint32_t memorySegmentGroup = 0; memorySegmentGroup < 2; ++memorySegmentGroup)
            {
	            DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo = {};
                CHECK_HR( adapter3->QueryVideoMemoryInfo(0, (DXGI_MEMORY_SEGMENT_GROUP)memorySegmentGroup, &videoMemoryInfo) );

                switch(memorySegmentGroup)
                {
                case 0:
                    PrintStructBegin(L"DXGI_QUERY_VIDEO_MEMORY_INFO [DXGI_MEMORY_SEGMENT_GROUP_LOCAL]");
                    break;
                case 1:
                    PrintStructBegin(L"DXGI_QUERY_VIDEO_MEMORY_INFO [DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL]");
                    break;
                default:
                    assert(0);
                }
                Print_DXGI_QUERY_VIDEO_MEMORY_INFO(videoMemoryInfo);
                PrintStructEnd();
            }
        }

		wprintf(L"\n");

        --g_Indent;

        SAFE_RELEASE(adapter1);
        adapterIndex++;
    }

    CHECK_HR( dxgiFactory->EnumAdapters1(0, &adapter1) );
	assert(adapter1 != nullptr);

    ID3D12Device* device = nullptr;
    CHECK_HR( ::D3D12CreateDevice(adapter1, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)) );
	assert(device != nullptr);

    wprintf(L"\n");
    wprintf(L"D3D12_FEATURE_DATA_D3D12_OPTIONS:\n");
    wprintf(L"=================================\n");
    D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
    CHECK_HR( device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options)) );
    Print_D3D12_FEATURE_DATA_D3D12_OPTIONS(options);

    wprintf(L"\n");
    wprintf(L"D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT:\n");
    wprintf(L"===============================================\n");
    D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT gpuVirtualAddressSupport = {};
    CHECK_HR( device->CheckFeatureSupport(D3D12_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT, &gpuVirtualAddressSupport, sizeof(gpuVirtualAddressSupport)) );
    Print_D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT(gpuVirtualAddressSupport);
    
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

    wprintf(L"\n");
    wprintf(L"D3D12_FEATURE_DATA_ROOT_SIGNATURE:\n");
    wprintf(L"==================================\n");
    D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignature = {};
    rootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    CHECK_HR( device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rootSignature, sizeof(rootSignature)) );
    Print_D3D12_FEATURE_DATA_ROOT_SIGNATURE(rootSignature);

    D3D12_FEATURE_DATA_ARCHITECTURE1 architecture1 = {};
    HRESULT hr = device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE1, &architecture1, sizeof(architecture1));
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

    wprintf(L"\n");
    wprintf(L"D3D12_FEATURE_DATA_D3D12_OPTIONS2:\n");
    wprintf(L"==================================\n");
    D3D12_FEATURE_DATA_D3D12_OPTIONS2 options2 = {};
    CHECK_HR( device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS2, &options2, sizeof(options2)) );
    Print_D3D12_FEATURE_DATA_D3D12_OPTIONS2(options2);

    wprintf(L"\n");
    wprintf(L"D3D12_FEATURE_DATA_SHADER_CACHE:\n");
    wprintf(L"================================\n");
    D3D12_FEATURE_DATA_SHADER_CACHE shaderCache = {};
    CHECK_HR( device->CheckFeatureSupport(D3D12_FEATURE_SHADER_CACHE, &shaderCache, sizeof(shaderCache)) );
    Print_D3D12_FEATURE_DATA_SHADER_CACHE(shaderCache);

    wprintf(L"\n");
    wprintf(L"D3D12_FEATURE_DATA_D3D12_OPTIONS3:\n");
    wprintf(L"==================================\n");
    D3D12_FEATURE_DATA_D3D12_OPTIONS3 options3 = {};
    CHECK_HR( device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS3, &options3, sizeof(options3)) );
    Print_D3D12_FEATURE_DATA_D3D12_OPTIONS3(options3);

    wprintf(L"\n");
    wprintf(L"D3D12_FEATURE_DATA_EXISTING_HEAPS:\n");
    wprintf(L"==================================\n");
    D3D12_FEATURE_DATA_EXISTING_HEAPS existingHeaps = {};
    CHECK_HR( device->CheckFeatureSupport(D3D12_FEATURE_EXISTING_HEAPS, &existingHeaps, sizeof(existingHeaps)) );
    Print_D3D12_FEATURE_DATA_EXISTING_HEAPS(existingHeaps);

    wprintf(L"\n");
    wprintf(L"D3D12_HEAP_PROPERTIES:\n");
    wprintf(L"======================\n");
    for(uint32_t heapType = D3D12_HEAP_TYPE_DEFAULT; heapType <= D3D12_HEAP_TYPE_READBACK; ++heapType)
    {
        switch(heapType)
        {
        case D3D12_HEAP_TYPE_DEFAULT:
            PrintStructBegin(L"D3D12_HEAP_PROPERTIES [D3D12_HEAP_TYPE_DEFAULT]");
            break;
        case D3D12_HEAP_TYPE_UPLOAD:
            PrintStructBegin(L"D3D12_HEAP_PROPERTIES [D3D12_HEAP_TYPE_UPLOAD]");
            break;
        case D3D12_HEAP_TYPE_READBACK:
            PrintStructBegin(L"D3D12_HEAP_PROPERTIES [D3D12_HEAP_TYPE_READBACK]");
            break;
        default:
            assert(0);
        }

        D3D12_HEAP_PROPERTIES heapProperties = device->GetCustomHeapProperties(0, (D3D12_HEAP_TYPE)heapType);
        Print_D3D12_HEAP_PROPERTIES(heapProperties);
        
        PrintStructEnd();
    }

    SAFE_RELEASE(device);
    SAFE_RELEASE(adapter1);
    SAFE_RELEASE(dxgiFactory);
}
