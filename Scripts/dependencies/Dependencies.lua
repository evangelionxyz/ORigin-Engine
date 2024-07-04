-- Copyright (c) Evangelion Manuhutu | ORigin Engine

VULKAN_SDK = os.getenv("VULKAN_SDK" or "VK_SDK_PATH")
ASSIMP_SDK = os.getenv("ASSIMP_SDK")

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/ORigin/vendor/origin-glfw/include"
IncludeDir["GLAD"] = "%{wks.location}/ORigin/vendor/glad/include"
IncludeDir["ASSIMP"] = "%{wks.location}/ORigin/vendor/assimp/include"
IncludeDir["IMGUI"] = "%{wks.location}/ORigin/vendor/imgui"
IncludeDir["IMGUIZMO"] = "%{wks.location}/ORigin/vendor/ImGuizmo"
IncludeDir["BOX2D"] = "%{wks.location}/ORigin/vendor/Box2D/include"
IncludeDir["STBI"] = "%{wks.location}/ORigin/vendor/stb"
IncludeDir["SPDLOG"] = "%{wks.location}/ORigin/vendor/spdlog/include"
IncludeDir["GLM"] = "%{wks.location}/ORigin/vendor/glm"
IncludeDir["ENTT"] = "%{wks.location}/ORigin/vendor/entt/"
IncludeDir["MONO"] = "%{wks.location}/ORigin/vendor/mono/include"
IncludeDir["Miniaudio"] = "%{wks.location}/ORigin/vendor/miniaudio"
IncludeDir["FILEWATCHER"] = "%{wks.location}/ORigin/vendor/Filewatcher/include"
IncludeDir["YAML_CPP"] = "%{wks.location}/ORigin/vendor/yaml-cpp/include"
IncludeDir["msdf_atlas_gen"] = "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen"
IncludeDir["msdfgen"] = "%{wks.location}/ORigin/vendor/msdf-atlas-gen/msdfgen"
IncludeDir["PhysX"] = "%{wks.location}/ORigin/vendor/PhysX/physx/include"
IncludeDir["JoltPhysics"] = "%{wks.location}/ORigin/vendor/JoltPhysics"
IncludeDir["OPTICK"] = "%{wks.location}/ORigin/vendor/Optick/src/"

LibraryDir = {}
Library = {}

-- Windows
Library["WindowsMONO"] = "%{wks.location}/ORigin/vendor/mono/lib/%{cfg.buildcfg}/libmono-static-sgen.lib"

IncludeDir["WindowsVulkanSDK"] = "%{VULKAN_SDK}/include" -- includes
LibraryDir["WindowsVulkanSDK"] = "%{VULKAN_SDK}/lib" -- library
Library["WVulkan"] = "%{LibraryDir.WindowsVulkanSDK}/vulkan-1.lib"
Library["WShaderC_Debug"] = "%{LibraryDir.WindowsVulkanSDK}/shaderc_sharedd.lib"
Library["WSPIRV_Cross_Debug"] = "%{LibraryDir.WindowsVulkanSDK}/spirv-cross-cored.lib"
Library["WSPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.WindowsVulkanSDK}/spirv-cross-glsld.lib"
Library["WSPIRV_Tools_Debug"] = "%{LibraryDir.WindowsVulkanSDK}/SPIRV-Toolsd.lib"
Library["WShaderC_Release"] = "%{LibraryDir.WindowsVulkanSDK}/shaderc_shared.lib"
Library["WSPIRV_Cross_Release"] = "%{LibraryDir.WindowsVulkanSDK}/spirv-cross-core.lib"
Library["WSPIRV_Cross_GLSL_Release"] = "%{LibraryDir.WindowsVulkanSDK}/spirv-cross-glsl.lib"
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"

-- Linux
Library["LinuxMONO"] = "%{wks.location}/ORigin/vendor/mono/lib/Linux/libmonosgen.a"
IncludeDir["LinuxVulkanSDK"] = "%{wks.location}/ORigin/vendor/VulkanSDK/1.3.261.1/x86_64/include" -- includes
LibraryDir["LinuxVulkanSDK"] = "%{wks.location}/ORigin/vendor/VulkanSDK/1.3.261.1/x86_64/lib" -- library
Library["LVulkan"] = "%{LibraryDir.LinuxVulkanSDK}/libvulkan.so"
Library["LShaderC_Release"] = "%{LibraryDir.LinuxVulkanSDK}/libshaderc.a"
Library["LSPIRV_Cross_Release"] = "%{LibraryDir.LinuxVulkanSDK}/libspirv-cross-core.a"
Library["LSPIRV_Cross_GLSL_Release"] = "%{LibraryDir.LinuxVulkanSDK}/libspirv-cross-glsl.a"

-- Dependencies Lua File
group "Dependecies"
include "assimp.lua"
include "origin-glfw.lua"
include "glad.lua"
include "imgui.lua"
include "yaml-cpp.lua"
include "box2d.lua"
include "msdf-atlas-gen.lua"
include "JoltPhysics.lua"
include "Optick.lua"
group ""

group "PhysX"
include "FastXml.lua"
include "LowLevel.lua"
include "LowLevelAABB.lua"
include "LowLevelDynamics.lua"
include "PhysX.lua"
include "PhysXCharacterKinematic.lua"
include "PhysXCommon.lua"
include "PhysXCooking.lua"
include "PhysXExtensions.lua"
include "PhysXFoundation.lua"
include "PhysXPvdSDK.lua"
include "PhysXTask.lua"
include "PhysXVehicle.lua"
include "PhysXVehicle2.lua"
include "SceneQuery.lua"
include "SimulationController.lua"
group ""