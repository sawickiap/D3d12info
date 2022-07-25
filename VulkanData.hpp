#pragma once

// Define to 0 to disable usage of the entire Vulkan.
#define USE_VULKAN 1

#if USE_VULKAN

class Vulkan_Initialize_RAII
{
public:
    // Prints parameters related to Vulkan itself, regardless of whether intialization succeeded.
    static void PrintStaticParams();

	Vulkan_Initialize_RAII();
	~Vulkan_Initialize_RAII();
	bool IsInitialized() const { return m_Initialized; }

	// Prints structs from Vulkan related to the specific adapter.
    void PrintData(const DXGI_ADAPTER_DESC& adapterDesc);
	
private:
	bool m_Initialized = false;
};

#else

class Vulkan_Initialize_RAII { };

#endif // #if USE_VULKAN
