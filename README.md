# D3d12info

A Windows console program that gets all the information about the GPU (graphics chip) installed in the system, through DXGI and Direct3D 12 (D3D12) + AMD AGS, NVAPI, WinAPI, and some other sources.

Built and tested on Windows 10 64-bit using Visual Studio 2022.

![Example output](Docs/Gfx/Example_output.png "Example output")

Inspired by `vulkaninfo` - a similar tool that prints all the information about device capabilities using Vulkan API.

An alternative for D3D12 is a GUI program "DirectX Caps Viewer", which is part of Windows SDK and can be found in path like:
`c:\Program Files (x86)\Windows Kits\10\bin\*\x64\dxcapsviewer.exe`.

# Download

To download an archive with compiled binary, go to **[Releases](https://github.com/sawickiap/D3d12info/releases)**. This is a portable program - it doesn't need installation.

Version history can be found in file [CHANGELOG.md](CHANGELOG.md).

# Command-line syntax

```
Options:
  -v --Version         Only print program version information.
  -h --Help            Only print this help (command line syntax).
  -l --List            Only print the list of all adapters.
  -a --Adapter=<Index> Print details of adapter at specified index (default is the first hardware adapter).
  -j --JSON            Print output in JSON format instead of human-friendly text.
  -f --Formats         Include information about DXGI format capabilities.
  -e --Enums           Include information about all known enums and their values.
  --PureD3D12          Extract information only from D3D12 and no other sources.
  --WARP               Use WARP adapter.
```

# License

The project is open source under MIT license. See file [LICENSE.txt](LICENSE.txt).

# Dependencies and third-party libraries

The project source code depends on:

- C++ standard library, including some of the latest C++11/14/17/20 features
- WinAPI from Windows 10 with some reasonably new Windows SDK, including Direct3D 12
- Visual Studio 2022

The project uses following thirt-party libraries:

- **[AMD GPU Services](https://github.com/GPUOpen-LibrariesAndSDKs/AGS_SDK)** - custom vendor extensions to graphics APIs by AMD.
  - You need to obtain your copy and globally define `AMD_AGS_PATH` environment variable. You need at least version 6.0.1.
  - Can compile without it - see macro `USE_AGS`.
- **[DirectX 12 Agility SDK](https://devblogs.microsoft.com/directx/directx12agility/)** - latest API to Direct3D, by Microsoft.
  - Directory: Src\ThirdParty\microsoft.direct3d.d3d12.*
- **[NVAPI](https://developer.nvidia.com/nvapi)** - custom vendor extensions to graphics APIs by Nvidia.
  - You need to obtain your copy and globally define `NVIDIA_NVAPI_PATH` environment variable. You need at least version R520.
  - Can compile without it - see macro `USE_NVAPI`.
- **[RapidJSON](https://rapidjson.org/)** - a fast JSON parser/generator, by Tencent. License: MIT.
  - Directory: Src\ThirdParty\rapidjson
- **[Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)**
  - You need to install Vulkan SDK on your PC. You need at least version 1.3.216.0.
  - Can compile without it - see macro `USE_VULKAN`.
