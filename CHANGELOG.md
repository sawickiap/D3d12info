# Version 3.12.1 (2025-06-07)

Changes:

- Preview version: Fixed enabling of `D3D12CooperativeVectorExperiment`. (Reported by @didito as #34)

Compiled with DirectX 12 Agility SDK 1.616.0 ("D3d12info.exe") / 1.717.0-preview ("D3d12info\_preview.exe"), AMD AGS 6.3.0, AMD device\_info from 2025-03-11 (commit hash `1e9040681766423e6b361c6fd541834c3864a00b`), NVAPI R575, Intel GPU Detect from 2025-04-28 (commit hash `649cf14acd63e1d7ad7e30d3c558fb447f9f8c5b`) - all same as previous version.

# Version 3.12.0 (2025-06-07)

Changes:

- Refactored vkd3d-detection (previous variant didn't work). (Developed by @Devaniti as #36)
- Added Wine detection. (Developed by @Devaniti as #36)

Compiled with DirectX 12 Agility SDK 1.616.0 ("D3d12info.exe") / 1.717.0-preview ("D3d12info\_preview.exe"), AMD AGS 6.3.0, AMD device\_info from 2025-03-11 (commit hash `1e9040681766423e6b361c6fd541834c3864a00b`), NVAPI R575, Intel GPU Detect from 2025-04-28 (commit hash `649cf14acd63e1d7ad7e30d3c558fb447f9f8c5b`) - all same as previous version.

# Version 3.11.1 (2025-06-04)

Changes:

- Fixed printing of `D3D12_FEATURE_DATA_COOPERATIVE_VECTOR::pVectorAccumulateProperties`.

Compiled with DirectX 12 Agility SDK 1.616.0 ("D3d12info.exe") / 1.717.0-preview ("D3d12info\_preview.exe"), AMD AGS 6.3.0, AMD device\_info from 2025-03-11 (commit hash `1e9040681766423e6b361c6fd541834c3864a00b`), NVAPI R575, Intel GPU Detect from 2025-04-28 (commit hash `649cf14acd63e1d7ad7e30d3c558fb447f9f8c5b`) - all same as previous version.


# Version 3.11.0 (2025-06-04)

Changes:

- Updated Agility SDK to version 1.616.0 / 1.717.0-preview. (#32) Updated information printed:
  - Added Cooperative Vector.
  - Removed DirectSR, which disappeared from the SDK.
  - Other minor updates, including new `D3D12_RAYTRACING_TIER_1_2`, `D3D_SHADER_MODEL_6_9`, `D3D12_FEATURE_DATA_HARDWARE_SCHEDULING_QUEUE_GROUPINGS`.
- Added vkd3d-proton detection using `IVKD3DCoreInterface`. (#30)
- Fixed usage of `D3D12GetInterface` function to fix compatibility with Wine. (#29)
- Updated 3rd party libraries from GPU vendors.

Compiled with DirectX 12 Agility SDK 1.616.0 ("D3d12info.exe") / 1.717.0-preview ("D3d12info\_preview.exe"), AMD AGS 6.3.0, AMD device\_info from 2025-03-11 (commit hash `1e9040681766423e6b361c6fd541834c3864a00b`), NVAPI R575, Intel GPU Detect from 2025-04-28 (commit hash `649cf14acd63e1d7ad7e30d3c558fb447f9f8c5b`).

# Version 3.10.1 (2025-04-23)

Changes:

- Fixed JSON printing in case of multi-line strings. (Developed by @Devaniti as #28)

Compiled with DirectX 12 Agility SDK 1.615.1 ("D3d12info.exe") / 1.716.1-preview ("D3d12info\_preview.exe"), AMD AGS 6.2.0, AMD device\_info from 2024-09-24 (commit hash `7379d082f1d8d64c9d1168b84b7f6b2a9702c82f`), NVAPI R570, Intel GPU Detect from 2023-07-18 (commit hash `dd962c44acdad4ea21632e0bd144ab139e8ad8eb`) - all same as previous version.

# Version 3.10.0 (2025-03-01)

Changes:

- Added command-line parameter `-o --OutputFile=<FilePath>` that prints the output to a given file instead of the standard output. (Developed by @Devaniti as #26)
  - It can be useful for automated processing, where additional messages printed to the standard output e.g., by implicit Vulkan layers can cause problems.
- Added command-line parameter `--MinimizeJson` that prints the JSON output in a compact format, suitable for efficient automated processing. (Developed by @Devaniti as #26)
- Removed command-line parameter `--AllNonSoftware`, which is the default behavior anyway. (Developed by @Devaniti as #26)
- Improved quality of the source code, including:
  - Rewrite of the entire output printing code, which results in some changes in the text output formatting. (Developed by @Devaniti as #26)
  - Fixes in enum values, including ones that fix the correctness of the JSON output.
  - Enabled ClangFormat, reformatted the entire code. (Developed by @Devaniti as #27)

Compiled with DirectX 12 Agility SDK 1.615.1 ("D3d12info.exe") / 1.716.1-preview ("D3d12info\_preview.exe"), AMD AGS 6.2.0, AMD device\_info from 2024-09-24 (commit hash `7379d082f1d8d64c9d1168b84b7f6b2a9702c82f`), NVAPI R570, Intel GPU Detect from 2023-07-18 (commit hash `dd962c44acdad4ea21632e0bd144ab139e8ad8eb`).

# Version 3.9.1 (2025-02-09)

Changes:

- Fixed numeric value of the new `NV_GPU_ARCH_IMPLEMENTATION_GB202` enum item.
- Changed printing of the new `adapterType` to bit flags.

Compiled with DirectX 12 Agility SDK 1.615.0 ("D3d12info.exe") / 1.716.0-preview ("D3d12info\_preview.exe"), AMD AGS 6.2.0, AMD device\_info from 2024-09-24 (commit hash `7379d082f1d8d64c9d1168b84b7f6b2a9702c82f`), NVAPI R570, Intel GPU Detect from 2023-07-18 (commit hash `dd962c44acdad4ea21632e0bd144ab139e8ad8eb`) - all same as previous version.

# Version 3.9.0 (2025-02-08)

Changes:

- Updated NVAPI from version R550 to R570. Added printing of new information:
  - New ray tracing capabilities:
    - `NVAPI_D3D12_RAYTRACING_CAPS_TYPE_CLUSTER_OPERATIONS`
    - `NVAPI_D3D12_RAYTRACING_CAPS_TYPE_PARTITIONED_TLAS`
    - `NVAPI_D3D12_RAYTRACING_CAPS_TYPE_SPHERES`
    - `NVAPI_D3D12_RAYTRACING_CAPS_TYPE_LINEAR_SWEPT_SPHERES`
  - `NvAPI_D3D12_GetPhysicalDeviceCooperativeVectorProperties` - array of `NVAPI_COOPERATIVE_VECTOR_PROPERTIES` for different data types.
  - `adapterType` of type `NV_ADAPTER_TYPE`.

Compiled with DirectX 12 Agility SDK 1.615.0 ("D3d12info.exe") / 1.716.0-preview ("D3d12info\_preview.exe"), AMD AGS 6.2.0, AMD device\_info from 2024-09-24 (commit hash `7379d082f1d8d64c9d1168b84b7f6b2a9702c82f`), NVAPI R570, Intel GPU Detect from 2023-07-18 (commit hash `dd962c44acdad4ea21632e0bd144ab139e8ad8eb`).

# Version 3.8.0 (2025-02-01)

Changes:

- Updated Agility SDK to version 1.615.0 / 1.716.0-preview. Updated information printed:
  - Added `D3D12_FEATURE_DATA_BYTECODE_BYPASS_HASH_SUPPORTED`.
  - Preview only: removed `D3D12_FEATURE_DATA_D3D12_OPTIONS22`, added `D3D12_TIGHT_ALIGNMENT_TIER`.
- Added usage of [AMD device_info](https://github.com/GPUOpen-Tools/device_info) library, which contains an up-to-date list of AMD GPUs and their capabilities. Added printing of its structures `GDT_GfxCardInfo`, `GDT_DeviceInfo`.

Compiled with DirectX 12 Agility SDK 1.615.0 ("D3d12info.exe") / 1.716.0-preview ("D3d12info\_preview.exe"), AMD AGS 6.2.0, AMD device\_info from 2024-09-24 (commit hash `7379d082f1d8d64c9d1168b84b7f6b2a9702c82f`), NVAPI R550, Intel GPU Detect from 2023-07-18 (commit hash `dd962c44acdad4ea21632e0bd144ab139e8ad8eb`).

# Version 3.7.3 (2024-12-30)

Changes:

- Fixed a bug in format printing in JSON mode. (Developed by @Devaniti as #25)

Compiled with DirectX 12 Agility SDK 1.614.1 ("D3d12info.exe") / 1.715.1-preview ("D3d12info_preview.exe"), AMD AGS 6.2.0, NVAPI R550, Intel GPU Detect from 2023-07-18 (all same as previous version).

# Version 3.7.2 (2024-11-27)

Changes:

- Added digital signing of the executables, which will hopefully reduce security warnings shown by Windows when launching them. (Developed by @Devaniti as #24)

Compiled with DirectX 12 Agility SDK 1.614.1 ("D3d12info.exe") / 1.715.1-preview ("D3d12info_preview.exe"), AMD AGS 6.2.0, NVAPI R550, Intel GPU Detect from 2023-07-18 (all same as previous version).

# Version 3.7.1 (2024-10-29)

Changes:

- Updated Agility SDK to version 1.715.1-preview, which updated the default upscaler from AMD FidelityFX Super Resolution (FSR) 2.2 to 3.1.

Compiled with DirectX 12 Agility SDK 1.614.1 ("D3d12info.exe") / 1.715.1-preview ("D3d12info_preview.exe"), AMD AGS 6.2.0, NVAPI R550, Intel GPU Detect from 2023-07-18.

# Version 3.7.0 (2024-07-18)

Changes:

- Updated Agility SDK to version 1.715.0-preview. Added new information:
  - Experiment `D3D12StateObjectsExperiment`, enum value `D3D12_WORK_GRAPHS_TIER_1_1` - for the new GPU work graph mesh nodes.
  - Structure `D3D12_FEATURE_DATA_D3D12_OPTIONS22` with `TightAlignmentSupported`.
  - Structure `D3D12_FEATURE_DATA_APPLICATION_SPECIFIC_DRIVER_STATE` with `Supported`.
- Updated Agility SDK to version 1.614.1. No changes in reported information.

Compiled with DirectX 12 Agility SDK 1.614.1 ("D3d12info.exe") / 1.715.0-preview ("D3d12info_preview.exe"), AMD AGS 6.2.0, NVAPI R550, Intel GPU Detect from 2023-07-18.

# Version 3.6.0 (2024-06-08)

Changes:

- Updated Agility SDK to version 1.714.0-preview.
  - Added information about the new DirectSR - available variants.
  - Removed information that disappeared from the SDK: `D3D12_FEATURE_DATA_D3D12_OPTIONS_EXPERIMENTAL`, `D3D12_FEATURE_DATA_WAVE_MMA`.
- Added information about meta commands and their parameters. Off by default, enabled with new command-line parameter: `--MetaCommands`.

Compiled with DirectX 12 Agility SDK 1.614.0 ("D3d12info.exe") / 1.714.0-preview ("D3d12info_preview.exe"), AMD AGS 6.2.0, NVAPI R550, Intel GPU Detect from 2023-07-18.

# Version 3.5.1 (2024-05-27)

Changes:

- Changed project options to use Runtime Library = Multi-threaded (not "DLL") version so the executables no longer need .dll files from Visual C++ Redistributable libraries installed in the system to work. (Developed by @Devaniti as #21)

Compiled with DirectX 12 Agility SDK 1.614.0 ("D3d12info.exe") / 1.711.3-preview ("D3d12info_preview.exe"), AMD AGS 6.2.0, NVAPI R550, Intel GPU Detect from 2023-07-18 (all same as previous version).

# Version 3.5.0 (2024-05-17)

Changes:

- Updated DirectX 12 Agility SDK to version 1.614.0. No new data to fetch.

Compiled with DirectX 12 Agility SDK 1.614.0 ("D3d12info.exe") / 1.711.3-preview ("D3d12info_preview.exe"), AMD AGS 6.2.0, NVAPI R550, Intel GPU Detect from 2023-07-18.

# Version 3.4.0 (2024-03-26)

Changes:

- Added command-line parameter `-x` / `--EnableExperimental=<on/off>` to control if `D3D12EnableExperimentalFeatures` function should be called, which is now on in "D3d12info_preview.exe" but off in "D3d12info.exe" by default. (Developed by @Devaniti as #19)
- Updated Agility SDK to version 1.613.1. No new data to fetch.

Compiled with DirectX 12 Agility SDK 1.613.1 ("D3d12info.exe") / 1.711.3-preview ("D3d12info_preview.exe"), AMD AGS 6.2.0, NVAPI R550, Intel GPU Detect from 2023-07-18.

# Version 3.3.0 (2024-03-11)

Changes:

- Updated Agility SDK to version 1.613.0. Added `D3D12_FEATURE_DATA_D3D12_OPTIONS21` (#18 - thanks @oscarbg).
- Improved decoding of `NV_GPU_ARCH_INFO::implementation_id` (thanks @Devaniti).
- Updated subsystem vendor id list (#17 - thanks @Devaniti).

Compiled with DirectX 12 Agility SDK 1.613.0 ("D3d12info.exe") / 1.711.3-preview ("D3d12info_preview.exe"), AMD AGS 6.2.0, NVAPI R550, Intel GPU Detect from 2023-07-18.

# Version 3.2.0 (2024-03-03)

Changes:

- Updated NVAPI to version R550. Added information returned by: `NvAPI_GPU_GetGPUInfo` / `NV_GPU_INFO`, `NvAPI_GPU_GetGspFeatures` / `NV_GPU_GSP_INFO`, `NvAPI_D3D12_GetOptimalThreadCountForMesh`.
- Improved decoding of `VendorId` and `SubSystemId`. (Thanks @Devaniti !)

Compiled with DirectX 12 Agility SDK 1.611.0 ("D3d12info.exe") / 1.711.3-preview ("D3d12info_preview.exe"), AMD AGS 6.2.0, NVAPI R550, Intel GPU Detect from 2023-07-18.

# Version 3.1.0 (2024-01-20)

Changes:

- Changed default behavior to query vendor-specific APIs only if `VendorID` matches. Added command-line parameter `--ForceVendorAPI` to force the old behavior of always querying them if possible. (Developed by @Devaniti as #15)
- Added stderr/stdout flush before program exit. (Developed by @Devaniti as #14)
- Changed Git repository and Cmake script to pull third-party libraries (RapidJSON, AGS, NVAPI, Intel GPU Detect) as Git submodules. (Developed by @Devaniti as #13)

Compiled with DirectX 12 Agility SDK 1.611.0 ("D3d12info.exe") / 1.711.3-preview ("D3d12info_preview.exe"), AMD AGS 6.2.0, NVAPI R535-developer, Intel GPU Detect from 2023-07-18 (all same as previous version).

# Version 3.0.0 (2024-01-10)

Changes:

- Parameters of all non-SOFTWARE and non-WARP adapters are now printed by default, not just the first one. New command-line parameters are available: `--AllNonSoftware` (same as default behavior), `--AllAdapters` (to print all adapters). Existing parameters `--Adapter=<Index>`, `--WARP`, and other are still available.
- Changed structure of the output JSON format to simplify automated processing. It now always contains an array of adapters. Renamed and reorganized some of the parameters printed.
- Added information returned by `D3D12_FEATURE_DATA_COMMAND_QUEUE_PRIORITY`.
- Added Windows version returned by function `RtlGetVersion` from "ntdll.dll".

This whole change was developed by @Devaniti as #12.

Compiled with DirectX 12 Agility SDK 1.611.0 ("D3d12info.exe") / 1.711.3-preview ("D3d12info_preview.exe"), AMD AGS 6.2.0, NVAPI R535-developer, Intel GPU Detect from 2023-07-18 (all same as previous version).

# Version 2.2.0 (2023-12-15)

Changes:

- Added usage of [Intel GPU Detect library](https://github.com/GameTechDev/gpudetect), printed as new section "Intel GPUDetect::GPUData" on Intel GPUs.

Compiled with DirectX 12 Agility SDK 1.611.0 ("D3d12info.exe") / 1.711.3-preview ("D3d12info_preview.exe"), AMD AGS 6.2.0, NVAPI R535-developer, Intel GPU Detect from 2023-07-18.

# Version 2.1.0 (2023-11-10)

Changes:

- Added information returned by `ID3D12Device::GetDescriptorHandleIncrementSize`.

Compiled with DirectX 12 Agility SDK 1.611.0 ("D3d12info.exe") / 1.711.3-preview ("D3d12info_preview.exe"), AMD AGS 6.2.0, NVAPI R535-developer - all same as the previous version.

# Version 2.0.0 (2023-11-01)

Changes, from most to least important:

- Introduced two executables: "D3d12info.exe" (compiled with the latest main version of the DirectX 12 Agility SDK) and "D3d12info_preview.exe" (compiled with the latest preview version of the SDK), as they both have some distinct parameters to query.
  - Added information unique to the main SDK version: `D3D12_FEATURE_DATA_PREDICATION`, `D3D12_FEATURE_DATA_HARDWARE_COPY`, `D3D12_FEATURE_DATA_D3D12_OPTIONS20::RecreateAtTier`.
- Fixed incorrect printing of `D3D12_FEATURE_DATA_FORMAT_SUPPORT::Support2`.
- Added information `CheckInterfaceSupport`, which returns user mode driver version.
- Added information `DXGI_FEATURE_PRESENT_ALLOW_TEARING`.
- Added protection against crash in the buggy AMD driver. Also added generic handling of SEH exceptions (e.g. access violation), which are now reported to standard error and returned as process exit code -4.

- Changed printing of `agsGetVersionNumber` to decode as version X.Y.Z.
- Removed dummy enum item `DXGI_FORMAT_FORCE_UINT` from the list of formats to query.
- Improved error message printed when Developer Mode is not enabled in Windows settings.

Compiled with DirectX 12 Agility SDK 1.611.0 ("D3d12info.exe") / 1.711.3-preview ("D3d12info_preview.exe"), AMD AGS 6.2.0, NVAPI R535-developer.

# Version 1.7.0 (2023-09-07)

Changes:

- Updated AMD AGS to version 6.2 to report support for the new `shaderClock` intrinsics.

Compiled with DirectX 12 Agility SDK 1.711.3-preview, AMD AGS 6.2.0, NVAPI R535-developer.

# Version 1.6.0 (2023-08-05)

Changes:

- Updated NVAPI to version R535-Developer - added support for new parameters: `NVAPI_D3D12_RAYTRACING_CAPS_TYPE_DISPLACEMENT_MICROMAP` and `NvAPI_D3D12_GetNeedsAppFPBlendClamping` (#11 - thanks @oscarbg).

Compiled with DirectX 12 Agility SDK 1.711.3-preview, AMD AGS 6.1.0, NVAPI R535-developer.

# Version 1.5.3 (2023-08-04)

Changes:

- Recompiled the application with AMD AGS and NVAPI libraries enabled.

Compiled with DirectX 12 Agility SDK 1.711.3-preview, AMD AGS 6.1.0, NVAPI R530-developer (same as previous version).

# Version 1.5.2 (2023-08-03)

Changes:

- Fixed printing of `D3D12_FEATURE_DATA_WAVE_MMA` (#10 - thanks @xoofx).

Compiled with DirectX 12 Agility SDK 1.711.3-preview, AMD AGS 6.1.0, NVAPI R530-developer (same as previous version).

# Version 1.5.1 (2023-08-02)

Changes:

- Added missing `D3D12_FEATURE_DATA_D3D12_OPTIONS12::RelaxedFormatCastingSupported` (#9 - thanks @didito).

Compiled with DirectX 12 Agility SDK 1.711.3-preview, AMD AGS 6.1.0, NVAPI R530-developer (same as previous version).

# Version 1.5.0 (2023-06-24)

Changes:

- Updated DirectX Agility SDK to the latest version 1.711.3-preview. Added new information:
  - `D3D12_FEATURE_DATA_D3D12_OPTIONS20`
  - `D3D12_FEATURE_DATA_D3D12_OPTIONS_EXPERIMENTAL` - `WorkGraphsTier`
  - `D3D12_FEATURE_DATA_WAVE_MMA`
- Added usage of `D3D12EnableExperimentalFeatures`.
- Created Cmake script for the project.

Compiled with DirectX 12 Agility SDK 1.711.3-preview, AMD AGS 6.1.0, NVAPI R530-developer.

# Version 1.4.0 (2023-04-01)

Changes:

- Updated DirectX Agility SDK to the latest version 1.710.0-preview. Added new information:
    - `D3D12_FEATURE_DATA_D3D12_OPTIONS16::GPUUploadHeapSupported` (it was reserved before)
    - `D3D12_FEATURE_DATA_D3D12_OPTIONS17`
    - `D3D12_FEATURE_DATA_D3D12_OPTIONS18`
    - `D3D12_FEATURE_DATA_D3D12_OPTIONS19`
    - Root signature version 1.2
    - `DXGI_FORMAT_A4B4G4R4_UNORM`
- Updated NVAPI to the latest version R530-developer. Added new information: `NvAPI_D3D12_QueryWorkstationFeatureProperties`.

Compiled with DirectX 12 Agility SDK 1.710.0-preview, AMD AGS 6.1.0, NVAPI R530-developer.

# Version 1.3.1 (2023-02-26)

Changes: Fixed printing of sizes in section about `NvAPI_GPU_GetMemoryInfoEx - NV_GPU_MEMORY_INFO_EX`. (#7)

Compiled with DirectX 12 Agility SDK 1.608.2, AMD AGS 6.1.0, NVAPI R525-developer - all same as last time.

# Version 1.3.0 (2023-01-31)

Compiled with DirectX 12 Agility SDK 1.608.2, AMD AGS 6.1.0, NVAPI R525-developer.

# Version 1.2.0 (2023-01-04)

Changes: Decided to use normal not preview version of DirectX 12 Agility SDK. Added new information: `D3D12_FEATURE_DATA_D3D12_OPTIONS15`.

Compiled with DirectX 12 Agility SDK 1.608.2, AMD AGS 6.0.1, NVAPI R520-developer.

# Version 1.1.0 (2022-10-29)

Changes: Updated NVAPI to the latest version R520-developer. Added new information, including query for the new features - function `NvAPI_D3D12_GetRaytracingCaps`, enum `NVAPI_D3D12_RAYTRACING_THREAD_REORDERING_CAPS`, `NVAPI_D3D12_RAYTRACING_OPACITY_MICROMAP_CAPS`, and some other new or missing queries.

Compiled with DirectX 12 Agility SDK 1.706.3-preview, AMD AGS 6.0.1, NVAPI R520-developer.

# Version 1.0.0 (2022-07-27)

First release considered complete.

Compiled with DirectX 12 Agility SDK 1.706.3-preview, AMD AGS 6.0.1, NVAPI R515-developer.

# Version 0.0.1 (2018-11-09)

First official release - a draft.
