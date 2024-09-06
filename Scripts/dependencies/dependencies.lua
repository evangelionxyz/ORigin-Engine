-- Copyright (c) Evangelion Manuhutu | ORigin Engine

IncludeDir = {}
LibraryDir = {}
Library = {}

VULKAN_SDK = os.getenv("VULKAN_SDK" or "VK_SDK_PATH")

IncludeDir["GLFW"]                     = "%{wks.location}/ORigin/vendor/origin-glfw/include"
IncludeDir["GLAD"]                     = "%{wks.location}/ORigin/vendor/glad/include"
IncludeDir["ASSIMP"]                   = "%{wks.location}/ORigin/vendor/Assimp/include"
IncludeDir["IMGUI"]                    = "%{wks.location}/ORigin/vendor/imgui"
IncludeDir["IMGUIZMO"]                 = "%{wks.location}/ORigin/vendor/ImGuizmo"
IncludeDir["BOX2D"]                    = "%{wks.location}/ORigin/vendor/Box2D/include"
IncludeDir["STBI"]                     = "%{wks.location}/ORigin/vendor/stb"
IncludeDir["SPDLOG"]                   = "%{wks.location}/ORigin/vendor/spdlog/include"
IncludeDir["GLM"]                      = "%{wks.location}/ORigin/vendor/glm"
IncludeDir["ENTT"]                     = "%{wks.location}/ORigin/vendor/entt"
IncludeDir["MONO"]                     = "%{wks.location}/ORigin/vendor/mono/include"
IncludeDir["MINIAUDIO"]                = "%{wks.location}/ORigin/vendor/miniaudio"
IncludeDir["FILEWATCHER"]              = "%{wks.location}/ORigin/vendor/Filewatcher/include"
IncludeDir["YAMLCPP"]                  = "%{wks.location}/ORigin/vendor/yaml-cpp/include"
IncludeDir["MSDFATLASGEN"]             = "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen"
IncludeDir["MSDFGEN"]                  = "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen"
IncludeDir["JOLT"]                     = "%{wks.location}/ORigin/vendor/JoltPhysics"

-- Windows
Library["MONO"]                        = "%{wks.location}/ORigin/vendor/mono/win32/libmono-static-sgen.lib"
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