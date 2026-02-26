/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2026 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#pragma once

// Just declare GUIDs, don't define them in header
#include <guiddef.h>

// # From vkd3d-proton project, see:
// https://github.com/HansKristian-Work/vkd3d-proton/issues/2459
// https://github.com/HansKristian-Work/vkd3d-proton/blob/master/include/vkd3d_device_vkd3d_ext.idl
DEFINE_GUID(IID_ID3D12DXVKInteropDevice, 0x39da4e09, 0xbd1c, 0x4198, 0x9f, 0xae, 0x86, 0xbb, 0xe3, 0xbe, 0x41, 0xfd);

// D3d12info GUID for use with ID3D12ApplicationIdentity
DEFINE_GUID(APPID_D3D12INFO, 0x86671909, 0x7f0b, 0x44d6, 0xb0, 0xf9, 0xbe, 0xca, 0x2f, 0xc7, 0x4e, 0x2e);
