add_library("ORigin" STATIC
      "ORigin/sources/Origin/Animation/Animation.h"
      "ORigin/sources/Origin/Animation/AnimationController.cpp"
      "ORigin/sources/Origin/Animation/AnimationController.h"
      "ORigin/sources/Origin/Animation/AnimationState.h"
      "ORigin/sources/Origin/Animation/Animator.cpp"
      "ORigin/sources/Origin/Animation/Animator.h"
      "ORigin/sources/Origin/Animation/ModelAnimation.cpp"
      "ORigin/sources/Origin/Animation/ModelAnimation.h"
      "ORigin/sources/Origin/Animation/SpriteAnimation.cpp"
      "ORigin/sources/Origin/Animation/SpriteAnimation.h"
      "ORigin/sources/Origin/Asset/Asset.cpp"
      "ORigin/sources/Origin/Asset/Asset.h"
      "ORigin/sources/Origin/Asset/AssetImporter.cpp"
      "ORigin/sources/Origin/Asset/AssetImporter.h"
      "ORigin/sources/Origin/Asset/AssetManager.h"
      "ORigin/sources/Origin/Asset/AssetManagerBase.h"
      "ORigin/sources/Origin/Asset/AssetMetadata.h"
      "ORigin/sources/Origin/Asset/EditorAssetManager.cpp"
      "ORigin/sources/Origin/Asset/EditorAssetManager.h"
      "ORigin/sources/Origin/Audio/AudioEngine.cpp"
      "ORigin/sources/Origin/Audio/AudioEngine.h"
      "ORigin/sources/Origin/Audio/AudioListener.cpp"
      "ORigin/sources/Origin/Audio/AudioListener.h"
      "ORigin/sources/Origin/Audio/AudioSource.cpp"
      "ORigin/sources/Origin/Audio/AudioSource.h"
      "ORigin/sources/Origin/Core/AppEvent.h"
      "ORigin/sources/Origin/Core/Application.cpp"
      "ORigin/sources/Origin/Core/Application.h"
      "ORigin/sources/Origin/Core/Assert.h"
      "ORigin/sources/Origin/Core/Base.h"
      "ORigin/sources/Origin/Core/Buffer.h"
      "ORigin/sources/Origin/Core/CommandManager.h"
      "ORigin/sources/Origin/Core/ConsoleManager.cpp"
      "ORigin/sources/Origin/Core/ConsoleManager.h"
      "ORigin/sources/Origin/Core/Event.h"
      "ORigin/sources/Origin/Core/Input.cpp"
      "ORigin/sources/Origin/Core/Input.h"
      "ORigin/sources/Origin/Core/KeyCodes.h"
      "ORigin/sources/Origin/Core/KeyEvent.h"
      "ORigin/sources/Origin/Core/Layer.cpp"
      "ORigin/sources/Origin/Core/Layer.h"
      "ORigin/sources/Origin/Core/LayerStack.cpp"
      "ORigin/sources/Origin/Core/LayerStack.h"
      "ORigin/sources/Origin/Core/Log.cpp"
      "ORigin/sources/Origin/Core/Log.h"
      "ORigin/sources/Origin/Core/MouseCodes.h"
      "ORigin/sources/Origin/Core/MouseEvent.h"
      "ORigin/sources/Origin/Core/Random.cpp"
      "ORigin/sources/Origin/Core/Random.h"
      "ORigin/sources/Origin/Core/Time.cpp"
      "ORigin/sources/Origin/Core/Time.h"
      "ORigin/sources/Origin/Core/UUID.cpp"
      "ORigin/sources/Origin/Core/UUID.h"
      "ORigin/sources/Origin/Core/Window.cpp"
      "ORigin/sources/Origin/Core/Window.h"
    "ORigin/sources/Origin/EntryPoint.h"
      "ORigin/sources/Origin/GUI/GuiLayer.cpp"
      "ORigin/sources/Origin/GUI/GuiLayer.h"
      "ORigin/sources/Origin/GUI/UI.h"
      "ORigin/sources/Origin/Math/AABB.h"
      "ORigin/sources/Origin/Math/Math.cpp"
      "ORigin/sources/Origin/Math/Math.h"
      "ORigin/sources/Origin/Math/OBB.h"
        "ORigin/sources/Origin/Physics/2D/Contact2DListener.cpp"
        "ORigin/sources/Origin/Physics/2D/Contact2DListener.h"
        "ORigin/sources/Origin/Physics/2D/Physics2D.cpp"
        "ORigin/sources/Origin/Physics/2D/Physics2D.h"
      "ORigin/sources/Origin/Physics/JoltBodyActivationListener.cpp"
      "ORigin/sources/Origin/Physics/JoltBodyActivationListener.h"
      "ORigin/sources/Origin/Physics/JoltContactListener.cpp"
      "ORigin/sources/Origin/Physics/JoltContactListener.h"
      "ORigin/sources/Origin/Physics/PhysicsBody.h"
      "ORigin/sources/Origin/Physics/PhysicsEngine.cpp"
      "ORigin/sources/Origin/Physics/PhysicsEngine.h"
      "ORigin/sources/Origin/Profiler/Profiler.h"
      "ORigin/sources/Origin/Project/Project.cpp"
      "ORigin/sources/Origin/Project/Project.h"
      "ORigin/sources/Origin/Renderer/Buffer.cpp"
      "ORigin/sources/Origin/Renderer/Buffer.h"
      "ORigin/sources/Origin/Renderer/Font.cpp"
      "ORigin/sources/Origin/Renderer/Font.h"
      "ORigin/sources/Origin/Renderer/Framebuffer.cpp"
      "ORigin/sources/Origin/Renderer/Framebuffer.h"
      "ORigin/sources/Origin/Renderer/Frustum.cpp"
      "ORigin/sources/Origin/Renderer/Frustum.h"
      "ORigin/sources/Origin/Renderer/GraphicsContext.cpp"
      "ORigin/sources/Origin/Renderer/GraphicsContext.h"
      "ORigin/sources/Origin/Renderer/Material.cpp"
      "ORigin/sources/Origin/Renderer/Material.h"
      "ORigin/sources/Origin/Renderer/MaterialLibrary.cpp"
      "ORigin/sources/Origin/Renderer/MaterialLibrary.h"
      "ORigin/sources/Origin/Renderer/MeshRenderer.cpp"
      "ORigin/sources/Origin/Renderer/MeshRenderer.h"
      "ORigin/sources/Origin/Renderer/MeshVertexData.h"
      "ORigin/sources/Origin/Renderer/ModelLoader.cpp"
      "ORigin/sources/Origin/Renderer/ModelLoader.h"
      "ORigin/sources/Origin/Renderer/ParticleSystem.cpp"
      "ORigin/sources/Origin/Renderer/ParticleSystem.h"
      "ORigin/sources/Origin/Renderer/RenderCommand.h"
      "ORigin/sources/Origin/Renderer/Renderer.cpp"
      "ORigin/sources/Origin/Renderer/Renderer.h"
      "ORigin/sources/Origin/Renderer/Renderer2D.cpp"
      "ORigin/sources/Origin/Renderer/Renderer2D.h"
      "ORigin/sources/Origin/Renderer/RendererAPI.cpp"
      "ORigin/sources/Origin/Renderer/RendererAPI.h"
      "ORigin/sources/Origin/Renderer/ScreenRenderer.cpp"
      "ORigin/sources/Origin/Renderer/ScreenRenderer.h"
      "ORigin/sources/Origin/Renderer/Shader.cpp"
      "ORigin/sources/Origin/Renderer/Shader.h"
      "ORigin/sources/Origin/Renderer/ShaderLibrary.cpp"
      "ORigin/sources/Origin/Renderer/ShaderLibrary.h"
      "ORigin/sources/Origin/Renderer/SubTexture2D.cpp"
      "ORigin/sources/Origin/Renderer/SubTexture2D.h"
      "ORigin/sources/Origin/Renderer/Texture.cpp"
      "ORigin/sources/Origin/Renderer/Texture.h"
      "ORigin/sources/Origin/Renderer/UniformBuffer.cpp"
      "ORigin/sources/Origin/Renderer/UniformBuffer.h"
      "ORigin/sources/Origin/Renderer/VertexArray.cpp"
      "ORigin/sources/Origin/Renderer/VertexArray.h"
      "ORigin/sources/Origin/Scene/Camera.h"
        "ORigin/sources/Origin/Scene/Components/Components.h"
        "ORigin/sources/Origin/Scene/Components/ComponentsCommand.h"
        "ORigin/sources/Origin/Scene/Components/PhysicsComponent.cpp"
        "ORigin/sources/Origin/Scene/Components/PhysicsComponents.h"
      "ORigin/sources/Origin/Scene/EditorCamera.cpp"
      "ORigin/sources/Origin/Scene/EditorCamera.h"
      "ORigin/sources/Origin/Scene/Entity.cpp"
      "ORigin/sources/Origin/Scene/Entity.h"
      "ORigin/sources/Origin/Scene/EntityManager.cpp"
      "ORigin/sources/Origin/Scene/EntityManager.h"
      "ORigin/sources/Origin/Scene/Lighting.cpp"
      "ORigin/sources/Origin/Scene/Lighting.h"
      "ORigin/sources/Origin/Scene/Scene.cpp"
      "ORigin/sources/Origin/Scene/Scene.h"
      "ORigin/sources/Origin/Scene/SceneCamera.cpp"
      "ORigin/sources/Origin/Scene/SceneCamera.h"
      "ORigin/sources/Origin/Scene/SceneCommand.h"
      "ORigin/sources/Origin/Scene/ScriptableEntity.h"
      "ORigin/sources/Origin/Scene/ShadowRenderer.cpp"
      "ORigin/sources/Origin/Scene/ShadowRenderer.h"
      "ORigin/sources/Origin/Scene/Skybox.cpp"
      "ORigin/sources/Origin/Scene/Skybox.h"
      "ORigin/sources/Origin/Scene/SpriteSheet.cpp"
      "ORigin/sources/Origin/Scene/SpriteSheet.h"
      "ORigin/sources/Origin/Scene/UIRenderer.cpp"
      "ORigin/sources/Origin/Scene/UIRenderer.h"
      "ORigin/sources/Origin/Scripting/ScriptClass.cpp"
      "ORigin/sources/Origin/Scripting/ScriptClass.h"
      "ORigin/sources/Origin/Scripting/ScriptEngine.cpp"
      "ORigin/sources/Origin/Scripting/ScriptEngine.h"
      "ORigin/sources/Origin/Scripting/ScriptField.h"
      "ORigin/sources/Origin/Scripting/ScriptGlue.cpp"
      "ORigin/sources/Origin/Scripting/ScriptGlue.h"
      "ORigin/sources/Origin/Scripting/ScriptInstance.cpp"
      "ORigin/sources/Origin/Scripting/ScriptInstance.h"
      "ORigin/sources/Origin/Serializer/MaterialSerializer.cpp"
      "ORigin/sources/Origin/Serializer/MaterialSerializer.h"
      "ORigin/sources/Origin/Serializer/ProjectSerializer.cpp"
      "ORigin/sources/Origin/Serializer/ProjectSerializer.h"
      "ORigin/sources/Origin/Serializer/SceneSerializer.cpp"
      "ORigin/sources/Origin/Serializer/SceneSerializer.h"
      "ORigin/sources/Origin/Serializer/Serializer.h"
      "ORigin/sources/Origin/Serializer/SpriteSheetSerializer.cpp"
      "ORigin/sources/Origin/Serializer/SpriteSheetSerializer.h"
      "ORigin/sources/Origin/Utils/PlatformUtils.h"
      "ORigin/sources/Origin/Utils/StringUtils.cpp"
      "ORigin/sources/Origin/Utils/StringUtils.h"
      "ORigin/sources/Origin/Utils/Utils.h"
  "ORigin/sources/Origin.h"
      "ORigin/sources/Platform/Linux/LinuxUtils.cpp"
      "ORigin/sources/Platform/Linux/LinuxWindow.cpp"
      "ORigin/sources/Platform/OpenGL/OpenGLBuffer.cpp"
      "ORigin/sources/Platform/OpenGL/OpenGLBuffer.h"
      "ORigin/sources/Platform/OpenGL/OpenGLContext.cpp"
      "ORigin/sources/Platform/OpenGL/OpenGLContext.h"
      "ORigin/sources/Platform/OpenGL/OpenGLFramebuffer.cpp"
      "ORigin/sources/Platform/OpenGL/OpenGLFramebuffer.h"
      "ORigin/sources/Platform/OpenGL/OpenGLRendererAPI.cpp"
      "ORigin/sources/Platform/OpenGL/OpenGLRendererAPI.h"
      "ORigin/sources/Platform/OpenGL/OpenGLShader.cpp"
      "ORigin/sources/Platform/OpenGL/OpenGLShader.h"
      "ORigin/sources/Platform/OpenGL/OpenGLSkybox.cpp"
      "ORigin/sources/Platform/OpenGL/OpenGLSkybox.h"
      "ORigin/sources/Platform/OpenGL/OpenGLTexture.cpp"
      "ORigin/sources/Platform/OpenGL/OpenGLTexture.h"
      "ORigin/sources/Platform/OpenGL/OpenGLUniformBuffer.cpp"
      "ORigin/sources/Platform/OpenGL/OpenGLUniformBuffer.h"
      "ORigin/sources/Platform/OpenGL/OpenGLVertexArray.cpp"
      "ORigin/sources/Platform/OpenGL/OpenGLVertexArray.h"
      "ORigin/sources/Platform/Vulkan/VulkanContext.cpp"
      "ORigin/sources/Platform/Vulkan/VulkanContext.h"
      "ORigin/sources/Platform/Vulkan/VulkanRendererAPI.cpp"
      "ORigin/sources/Platform/Vulkan/VulkanRendererAPI.h"
      "ORigin/sources/Platform/Vulkan/VulkanShader.cpp"
      "ORigin/sources/Platform/Vulkan/VulkanShader.h"
  "ORigin/sources/pch.cpp"
  "ORigin/sources/pch.h"
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_dependencies("ORigin"
    "ASSIMP"
    "BOX2D"
    "GLFW"
    "GLAD"
    "IMGUI"
    "OPTICK"
    "MSDFATLASGEN"
    "MSDFGEN"
    "FreeType"
    "YAMLCPP"
    "JOLT"
  )
  set_target_properties("ORigin" PROPERTIES
    OUTPUT_NAME "ORigin"
    ARCHIVE_OUTPUT_DIRECTORY "/home/evangelion/dev/ORigin-Engine//Binaries/Debug/ORigin"
    LIBRARY_OUTPUT_DIRECTORY "/home/evangelion/dev/ORigin-Engine//Binaries/Debug/ORigin"
    RUNTIME_OUTPUT_DIRECTORY "/home/evangelion/dev/ORigin-Engine//Binaries/Debug/ORigin"
  )
endif()
target_include_directories("ORigin" PRIVATE
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/sources>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Box2D/include>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/spdlog/include>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/stb>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/origin-glfw/include>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/imgui>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/ImGuizmo>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/glad/include>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/glm>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/miniaudio>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/entt/>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Filewatcher/include>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Optick/src/>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/yaml-cpp/include>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Assimp/include>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/JoltPhysics>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/msdf-atlas-gen/msdfgen>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/mono/include>
  $<$<CONFIG:Debug>:/usr/include>
  $<$<CONFIG:Debug>:/usr/include/vulkan>
)
target_compile_definitions("ORigin" PRIVATE
  $<$<CONFIG:Debug>:GLFW_INCLUDE_NONE>
  $<$<CONFIG:Debug>:_CRT_SECURE_NO_WARNINGS>
  $<$<CONFIG:Debug>:OGN_PLATFORM_LINUX>
  $<$<CONFIG:Debug>:VK_VERSION_1_0>
  $<$<CONFIG:Debug>:GLFW_INCLUDE_VULKAN>
  $<$<CONFIG:Debug>:OGN_DEBUG>
  $<$<CONFIG:Debug>:_DEBUG>
)
target_link_directories("ORigin" PRIVATE
  $<$<CONFIG:Debug>:/usr/lib>
)
target_link_libraries("ORigin"
  $<$<CONFIG:Debug>:ASSIMP>
  $<$<CONFIG:Debug>:BOX2D>
  $<$<CONFIG:Debug>:GLFW>
  $<$<CONFIG:Debug>:GLAD>
  $<$<CONFIG:Debug>:IMGUI>
  $<$<CONFIG:Debug>:OPTICK>
  $<$<CONFIG:Debug>:MSDFATLASGEN>
  $<$<CONFIG:Debug>:MSDFGEN>
  $<$<CONFIG:Debug>:FreeType>
  $<$<CONFIG:Debug>:YAMLCPP>
  $<$<CONFIG:Debug>:JOLT>
  $<$<CONFIG:Debug>:/Lib/shaderc_sharedd.lib>
  $<$<CONFIG:Debug>:/Lib/spirv-cross-cored.lib>
  $<$<CONFIG:Debug>:/Lib/spirv-cross-glsld.lib>
  $<$<CONFIG:Debug>:/Lib/SPIRV-Toolsd.lib>
  $<$<CONFIG:Debug>:vulkan>
  $<$<CONFIG:Debug>:shaderc_shared>
  $<$<CONFIG:Debug>:spirv-cross-core>
  $<$<CONFIG:Debug>:spirv-cross-glsl>
  $<$<CONFIG:Debug>:monosgen-2.0>
  $<$<CONFIG:Debug>:pthread>
  $<$<CONFIG:Debug>:dl>
  $<$<CONFIG:Debug>:m>
  $<$<CONFIG:Debug>:rt>
  $<$<CONFIG:Debug>:glib-2.0>
)
target_compile_options("ORigin" PRIVATE
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-fPIC>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-g>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-m64>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-fPIC>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-g>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-std=c++20>
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set_target_properties("ORigin" PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE True
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()
if(CMAKE_BUILD_TYPE STREQUAL Debug)
target_precompile_headers("ORigin" PUBLIC sources/pch.h)
endif()
if(CMAKE_BUILD_TYPE STREQUAL Release)
  add_dependencies("ORigin"
    "ASSIMP"
    "BOX2D"
    "GLFW"
    "GLAD"
    "IMGUI"
    "OPTICK"
    "MSDFATLASGEN"
    "MSDFGEN"
    "FreeType"
    "YAMLCPP"
    "JOLT"
  )
  set_target_properties("ORigin" PROPERTIES
    OUTPUT_NAME "ORigin"
    ARCHIVE_OUTPUT_DIRECTORY "/home/evangelion/dev/ORigin-Engine//Binaries/Release/ORigin"
    LIBRARY_OUTPUT_DIRECTORY "/home/evangelion/dev/ORigin-Engine//Binaries/Release/ORigin"
    RUNTIME_OUTPUT_DIRECTORY "/home/evangelion/dev/ORigin-Engine//Binaries/Release/ORigin"
  )
endif()
target_include_directories("ORigin" PRIVATE
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/sources>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Box2D/include>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/spdlog/include>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/stb>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/origin-glfw/include>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/imgui>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/ImGuizmo>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/glad/include>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/glm>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/miniaudio>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/entt/>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Filewatcher/include>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Optick/src/>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/yaml-cpp/include>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Assimp/include>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/JoltPhysics>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/msdf-atlas-gen/msdfgen>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/mono/include>
  $<$<CONFIG:Release>:/usr/include>
  $<$<CONFIG:Release>:/usr/include/vulkan>
)
target_compile_definitions("ORigin" PRIVATE
  $<$<CONFIG:Release>:GLFW_INCLUDE_NONE>
  $<$<CONFIG:Release>:_CRT_SECURE_NO_WARNINGS>
  $<$<CONFIG:Release>:OGN_PLATFORM_LINUX>
  $<$<CONFIG:Release>:VK_VERSION_1_0>
  $<$<CONFIG:Release>:GLFW_INCLUDE_VULKAN>
  $<$<CONFIG:Release>:OGN_RELEASE>
  $<$<CONFIG:Release>:NDEBUG>
)
target_link_directories("ORigin" PRIVATE
  $<$<CONFIG:Release>:/usr/lib>
)
target_link_libraries("ORigin"
  $<$<CONFIG:Release>:ASSIMP>
  $<$<CONFIG:Release>:BOX2D>
  $<$<CONFIG:Release>:GLFW>
  $<$<CONFIG:Release>:GLAD>
  $<$<CONFIG:Release>:IMGUI>
  $<$<CONFIG:Release>:OPTICK>
  $<$<CONFIG:Release>:MSDFATLASGEN>
  $<$<CONFIG:Release>:MSDFGEN>
  $<$<CONFIG:Release>:FreeType>
  $<$<CONFIG:Release>:YAMLCPP>
  $<$<CONFIG:Release>:JOLT>
  $<$<CONFIG:Release>:/Lib/shaderc_shared.lib>
  $<$<CONFIG:Release>:/Lib/spirv-cross-core.lib>
  $<$<CONFIG:Release>:/Lib/spirv-cross-glsl.lib>
  $<$<CONFIG:Release>:vulkan>
  $<$<CONFIG:Release>:shaderc_shared>
  $<$<CONFIG:Release>:spirv-cross-core>
  $<$<CONFIG:Release>:spirv-cross-glsl>
  $<$<CONFIG:Release>:monosgen-2.0>
  $<$<CONFIG:Release>:pthread>
  $<$<CONFIG:Release>:dl>
  $<$<CONFIG:Release>:m>
  $<$<CONFIG:Release>:rt>
  $<$<CONFIG:Release>:glib-2.0>
)
target_compile_options("ORigin" PRIVATE
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-O2>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-fPIC>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-m64>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-O2>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-fPIC>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-std=c++20>
)
if(CMAKE_BUILD_TYPE STREQUAL Release)
  set_target_properties("ORigin" PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE True
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()
if(CMAKE_BUILD_TYPE STREQUAL Release)
target_precompile_headers("ORigin" PUBLIC sources/pch.h)
endif()
if(CMAKE_BUILD_TYPE STREQUAL Dist)
  add_dependencies("ORigin"
    "ASSIMP"
    "BOX2D"
    "GLFW"
    "GLAD"
    "IMGUI"
    "OPTICK"
    "MSDFATLASGEN"
    "MSDFGEN"
    "FreeType"
    "YAMLCPP"
    "JOLT"
  )
  set_target_properties("ORigin" PROPERTIES
    OUTPUT_NAME "ORigin"
    ARCHIVE_OUTPUT_DIRECTORY "/home/evangelion/dev/ORigin-Engine//Binaries/Dist/ORigin"
    LIBRARY_OUTPUT_DIRECTORY "/home/evangelion/dev/ORigin-Engine//Binaries/Dist/ORigin"
    RUNTIME_OUTPUT_DIRECTORY "/home/evangelion/dev/ORigin-Engine//Binaries/Dist/ORigin"
  )
endif()
target_include_directories("ORigin" PRIVATE
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/sources>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Box2D/include>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/spdlog/include>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/stb>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/origin-glfw/include>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/imgui>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/ImGuizmo>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/glad/include>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/glm>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/miniaudio>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/entt/>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Filewatcher/include>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Optick/src/>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/yaml-cpp/include>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Assimp/include>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/JoltPhysics>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/msdf-atlas-gen/msdfgen>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/mono/include>
  $<$<CONFIG:Dist>:/usr/include>
  $<$<CONFIG:Dist>:/usr/include/vulkan>
)
target_compile_definitions("ORigin" PRIVATE
  $<$<CONFIG:Dist>:GLFW_INCLUDE_NONE>
  $<$<CONFIG:Dist>:_CRT_SECURE_NO_WARNINGS>
  $<$<CONFIG:Dist>:OGN_RELEASE>
  $<$<CONFIG:Dist>:OGN_PLATFORM_LINUX>
  $<$<CONFIG:Dist>:VK_VERSION_1_0>
  $<$<CONFIG:Dist>:GLFW_INCLUDE_VULKAN>
  $<$<CONFIG:Dist>:OGN_DISTRIBUTION>
  $<$<CONFIG:Dist>:NDEBUG>
)
target_link_directories("ORigin" PRIVATE
  $<$<CONFIG:Dist>:/usr/lib>
)
target_link_libraries("ORigin"
  $<$<CONFIG:Dist>:ASSIMP>
  $<$<CONFIG:Dist>:BOX2D>
  $<$<CONFIG:Dist>:GLFW>
  $<$<CONFIG:Dist>:GLAD>
  $<$<CONFIG:Dist>:IMGUI>
  $<$<CONFIG:Dist>:OPTICK>
  $<$<CONFIG:Dist>:MSDFATLASGEN>
  $<$<CONFIG:Dist>:MSDFGEN>
  $<$<CONFIG:Dist>:FreeType>
  $<$<CONFIG:Dist>:YAMLCPP>
  $<$<CONFIG:Dist>:JOLT>
  $<$<CONFIG:Dist>:/Lib/shaderc_shared.lib>
  $<$<CONFIG:Dist>:/Lib/spirv-cross-core.lib>
  $<$<CONFIG:Dist>:/Lib/spirv-cross-glsl.lib>
  $<$<CONFIG:Dist>:vulkan>
  $<$<CONFIG:Dist>:shaderc_shared>
  $<$<CONFIG:Dist>:spirv-cross-core>
  $<$<CONFIG:Dist>:spirv-cross-glsl>
  $<$<CONFIG:Dist>:monosgen-2.0>
  $<$<CONFIG:Dist>:pthread>
  $<$<CONFIG:Dist>:dl>
  $<$<CONFIG:Dist>:m>
  $<$<CONFIG:Dist>:rt>
  $<$<CONFIG:Dist>:glib-2.0>
)
target_compile_options("ORigin" PRIVATE
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:C>>:-O2>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:C>>:-fPIC>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:CXX>>:-m64>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:CXX>>:-O2>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:CXX>>:-fPIC>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:CXX>>:-std=c++20>
)
if(CMAKE_BUILD_TYPE STREQUAL Dist)
  set_target_properties("ORigin" PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE True
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()
if(CMAKE_BUILD_TYPE STREQUAL Dist)
target_precompile_headers("ORigin" PUBLIC sources/pch.h)
endif()