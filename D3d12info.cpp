#include "pch.hpp"
#include "Utils.hpp"
#include "Enums.hpp"

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
static void Print_size(const wchar_t* name, uint64_t value)
{
    PrintIndent();
    PrintName(name);
    if(value == 0)
        wprintf(L" = 0\n");
    else if(value < 1024)
        wprintf(L" = %zu (0x%llx)\n", value, value);
    else
    {
        wstring sizeStr = SizeToStr(value);
        wprintf(L" = %zu (0x%llx) (%s)\n", value, value, sizeStr.c_str());
    }
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
    const EnumItem* enumItems)
{
    PrintIndent();
    PrintName(name);
    const wchar_t* enumItemName = FindEnumItemName(value, enumItems);
    if(enumItemName != nullptr)
    {
        wprintf(L" = %s (0x%X)\n", enumItemName, value);
        return;
    }
    wprintf(L" = 0x%X\n", value);
}

static void PrintFlags(const wchar_t* name, uint32_t value,
    const EnumItem* enumItems)
{
    PrintIndent();
    PrintName(name);
    wprintf(L" = 0x%X\n", value);

    ++g_Indent;
    size_t zeroFlagIndex = SIZE_MAX;
    for(size_t i = 0; enumItems[i].m_Name != nullptr; ++i)
    {
        if(enumItems[i].m_Value == 0)
        {
            zeroFlagIndex = i;
        }
        else
        {
            if((value & enumItems[i].m_Value) != 0)
            {
                PrintIndent();
                wprintf(L"%s\n", enumItems[i].m_Name);
            }
        }
    }
    if(value == 0 && zeroFlagIndex != SIZE_MAX)
    {
        PrintIndent();
        wprintf(L"%s\n", enumItems[zeroFlagIndex].m_Name);
    }
    --g_Indent;
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS(const D3D12_FEATURE_DATA_D3D12_OPTIONS& options)
{
    Print_BOOL(  L"DoublePrecisionFloatShaderOps       ", options.DoublePrecisionFloatShaderOps);
    Print_BOOL(  L"OutputMergerLogicOp                 ", options.OutputMergerLogicOp);
    PrintEnum(   L"MinPrecisionSupport                 ", options.MinPrecisionSupport, Enum_D3D12_SHADER_MIN_PRECISION_SUPPORT);
    PrintEnum(   L"TiledResourcesTier                  ", options.TiledResourcesTier, Enum_D3D12_TILED_RESOURCES_TIER);
    PrintEnum(   L"ResourceBindingTier                 ", options.ResourceBindingTier, Enum_D3D12_RESOURCE_BINDING_TIER);
    Print_BOOL(  L"PSSpecifiedStencilRefSupported      ", options.PSSpecifiedStencilRefSupported);
    Print_BOOL(  L"TypedUAVLoadAdditionalFormats       ", options.TypedUAVLoadAdditionalFormats);
    Print_BOOL(  L"ROVsSupported                       ", options.ROVsSupported);
    PrintEnum(   L"ConservativeRasterizationTier       ", options.ConservativeRasterizationTier, Enum_D3D12_CONSERVATIVE_RASTERIZATION_TIER);
    Print_uint32(L"MaxGPUVirtualAddressBitsPerResource ", options.MaxGPUVirtualAddressBitsPerResource);
    Print_BOOL(  L"StandardSwizzle64KBSupported        ", options.StandardSwizzle64KBSupported);
    PrintEnum(   L"CrossNodeSharingTier                ", options.CrossNodeSharingTier, Enum_D3D12_CROSS_NODE_SHARING_TIER);
    Print_BOOL(  L"CrossAdapterRowMajorTextureSupported", options.CrossAdapterRowMajorTextureSupported);
    Print_BOOL(  L"VPAndRTArrayIndexFromAnyShaderFeedingRasterizerSupportedWithoutGSEmulation", options.VPAndRTArrayIndexFromAnyShaderFeedingRasterizerSupportedWithoutGSEmulation);
    PrintEnum(   L"ResourceHeapTier                    ", options.ResourceHeapTier, Enum_D3D12_RESOURCE_HEAP_TIER);
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
        PrintEnum(L"pFeatureLevelsRequested", featureLevels.pFeatureLevelsRequested[i], Enum_D3D_FEATURE_LEVEL);
        StepArray();
    }
    EndArray();
    PrintEnum(L"MaxSupportedFeatureLevel", featureLevels.MaxSupportedFeatureLevel, Enum_D3D_FEATURE_LEVEL);
}

static void Print_D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT(const D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT& virtualAddressSupport)
{
    Print_uint32(L"MaxGPUVirtualAddressBitsPerResource", virtualAddressSupport.MaxGPUVirtualAddressBitsPerResource);
    Print_uint32(L"MaxGPUVirtualAddressBitsPerProcess ", virtualAddressSupport.MaxGPUVirtualAddressBitsPerProcess);
}

static void Print_D3D12_FEATURE_DATA_SHADER_MODEL(const D3D12_FEATURE_DATA_SHADER_MODEL& shaderModel)
{
    PrintEnum(L"HighestShaderModel", shaderModel.HighestShaderModel, Enum_D3D_SHADER_MODEL);
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
    PrintEnum(L"HighestVersion", rootSignature.HighestVersion, Enum_D3D_ROOT_SIGNATURE_VERSION);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS2(const D3D12_FEATURE_DATA_D3D12_OPTIONS2& options2)
{
    Print_BOOL(L"DepthBoundsTestSupported       ", options2.DepthBoundsTestSupported);
    PrintEnum (L"ProgrammableSamplePositionsTier", options2.ProgrammableSamplePositionsTier, Enum_D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER);
}

static void Print_D3D12_FEATURE_DATA_SHADER_CACHE(const D3D12_FEATURE_DATA_SHADER_CACHE& shaderCache)
{
    PrintFlags(L"SupportFlags", shaderCache.SupportFlags, Enum_D3D12_SHADER_CACHE_SUPPORT_FLAGS);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS3(const D3D12_FEATURE_DATA_D3D12_OPTIONS3& options3)
{
    Print_BOOL(L"CopyQueueTimestampQueriesSupported", options3.CopyQueueTimestampQueriesSupported);
    Print_BOOL(L"CastingFullyTypedFormatSupported  ", options3.CastingFullyTypedFormatSupported);
    PrintFlags(L"WriteBufferImmediateSupportFlags  ", options3.WriteBufferImmediateSupportFlags, Enum_D3D12_COMMAND_LIST_SUPPORT_FLAGS);
    PrintEnum (L"ViewInstancingTier                ", options3.ViewInstancingTier, Enum_D3D12_VIEW_INSTANCING_TIER);
    Print_BOOL(L"BarycentricsSupported             ", options3.BarycentricsSupported);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS4(const D3D12_FEATURE_DATA_D3D12_OPTIONS4& options4)
{
    Print_BOOL(L"MSAA64KBAlignedTextureSupported", options4.MSAA64KBAlignedTextureSupported);
    PrintEnum(L"SharedResourceCompatibilityTier", options4.SharedResourceCompatibilityTier, Enum_D3D12_SHARED_RESOURCE_COMPATIBILITY_TIER);
    Print_BOOL(L"Native16BitShaderOpsSupported", options4.Native16BitShaderOpsSupported);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS5(const D3D12_FEATURE_DATA_D3D12_OPTIONS5& options5)
{
    Print_BOOL(L"SRVOnlyTiledResourceTier3", options5.SRVOnlyTiledResourceTier3);
    PrintEnum(L"RenderPassesTier", options5.RenderPassesTier, Enum_D3D12_RENDER_PASS_TIER);
    PrintEnum(L"RaytracingTier", options5.RaytracingTier, Enum_D3D12_RAYTRACING_TIER);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS6(const D3D12_FEATURE_DATA_D3D12_OPTIONS6& o)
{
    Print_BOOL(L"AdditionalShadingRatesSupported", o.AdditionalShadingRatesSupported);
    Print_BOOL(L"PerPrimitiveShadingRateSupportedWithViewportIndexing", o.PerPrimitiveShadingRateSupportedWithViewportIndexing);
    PrintEnum(L"VariableShadingRateTier", o.VariableShadingRateTier, Enum_D3D12_VARIABLE_SHADING_RATE_TIER);
    Print_uint32(L"ShadingRateImageTileSize", o.ShadingRateImageTileSize);
    Print_BOOL(L"BackgroundProcessingSupported", o.BackgroundProcessingSupported);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS7(const D3D12_FEATURE_DATA_D3D12_OPTIONS7& o)
{
    PrintEnum(L"MeshShaderTier", o.MeshShaderTier, Enum_D3D12_MESH_SHADER_TIER);
    PrintEnum(L"SamplerFeedbackTier", o.SamplerFeedbackTier, Enum_D3D12_SAMPLER_FEEDBACK_TIER);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS8(const D3D12_FEATURE_DATA_D3D12_OPTIONS8& o)
{
    Print_BOOL(L"UnalignedBlockTexturesSupported", o.UnalignedBlockTexturesSupported);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS9(const D3D12_FEATURE_DATA_D3D12_OPTIONS9& o)
{
    Print_BOOL(L"MeshShaderPipelineStatsSupported", o.MeshShaderPipelineStatsSupported);
    Print_BOOL(L"MeshShaderSupportsFullRangeRenderTargetArrayIndex", o.MeshShaderSupportsFullRangeRenderTargetArrayIndex);
    Print_BOOL(L"AtomicInt64OnTypedResourceSupported", o.AtomicInt64OnTypedResourceSupported);
    Print_BOOL(L"AtomicInt64OnGroupSharedSupported", o.AtomicInt64OnGroupSharedSupported);
    Print_BOOL(L"DerivativesInMeshAndAmplificationShadersSupported", o.DerivativesInMeshAndAmplificationShadersSupported);
    PrintEnum(L"WaveMMATier", o.WaveMMATier, Enum_D3D12_WAVE_MMA_TIER);
}

static void Print_D3D12_FEATURE_DATA_EXISTING_HEAPS(const D3D12_FEATURE_DATA_EXISTING_HEAPS& existingHeaps)
{
    Print_BOOL(L"Supported", existingHeaps.Supported);
}

static void Print_DXGI_QUERY_VIDEO_MEMORY_INFO(const DXGI_QUERY_VIDEO_MEMORY_INFO& videoMemoryInfo)
{
    Print_size(L"Budget", videoMemoryInfo.Budget);
    Print_size(L"CurrentUsage", videoMemoryInfo.CurrentUsage);
    Print_size(L"AvailableForReservation", videoMemoryInfo.AvailableForReservation);
    Print_size(L"CurrentReservation", videoMemoryInfo.CurrentReservation);
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
    PrintFlags  (L"Flags                ", desc.Flags, Enum_DXGI_ADAPTER_FLAG);
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
        // There are IN parameters - need to fill in commandQueuePriority.CommandListType, Priority...
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

        D3D12_FEATURE_DATA_D3D12_OPTIONS6 options6 = {};
        hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &options6, sizeof(options6));
        if(SUCCEEDED(hr))
        {
            wprintf(L"\n");
            wprintf(L"D3D12_FEATURE_DATA_D3D12_OPTIONS6:\n");
            wprintf(L"==================================\n");
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS6(options6);
        }

        D3D12_FEATURE_DATA_D3D12_OPTIONS7 options7 = {};
        hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &options7, sizeof(options7));
        if(SUCCEEDED(hr))
        {
            wprintf(L"\n");
            wprintf(L"D3D12_FEATURE_DATA_D3D12_OPTIONS7:\n");
            wprintf(L"==================================\n");
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS7(options7);
        }

        D3D12_FEATURE_DATA_D3D12_OPTIONS8 options8 = {};
        hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS8, &options8, sizeof(options8));
        if(SUCCEEDED(hr))
        {
            wprintf(L"\n");
            wprintf(L"D3D12_FEATURE_DATA_D3D12_OPTIONS8:\n");
            wprintf(L"==================================\n");
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS8(options8);
        }

        D3D12_FEATURE_DATA_D3D12_OPTIONS9 options9 = {};
        hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS9, &options9, sizeof(options9));
        if(SUCCEEDED(hr))
        {
            wprintf(L"\n");
            wprintf(L"D3D12_FEATURE_DATA_D3D12_OPTIONS9:\n");
            wprintf(L"==================================\n");
            Print_D3D12_FEATURE_DATA_D3D12_OPTIONS9(options9);
        }

        wprintf(L"\n");

        SAFE_RELEASE(device);
    }

    --g_Indent;
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
    if (!LoadLibraries())
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
