#pragma once

// Define to 0 to disable usage of the entire NvAPI.
#define USE_NVAPI 1

#if USE_NVAPI

class NvAPI_Inititalize_RAII
{
public:
    NvAPI_Inititalize_RAII();
    ~NvAPI_Inititalize_RAII();
    bool IsInitialized() const { return m_Initialized; }
    
    void PrintGeneralParams();
    void PrintData();
    void PrintDeviceData(ID3D12Device* device);

private:
    bool m_Initialized = false;
};

#else

class NvAPI_Inititalize_RAII;

#endif // #if USE_NVAPI
