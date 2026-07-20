/*
This file is part of D3d12info project:
https://github.com/sawickiap/D3d12info

Copyright (c) 2018-2026 Adam Sawicki, https://asawicki.info
License: MIT

For more information, see files README.md, LICENSE.txt.
*/
#include "CudaData.hpp"

#include "Enums.hpp"
#include "ReportFormatter/ReportFormatter.hpp"
#include "Utils.hpp"

// Macro set by Cmake.
#if USE_CUDA

#include <cuda.h>

////////////////////////////////////////////////////////////////////////////////
// PRIVATE

static std::wstring CudaVersionToString(int version)
{
    const int major = version / 1000;
    const int minor = version % 1000 / 10;
    const int patch = version % 10;
    if(patch != 0)
    {
        return std::format(L"{}.{}.{}", major, minor, patch);
    }
    else
    {
        return std::format(L"{}.{}", major, minor);
    }
}

template <size_t Count>
static void PrintCudaIntArray(std::wstring_view name, const int (&values)[Count])
{
    ReportFormatter& formatter = ReportFormatter::GetInstance();
    for(size_t i = 0; i < Count; ++i)
        formatter.AddFieldInt32(std::format(L"{}[{}]", name, i), values[i]);
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC

void Cuda_Initialize_RAII::PrintStaticParams()
{
    ReportFormatter& formatter = ReportFormatter::GetInstance();
    formatter.AddFieldString(L"CUDA compiled version", CudaVersionToString(CUDA_VERSION));
}

Cuda_Initialize_RAII::Cuda_Initialize_RAII()
{
    m_Initialized = cuInit(0) == CUDA_SUCCESS;
}

void Cuda_Initialize_RAII::PrintData() const
{
    ReportScopeObject scope(L"CUDA Runtime");
    ReportFormatter& formatter = ReportFormatter::GetInstance();

    int driverVersion;
    if(cuDriverGetVersion(&driverVersion) == CUDA_SUCCESS)
    {
        formatter.AddFieldString(L"cuDriverGetVersion", CudaVersionToString(driverVersion));
    }

    int deviceCount;
    if(cuDeviceGetCount(&deviceCount) == CUDA_SUCCESS)
    {
        formatter.AddFieldInt32(L"cuDeviceGetCount", deviceCount);
    }
}

void Cuda_Initialize_RAII::PrintAdapterData(const DXGI_ADAPTER_DESC& adapterDesc) const
{
    ReportFormatter& formatter = ReportFormatter::GetInstance();

    int deviceCount;
    if(cuDeviceGetCount(&deviceCount) != CUDA_SUCCESS)
    {
        return;
    }

    for(int i = 0; i < deviceCount; ++i)
    {
        CUdevice device;
        LUID luid;
        unsigned mask;
        if(cuDeviceGet(&device, i) == CUDA_SUCCESS && cuDeviceGetLuid((char*)&luid, &mask, device) == CUDA_SUCCESS &&
            memcmp(&luid, &adapterDesc.AdapterLuid, sizeof(luid)) == 0)
        {
            ReportScopeObject scope(L"CUDA");
            char name[64];
            if(cuDeviceGetName(name, sizeof(name), device) == CUDA_SUCCESS)
            {
                formatter.AddFieldString(L"name", StrToWstr(name, CP_UTF8));
            }

            CUuuid uuid;
            if(cuDeviceGetUuid(&uuid, device) == CUDA_SUCCESS)
            {
                formatter.AddFieldHexBytes(L"uuid", (void*)&uuid, sizeof(uuid));
            }

            size_t memSize;
            if(cuDeviceTotalMem(&memSize, device) == CUDA_SUCCESS)
            {
                formatter.AddFieldSize(L"memory", memSize);
            }

            int attr;

#define CUATTR(name) if (cuDeviceGetAttribute(&attr, CU_DEVICE_ATTRIBUTE_##name, device) == CUDA_SUCCESS) formatter.AddFieldInt32(L""#name, attr);
#define CUATTR_SIZE(name) if (cuDeviceGetAttribute(&attr, CU_DEVICE_ATTRIBUTE_##name, device) == CUDA_SUCCESS) formatter.AddFieldSize(L""#name, attr);
#define CUATTR_HEX32(name) if (cuDeviceGetAttribute(&attr, CU_DEVICE_ATTRIBUTE_##name, device) == CUDA_SUCCESS) formatter.AddFieldHex32(L""#name, attr);

            // clang-format off
            CUATTR(MAX_THREADS_PER_BLOCK)                       /**< Maximum number of threads per block */
            CUATTR(MAX_BLOCK_DIM_X)                             /**< Maximum block dimension X */
            CUATTR(MAX_BLOCK_DIM_Y)                             /**< Maximum block dimension Y */
            CUATTR(MAX_BLOCK_DIM_Z)                             /**< Maximum block dimension Z */
            CUATTR(MAX_GRID_DIM_X)                              /**< Maximum grid dimension X */
            CUATTR(MAX_GRID_DIM_Y)                              /**< Maximum grid dimension Y */
            CUATTR(MAX_GRID_DIM_Z)                              /**< Maximum grid dimension Z */
            CUATTR_SIZE(MAX_SHARED_MEMORY_PER_BLOCK)            /**< Maximum shared memory available per block in bytes */
            //CUATTR(SHARED_MEMORY_PER_BLOCK)                     /**< Deprecated, use CU_DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_BLOCK */
            CUATTR_SIZE(TOTAL_CONSTANT_MEMORY)                  /**< Memory available on device for __constant__ variables in a CUDA C kernel in bytes */
            CUATTR(WARP_SIZE)                                   /**< Warp size in threads */
            CUATTR_SIZE(MAX_PITCH)                              /**< Maximum pitch in bytes allowed by memory copies */
            CUATTR(MAX_REGISTERS_PER_BLOCK)                     /**< Maximum number of 32-bit registers available per block */
            //CUATTR(REGISTERS_PER_BLOCK)                         /**< Deprecated, use CU_DEVICE_ATTRIBUTE_MAX_REGISTERS_PER_BLOCK */
            CUATTR(CLOCK_RATE)                                  /**< Typical clock frequency in kilohertz */
            CUATTR_SIZE(TEXTURE_ALIGNMENT)                      /**< Alignment requirement for textures */
            //CUATTR(GPU_OVERLAP)                                 /**< Device can possibly copy memory and execute a kernel concurrently. Deprecated. Use instead CU_DEVICE_ATTRIBUTE_ASYNC_ENGINE_COUNT. */
            CUATTR(MULTIPROCESSOR_COUNT)                        /**< Number of multiprocessors on device */
            CUATTR(KERNEL_EXEC_TIMEOUT)                         /**< Specifies whether there is a run time limit on kernels */
            CUATTR(INTEGRATED)                                  /**< Device is integrated with host memory */
            CUATTR(CAN_MAP_HOST_MEMORY)                         /**< Device can map host memory into CUDA address space */
            CUATTR(COMPUTE_MODE)                                /**< Compute mode (See ::CUcomputemode for details) */
            CUATTR(MAXIMUM_TEXTURE1D_WIDTH)                     /**< Maximum 1D texture width */
            CUATTR(MAXIMUM_TEXTURE2D_WIDTH)                     /**< Maximum 2D texture width */
            CUATTR(MAXIMUM_TEXTURE2D_HEIGHT)                    /**< Maximum 2D texture height */
            CUATTR(MAXIMUM_TEXTURE3D_WIDTH)                     /**< Maximum 3D texture width */
            CUATTR(MAXIMUM_TEXTURE3D_HEIGHT)                    /**< Maximum 3D texture height */
            CUATTR(MAXIMUM_TEXTURE3D_DEPTH)                     /**< Maximum 3D texture depth */
            CUATTR(MAXIMUM_TEXTURE2D_LAYERED_WIDTH)             /**< Maximum 2D layered texture width */
            CUATTR(MAXIMUM_TEXTURE2D_LAYERED_HEIGHT)            /**< Maximum 2D layered texture height */
            CUATTR(MAXIMUM_TEXTURE2D_LAYERED_LAYERS)            /**< Maximum layers in a 2D layered texture */
            //CUATTR(MAXIMUM_TEXTURE2D_ARRAY_WIDTH)               /**< Deprecated, use CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LAYERED_WIDTH */
            //CUATTR(MAXIMUM_TEXTURE2D_ARRAY_HEIGHT)              /**< Deprecated, use CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LAYERED_HEIGHT */
            //CUATTR(MAXIMUM_TEXTURE2D_ARRAY_NUMSLICES)           /**< Deprecated, use CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LAYERED_LAYERS */
            CUATTR_SIZE(SURFACE_ALIGNMENT)                      /**< Alignment requirement for surfaces */
            CUATTR(CONCURRENT_KERNELS)                          /**< Device can possibly execute multiple kernels concurrently */
            CUATTR(ECC_ENABLED)                                 /**< Device has ECC support enabled */
            CUATTR_HEX32(PCI_BUS_ID)                            /**< PCI bus ID of the device */
            CUATTR_HEX32(PCI_DEVICE_ID)                         /**< PCI device ID of the device */
            CUATTR(TCC_DRIVER)                                  /**< Device is using TCC driver model */
            CUATTR(MEMORY_CLOCK_RATE)                           /**< Peak memory clock frequency in kilohertz */
            CUATTR(GLOBAL_MEMORY_BUS_WIDTH)                     /**< Global memory bus width in bits */
            CUATTR_SIZE(L2_CACHE_SIZE)                          /**< Size of L2 cache in bytes */
            CUATTR(MAX_THREADS_PER_MULTIPROCESSOR)              /**< Maximum resident threads per multiprocessor */
            CUATTR(ASYNC_ENGINE_COUNT)                          /**< Number of asynchronous engines */
            CUATTR(UNIFIED_ADDRESSING)                          /**< Device shares a unified address space with the host */
            CUATTR(MAXIMUM_TEXTURE1D_LAYERED_WIDTH)             /**< Maximum 1D layered texture width */
            CUATTR(MAXIMUM_TEXTURE1D_LAYERED_LAYERS)            /**< Maximum layers in a 1D layered texture */
            //CUATTR(CAN_TEX2D_GATHER)                            /**< Deprecated, do not use. */
            CUATTR(MAXIMUM_TEXTURE2D_GATHER_WIDTH)              /**< Maximum 2D texture width if CUDA_ARRAY3D_TEXTURE_GATHER is set */
            CUATTR(MAXIMUM_TEXTURE2D_GATHER_HEIGHT)             /**< Maximum 2D texture height if CUDA_ARRAY3D_TEXTURE_GATHER is set */
            CUATTR(MAXIMUM_TEXTURE3D_WIDTH_ALTERNATE)           /**< Alternate maximum 3D texture width */
            CUATTR(MAXIMUM_TEXTURE3D_HEIGHT_ALTERNATE)          /**< Alternate maximum 3D texture height */
            CUATTR(MAXIMUM_TEXTURE3D_DEPTH_ALTERNATE)           /**< Alternate maximum 3D texture depth */
            CUATTR_HEX32(PCI_DOMAIN_ID)                         /**< PCI domain ID of the device */
            CUATTR_SIZE(TEXTURE_PITCH_ALIGNMENT)                /**< Pitch alignment requirement for textures */
            CUATTR(MAXIMUM_TEXTURECUBEMAP_WIDTH)                /**< Maximum cubemap texture width/height */
            CUATTR(MAXIMUM_TEXTURECUBEMAP_LAYERED_WIDTH)        /**< Maximum cubemap layered texture width/height */
            CUATTR(MAXIMUM_TEXTURECUBEMAP_LAYERED_LAYERS)       /**< Maximum layers in a cubemap layered texture */
            CUATTR(MAXIMUM_SURFACE1D_WIDTH)                     /**< Maximum 1D surface width */
            CUATTR(MAXIMUM_SURFACE2D_WIDTH)                     /**< Maximum 2D surface width */
            CUATTR(MAXIMUM_SURFACE2D_HEIGHT)                    /**< Maximum 2D surface height */
            CUATTR(MAXIMUM_SURFACE3D_WIDTH)                     /**< Maximum 3D surface width */
            CUATTR(MAXIMUM_SURFACE3D_HEIGHT)                    /**< Maximum 3D surface height */
            CUATTR(MAXIMUM_SURFACE3D_DEPTH)                     /**< Maximum 3D surface depth */
            CUATTR(MAXIMUM_SURFACE1D_LAYERED_WIDTH)             /**< Maximum 1D layered surface width */
            CUATTR(MAXIMUM_SURFACE1D_LAYERED_LAYERS)            /**< Maximum layers in a 1D layered surface */
            CUATTR(MAXIMUM_SURFACE2D_LAYERED_WIDTH)             /**< Maximum 2D layered surface width */
            CUATTR(MAXIMUM_SURFACE2D_LAYERED_HEIGHT)            /**< Maximum 2D layered surface height */
            CUATTR(MAXIMUM_SURFACE2D_LAYERED_LAYERS)            /**< Maximum layers in a 2D layered surface */
            CUATTR(MAXIMUM_SURFACECUBEMAP_WIDTH)                /**< Maximum cubemap surface width */
            CUATTR(MAXIMUM_SURFACECUBEMAP_LAYERED_WIDTH)        /**< Maximum cubemap layered surface width */
            CUATTR(MAXIMUM_SURFACECUBEMAP_LAYERED_LAYERS)       /**< Maximum layers in a cubemap layered surface */
            //CUATTR(MAXIMUM_TEXTURE1D_LINEAR_WIDTH)              /**< Deprecated, do not use. Use cudaDeviceGetTexture1DLinearMaxWidth() or cuDeviceGetTexture1DLinearMaxWidth() instead. */
            CUATTR(MAXIMUM_TEXTURE2D_LINEAR_WIDTH)              /**< Maximum 2D linear texture width */
            CUATTR(MAXIMUM_TEXTURE2D_LINEAR_HEIGHT)             /**< Maximum 2D linear texture height */
            CUATTR_SIZE(MAXIMUM_TEXTURE2D_LINEAR_PITCH)         /**< Maximum 2D linear texture pitch in bytes */
            CUATTR(MAXIMUM_TEXTURE2D_MIPMAPPED_WIDTH)           /**< Maximum mipmapped 2D texture width */
            CUATTR(MAXIMUM_TEXTURE2D_MIPMAPPED_HEIGHT)          /**< Maximum mipmapped 2D texture height */
            CUATTR(COMPUTE_CAPABILITY_MAJOR)                    /**< Major compute capability version number */
            CUATTR(COMPUTE_CAPABILITY_MINOR)                    /**< Minor compute capability version number */
            CUATTR(MAXIMUM_TEXTURE1D_MIPMAPPED_WIDTH)           /**< Maximum mipmapped 1D texture width */
            CUATTR(STREAM_PRIORITIES_SUPPORTED)                 /**< Device supports stream priorities */
            CUATTR(GLOBAL_L1_CACHE_SUPPORTED)                   /**< Device supports caching globals in L1 */
            CUATTR(LOCAL_L1_CACHE_SUPPORTED)                    /**< Device supports caching locals in L1 */
            CUATTR_SIZE(MAX_SHARED_MEMORY_PER_MULTIPROCESSOR)   /**< Maximum shared memory available per multiprocessor in bytes */
            CUATTR(MAX_REGISTERS_PER_MULTIPROCESSOR)            /**< Maximum number of 32-bit registers available per multiprocessor */
            CUATTR(MANAGED_MEMORY)                              /**< Device can allocate managed memory on this system */
            CUATTR(MULTI_GPU_BOARD)                             /**< Device is on a multi-GPU board */
            CUATTR_HEX32(MULTI_GPU_BOARD_GROUP_ID)              /**< Unique id for a group of devices on the same multi-GPU board */
            CUATTR(HOST_NATIVE_ATOMIC_SUPPORTED)                /**< Link between the device and the host supports all native atomic operations */
            CUATTR(SINGLE_TO_DOUBLE_PRECISION_PERF_RATIO)       /**< Ratio of single precision performance (in floating-point operations per second) to double precision performance */
            CUATTR(PAGEABLE_MEMORY_ACCESS)                      /**< Device supports coherently accessing pageable memory without calling cudaHostRegister on it */
            CUATTR(CONCURRENT_MANAGED_ACCESS)                   /**< Device can coherently access managed memory concurrently with the CPU */
            CUATTR(COMPUTE_PREEMPTION_SUPPORTED)                /**< Device supports compute preemption. */
            CUATTR(CAN_USE_HOST_POINTER_FOR_REGISTERED_MEM)     /**< Device can access host registered memory at the same virtual address as the CPU */
            //CUATTR(CAN_USE_STREAM_MEM_OPS_V1)                   /**< Deprecated, along with v1 MemOps API, ::cuStreamBatchMemOp and related APIs are supported. */
            //CUATTR(CAN_USE_64_BIT_STREAM_MEM_OPS_V1)            /**< Deprecated, along with v1 MemOps API, 64-bit operations are supported in ::cuStreamBatchMemOp and related APIs. */
            //CUATTR(CAN_USE_STREAM_WAIT_VALUE_NOR_V1)            /**< Deprecated, along with v1 MemOps API, ::CU_STREAM_WAIT_VALUE_NOR is supported. */
            CUATTR(COOPERATIVE_LAUNCH)                          /**< Device supports launching cooperative kernels via ::cuLaunchCooperativeKernel */
            //CUATTR(COOPERATIVE_MULTI_DEVICE_LAUNCH)             /**< Deprecated, ::cuLaunchCooperativeKernelMultiDevice is deprecated. */
            CUATTR_SIZE(MAX_SHARED_MEMORY_PER_BLOCK_OPTIN)      /**< Maximum optin shared memory per block */
            CUATTR(CAN_FLUSH_REMOTE_WRITES)                     /**< The ::CU_STREAM_WAIT_VALUE_FLUSH flag and the ::CU_STREAM_MEM_OP_FLUSH_REMOTE_WRITES MemOp are supported on the device. See \ref CUDA_MEMOP for additional details. */
            CUATTR(HOST_REGISTER_SUPPORTED)                     /**< Device supports host memory registration via ::cudaHostRegister. */
            CUATTR(PAGEABLE_MEMORY_ACCESS_USES_HOST_PAGE_TABLES)/**< Device accesses pageable memory via the host's page tables. */
            CUATTR(DIRECT_MANAGED_MEM_ACCESS_FROM_HOST)         /**< The host can directly access managed memory on the device without migration. */
            //CUATTR(VIRTUAL_ADDRESS_MANAGEMENT_SUPPORTED)        /**< Deprecated, Use CU_DEVICE_ATTRIBUTE_VIRTUAL_MEMORY_MANAGEMENT_SUPPORTED*/
            CUATTR(VIRTUAL_MEMORY_MANAGEMENT_SUPPORTED)         /**< Device supports virtual memory management APIs like ::cuMemAddressReserve, ::cuMemCreate, ::cuMemMap and related APIs */
            CUATTR(HANDLE_TYPE_POSIX_FILE_DESCRIPTOR_SUPPORTED) /**< Device supports exporting memory to a posix file descriptor with ::cuMemExportToShareableHandle, if requested via ::cuMemCreate */
            CUATTR(HANDLE_TYPE_WIN32_HANDLE_SUPPORTED)          /**< Device supports exporting memory to a Win32 NT handle with ::cuMemExportToShareableHandle, if requested via ::cuMemCreate */
            CUATTR(HANDLE_TYPE_WIN32_KMT_HANDLE_SUPPORTED)      /**< Device supports exporting memory to a Win32 KMT handle with ::cuMemExportToShareableHandle, if requested via ::cuMemCreate */
            CUATTR(MAX_BLOCKS_PER_MULTIPROCESSOR)               /**< Maximum number of blocks per multiprocessor */
            CUATTR(GENERIC_COMPRESSION_SUPPORTED)               /**< Device supports compression of memory */
            CUATTR_SIZE(MAX_PERSISTING_L2_CACHE_SIZE)           /**< Maximum L2 persisting lines capacity setting in bytes. */
            CUATTR_SIZE(MAX_ACCESS_POLICY_WINDOW_SIZE)          /**< Maximum value of CUaccessPolicyWindow::num_bytes. */
            CUATTR(GPU_DIRECT_RDMA_WITH_CUDA_VMM_SUPPORTED)     /**< Device supports specifying the GPUDirect RDMA flag with ::cuMemCreate */
            CUATTR_SIZE(RESERVED_SHARED_MEMORY_PER_BLOCK)       /**< Shared memory reserved by CUDA driver per block in bytes */
            CUATTR(SPARSE_CUDA_ARRAY_SUPPORTED)                 /**< Device supports sparse CUDA arrays and sparse CUDA mipmapped arrays */
            CUATTR(READ_ONLY_HOST_REGISTER_SUPPORTED)           /**< Device supports using the ::cuMemHostRegister flag ::CU_MEMHOSTERGISTER_READ_ONLY to register memory that must be mapped as read-only to the GPU */
            CUATTR(TIMELINE_SEMAPHORE_INTEROP_SUPPORTED)        /**< External timeline semaphore interop is supported on the device */
            CUATTR(MEMORY_POOLS_SUPPORTED)                      /**< Device supports using the ::cuMemAllocAsync and ::cuMemPool family of APIs */
            CUATTR(GPU_DIRECT_RDMA_SUPPORTED)                   /**< Device supports GPUDirect RDMA APIs, like nvidia_p2p_get_pages (see https://docs.nvidia.com/cuda/gpudirect-rdma for more information) */
            CUATTR(GPU_DIRECT_RDMA_FLUSH_WRITES_OPTIONS)        /**< The returned attribute shall be interpreted as a bitmask, where the individual bits are described by the ::CUflushGPUDirectRDMAWritesOptions enum */
            CUATTR(GPU_DIRECT_RDMA_WRITES_ORDERING)             /**< GPUDirect RDMA writes to the device do not need to be flushed for consumers within the scope indicated by the returned attribute. See ::CUGPUDirectRDMAWritesOrdering for the numerical values returned here. */
            CUATTR(MEMPOOL_SUPPORTED_HANDLE_TYPES)              /**< Handle types supported with mempool based IPC */
            CUATTR(CLUSTER_LAUNCH)                              /**< Indicates device supports cluster launch */
            CUATTR(DEFERRED_MAPPING_CUDA_ARRAY_SUPPORTED)       /**< Device supports deferred mapping CUDA arrays and CUDA mipmapped arrays */
            CUATTR(CAN_USE_64_BIT_STREAM_MEM_OPS)               /**< 64-bit operations are supported in ::cuStreamBatchMemOp and related MemOp APIs. */
            CUATTR(CAN_USE_STREAM_WAIT_VALUE_NOR)               /**< ::CU_STREAM_WAIT_VALUE_NOR is supported by MemOp APIs. */
            CUATTR(DMA_BUF_SUPPORTED)                           /**< Device supports buffer sharing with dma_buf mechanism. */
            CUATTR(IPC_EVENT_SUPPORTED)                         /**< Device supports IPC Events. */
            CUATTR(MEM_SYNC_DOMAIN_COUNT)                       /**< Number of memory domains the device supports. */
            CUATTR(TENSOR_MAP_ACCESS_SUPPORTED)                 /**< Device supports accessing memory using Tensor Map. */
            CUATTR(HANDLE_TYPE_FABRIC_SUPPORTED)                /**< Device supports exporting memory to a fabric handle with cuMemExportToShareableHandle() or requested with cuMemCreate() */
            CUATTR(UNIFIED_FUNCTION_POINTERS)                   /**< Device supports unified function pointers. */
            CUATTR(NUMA_CONFIG)                                 /**< NUMA configuration of a device: value is of type ::CUdeviceNumaConfig enum */
            CUATTR_HEX32(NUMA_ID)                               /**< NUMA node ID of the GPU memory */
            CUATTR(MULTICAST_SUPPORTED)                         /**< Device supports switch multicast and reduction operations. */
            CUATTR(MPS_ENABLED)                                 /**< Indicates if contexts created on this device will be shared via MPS */
            CUATTR_HEX32(HOST_NUMA_ID)                          /**< NUMA ID of the host node closest to the device. Returns -1 when system does not support NUMA. */
            CUATTR(D3D12_CIG_SUPPORTED)                         /**< Device supports CIG with D3D12. */
            CUATTR(MEM_DECOMPRESS_ALGORITHM_MASK)               /**< The returned valued shall be interpreted as a bitmask, where the individual bits are described by the ::CUmemDecompressAlgorithm enum. */
            CUATTR_SIZE(MEM_DECOMPRESS_MAXIMUM_LENGTH)          /**< The returned valued is the maximum length in bytes of a single decompress operation that is allowed. */
            CUATTR(VULKAN_CIG_SUPPORTED)                        /**< Device supports CIG with Vulkan. */
            CUATTR_HEX32(GPU_PCI_DEVICE_ID)                     /**< The combined 16-bit PCI device ID and 16-bit PCI vendor ID. */
            CUATTR_HEX32(GPU_PCI_SUBSYSTEM_ID)                  /**< The combined 16-bit PCI subsystem ID and 16-bit PCI subsystem vendor ID. */
            CUATTR(HOST_NUMA_VIRTUAL_MEMORY_MANAGEMENT_SUPPORTED) /**< Device supports HOST_NUMA location with the virtual memory management APIs like ::cuMemCreate, ::cuMemMap and related APIs */
            CUATTR(HOST_NUMA_MEMORY_POOLS_SUPPORTED)            /**< Device supports HOST_NUMA location with the ::cuMemAllocAsync and ::cuMemPool family of APIs */
            CUATTR(HOST_NUMA_MULTINODE_IPC_SUPPORTED)           /**< Device supports HOST_NUMA location IPC between nodes in a multi-node system. */
            CUATTR(HOST_MEMORY_POOLS_SUPPORTED)                 /**< Device suports HOST location with the ::cuMemAllocAsync and ::cuMemPool family of APIs */
            CUATTR(HOST_VIRTUAL_MEMORY_MANAGEMENT_SUPPORTED)    /**< Device supports HOST location with the virtual memory management APIs like ::cuMemCreate, ::cuMemMap and related APIs */
            CUATTR(HOST_ALLOC_DMA_BUF_SUPPORTED)                /**< Device supports page-locked host memory buffer sharing with dma_buf mechanism. */
            CUATTR(ONLY_PARTIAL_HOST_NATIVE_ATOMIC_SUPPORTED)   /**< Link between the device and the host supports only some native atomic operations */
            CUATTR(ATOMIC_REDUCTION_SUPPORTED)                  /**< Device supports atomic reduction operations in stream batch memory operations */
            // clang-format on
#undef CUATTR
#undef CUATTR_SIZE
#undef CUATTR_HEX32
        }
    }
}

#endif // #if USE_CUDA
