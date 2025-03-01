/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2024 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#pragma once

// Macro set by Cmake.
#if USE_VULKAN

class Vulkan_Initialize_RAII
{
public:
    // Prints parameters related to Vulkan itself, regardless of whether intialization succeeded.
    static void PrintStaticParams();

    Vulkan_Initialize_RAII();
    ~Vulkan_Initialize_RAII();
    bool IsInitialized() const
    {
        return m_Initialized;
    }

    // Prints structs from Vulkan related to the specific adapter.
    void PrintData(const DXGI_ADAPTER_DESC& adapterDesc);

private:
    bool m_Initialized = false;
};

#else

class Vulkan_Initialize_RAII
{
};

#endif // #if USE_VULKAN
