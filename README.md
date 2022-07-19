# D3d12info

A simple Windows console program that gets all the information about the GPU (graphics chip) installed in the system, through DXGI and Direct3D 12 (D3D12) + AMD AGS, NVAPI, WinAPI, and some other sources.

Built and tested on Windows 10 64-bit using Visual Studio 2022.

![Example output](Docs/Gfx/Example_output.png "Example output")

Run it with parameter `-h` to see command line syntax.

Inspired by `vulkaninfo` - a similar tool that prints all the information about device capabilities using Vulkan API.

Alternative for D3D12 is GUI program "DirectX Caps Viewer", which is part of Windows SDK and can be found in path like:
`c:\Program Files (x86)\Windows Kits\10\bin\*\x64\dxcapsviewer.exe`.

# License

The project is open source under MIT license. See file [LICENSE.txt](LICENSE.txt).

# Dependencies and third-party libraries

The project source code depends on:

- C++ standard library, including some of the latest C++11/14/17/20 features
- WinAPI from Windows 10 with some reasonably new Windows SDK, including Direct3D 12
- Visual Studio 2022

The project uses following thirt-party libraries:

- **[AMD GPU Services](https://github.com/GPUOpen-LibrariesAndSDKs/AGS_SDK)** - custom vendor extensions to graphics APIs by AMD.
  - Linked externally. Can compile without it - see macro `USE_AGS`.
- **[DirectX 12 Agility SDK](https://devblogs.microsoft.com/directx/directx12agility/)** - latest API to Direct3D, by Microsoft.
  - Directory: ThirdParty\microsoft.direct3d.d3d12.*
- **[NVAPI](https://developer.nvidia.com/nvapi)** - custom vendor extensions to graphics APIs by Nvidia.
  - Linked externally. Can compile without it - see macro `USE_NVAPI`.
- **[RapidJSON](https://rapidjson.org/)** - a fast JSON parser/generator, by Tencent. License: MIT.
  - Directory: ThirdParty\rapidjson
- **[Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)**
  - Linked externally. Can compile without it - see macro `USE_VULKAN`.
