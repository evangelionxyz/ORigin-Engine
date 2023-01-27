-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

-- Set Dependencies Folder

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/ORigin/vendor/glfw/include"
IncludeDir["GLAD"] = "%{wks.location}/ORigin/vendor/glad/include"
IncludeDir["IMGUI"] = "%{wks.location}/ORigin/vendor/imgui"
IncludeDir["IMGUIZMO"] = "%{wks.location}/ORigin/vendor/ImGuizmo"
IncludeDir["BOX2D"] = "%{wks.location}/ORigin/vendor/Box2D/include"
IncludeDir["STBI"] = "%{wks.location}/ORigin/vendor/stb_image"
IncludeDir["SPDLOG"] = "%{wks.location}/ORigin/vendor/spdlog/include"
IncludeDir["GLM"] = "%{wks.location}/ORigin/vendor/glm"
IncludeDir["ENTT"] = "%{wks.location}/ORigin/vendor/entt/"
IncludeDir["MONO"] = "%{wks.location}/ORigin/vendor/mono/include"
IncludeDir["YAML_CPP"] = "%{wks.location}/ORigin/vendor/yaml-cpp/include"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["MONO"] = "%{wks.location}/ORigin/vendor/mono/lib/%{cfg.buildcfg}"

Library = {}
Library["MONO"] = "%{LibraryDir.MONO}/libmono-static-sgen.lib"

Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

-- Windows-Only
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"

-- Dependencies Lua File
group "vendor"
include "glfw.lua"
include "glad.lua"
include "imgui.lua"
include "yaml-cpp.lua"
include "box2d.lua"
group ""