/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2025 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#pragma once

// Macro set by Cmake.
#if USE_AMD_DEVICE_INFO

class AmdDeviceInfo_Initialize_RAII
{
public:
    // Prints parameters related to AMD's library itself, regardless of selected adapter.
    static void PrintStaticParams();


	struct DeviceId
	{
		uint32_t deviceId, revisionId;
	};
	void PrintDeviceData(const DeviceId& id);
};

#else

class AmdDeviceInfo_Initialize_RAII { };

#endif // #if USE_AMD_DEVICE_INFO
