-- Copyright (c) Evangelion Manuhutu | ORigin Engine

IncludeDir = {}
LibraryDir = {}
Library = {}

VULKAN_SDK = os.getenv("VULKAN_SDK" or "VK_SDK_PATH")

IncludeDir["GLFW"]                     = "%{THIRD_PARTY_DIR}/origin-glfw/include"
IncludeDir["GLAD"]                     = "%{THIRD_PARTY_DIR}/glad/include"
IncludeDir["ASSIMP"]                   = "%{THIRD_PARTY_DIR}/Assimp/include"
IncludeDir["IMGUI"]                    = "%{THIRD_PARTY_DIR}/imgui"
IncludeDir["IMGUIZMO"]                 = "%{THIRD_PARTY_DIR}/ImGuizmo"
IncludeDir["BOX2D"]                    = "%{THIRD_PARTY_DIR}/Box2D/include"
IncludeDir["STBI"]                     = "%{THIRD_PARTY_DIR}/stb"
IncludeDir["SPDLOG"]                   = "%{THIRD_PARTY_DIR}/spdlog/include"
IncludeDir["GLM"]                      = "%{THIRD_PARTY_DIR}/glm"
IncludeDir["ENTT"]                     = "%{THIRD_PARTY_DIR}/entt"
IncludeDir["FMT"]                      = "%{THIRD_PARTY_DIR}/FMT/include"
IncludeDir["MONO"]                     = "%{THIRD_PARTY_DIR}/mono/include"
IncludeDir["MINIAUDIO"]                = "%{THIRD_PARTY_DIR}/miniaudio"
IncludeDir["FILEWATCHER"]              = "%{THIRD_PARTY_DIR}/Filewatcher/include"
IncludeDir["YAMLCPP"]                  = "%{THIRD_PARTY_DIR}/yaml-cpp/include"
IncludeDir["MSDFATLASGEN"]             = "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdf-atlas-gen"
IncludeDir["MSDFGEN"]                  = "%{THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen"
IncludeDir["JOLT"]                     = "%{THIRD_PARTY_DIR}/JoltPhysics"

-- Windows
Library["MONO"]                        = "%{THIRD_PARTY_DIR}/mono/win32/libmono-static-sgen.lib"
IncludeDir["VulkanSDK"]                = "%{VULKAN_SDK}/Include/" -- includes
LibraryDir["VulkanSDK"]                = "%{VULKAN_SDK}/Lib/" -- library
Library["Vulkan1Lib"]                  = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["ShaderC_Debug"]               = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"]           = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"]      = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"]           = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"
Library["ShaderC_Release"]             = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"]         = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"]    = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

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