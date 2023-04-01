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
