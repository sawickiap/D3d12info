/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2025 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#pragma once

// Macro set by Cmake.
#if USE_INTEL_GPUDETECT

namespace IntelData
{

    // Prints parameters related to Intel's library itself, regardless of selected adapter.
    void PrintStaticParams();

    void PrintAdapterData(IDXGIAdapter* adapter);

} // namespace IntelData

#endif // #if USE_INTEL_GPUDETECT
