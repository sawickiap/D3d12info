# This file is part of D3d12info project:
# https://github.com/sawickiap/D3d12info
# 
# Copyright (c) 2018-2024 Adam Sawicki, https://asawicki.info
# License: MIT
# 
# For more information, see files README.md, LICENSE.txt.

cmake_minimum_required(VERSION 3.17)

project(D3d12info LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CPP_FILES
    Src/AgsData.cpp
    Src/AmdDeviceInfoData.cpp
    Src/IntelData.cpp
    Src/Main.cpp
    Src/NvApiData.cpp
    Src/Printer.cpp
    Src/Resources.rc
    Src/Utils.cpp
    Src/VulkanData.cpp
    Src/ReportFormatter/TextReportFormatter.cpp
    Src/ReportFormatter/JSONReportFormatter.cpp
    Src/ReportFormatter/ReportFormatter.cpp
)

set(HPP_FILES
    Src/AgsData.hpp
    Src/AmdDeviceInfoData.hpp
    Src/Enums.hpp
    Src/IntelData.hpp
    Src/NvApiData.hpp
    Src/pch.hpp
    Src/Printer.hpp
    Src/Utils.hpp
    Src/VulkanData.hpp
    Src/ReportFormatter/TextReportFormatter.hpp
    Src/ReportFormatter/JSONReportFormatter.hpp
    Src/ReportFormatter/ReportFormatter.hpp
)

set(INTEL_GPUDETECT_CFG_FILE "Src/ThirdParty/gpudetect/IntelGfx.cfg")

option(ENABLE_AGS "Enables usage of AMD GPU Services (AGS) library." ON)
if(ENABLE_AGS)
    if(EXISTS "${PROJECT_SOURCE_DIR}/Src/ThirdParty/AGS_SDK/ags_lib/inc/amd_ags.h")
        message(STATUS "AMD AGS library used.")
    else()
        message(FATAL_ERROR "AMD AGS library not found. This is likely due to missing submodule. Please initialize submodules or set ENABLE_AGS to OFF.")
    endif()
else()
    message(STATUS "AMD AGS library not used.")
endif()

option(ENABLE_AMD_DEVICE_INFO "Enables usage of AMD device_info library." ON)
if(ENABLE_AMD_DEVICE_INFO)
    if(EXISTS "${PROJECT_SOURCE_DIR}/Src/ThirdParty/AMD_device_info/DeviceInfo.h")
        message(STATUS "AMD device_info library used.")
    else()
        message(FATAL_ERROR "AMD device_info library not found. This is likely due to missing submodule. Please initialize submodules or set ENABLE_AMD_DEVICE_INFO to OFF.")
    endif()
else()
    message(STATUS "AMD device_info library not used.")
endif()

option(ENABLE_NVAPI "Enables usage of NVAPI library." ON)
if(ENABLE_NVAPI)
    if(EXISTS "${PROJECT_SOURCE_DIR}/Src/ThirdParty/nvapi/nvapi.h")
        message(STATUS "NVAPI library used.")
    else()
        message(FATAL_ERROR "NVAPI library not found. This is likely due to missing submodule. Please initialize submodules or set ENABLE_NVAPI to OFF.")
    endif()
else()
    message(STATUS "NVAPI library not used.")
endif()

option(ENABLE_VULKAN "Enables usage of Vulkan." ON)
if(ENABLE_VULKAN)
    if(EXISTS "${PROJECT_SOURCE_DIR}/Src/ThirdParty/Vulkan-Headers/CMakeLists.txt")
        message(STATUS "Vulkan library used.")
        add_subdirectory("${PROJECT_SOURCE_DIR}/Src/ThirdParty/Vulkan-Headers" Vulkan-Headers)
    else()
        message(FATAL_ERROR "Vulkan library not found. This is likely due to missing submodule. Please initialize submodules or set ENABLE_VULKAN to OFF.")
    endif()
else()
    message(STATUS "Vulkan library not used.")
endif()

option(ENABLE_INTEL_GPUDETECT "Enable usage of Intel GPU Detect library." ON)
if(ENABLE_INTEL_GPUDETECT)
    if(EXISTS "${PROJECT_SOURCE_DIR}/Src/ThirdParty/gpudetect/GPUDetect.h")
        message(STATUS "Intel GPU Detect library used.")
    else()
        message(FATAL_ERROR "Intel GPU Detect library not found. This is likely due to missing submodule. Please initialize submodules or set ENABLE_INTEL_GPUDETECT to OFF.")
    endif()
else()
    message(STATUS "Intel GPU Detect library not used.")
endif()

function(add_my_executable USE_PREVIEW_AGILITY_SDK)
    set(EXE_NAME "D3d12info")
    if(USE_PREVIEW_AGILITY_SDK)
        set(EXE_NAME "${EXE_NAME}_preview")
    endif()

    add_executable(${EXE_NAME} ${CPP_FILES} ${HPP_FILES})
    target_include_directories(${EXE_NAME} PRIVATE Src)

    # Change Visual C++ runtime library to get rid of dependency on Visual C++ Redistributable
    set_property(TARGET ${EXE_NAME} PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    
    target_compile_options(${EXE_NAME} PRIVATE /W4 /wd4100 /wd4189)
    target_compile_definitions(${EXE_NAME} PRIVATE UNICODE _UNICODE)
    target_precompile_headers(${EXE_NAME} PRIVATE "Src/pch.hpp")
    
    if(USE_PREVIEW_AGILITY_SDK)
        target_compile_definitions(${EXE_NAME} PRIVATE USE_PREVIEW_AGILITY_SDK=1)
        set(AGILITY_SDK_DIRECTORY "${PROJECT_SOURCE_DIR}/Src/ThirdParty/microsoft.direct3d.d3d12.1.716.1-preview")
        add_custom_command(TARGET ${EXE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${EXE_NAME}>/D3D12_preview"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${AGILITY_SDK_DIRECTORY}/build/native/bin/x64/D3D12Core.dll"
                "$<TARGET_FILE_DIR:${EXE_NAME}>/D3D12_preview/"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${AGILITY_SDK_DIRECTORY}/build/native/bin/x64/DirectSR.dll"
                "$<TARGET_FILE_DIR:${EXE_NAME}>/D3D12_preview/")
    else()
        set(AGILITY_SDK_DIRECTORY "${PROJECT_SOURCE_DIR}/Src/ThirdParty/microsoft.direct3d.d3d12.1.615.1")
        add_custom_command(TARGET ${EXE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${EXE_NAME}>/D3D12"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${AGILITY_SDK_DIRECTORY}/build/native/bin/x64/D3D12Core.dll"
                "$<TARGET_FILE_DIR:${EXE_NAME}>/D3D12/")
    endif()

    if(ENABLE_AGS)
        target_compile_definitions(${EXE_NAME} PRIVATE USE_AGS=1)
        target_include_directories(${EXE_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/Src/ThirdParty/AGS_SDK/ags_lib/inc")
        target_link_libraries(${EXE_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/Src/ThirdParty/AGS_SDK/ags_lib/lib/amd_ags_x64.lib")
        add_custom_command(TARGET ${EXE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/Src/ThirdParty/AGS_SDK/ags_lib/lib/amd_ags_x64.dll" "$<TARGET_FILE_DIR:${EXE_NAME}>/")
    endif()

    if(ENABLE_AMD_DEVICE_INFO)
        target_compile_definitions(${EXE_NAME} PRIVATE USE_AMD_DEVICE_INFO=1)
        set(AMD_DEVICE_INFO_SOURCE_FILES
            "Src/ThirdParty/AMD_device_info/DeviceInfo.cpp"
            "Src/ThirdParty/AMD_device_info/DeviceInfo.h")
        target_sources(${EXE_NAME} PRIVATE ${AMD_DEVICE_INFO_SOURCE_FILES})
        source_group("ThirdParty/AMD_device_info" FILES ${AMD_DEVICE_INFO_SOURCE_FILES})
    endif()

    if(ENABLE_NVAPI)
        target_compile_definitions(${EXE_NAME} PRIVATE USE_NVAPI=1)
        target_include_directories(${EXE_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/Src/ThirdParty/nvapi")
        target_link_libraries(${EXE_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/Src/ThirdParty/nvapi/amd64/nvapi64.lib")
    endif()

    if (ENABLE_VULKAN)
        target_compile_definitions(${EXE_NAME} PRIVATE USE_VULKAN=1)
        target_link_libraries(${EXE_NAME} PRIVATE Vulkan::Headers)
    endif()

    if(ENABLE_INTEL_GPUDETECT)
        target_compile_definitions(${EXE_NAME} PRIVATE USE_INTEL_GPUDETECT=1)
        target_link_libraries(${EXE_NAME} PRIVATE "d3d11.lib")
        target_sources(${EXE_NAME} PRIVATE
            "${PROJECT_SOURCE_DIR}/Src/Resources.rc"
            "${PROJECT_SOURCE_DIR}/${INTEL_GPUDETECT_CFG_FILE}")
        source_group("ThirdParty" FILES "${PROJECT_SOURCE_DIR}/${INTEL_GPUDETECT_CFG_FILE}")
    endif()
endfunction()

add_my_executable(FALSE)
add_my_executable(TRUE)
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "D3d12info")
