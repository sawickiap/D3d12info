cmake_minimum_required(VERSION 3.17)

project(D3d12info LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
file(GLOB CPP_FILES "Src/*.cpp")
file(GLOB HPP_FILES "Src/*.hpp")
set(RAPIDJSON_NATVIS_FILE "Src/ThirdParty/rapidjson/contrib/natvis/rapidjson.natvis")
set(INTEL_GPUDETECT_CFG_FILE "Src/ThirdParty/intel_gpudetect/IntelGfx.cfg")

set(AGS_DIRECTORY "" CACHE STRING "Path to AMD GPU Services (AGS) library - \"ags_lib\" directory. Leave empty to compile without AGS.")
if(AGS_DIRECTORY)
    if(EXISTS "${AGS_DIRECTORY}/inc/amd_ags.h")
        message(STATUS "AMD AGS library used from \"${AGS_DIRECTORY}\".")
    else()
        message(FATAL_ERROR "AMD AGS library not found - cannot find file \"${AGS_DIRECTORY}/inc/amd_ags.h\".")
        unset(AGS_DIRECTORY)
    endif()
else()
    message(STATUS "AMD AGS library not used.")
endif()

set(NVAPI_DIRECTORY "" CACHE STRING "Path to NVAPI library. Leave empty to compile without NVAPI.")
if(NVAPI_DIRECTORY)
    if(EXISTS "${NVAPI_DIRECTORY}/nvapi.h")
        message(STATUS "NVAPI library used from \"${NVAPI_DIRECTORY}\".")
    else()
        message(FATAL_ERROR "NVAPI library not found - cannot find file \"${NVAPI_DIRECTORY}/nvapi.h\".")
        unset(NVAPI_DIRECTORY)
    endif()
else()
    message(STATUS "NVAPI library not used.")
endif()

option(ENABLE_VULKAN "Enables usage of Vulkan SDK." ON)
if(ENABLE_VULKAN)
    # 1.3.231 is oldest tested Vulkan SDK version
    # if Cmake finds older version, it will print warning and set Vulkan_FOUND to false
    find_package(Vulkan 1.3.231)
    if (Vulkan_FOUND)
        message(STATUS "Vulkan SDK found. Include directory is \"${Vulkan_INCLUDE_DIRS}\".")
    else()
        message(STATUS "Vulkan SDK not found.")
    endif()
else()
    message(STATUS "Vulkan SDK not used.")
endif()

option(ENABLE_INTEL_GPUDETECT "Enable usage of Intel GPU Detect library." ON)
if(ENABLE_INTEL_GPUDETECT)
    message(STATUS "Intel GPU Detect library used.")
else()
    message(STATUS "Intel GPU Detect not library used.")
endif()

function(add_my_executable USE_PREVIEW_AGILITY_SDK)
    set(EXE_NAME "D3d12info")
    if(USE_PREVIEW_AGILITY_SDK)
        set(EXE_NAME "${EXE_NAME}_preview")
    endif()

    add_executable(${EXE_NAME} ${CPP_FILES} ${HPP_FILES})
    
    target_compile_options(${EXE_NAME} PRIVATE /W4 /wd4100 /wd4189)
    target_compile_definitions(${EXE_NAME} PRIVATE UNICODE _UNICODE)
    target_precompile_headers(${EXE_NAME} PRIVATE "Src/pch.hpp")
    
    target_sources(${EXE_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/${RAPIDJSON_NATVIS_FILE}")
    source_group("ThirdParty" FILES ${RAPIDJSON_NATVIS_FILE})
    
    if(USE_PREVIEW_AGILITY_SDK)
        target_compile_definitions(${EXE_NAME} PRIVATE USE_PREVIEW_AGILITY_SDK=1)
        set(AGILITY_SDK_DIRECTORY "${PROJECT_SOURCE_DIR}/Src/ThirdParty/microsoft.direct3d.d3d12.1.711.3-preview")
        add_custom_command(TARGET ${EXE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${EXE_NAME}>/D3D12_preview"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${AGILITY_SDK_DIRECTORY}/build/native/bin/x64/D3D12Core.dll"
                "$<TARGET_FILE_DIR:${EXE_NAME}>/D3D12_preview/")
    else()
        set(AGILITY_SDK_DIRECTORY "${PROJECT_SOURCE_DIR}/Src/ThirdParty/microsoft.direct3d.d3d12.1.611.0")
        add_custom_command(TARGET ${EXE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${EXE_NAME}>/D3D12"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${AGILITY_SDK_DIRECTORY}/build/native/bin/x64/D3D12Core.dll"
                "$<TARGET_FILE_DIR:${EXE_NAME}>/D3D12/")
    endif()

    if(AGS_DIRECTORY)
        target_compile_definitions(${EXE_NAME} PRIVATE USE_AGS=1)
        target_include_directories(${EXE_NAME} PRIVATE "${AGS_DIRECTORY}/inc")
        target_link_libraries(${EXE_NAME} PRIVATE "${AGS_DIRECTORY}/lib/amd_ags_x64.lib")
        add_custom_command(TARGET ${EXE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${AGS_DIRECTORY}/lib/amd_ags_x64.dll" "$<TARGET_FILE_DIR:${EXE_NAME}>/")
    endif()

    set(NVAPI_DIRECTORY "" CACHE STRING "Path to NVAPI library. Leave empty to compile without NVAPI.")
    if(NVAPI_DIRECTORY)
        target_compile_definitions(${EXE_NAME} PRIVATE USE_NVAPI=1)
        target_include_directories(${EXE_NAME} PRIVATE "${NVAPI_DIRECTORY}")
        target_link_libraries(${EXE_NAME} PRIVATE "${NVAPI_DIRECTORY}/amd64/nvapi64.lib")
    endif()

    if (Vulkan_FOUND)
        target_compile_definitions(${EXE_NAME} PRIVATE USE_VULKAN=1)
        target_include_directories(${EXE_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
        target_link_libraries(${EXE_NAME} PRIVATE ${Vulkan_LIBRARIES})
    endif()

    if(ENABLE_INTEL_GPUDETECT)
        target_compile_definitions(${EXE_NAME} PRIVATE USE_INTEL_GPUDETECT=1)
        target_link_libraries(${EXE_NAME} PRIVATE "d3d11.lib")
        target_sources(${EXE_NAME} PRIVATE
            "${PROJECT_SOURCE_DIR}/Src/Resources.rc"
            "${PROJECT_SOURCE_DIR}/Src/ThirdParty/intel_gpudetect/IntelGfx.cfg")
        source_group("ThirdParty" FILES "${PROJECT_SOURCE_DIR}/${INTEL_GPUDETECT_CFG_FILE}")
    endif()
endfunction()

add_my_executable(FALSE)
add_my_executable(TRUE)
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "D3d12info")
