/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2025 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#include "NvApiData.hpp"
#include "Printing.hpp"
#include "Enums.hpp"
#include "Json.hpp"
#include "Utils.hpp"

// Macro set by Cmake.
#if USE_NVAPI

#include <nvapi.h>

// Don't forget to update when linking with a new version!
static const wchar_t* NVAPI_COMPILED_VERSION = L"R570";

////////////////////////////////////////////////////////////////////////////////
// PRIVATE

static const EnumItem Enum_NV_EXTN_OP[] = {
	{ L"NV_EXTN_OP_SHFL", 1 },
	{ L"NV_EXTN_OP_SHFL_UP", 2 },
	{ L"NV_EXTN_OP_SHFL_DOWN", 3 },
	{ L"NV_EXTN_OP_SHFL_XOR", 4 },
	{ L"NV_EXTN_OP_VOTE_ALL", 5 },
	{ L"NV_EXTN_OP_VOTE_ANY", 6 },
	{ L"NV_EXTN_OP_VOTE_BALLOT", 7 },
	{ L"NV_EXTN_OP_GET_LANE_ID", 8 },
	{ L"NV_EXTN_OP_FP16_ATOMIC", 12 },
	{ L"NV_EXTN_OP_FP32_ATOMIC", 13 },
	{ L"NV_EXTN_OP_UINT64_ATOMIC", 20 },
	{ L"NV_EXTN_OP_GET_SHADING_RATE", 30 },
	{ L"NV_EXTN_OP_VPRS_EVAL_ATTRIB_AT_SAMPLE", 51 },
	{ NULL, UINT32_MAX } };
static EnumRegistration g_Enum_NV_EXTN_OP_Registration(L"NV_EXTN_OP", Enum_NV_EXTN_OP);

ENUM_BEGIN(NvAPI_Status)
    ENUM_ITEM(NVAPI_OK                                    )
    ENUM_ITEM(NVAPI_ERROR                                 )
    ENUM_ITEM(NVAPI_LIBRARY_NOT_FOUND                     )
    ENUM_ITEM(NVAPI_NO_IMPLEMENTATION                     )
    ENUM_ITEM(NVAPI_API_NOT_INITIALIZED                   )
    ENUM_ITEM(NVAPI_INVALID_ARGUMENT                      )
    ENUM_ITEM(NVAPI_NVIDIA_DEVICE_NOT_FOUND               )
    ENUM_ITEM(NVAPI_END_ENUMERATION                       )
    ENUM_ITEM(NVAPI_INVALID_HANDLE                        )
    ENUM_ITEM(NVAPI_INCOMPATIBLE_STRUCT_VERSION           )
    ENUM_ITEM(NVAPI_HANDLE_INVALIDATED                    )
    ENUM_ITEM(NVAPI_OPENGL_CONTEXT_NOT_CURRENT            )
    ENUM_ITEM(NVAPI_INVALID_POINTER                       )
    ENUM_ITEM(NVAPI_NO_GL_EXPERT                          )
    ENUM_ITEM(NVAPI_INSTRUMENTATION_DISABLED              )
    ENUM_ITEM(NVAPI_NO_GL_NSIGHT                          )
    ENUM_ITEM(NVAPI_EXPECTED_LOGICAL_GPU_HANDLE           )
    ENUM_ITEM(NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE          )
    ENUM_ITEM(NVAPI_EXPECTED_DISPLAY_HANDLE               )
    ENUM_ITEM(NVAPI_INVALID_COMBINATION                   )
    ENUM_ITEM(NVAPI_NOT_SUPPORTED                         )
    ENUM_ITEM(NVAPI_PORTID_NOT_FOUND                      )
    ENUM_ITEM(NVAPI_EXPECTED_UNATTACHED_DISPLAY_HANDLE    )
    ENUM_ITEM(NVAPI_INVALID_PERF_LEVEL                    )
    ENUM_ITEM(NVAPI_DEVICE_BUSY                           )
    ENUM_ITEM(NVAPI_NV_PERSIST_FILE_NOT_FOUND             )
    ENUM_ITEM(NVAPI_PERSIST_DATA_NOT_FOUND                )
    ENUM_ITEM(NVAPI_EXPECTED_TV_DISPLAY                   )
    ENUM_ITEM(NVAPI_EXPECTED_TV_DISPLAY_ON_DCONNECTOR     )
    ENUM_ITEM(NVAPI_NO_ACTIVE_SLI_TOPOLOGY                )
    ENUM_ITEM(NVAPI_SLI_RENDERING_MODE_NOTALLOWED         )
    ENUM_ITEM(NVAPI_EXPECTED_DIGITAL_FLAT_PANEL           )
    ENUM_ITEM(NVAPI_ARGUMENT_EXCEED_MAX_SIZE              )
    ENUM_ITEM(NVAPI_DEVICE_SWITCHING_NOT_ALLOWED          )
    ENUM_ITEM(NVAPI_TESTING_CLOCKS_NOT_SUPPORTED          )
    ENUM_ITEM(NVAPI_UNKNOWN_UNDERSCAN_CONFIG              )
    ENUM_ITEM(NVAPI_TIMEOUT_RECONFIGURING_GPU_TOPO        )
    ENUM_ITEM(NVAPI_DATA_NOT_FOUND                        )
    ENUM_ITEM(NVAPI_EXPECTED_ANALOG_DISPLAY               )
    ENUM_ITEM(NVAPI_NO_VIDLINK                            )
    ENUM_ITEM(NVAPI_REQUIRES_REBOOT                       )
    ENUM_ITEM(NVAPI_INVALID_HYBRID_MODE                   )
    ENUM_ITEM(NVAPI_MIXED_TARGET_TYPES                    )
    ENUM_ITEM(NVAPI_SYSWOW64_NOT_SUPPORTED                )
    ENUM_ITEM(NVAPI_IMPLICIT_SET_GPU_TOPOLOGY_CHANGE_NOT_ALLOWED)
    ENUM_ITEM(NVAPI_REQUEST_USER_TO_CLOSE_NON_MIGRATABLE_APPS)
    ENUM_ITEM(NVAPI_OUT_OF_MEMORY                         )
    ENUM_ITEM(NVAPI_WAS_STILL_DRAWING                     )
    ENUM_ITEM(NVAPI_FILE_NOT_FOUND                        )
    ENUM_ITEM(NVAPI_TOO_MANY_UNIQUE_STATE_OBJECTS         )
    ENUM_ITEM(NVAPI_INVALID_CALL                          )
    ENUM_ITEM(NVAPI_D3D10_1_LIBRARY_NOT_FOUND             )
    ENUM_ITEM(NVAPI_FUNCTION_NOT_FOUND                    )
    ENUM_ITEM(NVAPI_INVALID_USER_PRIVILEGE                )
    ENUM_ITEM(NVAPI_EXPECTED_NON_PRIMARY_DISPLAY_HANDLE   )
    ENUM_ITEM(NVAPI_EXPECTED_COMPUTE_GPU_HANDLE           )
    ENUM_ITEM(NVAPI_STEREO_NOT_INITIALIZED                )
    ENUM_ITEM(NVAPI_STEREO_REGISTRY_ACCESS_FAILED         )
    ENUM_ITEM(NVAPI_STEREO_REGISTRY_PROFILE_TYPE_NOT_SUPPORTED)
    ENUM_ITEM(NVAPI_STEREO_REGISTRY_VALUE_NOT_SUPPORTED   )
    ENUM_ITEM(NVAPI_STEREO_NOT_ENABLED                    )
    ENUM_ITEM(NVAPI_STEREO_NOT_TURNED_ON                  )
    ENUM_ITEM(NVAPI_STEREO_INVALID_DEVICE_INTERFACE       )
    ENUM_ITEM(NVAPI_STEREO_PARAMETER_OUT_OF_RANGE         )
    ENUM_ITEM(NVAPI_STEREO_FRUSTUM_ADJUST_MODE_NOT_SUPPORTED)
    ENUM_ITEM(NVAPI_TOPO_NOT_POSSIBLE                     )
    ENUM_ITEM(NVAPI_MODE_CHANGE_FAILED                    )
    ENUM_ITEM(NVAPI_D3D11_LIBRARY_NOT_FOUND               )
    ENUM_ITEM(NVAPI_INVALID_ADDRESS                       )
    ENUM_ITEM(NVAPI_STRING_TOO_SMALL                      )
    ENUM_ITEM(NVAPI_MATCHING_DEVICE_NOT_FOUND             )
    ENUM_ITEM(NVAPI_DRIVER_RUNNING                        )
    ENUM_ITEM(NVAPI_DRIVER_NOTRUNNING                     )
    ENUM_ITEM(NVAPI_ERROR_DRIVER_RELOAD_REQUIRED          )
    ENUM_ITEM(NVAPI_SET_NOT_ALLOWED                       )
    ENUM_ITEM(NVAPI_ADVANCED_DISPLAY_TOPOLOGY_REQUIRED    )
    ENUM_ITEM(NVAPI_SETTING_NOT_FOUND                     )
    ENUM_ITEM(NVAPI_SETTING_SIZE_TOO_LARGE                )
    ENUM_ITEM(NVAPI_TOO_MANY_SETTINGS_IN_PROFILE          )
    ENUM_ITEM(NVAPI_PROFILE_NOT_FOUND                     )
    ENUM_ITEM(NVAPI_PROFILE_NAME_IN_USE                   )
    ENUM_ITEM(NVAPI_PROFILE_NAME_EMPTY                    )
    ENUM_ITEM(NVAPI_EXECUTABLE_NOT_FOUND                  )
    ENUM_ITEM(NVAPI_EXECUTABLE_ALREADY_IN_USE             )
    ENUM_ITEM(NVAPI_DATATYPE_MISMATCH                     )
    ENUM_ITEM(NVAPI_PROFILE_REMOVED                       )
    ENUM_ITEM(NVAPI_UNREGISTERED_RESOURCE                 )
    ENUM_ITEM(NVAPI_ID_OUT_OF_RANGE                       )
    ENUM_ITEM(NVAPI_DISPLAYCONFIG_VALIDATION_FAILED       )
    ENUM_ITEM(NVAPI_DPMST_CHANGED                         )
    ENUM_ITEM(NVAPI_INSUFFICIENT_BUFFER                   )
    ENUM_ITEM(NVAPI_ACCESS_DENIED                         )
    ENUM_ITEM(NVAPI_MOSAIC_NOT_ACTIVE                     )
    ENUM_ITEM(NVAPI_SHARE_RESOURCE_RELOCATED              )
    ENUM_ITEM(NVAPI_REQUEST_USER_TO_DISABLE_DWM           )
    ENUM_ITEM(NVAPI_D3D_DEVICE_LOST                       )
    ENUM_ITEM(NVAPI_INVALID_CONFIGURATION                 )
    ENUM_ITEM(NVAPI_STEREO_HANDSHAKE_NOT_DONE             )
    ENUM_ITEM(NVAPI_EXECUTABLE_PATH_IS_AMBIGUOUS          )
    ENUM_ITEM(NVAPI_DEFAULT_STEREO_PROFILE_IS_NOT_DEFINED )
    ENUM_ITEM(NVAPI_DEFAULT_STEREO_PROFILE_DOES_NOT_EXIST )
    ENUM_ITEM(NVAPI_CLUSTER_ALREADY_EXISTS                )
    ENUM_ITEM(NVAPI_DPMST_DISPLAY_ID_EXPECTED             )
    ENUM_ITEM(NVAPI_INVALID_DISPLAY_ID                    )
    ENUM_ITEM(NVAPI_STREAM_IS_OUT_OF_SYNC                 )
    ENUM_ITEM(NVAPI_INCOMPATIBLE_AUDIO_DRIVER             )
    ENUM_ITEM(NVAPI_VALUE_ALREADY_SET                     )
    ENUM_ITEM(NVAPI_TIMEOUT                               )
    ENUM_ITEM(NVAPI_GPU_WORKSTATION_FEATURE_INCOMPLETE    )
    ENUM_ITEM(NVAPI_STEREO_INIT_ACTIVATION_NOT_DONE       )
    ENUM_ITEM(NVAPI_SYNC_NOT_ACTIVE                       )
    ENUM_ITEM(NVAPI_SYNC_MASTER_NOT_FOUND                 )
    ENUM_ITEM(NVAPI_INVALID_SYNC_TOPOLOGY                 )
    ENUM_ITEM(NVAPI_ECID_SIGN_ALGO_UNSUPPORTED            )
    ENUM_ITEM(NVAPI_ECID_KEY_VERIFICATION_FAILED          )
    ENUM_ITEM(NVAPI_FIRMWARE_OUT_OF_DATE                  )
    ENUM_ITEM(NVAPI_FIRMWARE_REVISION_NOT_SUPPORTED       )
    ENUM_ITEM(NVAPI_LICENSE_CALLER_AUTHENTICATION_FAILED  )
    ENUM_ITEM(NVAPI_D3D_DEVICE_NOT_REGISTERED             )
    ENUM_ITEM(NVAPI_RESOURCE_NOT_ACQUIRED                 )
    ENUM_ITEM(NVAPI_TIMING_NOT_SUPPORTED                  )
    ENUM_ITEM(NVAPI_HDCP_ENCRYPTION_FAILED                )
    ENUM_ITEM(NVAPI_PCLK_LIMITATION_FAILED                )
    ENUM_ITEM(NVAPI_NO_CONNECTOR_FOUND                    )
    ENUM_ITEM(NVAPI_HDCP_DISABLED                         )
    ENUM_ITEM(NVAPI_API_IN_USE                            )
    ENUM_ITEM(NVAPI_NVIDIA_DISPLAY_NOT_FOUND              )
    ENUM_ITEM(NVAPI_PRIV_SEC_VIOLATION                    )
    ENUM_ITEM(NVAPI_INCORRECT_VENDOR                      )
    ENUM_ITEM(NVAPI_DISPLAY_IN_USE                        )
    ENUM_ITEM(NVAPI_UNSUPPORTED_CONFIG_NON_HDCP_HMD       )
    ENUM_ITEM(NVAPI_MAX_DISPLAY_LIMIT_REACHED             )
    ENUM_ITEM(NVAPI_INVALID_DIRECT_MODE_DISPLAY           )
    ENUM_ITEM(NVAPI_GPU_IN_DEBUG_MODE                     )
    ENUM_ITEM(NVAPI_D3D_CONTEXT_NOT_FOUND                 )
    ENUM_ITEM(NVAPI_STEREO_VERSION_MISMATCH               )
    ENUM_ITEM(NVAPI_GPU_NOT_POWERED                       )
    ENUM_ITEM(NVAPI_ERROR_DRIVER_RELOAD_IN_PROGRESS       )
    ENUM_ITEM(NVAPI_WAIT_FOR_HW_RESOURCE                  )
    ENUM_ITEM(NVAPI_REQUIRE_FURTHER_HDCP_ACTION           )
    ENUM_ITEM(NVAPI_DISPLAY_MUX_TRANSITION_FAILED         )
    ENUM_ITEM(NVAPI_INVALID_DSC_VERSION                   )
    ENUM_ITEM(NVAPI_INVALID_DSC_SLICECOUNT                )
    ENUM_ITEM(NVAPI_INVALID_DSC_OUTPUT_BPP                )
    ENUM_ITEM(NVAPI_FAILED_TO_LOAD_FROM_DRIVER_STORE      )
    ENUM_ITEM(NVAPI_NO_VULKAN                             )
    ENUM_ITEM(NVAPI_REQUEST_PENDING                       )
    ENUM_ITEM(NVAPI_RESOURCE_IN_USE                       )
    ENUM_ITEM(NVAPI_INVALID_IMAGE                         )
    ENUM_ITEM(NVAPI_INVALID_PTX                           )
    ENUM_ITEM(NVAPI_NVLINK_UNCORRECTABLE                  )
    ENUM_ITEM(NVAPI_JIT_COMPILER_NOT_FOUND                )
    ENUM_ITEM(NVAPI_INVALID_SOURCE                        )
    ENUM_ITEM(NVAPI_ILLEGAL_INSTRUCTION                   )
    ENUM_ITEM(NVAPI_INVALID_PC                            )
    ENUM_ITEM(NVAPI_LAUNCH_FAILED                         )
    ENUM_ITEM(NVAPI_NOT_PERMITTED                         )
    ENUM_ITEM(NVAPI_CALLBACK_ALREADY_REGISTERED           )
    ENUM_ITEM(NVAPI_CALLBACK_NOT_FOUND                    )
ENUM_END(NvAPI_Status)

ENUM_BEGIN(NV_SYSTEM_TYPE)
    ENUM_ITEM(NV_SYSTEM_TYPE_UNKNOWN)
    ENUM_ITEM(NV_SYSTEM_TYPE_LAPTOP)
    ENUM_ITEM(NV_SYSTEM_TYPE_DESKTOP)
ENUM_END(NV_SYSTEM_TYPE)

ENUM_BEGIN(NV_GPU_TYPE)
    ENUM_ITEM(NV_SYSTEM_TYPE_GPU_UNKNOWN)
    ENUM_ITEM(NV_SYSTEM_TYPE_IGPU)
    ENUM_ITEM(NV_SYSTEM_TYPE_DGPU)
ENUM_END(NV_GPU_TYPE)

ENUM_BEGIN(NV_GPU_BUS_TYPE)
    ENUM_ITEM(NVAPI_GPU_BUS_TYPE_UNDEFINED)
    ENUM_ITEM(NVAPI_GPU_BUS_TYPE_PCI)
    ENUM_ITEM(NVAPI_GPU_BUS_TYPE_AGP)
    ENUM_ITEM(NVAPI_GPU_BUS_TYPE_PCI_EXPRESS)
    ENUM_ITEM(NVAPI_GPU_BUS_TYPE_FPCI)
    ENUM_ITEM(NVAPI_GPU_BUS_TYPE_AXI)
ENUM_END(NV_GPU_BUS_TYPE)

ENUM_BEGIN(NV_GPU_ARCHITECTURE_ID)
    ENUM_ITEM(NV_GPU_ARCHITECTURE_T2X  )
    ENUM_ITEM(NV_GPU_ARCHITECTURE_T3X  )
    ENUM_ITEM(NV_GPU_ARCHITECTURE_T4X  )
    ENUM_ITEM(NV_GPU_ARCHITECTURE_T12X )
    ENUM_ITEM(NV_GPU_ARCHITECTURE_NV40 )
    ENUM_ITEM(NV_GPU_ARCHITECTURE_NV50 )
    ENUM_ITEM(NV_GPU_ARCHITECTURE_G78  )
    ENUM_ITEM(NV_GPU_ARCHITECTURE_G80  )
    ENUM_ITEM(NV_GPU_ARCHITECTURE_G90  )
    ENUM_ITEM(NV_GPU_ARCHITECTURE_GT200)
    ENUM_ITEM(NV_GPU_ARCHITECTURE_GF100)
    ENUM_ITEM(NV_GPU_ARCHITECTURE_GF110)
    ENUM_ITEM(NV_GPU_ARCHITECTURE_GK100)
    ENUM_ITEM(NV_GPU_ARCHITECTURE_GK110)
    ENUM_ITEM(NV_GPU_ARCHITECTURE_GK200)
    ENUM_ITEM(NV_GPU_ARCHITECTURE_GM000)
    ENUM_ITEM(NV_GPU_ARCHITECTURE_GM200)
    ENUM_ITEM(NV_GPU_ARCHITECTURE_GP100)
    ENUM_ITEM(NV_GPU_ARCHITECTURE_GV100)
    ENUM_ITEM(NV_GPU_ARCHITECTURE_GV110)
    ENUM_ITEM(NV_GPU_ARCHITECTURE_TU100)
    ENUM_ITEM(NV_GPU_ARCHITECTURE_GA100)
    ENUM_ITEM(NV_GPU_ARCHITECTURE_AD100)
    ENUM_ITEM(NV_GPU_ARCHITECTURE_GB200)
ENUM_END(NV_GPU_ARCHITECTURE_ID)

// Sum of values from NV_GPU_ARCHITECTURE_ID and NV_GPU_ARCH_IMPLEMENTATION_ID!
static const EnumItem Enum_NV_ARCH_plus_IMPLEMENTATION_ID[] = {
    { L"NV_GPU_ARCH_IMPLEMENTATION_T20",   0xE0000020 },

    { L"NV_GPU_ARCH_IMPLEMENTATION_T30",   0xE0000030 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_T35",   0xE0000035 },

    { L"NV_GPU_ARCH_IMPLEMENTATION_T40",   0xE0000040 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_T124",  0xE0000040 },

    { L"NV_GPU_ARCH_IMPLEMENTATION_NV40",  0x00000040 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_NV41",  0x00000041 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_NV42",  0x00000042 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_NV43",  0x00000043 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_NV44",  0x00000044 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_NV44A", 0x0000004A },
    { L"NV_GPU_ARCH_IMPLEMENTATION_NV46",  0x00000046 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_NV47",  0x00000047 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_NV49",  0x00000049 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_NV4B",  0x0000004B },
    { L"NV_GPU_ARCH_IMPLEMENTATION_NV4C",  0x0000004C },
    { L"NV_GPU_ARCH_IMPLEMENTATION_NV4E",  0x0000004E },

    { L"NV_GPU_ARCH_IMPLEMENTATION_NV50",  0x00000050 },

    { L"NV_GPU_ARCH_IMPLEMENTATION_NV63",  0x00000063 }, // Are these really NV_GPU_ARCHITECTURE_G78 despite name NV6X?
    { L"NV_GPU_ARCH_IMPLEMENTATION_NV67",  0x00000067 },

    { L"NV_GPU_ARCH_IMPLEMENTATION_G84",   0x00000084 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_G86",   0x00000086 },

    { L"NV_GPU_ARCH_IMPLEMENTATION_G92",   0x00000092 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_G94",   0x00000094 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_G96",   0x00000096 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_G98",   0x00000098 },

    { L"NV_GPU_ARCH_IMPLEMENTATION_GT200", 0x000000A0 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GT212", 0x000000A2 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GT214", 0x000000A4 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GT215", 0x000000A3 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GT216", 0x000000A5 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GT218", 0x000000A8 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_MCP77", 0x000000AA }, // Is this still NV_GPU_ARCHITECTURE_GT200 despite name MCP77?
    { L"NV_GPU_ARCH_IMPLEMENTATION_GT21C", 0x000000AB },
    { L"NV_GPU_ARCH_IMPLEMENTATION_MCP79", 0x000000AC },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GT21A", 0x000000AD },
    { L"NV_GPU_ARCH_IMPLEMENTATION_MCP89", 0x000000AF },

    { L"NV_GPU_ARCH_IMPLEMENTATION_GF100", 0x000000C0 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GF104", 0x000000C4 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GF106", 0x000000C3 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GF108", 0x000000C1 },

    { L"NV_GPU_ARCH_IMPLEMENTATION_GF110", 0x000000D0 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GF116", 0x000000D6 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GF117", 0x000000D7 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GF118", 0x000000D8 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GF119", 0x000000D9 },

    { L"NV_GPU_ARCH_IMPLEMENTATION_GK104", 0x000000E4 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GK106", 0x000000E6 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GK107", 0x000000E7 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GK20A", 0x000000EA }, // Is this NV_GPU_ARCHITECTURE_GK200 because of name GK20A or still NV_GPU_ARCHITECTURE_GK110 because of where it's placed?
    { L"NV_GPU_ARCH_IMPLEMENTATION_GK110", 0x000000F0 },

    { L"NV_GPU_ARCH_IMPLEMENTATION_GK208", 0x00000108 },

    { L"NV_GPU_ARCH_IMPLEMENTATION_GM204", 0x00000124 }, // Are these really NV_GPU_ARCHITECTURE_GM200? Then, NV_GPU_ARCHITECTURE_GM000 is unused?
    { L"NV_GPU_ARCH_IMPLEMENTATION_GM206", 0x00000126 },

    { L"NV_GPU_ARCH_IMPLEMENTATION_GP100", 0x00000130 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GP000", 0x00000131 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GP102", 0x00000132 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GP104", 0x00000134 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GP106", 0x00000136 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GP107", 0x00000137 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GP108", 0x00000138 },

    { L"NV_GPU_ARCH_IMPLEMENTATION_GV100", 0x00000140 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GV10B", 0x0000014B }, // Is NV_GPU_ARCHITECTURE_GV110 unused?

    { L"NV_GPU_ARCH_IMPLEMENTATION_TU100", 0x00000160 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_TU102", 0x00000162 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_TU104", 0x00000164 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_TU106", 0x00000166 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_TU116", 0x00000168 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_TU117", 0x00000167 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_TU000", 0x00000161 },

    { L"NV_GPU_ARCH_IMPLEMENTATION_GA100", 0x00000170 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GA102", 0x00000172 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_GA104", 0x00000174 },

    { L"NV_GPU_ARCH_IMPLEMENTATION_AD102", 0x00000192 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_AD103", 0x00000193 },
    { L"NV_GPU_ARCH_IMPLEMENTATION_AD104", 0x00000194 },

    { L"NV_GPU_ARCH_IMPLEMENTATION_GB202", 0x00000182 },
	{ NULL, UINT32_MAX } };
static EnumRegistration g_Enum_NV_ARCH_plus_IMPLEMENTATION_ID(L"NV_ARCH_plus_IMPLEMENTATION_ID", Enum_NV_ARCH_plus_IMPLEMENTATION_ID);

ENUM_BEGIN(NV_GPU_CHIP_REVISION)
    ENUM_ITEM(NV_GPU_CHIP_REV_EMULATION_QT  )
    ENUM_ITEM(NV_GPU_CHIP_REV_EMULATION_FPGA)
    ENUM_ITEM(NV_GPU_CHIP_REV_A01           )
    ENUM_ITEM(NV_GPU_CHIP_REV_A02           )
    ENUM_ITEM(NV_GPU_CHIP_REV_A03           )
    ENUM_ITEM(NV_GPU_CHIP_REV_UNKNOWN       )
ENUM_END(NV_GPU_CHIP_REVISION)

ENUM_BEGIN(NV_GPU_ILLUMINATION_ATTRIB)
    ENUM_ITEM(NV_GPU_IA_LOGO_BRIGHTNESS)  
    ENUM_ITEM(NV_GPU_IA_SLI_BRIGHTNESS )
ENUM_END(NV_GPU_ILLUMINATION_ATTRIB)

ENUM_BEGIN(NV_GPU_WORKSTATION_FEATURE_TYPE)
    ENUM_ITEM(NV_GPU_WORKSTATION_FEATURE_TYPE_NVIDIA_RTX_VR_READY)
    ENUM_ITEM(NV_GPU_WORKSTATION_FEATURE_TYPE_PROVIZ)
ENUM_END(NV_GPU_WORKSTATION_FEATURE_TYPE)

ENUM_BEGIN(NVAPI_D3D12_RAYTRACING_THREAD_REORDERING_CAPS)
    ENUM_ITEM(NVAPI_D3D12_RAYTRACING_THREAD_REORDERING_CAP_NONE)
    ENUM_ITEM(NVAPI_D3D12_RAYTRACING_THREAD_REORDERING_CAP_STANDARD)
ENUM_END(NVAPI_D3D12_RAYTRACING_THREAD_REORDERING_CAPS)

ENUM_BEGIN(NVAPI_D3D12_RAYTRACING_OPACITY_MICROMAP_CAPS)
    ENUM_ITEM(NVAPI_D3D12_RAYTRACING_OPACITY_MICROMAP_CAP_NONE)
    ENUM_ITEM(NVAPI_D3D12_RAYTRACING_OPACITY_MICROMAP_CAP_STANDARD)
ENUM_END(NVAPI_D3D12_RAYTRACING_OPACITY_MICROMAP_CAPS)

ENUM_BEGIN(NV_ECC_CONFIGURATION)
    ENUM_ITEM(NV_ECC_CONFIGURATION_NOT_SUPPORTED)
    ENUM_ITEM(NV_ECC_CONFIGURATION_DEFERRED)
    ENUM_ITEM(NV_ECC_CONFIGURATION_IMMEDIATE)
ENUM_END(NV_ECC_CONFIGURATION)

ENUM_BEGIN(NVAPI_D3D12_RAYTRACING_DISPLACEMENT_MICROMAP_CAPS)
    ENUM_ITEM(NVAPI_D3D12_RAYTRACING_DISPLACEMENT_MICROMAP_CAP_NONE)
    ENUM_ITEM(NVAPI_D3D12_RAYTRACING_DISPLACEMENT_MICROMAP_CAP_STANDARD)
ENUM_END(NVAPI_D3D12_RAYTRACING_DISPLACEMENT_MICROMAP_CAPS)

ENUM_BEGIN(NVAPI_D3D12_RAYTRACING_CLUSTER_OPERATIONS_CAPS)
    ENUM_ITEM(NVAPI_D3D12_RAYTRACING_CLUSTER_OPERATIONS_CAP_NONE)
    ENUM_ITEM(NVAPI_D3D12_RAYTRACING_CLUSTER_OPERATIONS_CAP_STANDARD)
ENUM_END(NVAPI_D3D12_RAYTRACING_CLUSTER_OPERATIONS_CAPS)

ENUM_BEGIN(NVAPI_D3D12_RAYTRACING_PARTITIONED_TLAS_CAPS)
    ENUM_ITEM(NVAPI_D3D12_RAYTRACING_PARTITIONED_TLAS_CAP_NONE)
    ENUM_ITEM(NVAPI_D3D12_RAYTRACING_PARTITIONED_TLAS_CAP_STANDARD)
ENUM_END(NVAPI_D3D12_RAYTRACING_PARTITIONED_TLAS_CAPS)

ENUM_BEGIN(NVAPI_D3D12_RAYTRACING_SPHERES_CAPS)
    ENUM_ITEM(NVAPI_D3D12_RAYTRACING_SPHERES_CAP_NONE)
    ENUM_ITEM(NVAPI_D3D12_RAYTRACING_SPHERES_CAP_STANDARD)
ENUM_END(NVAPI_D3D12_RAYTRACING_SPHERES_CAPS)

ENUM_BEGIN(NVAPI_D3D12_RAYTRACING_LINEAR_SWEPT_SPHERES_CAPS)
    ENUM_ITEM(NVAPI_D3D12_RAYTRACING_LINEAR_SWEPT_SPHERES_CAP_NONE)
    ENUM_ITEM(NVAPI_D3D12_RAYTRACING_LINEAR_SWEPT_SPHERES_CAP_STANDARD)
ENUM_END(NVAPI_D3D12_RAYTRACING_LINEAR_SWEPT_SPHERES_CAPS)

ENUM_BEGIN(NVAPI_NVLINK_CAPS)
    ENUM_ITEM(NVAPI_NVLINK_CAPS_SUPPORTED)
    ENUM_ITEM(NVAPI_NVLINK_CAPS_P2P_SUPPORTED)
    ENUM_ITEM(NVAPI_NVLINK_CAPS_SYSMEM_ACCESS)
    ENUM_ITEM(NVAPI_NVLINK_CAPS_P2P_ATOMICS)
    ENUM_ITEM(NVAPI_NVLINK_CAPS_SYSMEM_ATOMICS)
    ENUM_ITEM(NVAPI_NVLINK_CAPS_PEX_TUNNELING)
    ENUM_ITEM(NVAPI_NVLINK_CAPS_SLI_BRIDGE)
    ENUM_ITEM(NVAPI_NVLINK_CAPS_SLI_BRIDGE_SENSABLE)
    ENUM_ITEM(NVAPI_NVLINK_CAPS_POWER_STATE_L0)
    ENUM_ITEM(NVAPI_NVLINK_CAPS_POWER_STATE_L1)
    ENUM_ITEM(NVAPI_NVLINK_CAPS_POWER_STATE_L2)
    ENUM_ITEM(NVAPI_NVLINK_CAPS_POWER_STATE_L3)
ENUM_END(NVAPI_NVLINK_CAPS)

ENUM_BEGIN(NV_ADAPTER_TYPE)
    ENUM_ITEM(NV_ADAPTER_TYPE_UNKNOWN)
    ENUM_ITEM(NV_ADAPTER_TYPE_WDDM)
    ENUM_ITEM(NV_ADAPTER_TYPE_MCDM)
    ENUM_ITEM(NV_ADAPTER_TYPE_TCC)
ENUM_END(NV_ADAPTER_TYPE)

ENUM_BEGIN(NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE)
    ENUM_ITEM(NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE_FLOAT16)
    ENUM_ITEM(NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE_FLOAT32)
    ENUM_ITEM(NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE_FLOAT64)
    ENUM_ITEM(NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE_SINT8)
    ENUM_ITEM(NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE_SINT16)
    ENUM_ITEM(NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE_SINT32)
    ENUM_ITEM(NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE_SINT64)
    ENUM_ITEM(NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE_UINT8)
    ENUM_ITEM(NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE_UINT16)
    ENUM_ITEM(NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE_UINT32)
    ENUM_ITEM(NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE_UINT64)
    ENUM_ITEM(NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE_SINT8_PACKED)
    ENUM_ITEM(NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE_UINT8_PACKED)
    ENUM_ITEM(NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE_FLOAT_E4M3)
    ENUM_ITEM(NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE_FLOAT_E5M2)
    ENUM_ITEM(NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE_INVALID)
ENUM_END(NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE)

/*
NVAPI has a hierarchy: logical devices contain physical devices. We only need
the NvPhysicalGpuHandle (and, in some cases, ID3D12Device) to query for
capabilities, not logical device.

There are two ways to query the list of devices:

1. NvAPI_EnumLogicalGPUs, NvAPI_GPU_GetLogicalGpuInfo
   struct NV_LOGICAL_GPU_DATA

   It queries for the list of logical devices, each having a list of physical devices.
   It allows correlating with device LUID. We need it, so we primarily use this method.

2. NvAPI_SYS_GetLogicalGPUs, NvAPI_SYS_GetPhysicalGPUs
   struct NV_LOGICAL_GPUS, NV_LOGICAL_GPU_HANDLE_DATA, NV_PHYSICAL_GPUS, NV_PHYSICAL_GPU_HANDLE_DATA

   It has an advantage of returning NV_ADAPTER_TYPE. We use this method as ancillary
   to just fetch this enum.
*/

static NvU32 g_LogicalGpuCount = 0;
static NvLogicalGpuHandle g_LogicalGpuHandles[NVAPI_MAX_LOGICAL_GPUS];
static NV_LOGICAL_GPU_DATA g_LogicalGpuData[NVAPI_MAX_LOGICAL_GPUS];
static LUID g_LogicalGpuLuids[NVAPI_MAX_LOGICAL_GPUS];
static NV_PHYSICAL_GPUS g_PhysicalGpus;

static wstring NvShortStringToStr(NvAPI_ShortString str)
{
    wchar_t w[NVAPI_SHORT_STRING_MAX];
    swprintf_s(w, L"%hs", str);
    return wstring{w};
}

static void LoadGpus()
{
    if(NvAPI_EnumLogicalGPUs(g_LogicalGpuHandles, &g_LogicalGpuCount) != NVAPI_OK)
        g_LogicalGpuCount = 0;
    for(NvU32 i = 0; i < g_LogicalGpuCount; ++i)
    {
        g_LogicalGpuData[i] = {
            .version = NV_LOGICAL_GPU_DATA_VER,
            .pOSAdapterId = &g_LogicalGpuLuids[i]};
        NvAPI_Status status = NvAPI_GPU_GetLogicalGpuInfo(g_LogicalGpuHandles[i], &g_LogicalGpuData[i]);
        assert(status == NVAPI_OK);
    }

    // Success in fetching this structure is optional.
    g_PhysicalGpus.version = NV_PHYSICAL_GPUS_VER;
    if (NvAPI_SYS_GetPhysicalGPUs(&g_PhysicalGpus) != NVAPI_OK)
    {
        ZeroMemory(&g_PhysicalGpus, sizeof g_PhysicalGpus);
    }
}

static bool FindPhysicalGpu(const LUID& adapterLuid, NvPhysicalGpuHandle& outGpu)
{
    NvU32 foundIndex = UINT32_MAX;
    for(NvU32 i = 0; i < g_LogicalGpuCount; ++i)
    {
        if(memcmp(&g_LogicalGpuLuids[i], &adapterLuid, sizeof(LUID)) == 0)
        {
            if(g_LogicalGpuData[i].physicalGpuCount != 1)
                // A logical GPU with multiple physical GPUs not supported by this tool.
                return false;
            if(foundIndex != UINT32_MAX)
                // Found multiple GPUs with same LUID.
                return false;
            else
                foundIndex = i;
        }
    }
    if(foundIndex != UINT32_MAX)
    {
        outGpu = g_LogicalGpuData[foundIndex].physicalGpuHandles[0];
        return true;
    }
    return false;
}

static bool FindPhysicalGpuAdapterType(NvPhysicalGpuHandle physicalGpuHandle, NV_ADAPTER_TYPE& outAdapterType)
{
    for(size_t i = 0; i < g_PhysicalGpus.gpuHandleCount; ++i)
    {
        if(g_PhysicalGpus.gpuHandleData[i].hPhysicalGpu == physicalGpuHandle)
        {
            outAdapterType = g_PhysicalGpus.gpuHandleData[i].adapterType;
            return true;
        }
    }
    return false;
}

static void PrintCooperativeVectorProperty(size_t index, const NVAPI_COOPERATIVE_VECTOR_PROPERTIES& props)
{
    if (g_UseJson)
    {
        Json::BeginObject();
    }
    else
    {
        PrintIndent();
        wprintf(L"NVAPI_COOPERATIVE_VECTOR_PROPERTIES %zu:\n", index);
        ++g_Indent;
    }

    Print_hex32(L"version", props.version);
    PrintEnum(L"inputType", props.inputType, Enum_NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE);
    PrintEnum(L"inputInterpretation", props.inputInterpretation, Enum_NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE);
    PrintEnum(L"matrixInterpretation", props.matrixInterpretation, Enum_NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE);
    PrintEnum(L"biasInterpretation", props.biasInterpretation, Enum_NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE);
    PrintEnum(L"resultType", props.resultType, Enum_NVAPI_COOPERATIVE_VECTOR_COMPONENT_TYPE);
    Print_BOOL(L"transpose", props.transpose);

    if (g_UseJson)
        Json::EndObject();
    else
    {
        --g_Indent;
    }
}

static void PrintCooperativeVectorProperties(const std::vector<NVAPI_COOPERATIVE_VECTOR_PROPERTIES>& props)
{
    if (g_UseJson)
    {
        Json::WriteString(L"NvAPI_D3D12_GetPhysicalDeviceCooperativeVectorProperties");
        Json::BeginArray();
    }
    else
    {
        PrintHeader(L"NvAPI_D3D12_GetPhysicalDeviceCooperativeVectorProperties", 1);
        ++g_Indent;
    }

    for(size_t i = 0; i < props.size(); ++i)
    {
        PrintCooperativeVectorProperty(i, props[i]);
    }

    if (g_UseJson)
        Json::EndArray();
    else
    {
        --g_Indent;
        PrintEmptyLine();
    }
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC

void NvAPI_Inititalize_RAII::PrintStaticParams()
{
    Print_string(L"NvAPI compiled version", NVAPI_COMPILED_VERSION);
    Print_uint32(L"NVAPI_SDK_VERSION", NVAPI_SDK_VERSION);

    NvAPI_ShortString nvShortString;
    if(NvAPI_GetInterfaceVersionString(nvShortString) == NVAPI_OK)
        Print_string(L"NvAPI_GetInterfaceVersionString", NvShortStringToStr(nvShortString).c_str());
}

NvAPI_Inititalize_RAII::NvAPI_Inititalize_RAII()
{
    m_Initialized = NvAPI_Initialize() == NVAPI_OK;
    if(m_Initialized)
        LoadGpus();
}

NvAPI_Inititalize_RAII::~NvAPI_Inititalize_RAII()
{
    if(m_Initialized)
        NvAPI_Unload();
}

void NvAPI_Inititalize_RAII::PrintData()
{
	assert(m_Initialized);
    
    NvU32 pDriverVersion = UINT32_MAX;
    NvAPI_ShortString szBuildBranchString = {};
    if(NvAPI_SYS_GetDriverAndBranchVersion(&pDriverVersion, szBuildBranchString) == NVAPI_OK)
    {
        ScopedStructRegion region(L"NvAPI_SYS_GetDriverAndBranchVersion");
        Print_uint32(L"pDriverVersion", pDriverVersion);
        Print_string(L"szBuildBranchString", NvShortStringToStr(szBuildBranchString).c_str());
    }

    {
        NV_DISPLAY_DRIVER_INFO info = {NV_DISPLAY_DRIVER_INFO_VER};
        if(NvAPI_SYS_GetDisplayDriverInfo(&info) == NVAPI_OK)
        {
            ScopedStructRegion region(L"NvAPI_SYS_GetDisplayDriverInfo - NV_DISPLAY_DRIVER_INFO");
            Print_uint32(L"driverVersion", info.driverVersion);
            Print_string(L"szBuildBranch", StrToWstr(info.szBuildBranch, CP_ACP).c_str());
            Print_BOOL(L"bIsDCHDriver", info.bIsDCHDriver != 0);
            Print_BOOL(L"bIsNVIDIAStudioPackage", info.bIsNVIDIAStudioPackage != 0);
            Print_BOOL(L"bIsNVIDIAGameReadyPackage", info.bIsNVIDIAGameReadyPackage != 0);
            Print_BOOL(L"bIsNVIDIARTXProductionBranchPackage", info.bIsNVIDIARTXProductionBranchPackage != 0);
            Print_BOOL(L"bIsNVIDIARTXNewFeatureBranchPackage", info.bIsNVIDIARTXNewFeatureBranchPackage != 0);
            Print_string(L"szBuildBaseBranch", StrToWstr(info.szBuildBaseBranch, CP_ACP).c_str());
        }
    }
}

void NvAPI_Inititalize_RAII::PrintD3d12DeviceData(ID3D12Device* device)
{
	assert(m_Initialized);
    
    wstring s;

    {
        NvU64 totalBytes = 0, freeBytes = 0;
        if(NvAPI_D3D12_QueryCpuVisibleVidmem(device, &totalBytes, &freeBytes) == NVAPI_OK)
        {
            ScopedStructRegion region(L"NvAPI_D3D12_QueryCpuVisibleVidmem");
            Print_size(L"pTotalBytes", totalBytes);
        }
    }

    {
        ScopedStructRegion region(L"NvAPI_D3D12_IsNvShaderExtnOpCodeSupported");
        for(const EnumItem* ei = Enum_NV_EXTN_OP; ei->m_Name != nullptr; ++ei)
        {
            bool supported = false;
            if(NvAPI_D3D12_IsNvShaderExtnOpCodeSupported(device, ei->m_Value, &supported) == NVAPI_OK)
                Print_BOOL(ei->m_Name, supported);
        }
    }

    {
        NvU32 threadCount = 0;
        if(NvAPI_D3D12_GetOptimalThreadCountForMesh(device, &threadCount) == NVAPI_OK)
        {
            ScopedStructRegion region(L"NvAPI_D3D12_GetOptimalThreadCountForMesh");
            Print_uint32(L"pThreadCount", (uint32_t)threadCount);
        }
    }

    {
        ScopedStructRegion region(L"NvAPI_D3D12_GetRaytracingCaps");
        NVAPI_D3D12_RAYTRACING_THREAD_REORDERING_CAPS threadReorderingCaps = {};
        if(NvAPI_D3D12_GetRaytracingCaps(device, NVAPI_D3D12_RAYTRACING_CAPS_TYPE_THREAD_REORDERING,
            &threadReorderingCaps, sizeof threadReorderingCaps) == NVAPI_OK)
        {
            PrintEnum(L"NVAPI_D3D12_RAYTRACING_CAPS_TYPE_THREAD_REORDERING", (uint32_t)threadReorderingCaps,
                Enum_NVAPI_D3D12_RAYTRACING_THREAD_REORDERING_CAPS);
        }

        NVAPI_D3D12_RAYTRACING_OPACITY_MICROMAP_CAPS opacityMicromapCaps = {};
        if(NvAPI_D3D12_GetRaytracingCaps(device, NVAPI_D3D12_RAYTRACING_CAPS_TYPE_OPACITY_MICROMAP,
            &opacityMicromapCaps, sizeof opacityMicromapCaps) == NVAPI_OK)
        {
            PrintEnum(L"NVAPI_D3D12_RAYTRACING_CAPS_TYPE_OPACITY_MICROMAP", (uint32_t)opacityMicromapCaps,
                Enum_NVAPI_D3D12_RAYTRACING_OPACITY_MICROMAP_CAPS);
        }

        NVAPI_D3D12_RAYTRACING_DISPLACEMENT_MICROMAP_CAPS displacementMicromapCaps = {};
        if(NvAPI_D3D12_GetRaytracingCaps(device, NVAPI_D3D12_RAYTRACING_CAPS_TYPE_DISPLACEMENT_MICROMAP,
            &displacementMicromapCaps, sizeof displacementMicromapCaps) == NVAPI_OK)
        {
            PrintEnum(L"NVAPI_D3D12_RAYTRACING_CAPS_TYPE_DISPLACEMENT_MICROMAP", (uint32_t)displacementMicromapCaps,
                Enum_NVAPI_D3D12_RAYTRACING_DISPLACEMENT_MICROMAP_CAPS);
        }

        if (NVAPI_D3D12_RAYTRACING_CLUSTER_OPERATIONS_CAPS caps = {};
            NvAPI_D3D12_GetRaytracingCaps(device, NVAPI_D3D12_RAYTRACING_CAPS_TYPE_CLUSTER_OPERATIONS,
            &caps, sizeof caps) == NVAPI_OK)
        {
            PrintEnum(L"NVAPI_D3D12_RAYTRACING_CAPS_TYPE_CLUSTER_OPERATIONS", (uint32_t)caps,
                Enum_NVAPI_D3D12_RAYTRACING_CLUSTER_OPERATIONS_CAPS);
        }

        if (NVAPI_D3D12_RAYTRACING_PARTITIONED_TLAS_CAPS caps = {};
            NvAPI_D3D12_GetRaytracingCaps(device, NVAPI_D3D12_RAYTRACING_CAPS_TYPE_PARTITIONED_TLAS,
                &caps, sizeof caps) == NVAPI_OK)
        {
            PrintEnum(L"NVAPI_D3D12_RAYTRACING_CAPS_TYPE_PARTITIONED_TLAS", (uint32_t)caps,
                Enum_NVAPI_D3D12_RAYTRACING_PARTITIONED_TLAS_CAPS);
        }

        if (NVAPI_D3D12_RAYTRACING_SPHERES_CAPS caps = {};
            NvAPI_D3D12_GetRaytracingCaps(device, NVAPI_D3D12_RAYTRACING_CAPS_TYPE_SPHERES,
                &caps, sizeof caps) == NVAPI_OK)
        {
            PrintEnum(L"NVAPI_D3D12_RAYTRACING_CAPS_TYPE_SPHERES", (uint32_t)caps,
                Enum_NVAPI_D3D12_RAYTRACING_SPHERES_CAPS);
        }

        if (NVAPI_D3D12_RAYTRACING_LINEAR_SWEPT_SPHERES_CAPS caps = {};
            NvAPI_D3D12_GetRaytracingCaps(device, NVAPI_D3D12_RAYTRACING_CAPS_TYPE_LINEAR_SWEPT_SPHERES,
                &caps, sizeof caps) == NVAPI_OK)
        {
            PrintEnum(L"NVAPI_D3D12_RAYTRACING_CAPS_TYPE_LINEAR_SWEPT_SPHERES", (uint32_t)caps,
                Enum_NVAPI_D3D12_RAYTRACING_LINEAR_SWEPT_SPHERES_CAPS);
        }
    }

    {
        ScopedStructRegion region(L"NvAPI_D3D12_QueryWorkstationFeatureProperties");
        NVAPI_D3D12_WORKSTATION_FEATURE_PROPERTIES_PARAMS params = {
            .version = NVAPI_D3D12_WORKSTATION_FEATURE_PROPERTIES_PARAMS_VER };

        params.workstationFeatureType = NV_D3D12_WORKSTATION_FEATURE_TYPE_PRESENT_BARRIER;
        if(NvAPI_D3D12_QueryWorkstationFeatureProperties(device, &params) == NVAPI_OK)
        {
            Print_BOOL(L"NV_D3D12_WORKSTATION_FEATURE_TYPE_PRESENT_BARRIER - supported", params.supported);
        }

        params.workstationFeatureType = NV_D3D12_WORKSTATION_FEATURE_TYPE_RDMA_BAR1_SUPPORT;
        if(NvAPI_D3D12_QueryWorkstationFeatureProperties(device, &params) == NVAPI_OK)
        {
            Print_BOOL(L"NV_D3D12_WORKSTATION_FEATURE_TYPE_RDMA_BAR1_SUPPORT - supported", params.supported);
            if(params.supported)
            {
                Print_uint64(L"NV_D3D12_WORKSTATION_FEATURE_TYPE_RDMA_BAR1_SUPPORT - rdmaHeapSize", params.rdmaInfo.rdmaHeapSize);
                
            }
        }
    }

    {
        bool appClampNeeded = false;
        if(NvAPI_D3D12_GetNeedsAppFPBlendClamping(device, &appClampNeeded) == NVAPI_OK)
        {
            ScopedStructRegion region(L"NvAPI_D3D12_GetNeedsAppFPBlendClamping");
            Print_BOOL(L"pAppClampNeeded", appClampNeeded);
        }
    }

    if(NvU32 count = 0;
        NvAPI_D3D12_GetPhysicalDeviceCooperativeVectorProperties(device, &count, nullptr) == NVAPI_OK &&
        count > 0)
    {
        if(std::vector<NVAPI_COOPERATIVE_VECTOR_PROPERTIES> props(count);
            NvAPI_D3D12_GetPhysicalDeviceCooperativeVectorProperties(device, &count, props.data()) == NVAPI_OK)
        {
            PrintCooperativeVectorProperties(props);
        }
    }
}

// Prints only implementationId as the numerical value, but searches enum
// using architectureId + implementationId.
static void PrintNvImplementationId(const wchar_t* name, uint32_t architectureId, uint32_t implementationId)
{
    if(g_UseJson)
        Print_uint32(name, implementationId);
    else
    {
        PrintIndent();
        PrintName(name);
        const wchar_t* enumItemName = FindEnumItemName(architectureId + implementationId,
            Enum_NV_ARCH_plus_IMPLEMENTATION_ID);
        if(enumItemName != nullptr)
            wprintf(L" = %s (0x%X)\n", enumItemName, implementationId);
        else
            wprintf(L" = 0x%X\n", implementationId);
    }
}

void NvAPI_Inititalize_RAII::PrintPhysicalGpuData(const LUID& adapterLuid)
{
	assert(m_Initialized);

    NvPhysicalGpuHandle gpu = {};
    // This will always fail on non Nvidia GPUs
    // so NVAPI can't be forced to run on non Nvidia GPU
    if(!FindPhysicalGpu(adapterLuid, gpu))
        return;

    ScopedStructRegion region(L"NvPhysicalGpuHandle");

    if(NV_ADAPTER_TYPE adapterType; FindPhysicalGpuAdapterType(gpu, adapterType))
    {
        PrintEnum(L"adapterType", (uint32_t)adapterType, Enum_NV_ADAPTER_TYPE);
    }

    NV_SYSTEM_TYPE systemType = {};
    if(NvAPI_GPU_GetSystemType(gpu, &systemType) == NVAPI_OK)
        PrintEnum(L"NvAPI_GPU_GetSystemType", systemType, Enum_NV_SYSTEM_TYPE);

    NvAPI_ShortString name = {};
    if(NvAPI_GPU_GetFullName(gpu, name) == NVAPI_OK)
        Print_string(L"NvAPI_GPU_GetFullName", StrToWstr(name, CP_ACP).c_str());

    NvU32 DeviceId = 0, SubSystemId = 0, RevisionId = 0, ExtDeviceId = 0;
    if(NvAPI_GPU_GetPCIIdentifiers(gpu, &DeviceId, &SubSystemId, &RevisionId, &ExtDeviceId) == NVAPI_OK)
    {
        Print_hex32(L"NvAPI_GPU_GetPCIIdentifiers - pDeviceID", DeviceId);
        PrintSubsystemId(L"NvAPI_GPU_GetPCIIdentifiers - pSubSystemId", SubSystemId);
        Print_hex32(L"NvAPI_GPU_GetPCIIdentifiers - pRevisionId", RevisionId);
        Print_hex32(L"NvAPI_GPU_GetPCIIdentifiers - pExtDeviceId", ExtDeviceId);
    }

    NV_GPU_TYPE gpuType = {};
    if(NvAPI_GPU_GetGPUType(gpu, &gpuType) == NVAPI_OK)
        PrintEnum(L"NvAPI_GPU_GetGPUType", gpuType, Enum_NV_GPU_TYPE);
    
    NV_GPU_BUS_TYPE busType = {};
    if(NvAPI_GPU_GetBusType(gpu, &busType) == NVAPI_OK)
        PrintEnum(L"NvAPI_GPU_GetBusType", busType, Enum_NV_GPU_BUS_TYPE);

    NvU32 biosRevision = 0;
    if(NvAPI_GPU_GetVbiosRevision(gpu, &biosRevision) == NVAPI_OK)
        Print_uint32(L"NvAPI_GPU_GetVbiosRevision", biosRevision);

    NvU32 biosOemRevision = 0;
    if(NvAPI_GPU_GetVbiosOEMRevision(gpu, &biosOemRevision) == NVAPI_OK)
        Print_uint32(L"NvAPI_GPU_GetVbiosOEMRevision", biosOemRevision);

    NvAPI_ShortString biosVersionString = {};
    if(NvAPI_GPU_GetVbiosVersionString(gpu, biosVersionString) == NVAPI_OK)
        Print_string(L"NvAPI_GPU_GetVbiosVersionString", StrToWstr(biosVersionString, CP_ACP).c_str());

    NvU32 physicalFrameBufferSize = 0;
    if(NvAPI_GPU_GetPhysicalFrameBufferSize(gpu, &physicalFrameBufferSize) == NVAPI_OK)
        Print_sizeKilobytes(L"NvAPI_GPU_GetPhysicalFrameBufferSize", physicalFrameBufferSize);

    NvU32 virtualFrameBufferSize = 0;
    if(NvAPI_GPU_GetVirtualFrameBufferSize(gpu, &virtualFrameBufferSize) == NVAPI_OK)
        Print_sizeKilobytes(L"NvAPI_GPU_GetVirtualFrameBufferSize", virtualFrameBufferSize);

    NV_GPU_ARCH_INFO archInfo = {NV_GPU_ARCH_INFO_VER};
    if(NvAPI_GPU_GetArchInfo(gpu, &archInfo) == NVAPI_OK)
    {
        PrintEnum(L"NvAPI_GPU_GetArchInfo - NV_GPU_ARCH_INFO::architecture_id", archInfo.architecture_id, Enum_NV_GPU_ARCHITECTURE_ID);
        PrintNvImplementationId(L"NvAPI_GPU_GetArchInfo - NV_GPU_ARCH_INFO::implementation_id",
            (uint32_t)archInfo.architecture_id,
            (uint32_t)archInfo.implementation_id);
        PrintEnum(L"NvAPI_GPU_GetArchInfo - NV_GPU_ARCH_INFO::revision_id", archInfo.revision_id, Enum_NV_GPU_CHIP_REVISION);
    }

    NV_GPU_VR_READY vrReady = {NV_GPU_VR_READY_VER};
    if(NvAPI_GPU_GetVRReadyData(gpu, &vrReady) == NVAPI_OK)
        Print_BOOL(L"NvAPI_GPU_GetVRReadyData - NV_GPU_VR_READY::isVRReady", vrReady.isVRReady != 0);

    NV_GPU_QUERY_ILLUMINATION_SUPPORT_PARM queryIlluminationSupportParm = {
        .version = NV_GPU_QUERY_ILLUMINATION_SUPPORT_PARM_VER,
        .hPhysicalGpu = gpu};
    for(const EnumItem* ei = Enum_NV_GPU_ILLUMINATION_ATTRIB; ei->m_Name != nullptr; ++ei)
    {
        queryIlluminationSupportParm.Attribute = (NV_GPU_ILLUMINATION_ATTRIB)ei->m_Value;
        if(NvAPI_GPU_QueryIlluminationSupport(&queryIlluminationSupportParm) == NVAPI_OK)
        {
            Print_BOOL(std::format(L"NvAPI_GPU_QueryIlluminationSupport({})", ei->m_Name).c_str(),
                queryIlluminationSupportParm.bSupported != 0);
        }
    }

    for(const EnumItem* ei = Enum_NV_GPU_WORKSTATION_FEATURE_TYPE; ei->m_Name != nullptr; ++ei)
    {
        NvAPI_Status status = NvAPI_GPU_QueryWorkstationFeatureSupport(gpu, (NV_GPU_WORKSTATION_FEATURE_TYPE)ei->m_Value);
        PrintEnum(std::format(L"NvAPI_GPU_QueryWorkstationFeatureSupport({})", ei->m_Name).c_str(),
            status, Enum_NvAPI_Status, true);
    }

    {
        NV_GPU_MEMORY_INFO_EX memInfo = {NV_GPU_MEMORY_INFO_EX_VER};
        if(NvAPI_GPU_GetMemoryInfoEx(gpu, &memInfo) == NVAPI_OK)
        {
            Print_size(L"NvAPI_GPU_GetMemoryInfoEx - NV_GPU_MEMORY_INFO_EX::dedicatedVideoMemory", memInfo.dedicatedVideoMemory);
            Print_size(L"NvAPI_GPU_GetMemoryInfoEx - NV_GPU_MEMORY_INFO_EX::availableDedicatedVideoMemory", memInfo.availableDedicatedVideoMemory);
            Print_size(L"NvAPI_GPU_GetMemoryInfoEx - NV_GPU_MEMORY_INFO_EX::systemVideoMemory", memInfo.systemVideoMemory);
            Print_size(L"NvAPI_GPU_GetMemoryInfoEx - NV_GPU_MEMORY_INFO_EX::sharedSystemMemory", memInfo.sharedSystemMemory);
            Print_size(L"NvAPI_GPU_GetMemoryInfoEx - NV_GPU_MEMORY_INFO_EX::curAvailableDedicatedVideoMemory", memInfo.curAvailableDedicatedVideoMemory);
            Print_size(L"NvAPI_GPU_GetMemoryInfoEx - NV_GPU_MEMORY_INFO_EX::dedicatedVideoMemoryEvictionsSize", memInfo.dedicatedVideoMemoryEvictionsSize);
            Print_uint64(L"NvAPI_GPU_GetMemoryInfoEx - NV_GPU_MEMORY_INFO_EX::dedicatedVideoMemoryEvictionCount", memInfo.dedicatedVideoMemoryEvictionCount);
            Print_size(L"NvAPI_GPU_GetMemoryInfoEx - NV_GPU_MEMORY_INFO_EX::dedicatedVideoMemoryPromotionsSize", memInfo.dedicatedVideoMemoryPromotionsSize);
            Print_uint64(L"NvAPI_GPU_GetMemoryInfoEx - NV_GPU_MEMORY_INFO_EX::dedicatedVideoMemoryPromotionCount", memInfo.dedicatedVideoMemoryPromotionCount);
        }
    }

    NvU32 shaderSubPipeCount = 0;
    if(NvAPI_GPU_GetShaderSubPipeCount(gpu, &shaderSubPipeCount) == NVAPI_OK)
        Print_uint32(L"NvAPI_GPU_GetShaderSubPipeCount", shaderSubPipeCount);

    NvU32 gpuCoreCount = 0;
    if(NvAPI_GPU_GetGpuCoreCount(gpu, &gpuCoreCount) == NVAPI_OK)
        Print_uint32(L"NvAPI_GPU_GetGpuCoreCount", gpuCoreCount);

    NV_GPU_ECC_STATUS_INFO GPUECCStatusInfo = {NV_GPU_ECC_STATUS_INFO_VER};
    if(NvAPI_GPU_GetECCStatusInfo(gpu, &GPUECCStatusInfo) == NVAPI_OK)
    {
        Print_BOOL(L"NvAPI_GPU_GetECCStatusInfo - NV_GPU_ECC_STATUS_INFO::isSupported", GPUECCStatusInfo.isSupported != 0);
        PrintEnum(L"NvAPI_GPU_GetECCStatusInfo - NV_GPU_ECC_STATUS_INFO::configurationOptions", GPUECCStatusInfo.configurationOptions,
            Enum_NV_ECC_CONFIGURATION);
        Print_BOOL(L"NvAPI_GPU_GetECCStatusInfo - NV_GPU_ECC_STATUS_INFO::isEnabled", GPUECCStatusInfo.isEnabled != 0);
    }

    {
        NvU32 busWidth = 0;
        if(NvAPI_GPU_GetRamBusWidth(gpu, &busWidth) == NVAPI_OK)
            Print_uint32(L"NvAPI_GPU_GetRamBusWidth", busWidth);
    }

    {
        NV_GPU_INFO gpuInfo = {NV_GPU_INFO_VER};
        if(NvAPI_GPU_GetGPUInfo(gpu, &gpuInfo) == NVAPI_OK)
        {
            Print_BOOL(L"NvAPI_GPU_GetGPUInfo - NV_GPU_INFO::bIsExternalGpu", gpuInfo.bIsExternalGpu);
            Print_uint32(L"NvAPI_GPU_GetGPUInfo - NV_GPU_INFO::rayTracingCores", gpuInfo.rayTracingCores);
            Print_uint32(L"NvAPI_GPU_GetGPUInfo - NV_GPU_INFO::tensorCores", gpuInfo.tensorCores);
        }
    }

    {
        NV_GPU_GSP_INFO gspInfo = {NV_GPU_GSP_INFO_VER};
        if(NvAPI_GPU_GetGspFeatures(gpu, &gspInfo) == NVAPI_OK)
        {
            PrintHexBytes(L"NvAPI_GPU_GetGspFeatures - NV_GPU_GSP_INFO::firmwareVersion",
                gspInfo.firmwareVersion, NVAPI_GPU_MAX_BUILD_VERSION_LENGTH);
        }
    }
}

#endif // #if USE_NVAPI
