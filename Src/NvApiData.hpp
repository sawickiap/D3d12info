#pragma once

// Macro set by Cmake.
#if USE_NVAPI

class NvAPI_Inititalize_RAII
{
public:
    // Prints parameters related to NVAPI itself, regardless of whether intialization succeeded.
    static void PrintStaticParams();
    
    NvAPI_Inititalize_RAII();
    ~NvAPI_Inititalize_RAII();
    bool IsInitialized() const { return m_Initialized; }
    
    // Prints structs from NVAPI unrelated to any specific GPU.
    void PrintData();
    void PrintD3d12DeviceData(ID3D12Device* device);
    void PrintPhysicalGpuData(const LUID& adapterLuid);

private:
    bool m_Initialized = false;
};

#else

class NvAPI_Inititalize_RAII { };

#endif // #if USE_NVAPI
