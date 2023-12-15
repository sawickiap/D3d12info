/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2023 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#pragma once

// Macro set by Cmake.
#if USE_AGS

class AGS_Initialize_RAII
{
public:
    // Prints parameters related to NVAPI itself, regardless of whether intialization succeeded.
    static void PrintStaticParams();

	AGS_Initialize_RAII();
	~AGS_Initialize_RAII();
	bool IsInitialized() const { return m_Initialized; }

	// Prints structs from AGS unrelated to any specific GPU.
    void PrintData();
	
	struct DeviceId
	{
		int vendorId, deviceId, revisionId;
	};
	void PrintAgsDeviceData(const DeviceId& id);

	// If fails, return null pointer.
	ComPtr<ID3D12Device> CreateDeviceAndPrintData(IDXGIAdapter* adapter, D3D_FEATURE_LEVEL featureLevel);
	void DestroyDevice(ComPtr<ID3D12Device>&& device);

private:
	bool m_Initialized = false;
};

#else

class AGS_Initialize_RAII { };

#endif // #if USE_AGS
