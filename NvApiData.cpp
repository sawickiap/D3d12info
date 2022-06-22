#include "pch.hpp"
#include "NvApiData.hpp"
#include "Printing.hpp"
#include "Enums.hpp"
#include "Utils.hpp"

#if USE_NVAPI

#include <nvapi.h>
#pragma comment(lib, "nvapi64.lib")

// Don't forget to update when linking with a new version!
static const wchar_t* NVAPI_COMPILED_VERSION = L"R515-developer";

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

ENUM_BEGIN(NV_ARCHITECTURE_GPU_ID)
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
ENUM_END(NV_ARCHITECTURE_GPU_ID)

ENUM_BEGIN(NV_ARCH_IMPLEMENTATION_GPU_ID)
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_T20  ) 
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_T30  ) 
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_T35  ) 
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_T40  ) 
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_T124 )  
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_NV40 )  
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_NV41 )  
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_NV42 )  
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_NV43 )  
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_NV44 )  
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_NV44A)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_NV46 )  
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_NV47 )  
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_NV49 )  
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_NV4B )  
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_NV4C )  
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_NV4E )  
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_NV50 )  
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_NV63 )  
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_NV67 )  
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_G84  ) 
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_G86  ) 
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_G92  ) 
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_G94  ) 
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_G96  ) 
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_G98  ) 
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GT200)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GT212)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GT214)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GT215)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GT216)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GT218)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_MCP77)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GT21C)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_MCP79)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GT21A)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_MCP89)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GF100)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GF104)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GF106)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GF108)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GF110)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GF116)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GF117)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GF118)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GF119)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GK104)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GK106)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GK107)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GK20A)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GK110)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GK208)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GM204)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GM206)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GP100)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GP000)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GP102)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GP104)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GP106)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GP107)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GP108)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GV100)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GV10B)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_TU100)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_TU102)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_TU104)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_TU106)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_TU116)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_TU117)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_TU000)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GA100)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GA102)   
    ENUM_ITEM(NV_GPU_ARCH_IMPLEMENTATION_GA104)  
ENUM_END(NV_ARCH_IMPLEMENTATION_GPU_ID)

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
    ENUM_ITEM(NV_GPU_WORKSTATION_FEATURE_TYPE_QUADRO_VR_READY)
    ENUM_ITEM(NV_GPU_WORKSTATION_FEATURE_TYPE_PROVIZ)
ENUM_END(NV_GPU_WORKSTATION_FEATURE_TYPE)

static NvPhysicalGpuHandle g_PhysicalGpus[NVAPI_MAX_PHYSICAL_GPUS];
static LUID g_PhysicalGpuLuids[NVAPI_MAX_PHYSICAL_GPUS];
static NvU32 g_PhysicalGpuCount = 0;

static wstring NvShortStringToStr(NvAPI_ShortString str)
{
    wchar_t w[NVAPI_SHORT_STRING_MAX];
    swprintf_s(w, L"%hs", str);
    return wstring{w};
}

static void LoadPhysicalGpus()
{
    if(NvAPI_EnumPhysicalGPUs(g_PhysicalGpus, &g_PhysicalGpuCount) != NVAPI_OK)
        g_PhysicalGpuCount = 0;
    for(NvU32 i = 0; i < g_PhysicalGpuCount; ++i)
    {
        NvAPI_Status status = NvAPI_GPU_GetAdapterIdFromPhysicalGpu(g_PhysicalGpus[i], &g_PhysicalGpuLuids[i]);
        assert(status == NVAPI_OK);
    }
}

static bool FindPhysicalGpu(const LUID& adapterLuid, NvPhysicalGpuHandle& outGpu)
{
    NvU32 foundIndex = UINT32_MAX;
    for(NvU32 i = 0; i < g_PhysicalGpuCount; ++i)
    {
        if(memcmp(&g_PhysicalGpuLuids[i], &adapterLuid, sizeof(LUID)) == 0)
        {
            if(foundIndex != UINT32_MAX)
                // Found multiple GPUs with same LUID.
                return false;
            else
                foundIndex = i;
        }
    }
    if(foundIndex != UINT32_MAX)
    {
        outGpu = g_PhysicalGpus[foundIndex];
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC

NvAPI_Inititalize_RAII::NvAPI_Inititalize_RAII()
{
    m_Initialized = NvAPI_Initialize() == NVAPI_OK;
    if(m_Initialized)
        LoadPhysicalGpus();
}

NvAPI_Inititalize_RAII::~NvAPI_Inititalize_RAII()
{
    if(m_Initialized)
        NvAPI_Unload();
}

void NvAPI_Inititalize_RAII::PrintGeneralParams()
{
    Print_string(L"NvAPI compiled version", NVAPI_COMPILED_VERSION);
    NvAPI_ShortString nvShortString;
    if(NvAPI_GetInterfaceVersionString(nvShortString) == NVAPI_OK)
        Print_string(L"NvAPI_GetInterfaceVersionString", NvShortStringToStr(nvShortString).c_str());
}

void NvAPI_Inititalize_RAII::PrintData()
{
    NvU32 pDriverVersion = UINT32_MAX;
    NvAPI_ShortString szBuildBranchString = {};
    if(NvAPI_SYS_GetDriverAndBranchVersion(&pDriverVersion, szBuildBranchString) == NVAPI_OK)
    {
        PrintStructBegin(L"NvAPI_SYS_GetDriverAndBranchVersion");
        Print_uint32(L"pDriverVersion", pDriverVersion);
        Print_string(L"szBuildBranchString", NvShortStringToStr(szBuildBranchString).c_str());
        PrintStructEnd();
    }

    {
        NV_DISPLAY_DRIVER_INFO info = {NV_DISPLAY_DRIVER_INFO_VER};
        if(NvAPI_SYS_GetDisplayDriverInfo(&info) == NVAPI_OK)
        {
            PrintStructBegin(L"NvAPI_SYS_GetDisplayDriverInfo - NV_DISPLAY_DRIVER_INFO");
            Print_uint32(L"driverVersion", info.driverVersion);
            Print_string(L"szBuildBranch", StrToWstr(info.szBuildBranch, CP_ACP).c_str());
            Print_BOOL(L"bIsDCHDriver", info.bIsDCHDriver != 0);
            Print_BOOL(L"bIsNVIDIAStudioPackage", info.bIsNVIDIAStudioPackage != 0);
            Print_BOOL(L"bIsNVIDIAGameReadyPackage", info.bIsNVIDIAGameReadyPackage != 0);
            Print_BOOL(L"bIsNVIDIARTXProductionBranchPackage", info.bIsNVIDIARTXProductionBranchPackage != 0);
            Print_BOOL(L"bIsNVIDIARTXNewFeatureBranchPackage", info.bIsNVIDIARTXNewFeatureBranchPackage != 0);
            PrintStructEnd();
        }
    }
}

void NvAPI_Inititalize_RAII::PrintD3d12DeviceData(ID3D12Device* device)
{
    wstring s;

    PrintStructBegin(L"NvAPI_D3D12_IsNvShaderExtnOpCodeSupported");
    for(const EnumItem* ei = Enum_NV_EXTN_OP; ei->m_Name != nullptr; ++ei)
    {
        bool supported = false;
        if(NvAPI_D3D12_IsNvShaderExtnOpCodeSupported(device, ei->m_Value, &supported) == NVAPI_OK)
            Print_BOOL(ei->m_Name, supported);
    }
    PrintStructEnd();
}

void NvAPI_Inititalize_RAII::PrintPhysicalGpuData(const LUID& adapterLuid)
{
    NvPhysicalGpuHandle gpu = {};
    if(!FindPhysicalGpu(adapterLuid, gpu))
        return;

    PrintStructBegin(L"NvPhysicalGpuHandle");

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
        Print_hex32(L"NvAPI_GPU_GetPCIIdentifiers - pSubSystemId", SubSystemId);
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
        Print_uint32(L"NvAPI_GPU_GetPhysicalFrameBufferSize", physicalFrameBufferSize, L"KB");

    NvU32 virtualFrameBufferSize = 0;
    if(NvAPI_GPU_GetVirtualFrameBufferSize(gpu, &virtualFrameBufferSize) == NVAPI_OK)
        Print_uint32(L"NvAPI_GPU_GetVirtualFrameBufferSize", virtualFrameBufferSize, L"KB");

    NV_GPU_ARCH_INFO archInfo = {NV_GPU_ARCH_INFO_VER};
    if(NvAPI_GPU_GetArchInfo(gpu, &archInfo) == NVAPI_OK)
    {
        PrintEnum(L"NvAPI_GPU_GetArchInfo - NV_GPU_ARCH_INFO::architecture_id", archInfo.architecture_id, Enum_NV_ARCHITECTURE_GPU_ID);
        PrintEnum(L"NvAPI_GPU_GetArchInfo - NV_GPU_ARCH_INFO::implementation_id", archInfo.implementation_id, Enum_NV_ARCH_IMPLEMENTATION_GPU_ID);
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
            status, Enum_NvAPI_Status);
    }

    {
        NV_DISPLAY_DRIVER_MEMORY_INFO memInfo = {NV_DISPLAY_DRIVER_MEMORY_INFO_VER};
        if(NvAPI_GPU_GetMemoryInfo(gpu, &memInfo) == NVAPI_OK)
        {
            Print_uint32(L"NvAPI_GPU_GetMemoryInfo - NV_DISPLAY_DRIVER_MEMORY_INFO::dedicatedVideoMemory", memInfo.dedicatedVideoMemory, L"KB");
            Print_uint32(L"NvAPI_GPU_GetMemoryInfo - NV_DISPLAY_DRIVER_MEMORY_INFO::systemVideoMemory", memInfo.systemVideoMemory, L"KB");
            Print_uint32(L"NvAPI_GPU_GetMemoryInfo - NV_DISPLAY_DRIVER_MEMORY_INFO::sharedSystemMemory", memInfo.sharedSystemMemory, L"KB");
        }
    }

    PrintStructEnd();
}

#endif // #if USE_NVAPI
