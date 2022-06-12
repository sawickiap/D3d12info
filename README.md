# D3d12info

Simple console tool to get all the information from DXGI and Direct3D 12 (D3D12) on current system.

Built and tested on Windows 10 64-bit using Visual Studio 2022.

Still in early development phase, not feature-complete, but can already be useful.

![Example output](Docs/Gfx/Example_output.png "Example output")

Run it with parameter `-h` to see command line syntax.

Inspired by `vulkaninfo` - a similar tool that prints all the information about device capabilities using Vulkan API,
bundled with Vulkan SDK (or even every Vulkan runtime? I'm not really sure :)

Alternative for D3D12 is GUI program "DirectX Caps Viewer", which is part of Windows SDK and can be found in path like:
`c:\Program Files (x86)\Windows Kits\10\bin\10.0.16299.0\x64\dxcapsviewer.exe`.
