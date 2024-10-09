cmake_minimum_required(VERSION 3.20)
project(OR1)

set(CMAKE_CXX_STANDARD 20)
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(THIRD_PARTY_DIR ${ROOT_DIR}/Engine/ThirdParty)
set(EDITOR_LIB_DIR ${ROOT_DIR}/Engine/Editor/Lib)
set(BUILD_DIR ${ROOT_DIR}/Engine/Build/CMake)
set(THIRD_BUILD_PARTY_DIR ${ROOT_DIR}/Engine/Build/CMake/ThirdParty)

if (WIN32)
    message("[ORigin] Platform: Windows")
    # Enable Hot Reload for MSVC compilers if supported.
    if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT 
        $<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>
    )
    endif()
elseif (UNIX AND NOT APPLE)
endif()

include(${THIRD_PARTY_DIR}/ProjectFiles/ASSIMP.cmake)
include(${THIRD_PARTY_DIR}/ProjectFiles/BOX2D.cmake)
include(${THIRD_PARTY_DIR}/ProjectFiles/FREETYPE.cmake)
include(${THIRD_PARTY_DIR}/ProjectFiles/GLAD.cmake)
include(${THIRD_PARTY_DIR}/ProjectFiles/GLFW.cmake)
include(${THIRD_PARTY_DIR}/ProjectFiles/IMGUI.cmake)
include(${THIRD_PARTY_DIR}/ProjectFiles/JOLT.cmake)
include(${THIRD_PARTY_DIR}/ProjectFiles/MSDFATLASGEN.cmake)
include(${THIRD_PARTY_DIR}/ProjectFiles/MSDFGEN.cmake)
include(${THIRD_PARTY_DIR}/ProjectFiles/YAMLCPP.cmake)

add_subdirectory(${THIRD_PARTY_DIR}/FMT)
add_subdirectory(Engine/ORigin)
add_subdirectory(Engine/Editor)
add_subdirectory(Engine/Sandbox)
add_subdirectory(Engine/Runtime)
add_subdirectory(Engine/Test)