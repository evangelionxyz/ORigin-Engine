cmake_minimum_required(VERSION 3.20)
project(OR1)

set(CMAKE_CXX_STANDARD 20)
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BUILD_DIR ${ROOT_DIR}/build)
set(VENDOR_DIR ${ROOT_DIR}/ORigin/vendor)

if (WIN32)
    if(NOT DEFINED ENV{VULKAN_SDK})
        message(FATAL_ERROR "[ORigin] VULKAN_SDK environment variable not defined. Please install the Vulkan SDK and set the environment variable.")
    endif()
    
    set(VULKAN_INCLUDE_DIR $ENV{VULKAN_SDK}/Include)
    set(VULKAN_LIBRARY_DIR $ENV{VULKAN_SDK}/Lib)

    message("[ORigin] Platform: Win32")
    message("[ORigin] Vulkan include dir: ${VULKAN_INCLUDE_DIR}")
    message("[ORigin] Vulkan lib dir: ${VULKAN_LIBRARY_DIR}")

    # Enable Hot Reload for MSVC compilers if supported.
    if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT 
        $<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>
    )
    endif()
elseif (UNIX AND NOT APPLE)
endif()

include(${VENDOR_DIR}/CMakeFiles/ASSIMP.cmake)
include(${VENDOR_DIR}/CMakeFiles/BOX2D.cmake)
include(${VENDOR_DIR}/CMakeFiles/FreeType.cmake)
include(${VENDOR_DIR}/CMakeFiles/GLAD.cmake)
include(${VENDOR_DIR}/CMakeFiles/GLFW.cmake)
include(${VENDOR_DIR}/CMakeFiles/IMGUI.cmake)
include(${VENDOR_DIR}/CMakeFiles/JOLT.cmake)
include(${VENDOR_DIR}/CMakeFiles/MSDFATLASGEN.cmake)
include(${VENDOR_DIR}/CMakeFiles/MSDFGEN.cmake)
include(${VENDOR_DIR}/CMakeFiles/YAMLCPP.cmake)
add_subdirectory(${VENDOR_DIR}/FMT)

add_subdirectory(ORigin)
add_subdirectory(ORigin-Editor)
add_subdirectory(Sandbox)
#include(ORigin-Runtime/Runtime.cmake)