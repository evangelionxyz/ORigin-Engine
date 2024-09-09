-- Copyright (c) Evangelion Manuhutu | ORigin Engine

IncludeDir = {}
LibraryDir = {}
Library = {}

IncludeDir["GLFW"]                     = "%{THIRD_PARTY_DIR}/GLFW/include"
IncludeDir["GLAD"]                     = "%{THIRD_PARTY_DIR}/GLAD/include"
IncludeDir["ASSIMP"]                   = "%{THIRD_PARTY_DIR}/Assimp/include"
IncludeDir["IMGUI"]                    = "%{THIRD_PARTY_DIR}/IMGUI"
IncludeDir["IMGUIZMO"]                 = "%{THIRD_PARTY_DIR}/IMGUIZMO"
IncludeDir["BOX2D"]                    = "%{THIRD_PARTY_DIR}/BOX2D/include"
IncludeDir["STBI"]                     = "%{THIRD_PARTY_DIR}/stb"
IncludeDir["GLM"]                      = "%{THIRD_PARTY_DIR}/GLM"
IncludeDir["ENTT"]                     = "%{THIRD_PARTY_DIR}/entt"
IncludeDir["FMT"]                      = "%{THIRD_PARTY_DIR}/FMT/include"
IncludeDir["MONO"]                     = "%{THIRD_PARTY_DIR}/Mono/include"
IncludeDir["MINIAUDIO"]                = "%{THIRD_PARTY_DIR}/miniaudio"
IncludeDir["FILEWATCHER"]              = "%{THIRD_PARTY_DIR}/Filewatcher/include"
IncludeDir["YAMLCPP"]                  = "%{THIRD_PARTY_DIR}/YAMLCPP/include"
IncludeDir["MSDFATLASGEN"]             = "%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdf-atlas-gen"
IncludeDir["MSDFGEN"]                  = "%{THIRD_PARTY_DIR}/MSDFATLASGEN/msdfgen"
IncludeDir["JOLT"]                     = "%{THIRD_PARTY_DIR}/JOLTPHYSICS"

-- Windows
EDITOR_LIB_DIR                         = "%{wks.location}Engine/Editor/Lib/"
Library["MONO"]                        = "%{EDITOR_LIB_DIR}/Mono/Win32/libmono-static-sgen.lib"

IncludeDir["VulkanSDK"]                = "%{THIRD_PARTY_DIR}/Vulkan/Include" -- includes
LibraryDir["VulkanSDK"]                = "%{EDITOR_LIB_DIR}/Vulkan/Win32" -- library
Library["Vulkan1Lib"]                  = "%{EDITOR_LIB_DIR}/Vulkan/Win32/vulkan-1.lib"

Library["ShaderC_Debug"]               = "%{EDITOR_LIB_DIR}/ShaderC/Win32/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"]           = "%{EDITOR_LIB_DIR}/ShaderC/Win32/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"]      = "%{EDITOR_LIB_DIR}/ShaderC/Win32/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"]           = "%{EDITOR_LIB_DIR}/ShaderC/Win32/SPIRV-Toolsd.lib"
Library["ShaderC_Release"]             = "%{EDITOR_LIB_DIR}/ShaderC/Win32/shaderc_shared.lib"
Library["SPIRV_Cross_Release"]         = "%{EDITOR_LIB_DIR}/ShaderC/Win32/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"]    = "%{EDITOR_LIB_DIR}/ShaderC/Win32/spirv-cross-glsl.lib"

Library["WinSock"]                     = "Ws2_32.lib"
Library["WinMM"]                       = "Winmm.lib"
Library["WinVersion"]                  = "Version.lib"
Library["BCrypt"]                      = "Bcrypt.lib"

-- Dependencies Lua File
group "Dependecies"
    include "assimp.lua"
    include "glfw.lua"
    include "glad.lua"
    include "imgui.lua"
    include "yaml-cpp.lua"
    include "box2d.lua"
    include "msdf-atlas-gen.lua"
    include "msdfgen.lua"
    include "freetype.lua"
    include "JoltPhysics.lua"
    include "fmt.lua"
group ""