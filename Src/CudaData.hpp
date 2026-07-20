/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2026 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#pragma once

// Macro set by Cmake.
#if USE_CUDA

class Cuda_Initialize_RAII
{
public:
    static void PrintStaticParams();

    Cuda_Initialize_RAII();

    bool IsInitialized() const
    {
        return m_Initialized;
    }

    // Prints CUDA Runtime status unrelated to a specific adapter.
    void PrintData() const;

    // Prints CUDA devices that match the adapter LUID.
    void PrintAdapterData(const DXGI_ADAPTER_DESC& adapterDesc) const;

private:
    bool m_Initialized = false;
};

#else

class Cuda_Initialize_RAII
{
};

#endif // #if USE_CUDA
