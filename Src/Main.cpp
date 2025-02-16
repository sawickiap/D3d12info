/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2025 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#include "NvApiData.hpp"
#include "AgsData.hpp"
#include "AmdDeviceInfoData.hpp"
#include "IntelData.hpp"
#include "VulkanData.hpp"
#include "Utils.hpp"
#include "Enums.hpp"
#include "Printer.hpp"
#include "ReportFormatter/ReportFormatter.hpp"

// For Direct3D 12 Agility SDK
extern "C"
{
#ifdef USE_PREVIEW_AGILITY_SDK
    __declspec(dllexport) extern const UINT D3D12SDKVersion = D3D12_PREVIEW_SDK_VERSION;
    __declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12_preview\\";
#else
    __declspec(dllexport) extern const UINT D3D12SDKVersion = D3D12_SDK_VERSION;
    __declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\";
#endif
 
}

// VendorIDs used for deciding whether to use Vendor specific APIs with each device
enum VENDOR_ID {
    VENDOR_ID_AMD = 0x1002,
    VENDOR_ID_NVIDIA = 0x10de,
    VENDOR_ID_INTEL = 0x8086
};

//#define AUTO_LINK_DX12    // use this on everything before Win10
#if defined(AUTO_LINK_DX12)

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

#else // #if defined(AUTO_LINK_DX12)

typedef HRESULT (WINAPI* PFN_DXGI_CREATE_FACTORY1)(REFIID riid, _COM_Outptr_ void **);

HMODULE g_DxgiLibrary = nullptr;
HMODULE g_Dx12Library = nullptr;

static const D3D_ROOT_SIGNATURE_VERSION HIGHEST_ROOT_SIGNATURE_VERSION = D3D_ROOT_SIGNATURE_VERSION_1_2;
static const D3D_FEATURE_LEVEL FEATURE_LEVELS_ARRAY[] =
{
    D3D_FEATURE_LEVEL_12_2,
    D3D_FEATURE_LEVEL_12_1,
    D3D_FEATURE_LEVEL_12_0,
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
};
static const D3D_FEATURE_LEVEL MAX_FEATURE_LEVEL = D3D_FEATURE_LEVEL_12_2;
static const D3D_FEATURE_LEVEL MIN_FEATURE_LEVEL = D3D_FEATURE_LEVEL_11_0;

const wchar_t* const DYN_LIB_DXGI = L"dxgi.dll";
const wchar_t* const DYN_LIB_DX12 = L"d3d12.dll";

// Why did't Microsoft define this type in their header?!
typedef HRESULT (WINAPI* PFN_D3D12_ENABLE_EXPERIMENTAL_FEATURES)(
    UINT                                    NumFeatures,
    _In_count_(NumFeatures) const IID*     pIIDs,
    _In_opt_count_(NumFeatures) void*      pConfigurationStructs,
    _In_opt_count_(NumFeatures) UINT*      pConfigurationStructSizes);

PFN_DXGI_CREATE_FACTORY1 g_CreateDXGIFactory1;
PFN_D3D12_CREATE_DEVICE g_D3D12CreateDevice;
PFN_D3D12_ENABLE_EXPERIMENTAL_FEATURES g_D3D12EnableExperimentalFeatures; // Optional, can be null.
PFN_D3D12_GET_INTERFACE g_D3D12GetInterface;

#endif // #if defined(AUTO_LINK_DX12)

// Command line flags
static bool g_ShowVersionAndQuit = false;
static bool g_ShowCommandLineSyntaxAndQuit = false;
static bool g_ShowCommandLineSyntaxAndFail = false;
static bool g_ListAdapters = false;
static bool g_ShowAllAdapters = true;
static bool g_SkipSoftwareAdapter = true;
static bool g_UseJsonOutput = false;
static bool g_UseJsonPrettyPrint = true;
static bool g_OutputToFile = false;
static bool g_PrintFormats = false;
static bool g_PrintMetaCommands = false;
static bool g_PrintEnums = false;
static bool g_PureD3D12 = false;
#ifdef USE_PREVIEW_AGILITY_SDK
static bool g_EnableExperimental = true;
#else
static bool g_EnableExperimental = false;
#endif
static bool g_ForceVendorAPI = false;
static bool g_WARP = false;
static std::wstring g_OutputFilePath;

// Derived flags
static bool g_PrintAdaptersAsArray = true;

static wstring LuidToStr(LUID value)
{
    wchar_t s[64];
    swprintf_s(s, L"%08X-%08X", (uint32_t)value.HighPart, (uint32_t)value.LowPart);
    return wstring{s};
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS(const D3D12_FEATURE_DATA_D3D12_OPTIONS& options)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS");
    ReportFormatter::GetInstance().AddFieldBool(L"DoublePrecisionFloatShaderOps", options.DoublePrecisionFloatShaderOps);
    ReportFormatter::GetInstance().AddFieldBool(L"OutputMergerLogicOp", options.OutputMergerLogicOp);
    ReportFormatter::GetInstance().AddFieldEnum(L"MinPrecisionSupport", options.MinPrecisionSupport, Enum_D3D12_SHADER_MIN_PRECISION_SUPPORT);
    ReportFormatter::GetInstance().AddFieldEnum(L"TiledResourcesTier", options.TiledResourcesTier, Enum_D3D12_TILED_RESOURCES_TIER);
    ReportFormatter::GetInstance().AddFieldEnum(L"ResourceBindingTier", options.ResourceBindingTier, Enum_D3D12_RESOURCE_BINDING_TIER);
    ReportFormatter::GetInstance().AddFieldBool(L"PSSpecifiedStencilRefSupported", options.PSSpecifiedStencilRefSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"TypedUAVLoadAdditionalFormats", options.TypedUAVLoadAdditionalFormats);
    ReportFormatter::GetInstance().AddFieldBool(L"ROVsSupported", options.ROVsSupported);
    ReportFormatter::GetInstance().AddFieldEnum(L"ConservativeRasterizationTier", options.ConservativeRasterizationTier, Enum_D3D12_CONSERVATIVE_RASTERIZATION_TIER);
    ReportFormatter::GetInstance().AddFieldUint32(L"MaxGPUVirtualAddressBitsPerResource", options.MaxGPUVirtualAddressBitsPerResource);
    ReportFormatter::GetInstance().AddFieldBool(L"StandardSwizzle64KBSupported", options.StandardSwizzle64KBSupported);
    ReportFormatter::GetInstance().AddFieldEnum(L"CrossNodeSharingTier", options.CrossNodeSharingTier, Enum_D3D12_CROSS_NODE_SHARING_TIER);
    ReportFormatter::GetInstance().AddFieldBool(L"CrossAdapterRowMajorTextureSupported", options.CrossAdapterRowMajorTextureSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"VPAndRTArrayIndexFromAnyShaderFeedingRasterizerSupportedWithoutGSEmulation", options.VPAndRTArrayIndexFromAnyShaderFeedingRasterizerSupportedWithoutGSEmulation);
    ReportFormatter::GetInstance().AddFieldEnum(L"ResourceHeapTier", options.ResourceHeapTier, Enum_D3D12_RESOURCE_HEAP_TIER);
}

static void Print_D3D12_FEATURE_DATA_ARCHITECTURE(const D3D12_FEATURE_DATA_ARCHITECTURE& architecture)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_ARCHITECTURE");
    ReportFormatter::GetInstance().AddFieldUint32(L"NodeIndex", architecture.NodeIndex);
    ReportFormatter::GetInstance().AddFieldBool(L"TileBasedRenderer", architecture.TileBasedRenderer);
    ReportFormatter::GetInstance().AddFieldBool(L"UMA", architecture.UMA);
    ReportFormatter::GetInstance().AddFieldBool(L"CacheCoherentUMA", architecture.CacheCoherentUMA);
}

static void Print_D3D12_FEATURE_DATA_ARCHITECTURE1(const D3D12_FEATURE_DATA_ARCHITECTURE1& architecture1)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_ARCHITECTURE1");
    ReportFormatter::GetInstance().AddFieldUint32(L"NodeIndex", architecture1.NodeIndex);
    ReportFormatter::GetInstance().AddFieldBool(L"TileBasedRenderer", architecture1.TileBasedRenderer);
    ReportFormatter::GetInstance().AddFieldBool(L"UMA", architecture1.UMA);
    ReportFormatter::GetInstance().AddFieldBool(L"CacheCoherentUMA", architecture1.CacheCoherentUMA);
    ReportFormatter::GetInstance().AddFieldBool(L"IsolatedMMU", architecture1.IsolatedMMU);
}

static void Print_D3D12_FEATURE_DATA_FEATURE_LEVELS(const D3D12_FEATURE_DATA_FEATURE_LEVELS& featureLevels)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_FEATURE_LEVELS");

    // NumFeatureLevels and pFeatureLevelsRequested are IN parameters
    // They let the app to specify what enum values does the app expect
    // So same API can be used when new feature levels are added in the future
    // No need to print those IN parameters here
    ReportFormatter::GetInstance().AddFieldEnum(L"MaxSupportedFeatureLevel", featureLevels.MaxSupportedFeatureLevel, Enum_D3D_FEATURE_LEVEL);
}

static void Print_D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT(const D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT& virtualAddressSupport)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT");
    ReportFormatter::GetInstance().AddFieldUint32(L"MaxGPUVirtualAddressBitsPerResource", virtualAddressSupport.MaxGPUVirtualAddressBitsPerResource);
    ReportFormatter::GetInstance().AddFieldUint32(L"MaxGPUVirtualAddressBitsPerProcess", virtualAddressSupport.MaxGPUVirtualAddressBitsPerProcess);
}

static void Print_D3D12_FEATURE_DATA_SHADER_MODEL(const D3D12_FEATURE_DATA_SHADER_MODEL& shaderModel)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_SHADER_MODEL");
    ReportFormatter::GetInstance().AddFieldEnum(L"HighestShaderModel", shaderModel.HighestShaderModel, Enum_D3D_SHADER_MODEL);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS1(const D3D12_FEATURE_DATA_D3D12_OPTIONS1& options1)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS1");
    ReportFormatter::GetInstance().AddFieldBool(L"WaveOps", options1.WaveOps);
    ReportFormatter::GetInstance().AddFieldUint32(L"WaveLaneCountMin", options1.WaveLaneCountMin);
    ReportFormatter::GetInstance().AddFieldUint32(L"WaveLaneCountMax", options1.WaveLaneCountMax);
    ReportFormatter::GetInstance().AddFieldUint32(L"TotalLaneCount", options1.TotalLaneCount);
    ReportFormatter::GetInstance().AddFieldBool(L"ExpandedComputeResourceStates", options1.ExpandedComputeResourceStates);
    ReportFormatter::GetInstance().AddFieldBool(L"Int64ShaderOps", options1.Int64ShaderOps);
}

static void Print_D3D12_FEATURE_DATA_ROOT_SIGNATURE(const D3D12_FEATURE_DATA_ROOT_SIGNATURE& rootSignature)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_ROOT_SIGNATURE");
    ReportFormatter::GetInstance().AddFieldEnum(L"HighestVersion", rootSignature.HighestVersion, Enum_D3D_ROOT_SIGNATURE_VERSION);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS2(const D3D12_FEATURE_DATA_D3D12_OPTIONS2& options2)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS2");
    ReportFormatter::GetInstance().AddFieldBool(L"DepthBoundsTestSupported", options2.DepthBoundsTestSupported);
    ReportFormatter::GetInstance().AddFieldEnum(L"ProgrammableSamplePositionsTier", options2.ProgrammableSamplePositionsTier, Enum_D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER);
}

static void Print_D3D12_FEATURE_DATA_SHADER_CACHE(const D3D12_FEATURE_DATA_SHADER_CACHE& shaderCache)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_SHADER_CACHE");
    ReportFormatter::GetInstance().AddFieldFlags(L"SupportFlags", shaderCache.SupportFlags, Enum_D3D12_SHADER_CACHE_SUPPORT_FLAGS);
}

static void Print_D3D12_FEATURE_DATA_COMMAND_QUEUE_PRIORITY(const std::array<bool, 9>& commandQueuePriority)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_COMMAND_QUEUE_PRIORITY");
    ReportFormatter::GetInstance().AddFieldBool(L"TYPE_DIRECT.PRIORITY_NORMAL.PriorityForTypeIsSupported", commandQueuePriority[0]);
    ReportFormatter::GetInstance().AddFieldBool(L"TYPE_DIRECT.PRIORITY_HIGH.PriorityForTypeIsSupported", commandQueuePriority[1]);
    ReportFormatter::GetInstance().AddFieldBool(L"TYPE_DIRECT.PRIORITY_GLOBAL_REALTIME.PriorityForTypeIsSupported", commandQueuePriority[2]);
    ReportFormatter::GetInstance().AddFieldBool(L"TYPE_COMPUTE.PRIORITY_NORMAL.PriorityForTypeIsSupported", commandQueuePriority[3]);
    ReportFormatter::GetInstance().AddFieldBool(L"TYPE_COMPUTE.PRIORITY_HIGH.PriorityForTypeIsSupported", commandQueuePriority[4]);
    ReportFormatter::GetInstance().AddFieldBool(L"TYPE_COMPUTE.PRIORITY_GLOBAL_REALTIME.PriorityForTypeIsSupported", commandQueuePriority[5]);
    ReportFormatter::GetInstance().AddFieldBool(L"TYPE_COPY.PRIORITY_NORMAL.PriorityForTypeIsSupported", commandQueuePriority[6]);
    ReportFormatter::GetInstance().AddFieldBool(L"TYPE_COPY.PRIORITY_HIGH.PriorityForTypeIsSupported", commandQueuePriority[7]);
    ReportFormatter::GetInstance().AddFieldBool(L"TYPE_COPY.PRIORITY_GLOBAL_REALTIME.PriorityForTypeIsSupported", commandQueuePriority[8]);
}

static void Print_D3D12_FEATURE_DATA_SERIALIZATION(const D3D12_FEATURE_DATA_SERIALIZATION& serialization)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_SERIALIZATION");
    ReportFormatter::GetInstance().AddFieldEnum(L"HeapSerializationTier", serialization.HeapSerializationTier, Enum_D3D12_HEAP_SERIALIZATION_TIER);
}

static void Print_D3D12_FEATURE_CROSS_NODE(const D3D12_FEATURE_DATA_CROSS_NODE& crossNode)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_CROSS_NODE");
    ReportFormatter::GetInstance().AddFieldEnum(L"SharingTier", crossNode.SharingTier, Enum_D3D12_CROSS_NODE_SHARING_TIER);
    ReportFormatter::GetInstance().AddFieldBool(L"AtomicShaderInstructions", crossNode.AtomicShaderInstructions);
}

static void Print_D3D12_FEATURE_PREDICATION(const D3D12_FEATURE_DATA_PREDICATION& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_PREDICATION");
    ReportFormatter::GetInstance().AddFieldBool(L"Supported", o.Supported);
}

static void Print_D3D12_FEATURE_HARDWARE_COPY(const D3D12_FEATURE_DATA_HARDWARE_COPY& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_HARDWARE_COPY");
    ReportFormatter::GetInstance().AddFieldBool(L"Supported", o.Supported);
}

#ifdef USE_PREVIEW_AGILITY_SDK
static void Print_D3D12_FEATURE_DATA_APPLICATION_SPECIFIC_DRIVER_STATE(const D3D12_FEATURE_DATA_APPLICATION_SPECIFIC_DRIVER_STATE& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_APPLICATION_SPECIFIC_DRIVER_STATE");
    ReportFormatter::GetInstance().AddFieldBool(L"Supported", o.Supported);
}
#endif // #ifdef USE_PREVIEW_AGILITY_SDK

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS3(const D3D12_FEATURE_DATA_D3D12_OPTIONS3& options3)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS3");
    ReportFormatter::GetInstance().AddFieldBool(L"CopyQueueTimestampQueriesSupported", options3.CopyQueueTimestampQueriesSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"CastingFullyTypedFormatSupported", options3.CastingFullyTypedFormatSupported);
    ReportFormatter::GetInstance().AddFieldFlags(L"WriteBufferImmediateSupportFlags", options3.WriteBufferImmediateSupportFlags, Enum_D3D12_COMMAND_LIST_SUPPORT_FLAGS);
    ReportFormatter::GetInstance().AddFieldEnum(L"ViewInstancingTier", options3.ViewInstancingTier, Enum_D3D12_VIEW_INSTANCING_TIER);
    ReportFormatter::GetInstance().AddFieldBool(L"BarycentricsSupported", options3.BarycentricsSupported);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS4(const D3D12_FEATURE_DATA_D3D12_OPTIONS4& options4)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS4");
    ReportFormatter::GetInstance().AddFieldBool(L"MSAA64KBAlignedTextureSupported", options4.MSAA64KBAlignedTextureSupported);
    ReportFormatter::GetInstance().AddFieldEnum(L"SharedResourceCompatibilityTier", options4.SharedResourceCompatibilityTier, Enum_D3D12_SHARED_RESOURCE_COMPATIBILITY_TIER);
    ReportFormatter::GetInstance().AddFieldBool(L"Native16BitShaderOpsSupported", options4.Native16BitShaderOpsSupported);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS5(const D3D12_FEATURE_DATA_D3D12_OPTIONS5& options5)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS5");
    ReportFormatter::GetInstance().AddFieldBool(L"SRVOnlyTiledResourceTier3", options5.SRVOnlyTiledResourceTier3);
    ReportFormatter::GetInstance().AddFieldEnum(L"RenderPassesTier", options5.RenderPassesTier, Enum_D3D12_RENDER_PASS_TIER);
    ReportFormatter::GetInstance().AddFieldEnum(L"RaytracingTier", options5.RaytracingTier, Enum_D3D12_RAYTRACING_TIER);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS6(const D3D12_FEATURE_DATA_D3D12_OPTIONS6& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS6");
    ReportFormatter::GetInstance().AddFieldBool(L"AdditionalShadingRatesSupported", o.AdditionalShadingRatesSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"PerPrimitiveShadingRateSupportedWithViewportIndexing", o.PerPrimitiveShadingRateSupportedWithViewportIndexing);
    ReportFormatter::GetInstance().AddFieldEnum(L"VariableShadingRateTier", o.VariableShadingRateTier, Enum_D3D12_VARIABLE_SHADING_RATE_TIER);
    ReportFormatter::GetInstance().AddFieldUint32(L"ShadingRateImageTileSize", o.ShadingRateImageTileSize);
    ReportFormatter::GetInstance().AddFieldBool(L"BackgroundProcessingSupported", o.BackgroundProcessingSupported);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS7(const D3D12_FEATURE_DATA_D3D12_OPTIONS7& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS7");
    ReportFormatter::GetInstance().AddFieldEnum(L"MeshShaderTier", o.MeshShaderTier, Enum_D3D12_MESH_SHADER_TIER);
    ReportFormatter::GetInstance().AddFieldEnum(L"SamplerFeedbackTier", o.SamplerFeedbackTier, Enum_D3D12_SAMPLER_FEEDBACK_TIER);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS8(const D3D12_FEATURE_DATA_D3D12_OPTIONS8& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS8");
    ReportFormatter::GetInstance().AddFieldBool(L"UnalignedBlockTexturesSupported", o.UnalignedBlockTexturesSupported);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS9(const D3D12_FEATURE_DATA_D3D12_OPTIONS9& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS9");
    ReportFormatter::GetInstance().AddFieldBool(L"MeshShaderPipelineStatsSupported", o.MeshShaderPipelineStatsSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"MeshShaderSupportsFullRangeRenderTargetArrayIndex", o.MeshShaderSupportsFullRangeRenderTargetArrayIndex);
    ReportFormatter::GetInstance().AddFieldBool(L"AtomicInt64OnTypedResourceSupported", o.AtomicInt64OnTypedResourceSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"AtomicInt64OnGroupSharedSupported", o.AtomicInt64OnGroupSharedSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"DerivativesInMeshAndAmplificationShadersSupported", o.DerivativesInMeshAndAmplificationShadersSupported);
    ReportFormatter::GetInstance().AddFieldEnum(L"WaveMMATier", o.WaveMMATier, Enum_D3D12_WAVE_MMA_TIER);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS10(const D3D12_FEATURE_DATA_D3D12_OPTIONS10& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS10");
    ReportFormatter::GetInstance().AddFieldBool(L"VariableRateShadingSumCombinerSupported", o.VariableRateShadingSumCombinerSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"MeshShaderPerPrimitiveShadingRateSupported", o.MeshShaderPerPrimitiveShadingRateSupported);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS11(const D3D12_FEATURE_DATA_D3D12_OPTIONS11& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS11");
    ReportFormatter::GetInstance().AddFieldBool(L"AtomicInt64OnDescriptorHeapResourceSupported", o.AtomicInt64OnDescriptorHeapResourceSupported);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS12(const D3D12_FEATURE_DATA_D3D12_OPTIONS12& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS12");
    ReportFormatter::GetInstance().AddFieldEnumSigned(L"MSPrimitivesPipelineStatisticIncludesCulledPrimitives", o.MSPrimitivesPipelineStatisticIncludesCulledPrimitives, Enum_D3D12_TRI_STATE);
    ReportFormatter::GetInstance().AddFieldBool(L"EnhancedBarriersSupported", o.EnhancedBarriersSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"RelaxedFormatCastingSupported", o.RelaxedFormatCastingSupported);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS13(const D3D12_FEATURE_DATA_D3D12_OPTIONS13& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS13");
    ReportFormatter::GetInstance().AddFieldBool(L"UnrestrictedBufferTextureCopyPitchSupported", o.UnrestrictedBufferTextureCopyPitchSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"UnrestrictedVertexElementAlignmentSupported", o.UnrestrictedVertexElementAlignmentSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"InvertedViewportHeightFlipsYSupported", o.InvertedViewportHeightFlipsYSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"InvertedViewportDepthFlipsZSupported", o.InvertedViewportDepthFlipsZSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"TextureCopyBetweenDimensionsSupported", o.TextureCopyBetweenDimensionsSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"AlphaBlendFactorSupported", o.AlphaBlendFactorSupported);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS14(const D3D12_FEATURE_DATA_D3D12_OPTIONS14& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS14");
    ReportFormatter::GetInstance().AddFieldBool(L"AdvancedTextureOpsSupported", o.AdvancedTextureOpsSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"WriteableMSAATexturesSupported", o.WriteableMSAATexturesSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"IndependentFrontAndBackStencilRefMaskSupported", o.IndependentFrontAndBackStencilRefMaskSupported);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS15(const D3D12_FEATURE_DATA_D3D12_OPTIONS15& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS15");
    ReportFormatter::GetInstance().AddFieldBool(L"TriangleFanSupported", o.TriangleFanSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"DynamicIndexBufferStripCutSupported", o.DynamicIndexBufferStripCutSupported);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS16(const D3D12_FEATURE_DATA_D3D12_OPTIONS16& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS16");
    ReportFormatter::GetInstance().AddFieldBool(L"DynamicDepthBiasSupported", o.DynamicDepthBiasSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"GPUUploadHeapSupported", o.GPUUploadHeapSupported);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS17(const D3D12_FEATURE_DATA_D3D12_OPTIONS17& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS17");
    ReportFormatter::GetInstance().AddFieldBool(L"NonNormalizedCoordinateSamplersSupported", o.NonNormalizedCoordinateSamplersSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"ManualWriteTrackingResourceSupported", o.ManualWriteTrackingResourceSupported);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS18(const D3D12_FEATURE_DATA_D3D12_OPTIONS18& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS18");
    ReportFormatter::GetInstance().AddFieldBool(L"RenderPassesValid", o.RenderPassesValid);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS19(const D3D12_FEATURE_DATA_D3D12_OPTIONS19& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS19");
    ReportFormatter::GetInstance().AddFieldBool(L"MismatchingOutputDimensionsSupported", o.MismatchingOutputDimensionsSupported);
    ReportFormatter::GetInstance().AddFieldUint32(L"SupportedSampleCountsWithNoOutputs", o.SupportedSampleCountsWithNoOutputs);
    ReportFormatter::GetInstance().AddFieldBool(L"PointSamplingAddressesNeverRoundUp", o.PointSamplingAddressesNeverRoundUp);
    ReportFormatter::GetInstance().AddFieldBool(L"RasterizerDesc2Supported", o.RasterizerDesc2Supported);
    ReportFormatter::GetInstance().AddFieldBool(L"NarrowQuadrilateralLinesSupported", o.NarrowQuadrilateralLinesSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"AnisoFilterWithPointMipSupported", o.AnisoFilterWithPointMipSupported);
    ReportFormatter::GetInstance().AddFieldUint32(L"MaxSamplerDescriptorHeapSize", o.MaxSamplerDescriptorHeapSize);
    ReportFormatter::GetInstance().AddFieldUint32(L"MaxSamplerDescriptorHeapSizeWithStaticSamplers", o.MaxSamplerDescriptorHeapSizeWithStaticSamplers);
    ReportFormatter::GetInstance().AddFieldUint32(L"MaxViewDescriptorHeapSize", o.MaxViewDescriptorHeapSize);
    ReportFormatter::GetInstance().AddFieldBool(L"ComputeOnlyCustomHeapSupported", o.ComputeOnlyCustomHeapSupported);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS20(const D3D12_FEATURE_DATA_D3D12_OPTIONS20& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS20");
    ReportFormatter::GetInstance().AddFieldBool(L"ComputeOnlyWriteWatchSupported", o.ComputeOnlyWriteWatchSupported);
    ReportFormatter::GetInstance().AddFieldEnum(L"RecreateAtTier", o.RecreateAtTier, Enum_D3D12_RECREATE_AT_TIER);
}

static void Print_D3D12_FEATURE_DATA_D3D12_OPTIONS21(const D3D12_FEATURE_DATA_D3D12_OPTIONS21& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_D3D12_OPTIONS21");
    ReportFormatter::GetInstance().AddFieldEnum(L"WorkGraphsTier", o.WorkGraphsTier, Enum_D3D12_WORK_GRAPHS_TIER);
    ReportFormatter::GetInstance().AddFieldEnum(L"ExecuteIndirectTier", o.ExecuteIndirectTier, Enum_D3D12_EXECUTE_INDIRECT_TIER);
    ReportFormatter::GetInstance().AddFieldBool(L"SampleCmpGradientAndBiasSupported", o.SampleCmpGradientAndBiasSupported);
    ReportFormatter::GetInstance().AddFieldBool(L"ExtendedCommandInfoSupported", o.ExtendedCommandInfoSupported);
}

static void Print_D3D12_FEATURE_DATA_BYTECODE_BYPASS_HASH_SUPPORTED(const D3D12_FEATURE_DATA_BYTECODE_BYPASS_HASH_SUPPORTED& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_BYTECODE_BYPASS_HASH_SUPPORTED");
    ReportFormatter::GetInstance().AddFieldBool(L"Supported", o.Supported);
}

#ifdef USE_PREVIEW_AGILITY_SDK
static void Print_D3D12_FEATURE_DATA_TIGHT_ALIGNMENT(const D3D12_FEATURE_DATA_TIGHT_ALIGNMENT& o)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_TIGHT_ALIGNMENT");
    ReportFormatter::GetInstance().AddFieldEnum(L"SupportTier", o.SupportTier, Enum_D3D12_TIGHT_ALIGNMENT_TIER);
}
#endif // #ifdef USE_PREVIEW_AGILITY_SDK

static void Print_D3D12_FEATURE_DATA_EXISTING_HEAPS(const D3D12_FEATURE_DATA_EXISTING_HEAPS& existingHeaps)
{
    ReportScopeObject scope(L"D3D12_FEATURE_DATA_EXISTING_HEAPS");
    ReportFormatter::GetInstance().AddFieldBool(L"Supported", existingHeaps.Supported);
}

static void Print_DXGI_QUERY_VIDEO_MEMORY_INFO(const DXGI_QUERY_VIDEO_MEMORY_INFO& videoMemoryInfo)
{
    // Not printing videoMemoryInfo.CurrentUsage, videoMemoryInfo.CurrentReservation.
    ReportFormatter::GetInstance().AddFieldSize(L"Budget", videoMemoryInfo.Budget);
    ReportFormatter::GetInstance().AddFieldSize(L"AvailableForReservation", videoMemoryInfo.AvailableForReservation);
}

static wstring MakeBuildDateTime()
{
    wchar_t s[128];
    swprintf_s(s, L"%hs, %hs", __DATE__, __TIME__);
    return wstring{s};
}

#ifdef _DEBUG
    static const wchar_t* const CONFIG_STR = L"Debug";
#else
    static const wchar_t* const CONFIG_STR = L"Release";
#endif

#ifdef _WIN64
    static const wchar_t* const CONFIG_BIT_STR = L"64-bit";
#else
    static const wchar_t* const CONFIG_BIT_STR = L"32-bit";
#endif

static wstring MakeCurrentDate()
{
    std::time_t time = std::time(nullptr);
    std::tm time2;
    localtime_s(&time2, &time);
    wchar_t dateStr[32];
    std::wcsftime(dateStr, _countof(dateStr), L"%Y-%m-%d", &time2);
    return wstring{dateStr};
}

static void PrintVersionHeader()
{
#ifdef USE_PREVIEW_AGILITY_SDK
    const wchar_t* const AGILITY_SDK_NOTE = L" (preview Agility SDK)";
#else
    const wchar_t* const AGILITY_SDK_NOTE = L"";
#endif
    Printer::PrintString(L"============================\n");
    Printer::PrintFormat(L"D3D12INFO {}{}\n", std::make_wformat_args(PROGRAM_VERSION, AGILITY_SDK_NOTE));
    Printer::PrintFormat(L"BuildDate: {}\n", std::make_wformat_args(MakeBuildDateTime()));
    Printer::PrintFormat(L"Configuration: {}, {}\n", std::make_wformat_args(CONFIG_STR, CONFIG_BIT_STR));
    Printer::PrintString(L"============================");
}

static void PrintVersionData()
{
    if (IsOutputConsole())
    {
        PrintVersionHeader();
        Printer::PrintNewLine();
        Printer::PrintNewLine();
    }

    ReportScopeObject scope(OutputSpecificString(L"General", L"Header"));

    if (IsOutputJson())
    {
        ReportFormatter::GetInstance().AddFieldString(L"Program", L"D3d12info");
        ReportFormatter::GetInstance().AddFieldString(L"Version", PROGRAM_VERSION);
        ReportFormatter::GetInstance().AddFieldString(L"Build Date", MakeBuildDateTime());
        ReportFormatter::GetInstance().AddFieldString(L"Configuration", CONFIG_STR);
        ReportFormatter::GetInstance().AddFieldString(L"Configuration bits", CONFIG_BIT_STR);
    }
    ReportFormatter::GetInstance().AddFieldString(L"Generated on", MakeCurrentDate().c_str());
#ifdef USE_PREVIEW_AGILITY_SDK
    if (IsOutputJson())
    {
        ReportFormatter::GetInstance().AddFieldBool(L"Using preview Agility SDK", true);
    }
    ReportFormatter::GetInstance().AddFieldUint32(L"D3D12_PREVIEW_SDK_VERSION", uint32_t(D3D12SDKVersion));
#else
    if (IsOutputJson())
    {
        ReportFormatter::GetInstance().AddFieldBool(L"Using preview Agility SDK", false);
    }
    ReportFormatter::GetInstance().AddFieldUint32(L"D3D12_SDK_VERSION", uint32_t(D3D12SDKVersion));
#endif

    if(!g_PureD3D12)
    {
#if USE_NVAPI
        NvAPI_Inititalize_RAII::PrintStaticParams();
#endif
#if USE_AGS
        AGS_Initialize_RAII::PrintStaticParams();
#endif
#if USE_AMD_DEVICE_INFO
        AmdDeviceInfo_Initialize_RAII::PrintStaticParams();
#endif
#if USE_VULKAN
        Vulkan_Initialize_RAII::PrintStaticParams();
#endif
#if USE_INTEL_GPUDETECT
        IntelData::PrintStaticParams();
#endif
    }
}

static void PrintEnums()
{
    ReportScopeObject scope(L"Enums");

    EnumCollection& enumCollection = EnumCollection::GetInstance();
    for(const auto it : enumCollection.m_Enums)
    {
        ReportScopeObject scope2(std::wstring(it.first.data(), it.first.length()));

        for(const EnumItem* item = it.second; item->m_Name != nullptr; ++item)
        {
            ReportFormatter::GetInstance().AddFieldUint32(item->m_Name, item->m_Value);
        }
    }
}

static void PrintOsVersionInfo()
{
    ReportScopeObject scope(L"OS Info");
    HMODULE m = GetModuleHandle(L"ntdll.dll");
    if(!m)
    {
        ReportFormatter::GetInstance().AddFieldString(L"Windows version", L"Unknown");
        return;
    }

    typedef int32_t(WINAPI * RtlGetVersionFunc)(OSVERSIONINFOEX*);
    RtlGetVersionFunc RtlGetVersion = (RtlGetVersionFunc)GetProcAddress(m, "RtlGetVersion");
    if(!RtlGetVersion)
    {
        ReportFormatter::GetInstance().AddFieldString(L"Windows version", L"Unknown");
        return;
    }

    OSVERSIONINFOEX osVersionInfo = {sizeof(osVersionInfo)};
    // Documentation says it always returns success.
    RtlGetVersion(&osVersionInfo);

    ReportFormatter::GetInstance().AddFieldString(L"Windows version", std::format(L"{}.{}.{}",
        osVersionInfo.dwMajorVersion, osVersionInfo.dwMinorVersion,
        osVersionInfo.dwBuildNumber));
}

static void PrintDXGIFeatureInfo()
{
    ReportScopeObject scope(L"DXGI_FEATURE");
    ComPtr<IDXGIFactory5> dxgiFactory = nullptr;
    HRESULT hr;
#if defined(AUTO_LINK_DX12)
    hr = ::CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
#else
    hr = g_CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
#endif
    if (FAILED(hr) || !dxgiFactory)
        return;
    BOOL allowTearing = FALSE;
    hr = dxgiFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING,
                                          &allowTearing, sizeof(allowTearing));
    if(SUCCEEDED(hr))
    {
        ReportFormatter::GetInstance().AddFieldBool(L"DXGI_FEATURE_PRESENT_ALLOW_TEARING", allowTearing);
    }
}

static void PrintSystemMemoryInfo()
{
    ReportScopeObject scope(L"System memory");
    
    if(uint64_t physicallyInstalledSystemMemory = 0;
        GetPhysicallyInstalledSystemMemory(&physicallyInstalledSystemMemory))
        ReportFormatter::GetInstance().AddFieldSizeKilobytes(L"GetPhysicallyInstalledSystemMemory", physicallyInstalledSystemMemory);

    if(MEMORYSTATUSEX memStatEx = {sizeof(MEMORYSTATUSEX)};
        GlobalMemoryStatusEx(&memStatEx))
    {
        ReportFormatter::GetInstance().AddFieldSize(L"MEMORYSTATUSEX::ullTotalPhys", memStatEx.ullTotalPhys);
        ReportFormatter::GetInstance().AddFieldSize(L"MEMORYSTATUSEX::ullTotalPageFile", memStatEx.ullTotalPageFile);
        ReportFormatter::GetInstance().AddFieldSize(L"MEMORYSTATUSEX::ullTotalVirtual", memStatEx.ullTotalVirtual);
    }
}

static void EnableExperimentalFeatures()
{
    if(g_D3D12EnableExperimentalFeatures == nullptr || !g_EnableExperimental)
        return;

#ifdef USE_PREVIEW_AGILITY_SDK
    static const UUID FEATURE_UUIDS[] = {
        D3D12ExperimentalShaderModels,
        D3D12TiledResourceTier4,
        D3D12StateObjectsExperiment };
    static const wchar_t* FEATURE_NAMES[] = {
        L"D3D12ExperimentalShaderModels",
        L"D3D12TiledResourceTier4",
        L"D3D12StateObjectsExperiment" };
#else
    static const UUID FEATURE_UUIDS[] = {
        D3D12ExperimentalShaderModels,
        D3D12TiledResourceTier4 };
    static const wchar_t* FEATURE_NAMES[] = {
        L"D3D12ExperimentalShaderModels",
        L"D3D12TiledResourceTier4" };
#endif // #ifdef USE_PREVIEW_AGILITY_SDK
    constexpr size_t FEATURE_COUNT = _countof(FEATURE_UUIDS);
    uint32_t featureBitMask = (1 << FEATURE_COUNT) - 1;

    void* configStructs[FEATURE_COUNT] = {};
    UINT configStructSizes[FEATURE_COUNT] = {};

    while(featureBitMask != 0)
    {
        UUID featuresToEnable[FEATURE_COUNT];
        uint32_t featuresToEnableCount = 0;
        for(size_t featureIndex = 0; featureIndex < FEATURE_COUNT; ++featureIndex)
        {
            if((featureBitMask & (1u << featureIndex)) != 0)
                featuresToEnable[featuresToEnableCount++] = FEATURE_UUIDS[featureIndex];
        }

        if(SUCCEEDED(g_D3D12EnableExperimentalFeatures(featuresToEnableCount, featuresToEnable, configStructs, configStructSizes)))
            break;
        --featureBitMask;
    }

    if(featureBitMask != 0) // Means enablement succeeded.
    {
        std::vector<std::wstring> enabledFeatures;

        for(size_t featureIndex = 0; featureIndex < FEATURE_COUNT; ++featureIndex)
        {
            if((featureBitMask & (1u << featureIndex)) != 0)
            {
                enabledFeatures.push_back(FEATURE_NAMES[featureIndex]);
            }
        }

        std::wstring enabledFeaturesStr = std::accumulate(enabledFeatures.begin(), enabledFeatures.end(), std::wstring{},
            [](const std::wstring& a, const std::wstring& b) { return a.empty() ? b : a + L"," + b; });
        ReportFormatter::GetInstance().AddFieldString(L"D3D12EnableExperimentalFeatures", enabledFeaturesStr);
    }
}

static void PrintAdapterDescMembers(const DXGI_ADAPTER_DESC& desc)
{
    ReportFormatter::GetInstance().AddFieldString(L"Description", desc.Description);
    ReportFormatter::GetInstance().AddFieldVendorId(L"VendorId", desc.VendorId);
    ReportFormatter::GetInstance().AddFieldHex32(L"DeviceId", desc.DeviceId);
    ReportFormatter::GetInstance().AddFieldSubsystemId(L"SubSysId", desc.SubSysId);
    ReportFormatter::GetInstance().AddFieldHex32(L"Revision", desc.Revision);
    ReportFormatter::GetInstance().AddFieldSize(L"DedicatedVideoMemory", desc.DedicatedVideoMemory);
    ReportFormatter::GetInstance().AddFieldSize(L"DedicatedSystemMemory", desc.DedicatedSystemMemory);
    ReportFormatter::GetInstance().AddFieldSize(L"SharedSystemMemory", desc.SharedSystemMemory);
    ReportFormatter::GetInstance().AddFieldString(L"AdapterLuid", LuidToStr(desc.AdapterLuid).c_str());
}

static void PrintAdapterDesc1Members(const DXGI_ADAPTER_DESC1& desc1)
{
    PrintAdapterDescMembers((const DXGI_ADAPTER_DESC&)desc1);
    ReportFormatter::GetInstance().AddFieldFlags(L"Flags", desc1.Flags, Enum_DXGI_ADAPTER_FLAG);
}

static void PrintAdapterDesc2Members(const DXGI_ADAPTER_DESC2& desc2)
{
    PrintAdapterDesc1Members((const DXGI_ADAPTER_DESC1&)desc2);
    ReportFormatter::GetInstance().AddFieldEnum(L"GraphicsPreemptionGranularity", desc2.GraphicsPreemptionGranularity, Enum_DXGI_GRAPHICS_PREEMPTION_GRANULARITY);
    ReportFormatter::GetInstance().AddFieldEnum(L"ComputePreemptionGranularity", desc2.ComputePreemptionGranularity, Enum_DXGI_COMPUTE_PREEMPTION_GRANULARITY);
}

static void PrintAdapterDesc(const DXGI_ADAPTER_DESC& desc)
{
    ReportScopeObject scope(L"DXGI_ADAPTER_DESC");
    PrintAdapterDescMembers(desc);
}

static void PrintAdapterDesc1(const DXGI_ADAPTER_DESC1& desc1)
{
    ReportScopeObject scope(L"DXGI_ADAPTER_DESC1");
    PrintAdapterDesc1Members(desc1);
}

static void PrintAdapterDesc2(const DXGI_ADAPTER_DESC2& desc2)
{
    ReportScopeObject scope(L"DXGI_ADAPTER_DESC2");
    PrintAdapterDesc2Members(desc2);
}

static void PrintAdapterDesc3(const DXGI_ADAPTER_DESC3& desc3)
{
    ReportScopeObject scope(L"DXGI_ADAPTER_DESC3");
    // Same members as DESC2. They only added new items to Flags.
    PrintAdapterDesc2Members((const DXGI_ADAPTER_DESC2&)desc3);
}

static void PrintAdapterDesc(IDXGIAdapter* adapter)
{
    if(ComPtr<IDXGIAdapter4> adapter4; SUCCEEDED(adapter->QueryInterface(IID_PPV_ARGS(&adapter4))))
    {
        if(DXGI_ADAPTER_DESC3 desc3; SUCCEEDED(adapter4->GetDesc3(&desc3)))
            PrintAdapterDesc3(desc3);
    }
    else if(ComPtr<IDXGIAdapter2> adapter2; SUCCEEDED(adapter->QueryInterface(IID_PPV_ARGS(&adapter2))))
    {
        if(DXGI_ADAPTER_DESC2 desc2; SUCCEEDED(adapter2->GetDesc2(&desc2)))
            PrintAdapterDesc2(desc2);
    }
    else if(ComPtr<IDXGIAdapter1> adapter1; SUCCEEDED(adapter->QueryInterface(IID_PPV_ARGS(&adapter1))))
    {
        if(DXGI_ADAPTER_DESC1 desc1; SUCCEEDED(adapter1->GetDesc1(&desc1)))
            PrintAdapterDesc1(desc1);
    }
    else if(DXGI_ADAPTER_DESC desc; SUCCEEDED(adapter->GetDesc(&desc)))
    {
        PrintAdapterDesc(desc);
    }
}

static void PrintAdapterMemoryInfo(IDXGIAdapter* adapter)
{
    ComPtr<IDXGIAdapter3> adapter3;
    if(SUCCEEDED(adapter->QueryInterface<IDXGIAdapter3>(&adapter3)))
    {
        for(uint32_t memorySegmentGroup = 0; memorySegmentGroup < 2; ++memorySegmentGroup)
        {
            DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo = {};
            if(SUCCEEDED(adapter3->QueryVideoMemoryInfo(0, (DXGI_MEMORY_SEGMENT_GROUP)memorySegmentGroup, &videoMemoryInfo)))
            {
                const wchar_t* structName = nullptr;
                switch(memorySegmentGroup)
                {
                case 0:
                    structName = L"DXGI_QUERY_VIDEO_MEMORY_INFO[DXGI_MEMORY_SEGMENT_GROUP_LOCAL]";
                    break;
                case 1:
                    structName = L"DXGI_QUERY_VIDEO_MEMORY_INFO[DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL]";
                    break;
                default:
                    assert(0);
                }
                {
                    ReportScopeObject scope(structName);
                    Print_DXGI_QUERY_VIDEO_MEMORY_INFO(videoMemoryInfo);
                }
            }
        }
    }
}

static void PrintAdapterInterfaceSupport(IDXGIAdapter* adapter)
{
    if(LARGE_INTEGER i; SUCCEEDED(adapter->CheckInterfaceSupport(__uuidof(IDXGIDevice), &i)))
    {
        ReportScopeObject scope(L"CheckInterfaceSupport");
        ReportFormatter::GetInstance().AddFieldMicrosoftVersion(L"UMDVersion", i.QuadPart);
    }
}

static void PrintAdapterData(IDXGIAdapter* adapter)
{
    assert(adapter != nullptr);
    PrintAdapterDesc(adapter);
    PrintAdapterMemoryInfo(adapter);
    PrintAdapterInterfaceSupport(adapter);
}

enum class FormatSupportResult { Ok, Failed, Crashed };

static FormatSupportResult CheckFormatSupport(ID3D12Device* device, D3D12_FEATURE_DATA_FORMAT_SUPPORT& formatSupport)
{
    __try
    {
        const HRESULT hr = device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, UINT(sizeof formatSupport));
        return SUCCEEDED(hr) ? FormatSupportResult::Ok : FormatSupportResult::Failed;
    }
    // This is needed because latest (as of November 2023) AMD drivers crash when calling:
    // ID3D12Device::CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, DXGI_FORMAT_A4B4G4R4_UNORM)
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return FormatSupportResult::Crashed;
    }
}

static void PrintFormatInformation(ID3D12Device* device)
{
    ReportScopeObject scope(L"Formats");

    D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport = {};
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels = {};
    D3D12_FEATURE_DATA_FORMAT_INFO formatInfo = {};
    for(size_t formatIndex = 0; Enum_DXGI_FORMAT[formatIndex].m_Name != nullptr; ++formatIndex)
    {
        const DXGI_FORMAT format = (DXGI_FORMAT)Enum_DXGI_FORMAT[formatIndex].m_Value;
        const wchar_t* name = Enum_DXGI_FORMAT[formatIndex].m_Name;

        formatSupport.Format = format;

        const FormatSupportResult formatSupportResult = CheckFormatSupport(device, formatSupport);
        if(formatSupportResult == FormatSupportResult::Crashed)
        {
            ErrorPrinter::PrintFormat(L"ERROR: ID3D12Device::CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, {}) crashed.\n", std::make_wformat_args(name));
            break;
        }

        ReportScopeObjectConditional scope2(OutputSpecificString(name, std::format(L"{}", (size_t)format)));

        if(formatSupportResult == FormatSupportResult::Ok)
        {
            scope2.Enable();
            ReportFormatter::GetInstance().AddFieldFlags(L"Support1", formatSupport.Support1, Enum_D3D12_FORMAT_SUPPORT1);
            ReportFormatter::GetInstance().AddFieldFlags(L"Support2", formatSupport.Support2, Enum_D3D12_FORMAT_SUPPORT2);

            ReportScopeObjectConditional scope3(IsOutputJson(), L"MultisampleQualityLevels");
            msQualityLevels.Format = format;
            for(msQualityLevels.SampleCount = 1; ; msQualityLevels.SampleCount *= 2)
            {
                if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, UINT(sizeof msQualityLevels))) &&
                    msQualityLevels.NumQualityLevels > 0)
                {
                    if(IsOutputJson())
                    {
                        ReportScopeObject scope4(std::format(L"{}", msQualityLevels.SampleCount));
                        ReportFormatter::GetInstance().AddFieldUint32(L"NumQualityLevels", msQualityLevels.NumQualityLevels);
                        ReportFormatter::GetInstance().AddFieldUint32(L"Flags", uint32_t(msQualityLevels.Flags));
                    }
                    else
                    {
                        bool multisampleTiled = (msQualityLevels.Flags & D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_TILED_RESOURCE) != 0;
                        ReportFormatter::GetInstance().AddFieldString(L"SampleCount", std::format(L"{}: NumQualityLevels = {}{}", msQualityLevels.SampleCount, msQualityLevels.NumQualityLevels, multisampleTiled ? L"  D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_TILED_RESOURCE" : L""));
                    }
                }
                else
                    break;
            }
        }

        formatInfo.Format = format;
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_INFO, &formatInfo, UINT(sizeof formatInfo))))
        {
            scope2.Enable();
            ReportFormatter::GetInstance().AddFieldUint32(L"PlaneCount", formatInfo.PlaneCount);
        }
    }
}

static void PrintDeviceOptions(ID3D12Device* device)
{
    if (D3D12_FEATURE_DATA_D3D12_OPTIONS1 options1 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS1, &options1, sizeof(options1))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS1(options1);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS2 options2 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS2, &options2, sizeof(options2))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS2(options2);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS3 options3 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS3, &options3, sizeof(options3))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS3(options3);

    if (D3D12_FEATURE_DATA_EXISTING_HEAPS existingHeaps = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_EXISTING_HEAPS, &existingHeaps, sizeof(existingHeaps))))
        Print_D3D12_FEATURE_DATA_EXISTING_HEAPS(existingHeaps);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS4 options4 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS4, &options4, sizeof(options4))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS4(options4);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS5(options5);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS6 options6 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &options6, sizeof(options6))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS6(options6);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS7 options7 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &options7, sizeof(options7))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS7(options7);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS8 options8 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS8, &options8, sizeof(options8))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS8(options8);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS9 options9 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS9, &options9, sizeof(options9))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS9(options9);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS10 options10 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS10, &options10, sizeof(options10))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS10(options10);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS11 options11 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS11, &options11, sizeof(options11))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS11(options11);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS12 options12 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS12, &options12, sizeof(options12))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS12(options12);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS13 options13 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS13, &options13, sizeof(options13))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS13(options13);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS14 options14 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS14, &options14, sizeof(options14))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS14(options14);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS15 options15 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS15, &options15, sizeof(options15))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS15(options15);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS16 options16 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS16, &options16, sizeof(options16))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS16(options16);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS17 options17 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS17, &options17, sizeof(options17))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS17(options17);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS18 options18 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS18, &options18, sizeof(options18))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS18(options18);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS19 options19 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS19, &options19, sizeof(options19))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS19(options19);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS20 options20 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS20, &options20, sizeof(options20))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS20(options20);

    if (D3D12_FEATURE_DATA_D3D12_OPTIONS21 options21 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS21, &options21, sizeof(options21))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS21(options21);

    if (D3D12_FEATURE_DATA_BYTECODE_BYPASS_HASH_SUPPORTED bytecodeBypassHashSupported = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_BYTECODE_BYPASS_HASH_SUPPORTED, &bytecodeBypassHashSupported, sizeof(bytecodeBypassHashSupported))))
        Print_D3D12_FEATURE_DATA_BYTECODE_BYPASS_HASH_SUPPORTED(bytecodeBypassHashSupported);

#ifdef USE_PREVIEW_AGILITY_SDK
    if (D3D12_FEATURE_DATA_TIGHT_ALIGNMENT tightAlignment = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_TIGHT_ALIGNMENT, &tightAlignment, sizeof(tightAlignment))))
        Print_D3D12_FEATURE_DATA_TIGHT_ALIGNMENT(tightAlignment);
#endif
}

static void PrintDescriptorSizes(ID3D12Device* device)
{
    ReportScopeObject scope(L"GetDescriptorHandleIncrementSize");
    ReportFormatter::GetInstance().AddFieldUint32(L"D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV",
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
    ReportFormatter::GetInstance().AddFieldUint32(L"D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER",
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER));
    ReportFormatter::GetInstance().AddFieldUint32(L"D3D12_DESCRIPTOR_HEAP_TYPE_RTV",
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
    ReportFormatter::GetInstance().AddFieldUint32(L"D3D12_DESCRIPTOR_HEAP_TYPE_DSV",
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV));
}

static void PrintMetaCommand(ID3D12Device5* device5, UINT index, const D3D12_META_COMMAND_DESC& desc)
{
    ReportScopeArrayItem scope;

    ReportFormatter::GetInstance().AddFieldString(L"Id", GuidToStr(desc.Id).c_str());
    ReportFormatter::GetInstance().AddFieldString(L"Name", desc.Name);
    ReportFormatter::GetInstance().AddFieldFlags(L"InitializationDirtyState", desc.InitializationDirtyState, Enum_D3D12_GRAPHICS_STATES);
    ReportFormatter::GetInstance().AddFieldFlags(L"ExecutionDirtyState", desc.ExecutionDirtyState, Enum_D3D12_GRAPHICS_STATES);

    for(UINT stageIndex = 0; stageIndex < 3; ++stageIndex)
    {
        UINT totalStructureSizeInBytes = 0;
        UINT paramCount = 0;
        HRESULT hr = device5->EnumerateMetaCommandParameters(desc.Id,
            (D3D12_META_COMMAND_PARAMETER_STAGE)stageIndex,
            &totalStructureSizeInBytes, &paramCount, nullptr);
        if(FAILED(hr))
            continue;

        {
            ReportScopeObject scope2(Enum_D3D12_META_COMMAND_PARAMETER_STAGE[stageIndex].m_Name);
            ReportFormatter::GetInstance().AddFieldUint32(L"TotalStructureSizeInBytes", totalStructureSizeInBytes);
        }

        if(paramCount > 0)
        {
            std::vector<D3D12_META_COMMAND_PARAMETER_DESC> paramDescs(paramCount);
            hr = device5->EnumerateMetaCommandParameters(desc.Id,
                (D3D12_META_COMMAND_PARAMETER_STAGE)stageIndex,
                nullptr, &paramCount, paramDescs.data());
            if(SUCCEEDED(hr))
            {
                ReportScopeArray scope2(L"Parameters");

                for(UINT paramIndex = 0; paramIndex < paramCount; ++paramIndex)
                {
                    const auto& paramDesc = paramDescs[paramIndex];

                    ReportScopeArrayItem scope3;

                    ReportFormatter::GetInstance().AddFieldString(L"Name", paramDesc.Name);
                    ReportFormatter::GetInstance().AddFieldEnum(L"Type", paramDesc.Type, Enum_D3D12_META_COMMAND_PARAMETER_TYPE);
                    ReportFormatter::GetInstance().AddFieldFlags(L"Flags", paramDesc.Flags, Enum_D3D12_META_COMMAND_PARAMETER_FLAGS);
                    ReportFormatter::GetInstance().AddFieldFlags(L"RequiredResourceState", paramDesc.RequiredResourceState, Enum_D3D12_RESOURCE_STATES);
                    ReportFormatter::GetInstance().AddFieldUint32(L"StructureOffset", paramDesc.StructureOffset);
                }
            }
        }
    }
}

static void PrintMetaCommands(ID3D12Device5* device5)
{
    UINT num = 0;
    if(FAILED(device5->EnumerateMetaCommands(&num, nullptr)))
        return;
    if(num == 0)
        return;
    std::vector<D3D12_META_COMMAND_DESC> descs(num);
    if(FAILED(device5->EnumerateMetaCommands(&num, descs.data())))
        return;

    ReportScopeArray scope(L"EnumerateMetaCommands");

    for(UINT i = 0; i < num; ++i)
        PrintMetaCommand(device5, i, descs[i]);
}

static void PrintCommandQueuePriorities(ID3D12Device* device)
{
    D3D12_COMMAND_LIST_TYPE cmdListTypes[] = {
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        D3D12_COMMAND_LIST_TYPE_COMPUTE,
        D3D12_COMMAND_LIST_TYPE_COPY
    };
    
    D3D12_COMMAND_QUEUE_PRIORITY cmdQueuePriorities[] = {
        D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        D3D12_COMMAND_QUEUE_PRIORITY_HIGH,
        D3D12_COMMAND_QUEUE_PRIORITY_GLOBAL_REALTIME
    };
    
    std::array<bool, 9> queuePrioritySupport = {};
    size_t queuePriorityIndex = 0;
    for (auto cmdListType : cmdListTypes)
    {
        for (auto cmdQueuePriority : cmdQueuePriorities)
        {
            D3D12_FEATURE_DATA_COMMAND_QUEUE_PRIORITY commandQueuePriority = {};
            commandQueuePriority.CommandListType = cmdListType;
            commandQueuePriority.Priority = cmdQueuePriority;
            if(FAILED(device->CheckFeatureSupport(D3D12_FEATURE_COMMAND_QUEUE_PRIORITY, &commandQueuePriority, sizeof(commandQueuePriority))))
                return;
    
            queuePrioritySupport[queuePriorityIndex++] = commandQueuePriority.PriorityForTypeIsSupported;
        }
    }
    
    Print_D3D12_FEATURE_DATA_COMMAND_QUEUE_PRIORITY(queuePrioritySupport);
}

#ifdef USE_PREVIEW_AGILITY_SDK
static void PrintDirectSROptimizationRankings(const DSR_OPTIMIZATION_TYPE* optimizationRankings)
{
    constexpr size_t count = _countof(DSR_SUPERRES_VARIANT_DESC::OptimizationRankings);

    uint32_t rankingsCopy[count];
    for(size_t i = 0; i < count; ++i)
    {
        rankingsCopy[i] = (uint32_t)optimizationRankings[i];
    }

    ReportFormatter::GetInstance().AddEnumArray(L"OptimizationRankings", rankingsCopy, count, Enum_DSR_OPTIMIZATION_TYPE);
}

static void PrintDirectSR(ID3D12Device* device)
{
    ComPtr<ID3D12DSRDeviceFactory> dsrDeviceFactory;
    if(FAILED(g_D3D12GetInterface(CLSID_D3D12DSRDeviceFactory, IID_PPV_ARGS(&dsrDeviceFactory))))
        return;
    ComPtr<IDSRDevice> dsrDevice;
    if(FAILED(dsrDeviceFactory->CreateDSRDevice(device, 0, IID_PPV_ARGS(&dsrDevice))))
        return;
    UINT numVariants = dsrDevice->GetNumSuperResVariants();
    if(numVariants == 0)
        return;

    ReportScopeArray scope(L"DirectSR");

    for(UINT variantIndex = 0; variantIndex < numVariants; ++variantIndex)
    {
        DSR_SUPERRES_VARIANT_DESC desc = {};
        if(SUCCEEDED(dsrDevice->GetSuperResVariantDesc(variantIndex, &desc)))
        {
            ReportScopeArrayItem scope2;

            ReportFormatter::GetInstance().AddFieldString(L"VariantId", GuidToStr(desc.VariantId).c_str());
            ReportFormatter::GetInstance().AddFieldString(L"VariantName", StrToWstr(desc.VariantName, CP_UTF8).c_str());
            ReportFormatter::GetInstance().AddFieldFlags(L"Flags", desc.Flags, Enum_DSR_SUPERRES_VARIANT_FLAGS);
            PrintDirectSROptimizationRankings(desc.OptimizationRankings);
            ReportFormatter::GetInstance().AddFieldEnum(L"OptimalTargetFormat", desc.OptimalTargetFormat, Enum_DXGI_FORMAT);
        }
    }
}
#endif // #ifdef USE_PREVIEW_AGILITY_SDK

static int PrintDeviceDetails(IDXGIAdapter1* adapter1, NvAPI_Inititalize_RAII* nvAPI, AGS_Initialize_RAII* ags)
{
    ComPtr<ID3D12Device> device;

    DXGI_ADAPTER_DESC desc = {};
    // On fail desc will be empty
    // So code that depends on VendorId will receive 0x0
    adapter1->GetDesc(&desc);

#if USE_AGS
    bool useAGS = g_ForceVendorAPI || desc.VendorId == VENDOR_ID_AMD;
    if(useAGS && ags && ags->IsInitialized())
    {
        ComPtr<IDXGIAdapter> adapter;
        if (SUCCEEDED(adapter1->QueryInterface(IID_PPV_ARGS(&adapter))))
            device = ags->CreateDeviceAndPrintData(adapter.Get(),
                                                    MIN_FEATURE_LEVEL);
    }
#endif

    if(!device)
    {
        HRESULT hr;
#if defined(AUTO_LINK_DX12)
        hr = ::D3D12CreateDevice(adapter1, MIN_FEATURE_LEVEL, IID_PPV_ARGS(&device));
#else
        hr = g_D3D12CreateDevice(adapter1, MIN_FEATURE_LEVEL, IID_PPV_ARGS(&device));
#endif
        if(hr == 0x887E0003)
            throw std::runtime_error("D3D12CreateDevice returned 0x887E0003. Make sure Developer Mode is enabled in Windows settings.");
        CHECK_HR(hr);
    }

    if (!device)
        return PROGRAM_EXIT_ERROR_D3D12;
     
    if(D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options))))
        Print_D3D12_FEATURE_DATA_D3D12_OPTIONS(options);

    if(D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT gpuVirtualAddressSupport = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT, &gpuVirtualAddressSupport, sizeof(gpuVirtualAddressSupport))))
        Print_D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT(gpuVirtualAddressSupport);

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

    if(D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignature = {.HighestVersion = HIGHEST_ROOT_SIGNATURE_VERSION};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rootSignature, sizeof(rootSignature))))
        Print_D3D12_FEATURE_DATA_ROOT_SIGNATURE(rootSignature);

    if(D3D12_FEATURE_DATA_ARCHITECTURE1 architecture1 = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE1, &architecture1, sizeof(architecture1))))
        Print_D3D12_FEATURE_DATA_ARCHITECTURE1(architecture1);
    else
    {
        if(D3D12_FEATURE_DATA_ARCHITECTURE architecture = {};
            SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE, &architecture, sizeof(architecture))))
            Print_D3D12_FEATURE_DATA_ARCHITECTURE(architecture);
    }

    {
        D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevels =
        {
            _countof(FEATURE_LEVELS_ARRAY), FEATURE_LEVELS_ARRAY, MAX_FEATURE_LEVEL
        };
        if(SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevels, sizeof(featureLevels))))
            Print_D3D12_FEATURE_DATA_FEATURE_LEVELS(featureLevels);
    }

    if(D3D12_FEATURE_DATA_SHADER_CACHE shaderCache = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_SHADER_CACHE, &shaderCache, sizeof(shaderCache))))
        Print_D3D12_FEATURE_DATA_SHADER_CACHE(shaderCache);

    PrintCommandQueuePriorities(device.Get());

    if(D3D12_FEATURE_DATA_SERIALIZATION serialization = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_SERIALIZATION, &serialization, sizeof(serialization))))
        Print_D3D12_FEATURE_DATA_SERIALIZATION(serialization);

    if(D3D12_FEATURE_DATA_CROSS_NODE crossNode = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_CROSS_NODE, &crossNode, sizeof(crossNode))))
        Print_D3D12_FEATURE_CROSS_NODE(crossNode);

    if(D3D12_FEATURE_DATA_PREDICATION predication = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_PREDICATION, &predication, sizeof(predication))))
        Print_D3D12_FEATURE_PREDICATION(predication);

    if(D3D12_FEATURE_DATA_HARDWARE_COPY hardwareCopy = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_HARDWARE_COPY, &hardwareCopy, sizeof(hardwareCopy))))
        Print_D3D12_FEATURE_HARDWARE_COPY(hardwareCopy);

#ifdef USE_PREVIEW_AGILITY_SDK
    if(D3D12_FEATURE_DATA_APPLICATION_SPECIFIC_DRIVER_STATE appSpecificDriverState = {};
        SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_APPLICATION_SPECIFIC_DRIVER_STATE, &appSpecificDriverState, sizeof(appSpecificDriverState))))
        Print_D3D12_FEATURE_DATA_APPLICATION_SPECIFIC_DRIVER_STATE(appSpecificDriverState);

    // TODO: In Agility SDK 1.715.0-preview how to query for D3D12_FEATURE_D3D12_OPTIONS_EXPERIMENTAL1?
    // What is the corresponding structure?
#endif

    // TODO: D3D12_FEATURE_PLACED_RESOURCE_SUPPORT_INFO - What is this? How to query it? What structure to use?

    PrintDeviceOptions(device.Get());

    PrintDescriptorSizes(device.Get());

    if(g_PrintMetaCommands)
    {
        ComPtr<ID3D12Device5> device5;
        if(SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&device5))))
            PrintMetaCommands(device5.Get());
    }

#ifdef USE_PREVIEW_AGILITY_SDK
    if(!g_PureD3D12)
    {
        PrintDirectSR(device.Get());
    }
#endif

#if USE_NVAPI
    bool useNVAPI = g_ForceVendorAPI || desc.VendorId == VENDOR_ID_NVIDIA;
    if(nvAPI && nvAPI->IsInitialized())
        nvAPI->PrintD3d12DeviceData(device.Get());
#endif

    if(g_PrintFormats)
        PrintFormatInformation(device.Get());

#if USE_AGS
    if(useAGS && ags && ags->IsInitialized())
        ags->DestroyDevice(std::move(device));
#endif

    return PROGRAM_EXIT_SUCCESS;
}

#if !defined(AUTO_LINK_DX12)

static bool LoadLibraries()
{
    g_DxgiLibrary = ::LoadLibraryEx(DYN_LIB_DXGI, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_DxgiLibrary)
    {
        ErrorPrinter::PrintFormat(L"could not load {}\n", std::make_wformat_args(DYN_LIB_DXGI));
        return false;
    }

    g_Dx12Library = ::LoadLibraryEx(DYN_LIB_DX12, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_Dx12Library)
    {
        ErrorPrinter::PrintFormat(L"could not load {}\n", std::make_wformat_args(DYN_LIB_DX12));
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

    g_D3D12EnableExperimentalFeatures = reinterpret_cast<PFN_D3D12_ENABLE_EXPERIMENTAL_FEATURES>(::GetProcAddress(g_Dx12Library, "D3D12EnableExperimentalFeatures"));
    // Optional, null is accepted.

    g_D3D12GetInterface = reinterpret_cast<PFN_D3D12_GET_INTERFACE>(::GetProcAddress(g_Dx12Library, "D3D12GetInterface"));
    if (!g_D3D12GetInterface)
    {
        return false;
    }

    return true;
}

static void UnloadLibraries()
{
    g_CreateDXGIFactory1 = nullptr;
    g_D3D12CreateDevice = nullptr;

    BOOL rc;

    rc = ::FreeLibrary(g_DxgiLibrary);
    assert(rc);
    g_DxgiLibrary = nullptr;

    rc = ::FreeLibrary(g_Dx12Library);
    assert(rc);
    g_Dx12Library = nullptr;
}

#endif

template <typename PrinterClass>
void PrintCommandLineSyntax()
{
    PrinterClass::PrintString(L"Options:\n");
    PrinterClass::PrintString(L"  -v --Version                     Only print program version information.\n");
    PrinterClass::PrintString(L"  -h --Help                        Only print this help (command line syntax).\n");
    PrinterClass::PrintString(L"  -l --List                        Only print the list of all adapters.\n");
    PrinterClass::PrintString(L"  -a --Adapter=<Index>             Print details of adapter at specified index.\n");
    PrinterClass::PrintString(L"  --AllNonSoftware                 Print details of all (except Software) adapters (default behavior).\n");
    PrinterClass::PrintString(L"  --AllAdapters                    Print details of all adapters.\n");
    PrinterClass::PrintString(L"  -j --JSON                        Print output in JSON format instead of human-friendly text.\n");
    PrinterClass::PrintString(L"  --JSONPrettyPrint                Print JSON in human friendly form. (default behavior)\n");
    PrinterClass::PrintString(L"  --JSONNoPrettyPrint              Print JSON in minimal size form.\n");
    PrinterClass::PrintString(L"  -o --OutputToFile=<FilePath>     Output to specified file.\n");
    PrinterClass::PrintString(L"  -f --Formats                     Include information about DXGI format capabilities.\n");
    PrinterClass::PrintString(L"  --MetaCommands                   Include information about meta commands.\n");
    PrinterClass::PrintString(L"  -e --Enums                       Include information about all known enums and their values.\n");
    PrinterClass::PrintString(L"  --PureD3D12                      Extract information only from D3D12 and no other sources.\n");
#ifdef USE_PREVIEW_AGILITY_SDK
    PrinterClass::PrintString(L"  -x --EnableExperimental=<on/off> Whether to enable experimental features before querying device capabilities. Default is on (off for D3d12info and on for D3d12info_preview).\n");
#else
    PrinterClass::PrintString(L"  -x --EnableExperimental=<on/off> Whether to enable experimental features before querying device capabilities. Default is off (off for D3d12info and on for D3d12info_preview).\n");
#endif
    PrinterClass::PrintString(L"  --ForceVendorAPI                 Tries to query info via vendor-specific APIs, even in case when vendor doesn't match.\n");
    PrinterClass::PrintString(L"  --WARP                           Use WARP adapter.\n");
}

static void ListAdapter(uint32_t adapterIndex, IDXGIAdapter* adapter,
    NvAPI_Inititalize_RAII* nvApi, AGS_Initialize_RAII* ags,
    AmdDeviceInfo_Initialize_RAII* amdDeviceInfo, Vulkan_Initialize_RAII* vk)
{
    ReportScopeArrayItem scope;

    if(!g_WARP && !g_ShowAllAdapters)
    {
        // In case of WARP, we queried adapter via different API that didn't use adapter index
        // In case we show all adapters, array index equals adapter index
        ReportFormatter::GetInstance().AddFieldUint32(L"AdapterIndex", adapterIndex);
    }

    PrintAdapterData(adapter);

    DXGI_ADAPTER_DESC desc = {};
    if(SUCCEEDED(adapter->GetDesc(&desc)))
    {
#if USE_NVAPI
        bool useNVAPI = g_ForceVendorAPI || desc.VendorId == VENDOR_ID_NVIDIA;
        if(useNVAPI && nvApi && nvApi->IsInitialized())
        {
            nvApi->PrintPhysicalGpuData(desc.AdapterLuid);
        }
#endif
#if USE_AGS
        bool useAGS = g_ForceVendorAPI || desc.VendorId == VENDOR_ID_AMD;
        if(useAGS && ags && ags->IsInitialized())
        {
            AGS_Initialize_RAII::DeviceId deviceId = {
                .vendorId = (int)desc.VendorId,
                .deviceId = (int)desc.DeviceId,
                .revisionId = (int)desc.Revision};
            ags->PrintAgsDeviceData(deviceId);
        }
#endif
#if USE_AMD_DEVICE_INFO
        bool useAmdDeviceInfo = g_ForceVendorAPI || desc.VendorId == VENDOR_ID_AMD;
        if (useAmdDeviceInfo && amdDeviceInfo)
        {
            AmdDeviceInfo_Initialize_RAII::DeviceId deviceId = { desc.DeviceId, desc.Revision };
            amdDeviceInfo->PrintDeviceData(deviceId);
        }
#endif
#if USE_VULKAN
        if(vk && vk->IsInitialized())
            vk->PrintData(desc);
#endif
    }
}

static void ListAdapters(IDXGIFactory4* dxgiFactory, NvAPI_Inititalize_RAII* nvApi, AGS_Initialize_RAII* ags,
    AmdDeviceInfo_Initialize_RAII* amdDeviceInfo, Vulkan_Initialize_RAII* vk)
{
    ComPtr<IDXGIAdapter> adapter;
    if(!g_WARP)
    {
        UINT adapterIndex = 0;
        while(dxgiFactory->EnumAdapters(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
        {
            ListAdapter(adapterIndex, adapter.Get(), nvApi, ags, amdDeviceInfo, vk);
            adapter.Reset();
            ++adapterIndex;
        }
    }
    else
    {
        CHECK_HR(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)));
        ListAdapter(0, adapter.Get(), nvApi, ags, amdDeviceInfo, vk);
    }
}

int InspectAdapter(NvAPI_Inititalize_RAII* nvApi, AGS_Initialize_RAII* ags,
    AmdDeviceInfo_Initialize_RAII* amdDeviceInfo, Vulkan_Initialize_RAII* vk,
    uint32_t& adapterIndex, ComPtr<IDXGIAdapter1>& adapter1)
{
    ReportScopeArrayItemConditional scope(g_PrintAdaptersAsArray);

    int programResult = PROGRAM_EXIT_SUCCESS;

    if(!g_WARP && !g_ShowAllAdapters)
    {
        // In case of WARP, we queried adapter via different API that didn't use adapter index
        // In case we show all adapters, array index equals adapter index
        ReportFormatter::GetInstance().AddFieldUint32(L"AdapterIndex", adapterIndex);
    }

    PrintAdapterData(adapter1.Get());

    DXGI_ADAPTER_DESC desc = {};
    if(SUCCEEDED(adapter1->GetDesc(&desc)))
    {
#if USE_NVAPI
        bool useNVAPI = g_ForceVendorAPI || desc.VendorId == VENDOR_ID_NVIDIA;
        if(useNVAPI && nvApi && nvApi->IsInitialized())
        {
            nvApi->PrintPhysicalGpuData(desc.AdapterLuid);
        }
#endif
#if USE_AGS
        bool useAGS = g_ForceVendorAPI || desc.VendorId == VENDOR_ID_AMD;
        if(useAGS && ags && ags->IsInitialized())
        {
            AGS_Initialize_RAII::DeviceId deviceId = {
                .vendorId = (int)desc.VendorId,
                .deviceId = (int)desc.DeviceId,
                .revisionId = (int)desc.Revision };
            ags->PrintAgsDeviceData(deviceId);
        }
#endif
#if USE_AMD_DEVICE_INFO
        bool useAmdDeviceInfo = g_ForceVendorAPI || desc.VendorId == VENDOR_ID_AMD;
        if (useAmdDeviceInfo && amdDeviceInfo)
        {
            AmdDeviceInfo_Initialize_RAII::DeviceId deviceId = { desc.DeviceId, desc.Revision };
            amdDeviceInfo->PrintDeviceData(deviceId);
        }
#endif
#if USE_VULKAN
        if(vk && vk->IsInitialized())
            vk->PrintData(desc);
#endif
#if USE_INTEL_GPUDETECT
        bool useGPUDetect = g_ForceVendorAPI || desc.VendorId == VENDOR_ID_INTEL;
        if(useGPUDetect && !g_PureD3D12)
        {
            ComPtr<IDXGIAdapter> adapter;
            adapter1->QueryInterface(IID_PPV_ARGS(&adapter));
            IntelData::PrintAdapterData(adapter.Get());
        }
#endif
    }

    programResult = PrintDeviceDetails(adapter1.Get(), nvApi, ags);

    return programResult;
}

static int InspectAllAdapters(IDXGIFactory4* dxgiFactory, NvAPI_Inititalize_RAII* nvApi, AGS_Initialize_RAII* ags,
    AmdDeviceInfo_Initialize_RAII* amdDeviceInfo, Vulkan_Initialize_RAII* vk)
{
    uint32_t adapterIndex = 0;
    bool anyInspected = false;
    ComPtr<IDXGIAdapter1> adapter1;
    while(dxgiFactory->EnumAdapters1(adapterIndex, &adapter1) != DXGI_ERROR_NOT_FOUND)
    {
        if (g_SkipSoftwareAdapter)
        {
            DXGI_ADAPTER_DESC1 desc = {};
            adapter1->GetDesc1(&desc);
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                ++adapterIndex;
                continue;
            }
        }

        int result = InspectAdapter(nvApi, ags, amdDeviceInfo, vk, adapterIndex, adapter1);
        anyInspected = true; 
        if(result != PROGRAM_EXIT_SUCCESS)
            return result;
        ++adapterIndex;
    }

    if (!anyInspected)
        throw std::runtime_error("No D3D12 adapters to show.");

    return PROGRAM_EXIT_SUCCESS;
}

// adapterIndex == UINT_MAX means first non-software and non-remote adapter.
static int InspectAdapter(IDXGIFactory4* dxgiFactory, NvAPI_Inititalize_RAII* nvApi, AGS_Initialize_RAII* ags,
    AmdDeviceInfo_Initialize_RAII* amdDeviceInfo, Vulkan_Initialize_RAII* vk, uint32_t adapterIndex)
{
    ComPtr<IDXGIAdapter1> adapter1;
    if(g_WARP)
    {
        CHECK_HR(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&adapter1)));
    }
    else if(adapterIndex != UINT32_MAX)
        dxgiFactory->EnumAdapters1(adapterIndex, &adapter1);
    else
    {
        // No explicit adapter requested: Choose first non-software and non-remote.
        adapterIndex = 0;
        DXGI_ADAPTER_DESC1 desc = {};
        while(dxgiFactory->EnumAdapters1(adapterIndex, &adapter1) != DXGI_ERROR_NOT_FOUND)
        {
            adapter1->GetDesc1(&desc);
            if(desc.Flags == 0)
                break;
            adapter1.Reset();
            ++adapterIndex;
        }
    }

    if(adapter1)
    {
        return InspectAdapter(nvApi, ags, amdDeviceInfo, vk, adapterIndex, adapter1);
    }

    throw std::runtime_error("No valid adapter chosen to show D3D12 device details.");
}

int wmain3(int argc, wchar_t** argv)
{
    UINT adapterIndex = UINT32_MAX;

    CmdLineParser cmdLineParser(argc, argv);

    enum CMD_LINE_PARAM
    {
        CMD_LINE_OPT_VERSION,
        CMD_LINE_OPT_HELP,
        CMD_LINE_OPT_LIST,
        CMD_LINE_OPT_ADAPTER,
        CMD_LINE_OPT_ALL_NON_SOFTWARE,
        CMD_LINE_OPT_ALL_ADAPTERS,
        CMD_LINE_OPT_JSON,
        CMD_LINE_OPT_JSON_PRETTY_PRINT,
        CMD_LINE_OPT_JSON_NO_PRETTY_PRINT,
        CMD_LINE_OPT_OUTPUT_TO_FILE,
        CMD_LINE_OPT_FORMATS,
        CMD_LINE_OPT_META_COMMANDS,
        CMD_LINE_OPT_ENUMS,
        CMD_LINE_OPT_PURE_D3D12,
        CMD_LINE_OPT_ENABLE_EXPERIMENTAL,
        CMD_LINE_OPT_FORCE_VENDOR_SPECIFIC,
        CMD_LINE_OPT_WARP,
    };

    cmdLineParser.RegisterOpt(CMD_LINE_OPT_VERSION,               L"Version",             false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_VERSION,               L'v',                   false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_HELP,                  L"Help",                false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_HELP,                  L'h',                   false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_LIST,                  L"List",                false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_LIST,                  L'l',                   false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_ADAPTER,               L"Adapter",             true);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_ADAPTER,               L'a',                   true);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_ALL_NON_SOFTWARE,      L"AllNonSoftware",      false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_ALL_ADAPTERS,          L"AllAdapters",         false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_JSON,                  L"JSON",                false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_JSON,                  L'j',                   false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_JSON_PRETTY_PRINT,     L"JSONPrettyPrint",     false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_JSON_NO_PRETTY_PRINT,  L"JSONNoPrettyPrint",   false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_OUTPUT_TO_FILE,        L'o',                   true);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_OUTPUT_TO_FILE,        L"OutputToFile",        true);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_FORMATS,               L"Formats",             false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_FORMATS,               L'f',                   false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_META_COMMANDS,         L"MetaCommands",        false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_ENUMS,                 L"Enums",               false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_ENUMS,                 L'e',                   false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_PURE_D3D12,            L"PureD3D12",           false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_ENABLE_EXPERIMENTAL,   L"EnableExperimental",  true);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_ENABLE_EXPERIMENTAL,   L'x',                   true);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_FORCE_VENDOR_SPECIFIC, L"ForceVendorAPI",      false);
    cmdLineParser.RegisterOpt(CMD_LINE_OPT_WARP,                  L"WARP",                false);

    CmdLineParser::RESULT cmdLineResult;
    while((cmdLineResult = cmdLineParser.ReadNextOpt()) != CmdLineParser::RESULT_END)
    {
        switch(cmdLineResult)
        {
        case CmdLineParser::RESULT_ERROR:
        case CmdLineParser::RESULT_PARAMETER:
            g_ShowCommandLineSyntaxAndFail = true;
            break;
        case CmdLineParser::RESULT_OPT:
            switch(cmdLineParser.GetOptId())
            {
            case CMD_LINE_OPT_VERSION:
                g_ShowVersionAndQuit = true;
                break;
            case CMD_LINE_OPT_HELP:
                g_ShowCommandLineSyntaxAndQuit = true;
                break;
            case CMD_LINE_OPT_LIST:
                if(cmdLineParser.IsOptEncountered(CMD_LINE_OPT_ADAPTER) || 
                   cmdLineParser.IsOptEncountered(CMD_LINE_OPT_ALL_NON_SOFTWARE) || 
                   cmdLineParser.IsOptEncountered(CMD_LINE_OPT_ALL_ADAPTERS) ||
                   cmdLineParser.IsOptEncountered(CMD_LINE_OPT_WARP))
                {
                    g_ShowCommandLineSyntaxAndFail = true;
                    break;
                }
                g_ListAdapters = true;
                break;
            case CMD_LINE_OPT_ADAPTER:
                if(cmdLineParser.IsOptEncountered(CMD_LINE_OPT_LIST) ||
                   cmdLineParser.IsOptEncountered(CMD_LINE_OPT_ALL_NON_SOFTWARE) || 
                   cmdLineParser.IsOptEncountered(CMD_LINE_OPT_ALL_ADAPTERS) ||
                   cmdLineParser.IsOptEncountered(CMD_LINE_OPT_WARP))
                {
                    g_ShowCommandLineSyntaxAndFail = true;
                    break;
                }
                g_ShowAllAdapters = false;
                adapterIndex = _wtoi(cmdLineParser.GetParameter().c_str());
                break;
            case CMD_LINE_OPT_ALL_NON_SOFTWARE:
                if(cmdLineParser.IsOptEncountered(CMD_LINE_OPT_LIST) ||
                   cmdLineParser.IsOptEncountered(CMD_LINE_OPT_ADAPTER) ||
                   cmdLineParser.IsOptEncountered(CMD_LINE_OPT_ALL_ADAPTERS) || 
                   cmdLineParser.IsOptEncountered(CMD_LINE_OPT_WARP))
                {
                    g_ShowCommandLineSyntaxAndFail = true;
                    break;
                }
                g_ShowAllAdapters = true;
                g_SkipSoftwareAdapter = true;
                adapterIndex = UINT32_MAX;
                break;
            case CMD_LINE_OPT_ALL_ADAPTERS:
                if(cmdLineParser.IsOptEncountered(CMD_LINE_OPT_LIST) ||
                   cmdLineParser.IsOptEncountered(CMD_LINE_OPT_ADAPTER) ||
                   cmdLineParser.IsOptEncountered(CMD_LINE_OPT_ALL_NON_SOFTWARE) || 
                   cmdLineParser.IsOptEncountered(CMD_LINE_OPT_WARP))
                {
                    g_ShowCommandLineSyntaxAndFail = true;
                    break;
                }
                g_ShowAllAdapters = true;
                g_SkipSoftwareAdapter = false;
                adapterIndex = UINT32_MAX;
                break;
            case CMD_LINE_OPT_JSON:
                g_UseJsonOutput = true;
                break;
            case CMD_LINE_OPT_JSON_PRETTY_PRINT:
                g_UseJsonPrettyPrint = true;
                break;
            case CMD_LINE_OPT_JSON_NO_PRETTY_PRINT:
                g_UseJsonPrettyPrint = false;
                break;
            case CMD_LINE_OPT_OUTPUT_TO_FILE:
                g_OutputToFile = true;
                g_OutputFilePath = cmdLineParser.GetParameter();
                break;
            case CMD_LINE_OPT_FORMATS:
                g_PrintFormats = true;
                break;
            case CMD_LINE_OPT_META_COMMANDS:
                g_PrintMetaCommands = true;
                break;
            case CMD_LINE_OPT_ENUMS:
                g_PrintEnums = true;
                break;
            case CMD_LINE_OPT_PURE_D3D12:
                if (cmdLineParser.IsOptEncountered(CMD_LINE_OPT_FORCE_VENDOR_SPECIFIC))
                {
                    g_ShowCommandLineSyntaxAndFail = true;
                    break;
                }
                g_PureD3D12 = true;
                break;
            case CMD_LINE_OPT_ENABLE_EXPERIMENTAL:
                {
                    std::wstring param = cmdLineParser.GetParameter();
                    bool isOn = ::_wcsicmp(param.c_str(), L"on") == 0;
                    bool isOff = ::_wcsicmp(param.c_str(), L"off") == 0;
                    if (!isOn && !isOff)
                    {
                        g_ShowCommandLineSyntaxAndFail = true;
                        break;
                    }
                    g_EnableExperimental = isOn;
                }
                break;
            case CMD_LINE_OPT_FORCE_VENDOR_SPECIFIC:
                if (cmdLineParser.IsOptEncountered(CMD_LINE_OPT_PURE_D3D12))
                {
                    g_ShowCommandLineSyntaxAndFail = true;
                    break;
                }
                g_ForceVendorAPI = true;
                break;
            case CMD_LINE_OPT_WARP:
                if(cmdLineParser.IsOptEncountered(CMD_LINE_OPT_LIST) ||
                   cmdLineParser.IsOptEncountered(CMD_LINE_OPT_ADAPTER) ||
                   cmdLineParser.IsOptEncountered(CMD_LINE_OPT_ALL_NON_SOFTWARE) || 
                   cmdLineParser.IsOptEncountered(CMD_LINE_OPT_ALL_ADAPTERS))
                {
                    g_ShowCommandLineSyntaxAndFail = true;
                    break;
                }
                g_WARP = true;
                break;
            default:
                g_ShowCommandLineSyntaxAndFail = true;
                break;
            }
            break;
        default:
            assert(0);
            g_ShowCommandLineSyntaxAndFail = true;
            break;
        }
    }

    if (g_ShowCommandLineSyntaxAndFail)
    {
        Printer::Initialize(false, {});
        PrintCommandLineSyntax<ErrorPrinter>();
        Printer::Release();
        return PROGRAM_EXIT_ERROR_COMMAND_LINE;
    }

    g_PrintAdaptersAsArray = g_ShowAllAdapters || g_UseJsonOutput;

    if (g_OutputToFile)
    {
        if (!Printer::Initialize(true, g_OutputFilePath))
        {
            ErrorPrinter::PrintFormat(L"Could not open file for writing: {}\n", std::make_wformat_args(g_OutputFilePath.c_str()));
            return PROGRAM_EXIT_ERROR_INIT;
        }
    }
    else
    {
        if (!Printer::Initialize(false, {}))
        {
            assert(0);
            return PROGRAM_EXIT_ERROR_INIT;
        }
    }

    ReportFormatter::Flags flags = ReportFormatter::Flags::None;

    if (g_UseJsonOutput)
    {
        flags = flags | ReportFormatter::Flags::UseJson;
    }
    if (g_UseJsonPrettyPrint)
    {
        flags = flags | ReportFormatter::Flags::UseJsonPrettyPrint;
    }

    ReportFormatter::CreateInstance(flags);

    if (g_ShowVersionAndQuit)
    {
        if (IsOutputConsole())
        {
            PrintVersionHeader();
        }
        else
        {
            PrintVersionData();
        }
        ReportFormatter::DestroyInstance();
        Printer::Release();
        return PROGRAM_EXIT_SUCCESS;
    }

    if (g_ShowCommandLineSyntaxAndQuit)
    {
        PrintCommandLineSyntax<Printer>();
        ReportFormatter::DestroyInstance();
        Printer::Release();
        return PROGRAM_EXIT_SUCCESS;
    }

    PrintVersionData();

#if !defined(AUTO_LINK_DX12)
    if(!LoadLibraries())
        throw std::runtime_error("Could not load DXGI & D3D12 libraries.");
#endif

    std::unique_ptr<NvAPI_Inititalize_RAII> nvApiObjPtr;
#if USE_NVAPI
    if(!g_PureD3D12)
        nvApiObjPtr = std::make_unique<NvAPI_Inititalize_RAII>();
#endif

    std::unique_ptr<AGS_Initialize_RAII> agsObjPtr;
#if USE_AGS
    if(!g_PureD3D12)
        agsObjPtr = std::make_unique<AGS_Initialize_RAII>();
#endif

    std::unique_ptr<AmdDeviceInfo_Initialize_RAII> amdDeviceInfoObjPtr;
#if USE_AMD_DEVICE_INFO
    if (!g_PureD3D12)
        amdDeviceInfoObjPtr = std::make_unique<AmdDeviceInfo_Initialize_RAII>();
#endif

    std::unique_ptr<Vulkan_Initialize_RAII> vkObjPtr;
#if USE_VULKAN
    if(!g_PureD3D12)
        vkObjPtr = std::make_unique<Vulkan_Initialize_RAII>();
#endif

    {
        ReportScopeObject scope(OutputSpecificString(L"System Info", L"SystemInfo"));

        if(!g_PureD3D12)
        {
            PrintOsVersionInfo();
            PrintSystemMemoryInfo();
        }

        PrintDXGIFeatureInfo();

#if USE_NVAPI
    if(nvApiObjPtr && nvApiObjPtr->IsInitialized())
        nvApiObjPtr->PrintData();
#endif
#if USE_AGS
    if(agsObjPtr && agsObjPtr->IsInitialized())
        agsObjPtr->PrintData();
#endif

        EnableExperimentalFeatures();
    }

    if(g_PrintEnums)
        PrintEnums();

    int programResult = PROGRAM_EXIT_SUCCESS;

    // Scope for COM objects.
    {
        ComPtr<IDXGIFactory4> dxgiFactory = nullptr;
#if defined(AUTO_LINK_DX12)
        CHECK_HR( ::CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) );
#else
        CHECK_HR( g_CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) );
#endif
        assert(dxgiFactory != nullptr);

        ReportScopeArrayConditional scopeArray(g_PrintAdaptersAsArray, OutputSpecificString(L"Adapter", L"Adapters"), ReportFormatter::ArraySuffix::None);
        ReportScopeObjectConditional scopeObject(!g_PrintAdaptersAsArray, L"Adapter");

        if(g_ListAdapters)
            ListAdapters(dxgiFactory.Get(), nvApiObjPtr.get(), agsObjPtr.get(), amdDeviceInfoObjPtr.get(), vkObjPtr.get());
        else
        {
            if(g_WARP)
                InspectAdapter(dxgiFactory.Get(), nvApiObjPtr.get(), agsObjPtr.get(), amdDeviceInfoObjPtr.get(), vkObjPtr.get(), UINT32_MAX);
            else if(!g_ShowAllAdapters)
                InspectAdapter(dxgiFactory.Get(), nvApiObjPtr.get(), agsObjPtr.get(), amdDeviceInfoObjPtr.get(), vkObjPtr.get(), adapterIndex);
            else
                InspectAllAdapters(dxgiFactory.Get(), nvApiObjPtr.get(), agsObjPtr.get(), amdDeviceInfoObjPtr.get(), vkObjPtr.get());
        }
    }

#if !defined(AUTO_LINK_DX12)
    UnloadLibraries();
#endif

    ReportFormatter::DestroyInstance();
    Printer::Release();

    return programResult;
}

int wmain2(int argc, wchar_t** argv)
{
    try
    {
        return wmain3(argc, argv);
    }
    catch(const std::exception& ex)
    {
        ErrorPrinter::PrintFormat("ERROR: {}\n", std::make_format_args(ex.what()));
        return PROGRAM_EXIT_ERROR_EXCEPTION;
    }
    catch(...)
    {
        ErrorPrinter::PrintString("UNKNOWN ERROR.\n");
        return PROGRAM_EXIT_ERROR_EXCEPTION;
    }
}

int wmain(int argc, wchar_t** argv)
{
    __try
    {
        return wmain2(argc, argv);
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        ErrorPrinter::PrintFormat("STRUCTURED EXCEPTION: 0x{:08X}\n", std::make_format_args(GetExceptionCode()));
        return PROGRAM_EXIT_ERROR_SEH_EXCEPTION;
    }
}
