add_library(ORigin
    sources/Origin/Animation/Animation.h
    sources/Origin/Animation/AnimationController.cpp
    sources/Origin/Animation/AnimationController.h
    sources/Origin/Animation/AnimationState.h
    sources/Origin/Animation/Animator.cpp
    sources/Origin/Animation/Animator.h
    sources/Origin/Animation/ModelAnimation.cpp
    sources/Origin/Animation/ModelAnimation.h
    sources/Origin/Animation/SpriteAnimation.cpp
    sources/Origin/Animation/SpriteAnimation.h
    sources/Origin/Asset/Asset.cpp
    sources/Origin/Asset/Asset.h
    sources/Origin/Asset/AssetImporter.cpp
    sources/Origin/Asset/AssetImporter.h
    sources/Origin/Asset/AssetManager.h
    sources/Origin/Asset/AssetManagerBase.h
    sources/Origin/Asset/AssetMetadata.h
    sources/Origin/Asset/EditorAssetManager.cpp
    sources/Origin/Asset/EditorAssetManager.h
    sources/Origin/Audio/AudioEngine.cpp
    sources/Origin/Audio/AudioEngine.h
    sources/Origin/Audio/AudioListener.cpp
    sources/Origin/Audio/AudioListener.h
    sources/Origin/Audio/AudioSource.cpp
    sources/Origin/Audio/AudioSource.h
    sources/Origin/Core/AppEvent.h
    sources/Origin/Core/Application.cpp
    sources/Origin/Core/Application.h
    sources/Origin/Core/Assert.h
    sources/Origin/Core/Base.h
    sources/Origin/Core/Buffer.h
    sources/Origin/Core/CommandManager.h
    sources/Origin/Core/ConsoleManager.cpp
    sources/Origin/Core/ConsoleManager.h
    sources/Origin/Core/Event.h
    sources/Origin/Core/Input.cpp
    sources/Origin/Core/Input.h
    sources/Origin/Core/KeyCodes.h
    sources/Origin/Core/KeyEvent.h
    sources/Origin/Core/Layer.cpp
    sources/Origin/Core/Layer.h
    sources/Origin/Core/LayerStack.cpp
    sources/Origin/Core/LayerStack.h
    sources/Origin/Core/Log.cpp
    sources/Origin/Core/Log.h
    sources/Origin/Core/MouseCodes.h
    sources/Origin/Core/MouseEvent.h
    sources/Origin/Core/Random.cpp
    sources/Origin/Core/Random.h
    sources/Origin/Core/Time.cpp
    sources/Origin/Core/Time.h
    sources/Origin/Core/UUID.cpp
    sources/Origin/Core/UUID.h
    sources/Origin/Core/Window.cpp
    sources/Origin/Core/Window.h
    sources/Origin/EntryPoint.h
    sources/Origin/GUI/GuiLayer.cpp
    sources/Origin/GUI/GuiLayer.h
    sources/Origin/GUI/UI.h
    sources/Origin/Math/AABB.h
    sources/Origin/Math/Math.cpp
    sources/Origin/Math/Math.h
    sources/Origin/Math/OBB.h
    sources/Origin/Physics/2D/Contact2DListener.cpp
    sources/Origin/Physics/2D/Contact2DListener.h
    sources/Origin/Physics/2D/Physics2D.cpp
    sources/Origin/Physics/2D/Physics2D.h
    sources/Origin/Physics/JoltBodyActivationListener.cpp
    sources/Origin/Physics/JoltBodyActivationListener.h
    sources/Origin/Physics/JoltContactListener.cpp
    sources/Origin/Physics/JoltContactListener.h
    sources/Origin/Physics/PhysicsBody.h
    sources/Origin/Physics/PhysicsEngine.cpp
    sources/Origin/Physics/PhysicsEngine.h
    sources/Origin/Profiler/Profiler.h
    sources/Origin/Project/Project.cpp
    sources/Origin/Project/Project.h
    sources/Origin/Renderer/Buffer.cpp
    sources/Origin/Renderer/Buffer.h
    sources/Origin/Renderer/Font.cpp
    sources/Origin/Renderer/Font.h
    sources/Origin/Renderer/Framebuffer.cpp
    sources/Origin/Renderer/Framebuffer.h
    sources/Origin/Renderer/Frustum.cpp
    sources/Origin/Renderer/Frustum.h
    sources/Origin/Renderer/GraphicsContext.cpp
    sources/Origin/Renderer/GraphicsContext.h
    sources/Origin/Renderer/Material.cpp
    sources/Origin/Renderer/Material.h
    sources/Origin/Renderer/MaterialLibrary.cpp
    sources/Origin/Renderer/MaterialLibrary.h
    sources/Origin/Renderer/MeshRenderer.cpp
    sources/Origin/Renderer/MeshRenderer.h
    sources/Origin/Renderer/MeshVertexData.h
    sources/Origin/Renderer/ModelLoader.cpp
    sources/Origin/Renderer/ModelLoader.h
    sources/Origin/Renderer/ParticleSystem.cpp
    sources/Origin/Renderer/ParticleSystem.h
    sources/Origin/Renderer/RenderCommand.h
    sources/Origin/Renderer/Renderer.cpp
    sources/Origin/Renderer/Renderer.h
    sources/Origin/Renderer/Renderer2D.cpp
    sources/Origin/Renderer/Renderer2D.h
    sources/Origin/Renderer/RendererAPI.cpp
    sources/Origin/Renderer/RendererAPI.h
    sources/Origin/Renderer/ScreenRenderer.cpp
    sources/Origin/Renderer/ScreenRenderer.h
    sources/Origin/Renderer/Shader.cpp
    sources/Origin/Renderer/Shader.h
    sources/Origin/Renderer/ShaderLibrary.cpp
    sources/Origin/Renderer/ShaderLibrary.h
    sources/Origin/Renderer/SubTexture2D.cpp
    sources/Origin/Renderer/SubTexture2D.h
    sources/Origin/Renderer/Texture.cpp
    sources/Origin/Renderer/Texture.h
    sources/Origin/Renderer/UniformBuffer.cpp
    sources/Origin/Renderer/UniformBuffer.h
    sources/Origin/Renderer/VertexArray.cpp
    sources/Origin/Renderer/VertexArray.h
    sources/Origin/Scene/Camera.h
    sources/Origin/Scene/Components/Components.h
    sources/Origin/Scene/Components/ComponentsCommand.h
    sources/Origin/Scene/Components/PhysicsComponent.cpp
    sources/Origin/Scene/Components/PhysicsComponents.h
    sources/Origin/Scene/EditorCamera.cpp
    sources/Origin/Scene/EditorCamera.h
    sources/Origin/Scene/Entity.cpp
    sources/Origin/Scene/Entity.h
    sources/Origin/Scene/EntityManager.cpp
    sources/Origin/Scene/EntityManager.h
    sources/Origin/Scene/Lighting.cpp
    sources/Origin/Scene/Lighting.h
    sources/Origin/Scene/Scene.cpp
    sources/Origin/Scene/Scene.h
    sources/Origin/Scene/SceneCamera.cpp
    sources/Origin/Scene/SceneCamera.h
    sources/Origin/Scene/SceneCommand.h
    sources/Origin/Scene/ScriptableEntity.h
    sources/Origin/Scene/ShadowRenderer.cpp
    sources/Origin/Scene/ShadowRenderer.h
    sources/Origin/Scene/Skybox.cpp
    sources/Origin/Scene/Skybox.h
    sources/Origin/Scene/SpriteSheet.cpp
    sources/Origin/Scene/SpriteSheet.h
    sources/Origin/Scene/UIRenderer.cpp
    sources/Origin/Scene/UIRenderer.h
    sources/Origin/Scripting/ScriptClass.cpp
    sources/Origin/Scripting/ScriptClass.h
    sources/Origin/Scripting/ScriptEngine.cpp
    sources/Origin/Scripting/ScriptEngine.h
    sources/Origin/Scripting/ScriptField.h
    sources/Origin/Scripting/ScriptGlue.cpp
    sources/Origin/Scripting/ScriptGlue.h
    sources/Origin/Scripting/ScriptInstance.cpp
    sources/Origin/Scripting/ScriptInstance.h
    sources/Origin/Serializer/MaterialSerializer.cpp
    sources/Origin/Serializer/MaterialSerializer.h
    sources/Origin/Serializer/ProjectSerializer.cpp
    sources/Origin/Serializer/ProjectSerializer.h
    sources/Origin/Serializer/SceneSerializer.cpp
    sources/Origin/Serializer/SceneSerializer.h
    sources/Origin/Serializer/Serializer.h
    sources/Origin/Serializer/SpriteSheetSerializer.cpp
    sources/Origin/Serializer/SpriteSheetSerializer.h
    sources/Origin/Utils/PlatformUtils.h
    sources/Origin/Utils/StringUtils.cpp
    sources/Origin/Utils/StringUtils.h
    sources/Origin/Utils/Utils.h
    sources/Origin.h
    sources/Platform/OpenGL/OpenGLBuffer.cpp
    sources/Platform/OpenGL/OpenGLBuffer.h
    sources/Platform/OpenGL/OpenGLContext.cpp
    sources/Platform/OpenGL/OpenGLContext.h
    sources/Platform/OpenGL/OpenGLFramebuffer.cpp
    sources/Platform/OpenGL/OpenGLFramebuffer.h
    sources/Platform/OpenGL/OpenGLRendererAPI.cpp
    sources/Platform/OpenGL/OpenGLRendererAPI.h
    sources/Platform/OpenGL/OpenGLShader.cpp
    sources/Platform/OpenGL/OpenGLShader.h
    sources/Platform/OpenGL/OpenGLSkybox.cpp
    sources/Platform/OpenGL/OpenGLSkybox.h
    sources/Platform/OpenGL/OpenGLTexture.cpp
    sources/Platform/OpenGL/OpenGLTexture.h
    sources/Platform/OpenGL/OpenGLUniformBuffer.cpp
    sources/Platform/OpenGL/OpenGLUniformBuffer.h
    sources/Platform/OpenGL/OpenGLVertexArray.cpp
    sources/Platform/OpenGL/OpenGLVertexArray.h
    sources/Platform/Vulkan/VulkanCommandBuffer.cpp
    sources/Platform/Vulkan/VulkanCommandBuffer.h
    sources/Platform/Vulkan/VulkanContext.cpp
    sources/Platform/Vulkan/VulkanContext.h
    sources/Platform/Vulkan/VulkanRendererAPI.cpp
    sources/Platform/Vulkan/VulkanRendererAPI.h
    sources/Platform/Vulkan/VulkanShader.cpp
    sources/Platform/Vulkan/VulkanShader.h
    sources/Origin/Core/LogLevel.h
    sources/Origin/Core/Types.h
    sources/pch.h
)

target_include_directories(ORigin PRIVATE
    sources
    ${THIRD_PARTY_DIR}/Box2D/include
    ${THIRD_PARTY_DIR}/FMT/include
    ${THIRD_PARTY_DIR}/stb
    ${THIRD_PARTY_DIR}/origin-glfw/include
    ${THIRD_PARTY_DIR}/imgui
    ${THIRD_PARTY_DIR}/ImGuizmo
    ${THIRD_PARTY_DIR}/glad/include
    ${THIRD_PARTY_DIR}/glm
    ${THIRD_PARTY_DIR}/miniaudio
    ${THIRD_PARTY_DIR}/entt
    ${THIRD_PARTY_DIR}/Filewatcher/include
    ${THIRD_PARTY_DIR}/yaml-cpp/include
    ${THIRD_PARTY_DIR}/Assimp/include
    ${THIRD_PARTY_DIR}/JoltPhysics
    ${THIRD_PARTY_DIR}/msdf-atlas-gen/msdfgen
    ${THIRD_PARTY_DIR}/msdf-atlas-gen/msdf-atlas-gen
)

add_dependencies(ORigin
    ASSIMP BOX2D GLFW GLAD
    IMGUI MSDFATLASGEN MSDFGEN FREETYPE
    YAMLCPP JOLT fmt
)

target_link_libraries(ORigin
    ASSIMP BOX2D GLFW GLAD
    IMGUI MSDFATLASGEN MSDFGEN FREETYPE
    YAMLCPP JOLT fmt
)

if (WIN32)
    target_sources(ORigin PRIVATE
        sources/Platform/DX11/DX11Context.cpp
        sources/Platform/DX11/DX11Context.h
        sources/Platform/DX11/DX11RendererAPI.cpp
        sources/Platform/DX11/DX11RendererAPI.h
        sources/Platform/Win32/Win32Utils.cpp
        sources/Platform/Win32/Win32Window.cpp
        sources/Platform/Win32/Win32Window.h
    )

    target_include_directories(ORigin PRIVATE
        ${THIRD_PARTY_DIR}/mono/include
        ${VULKAN_INCLUDE_DIR}
    )

    target_compile_definitions(ORigin PRIVATE
        NOMINMAX
        OGN_PLATFORM_WINDOWS
        GLFW_EXPOSE_NATIVE_WIN32
        _CRT_SECURE_NO_WARNINGS
        _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
        _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
    )

    target_link_libraries(ORigin
        opengl32.lib
        Ws2_32.lib
        Winmm.lib
        Version.lib
        Bcrypt.lib
        ${THIRD_PARTY_DIR}/mono/win32/libmono-static-sgen.lib
    )

    # Vulkan
    if (CMAKE_BUILD_TYPE STREQUAL Debug)
        target_link_libraries(ORigin
            ${VULKAN_LIBRARY_DIR}/shaderc_sharedd.lib
            ${VULKAN_LIBRARY_DIR}/spirv-cross-cored.lib
            ${VULKAN_LIBRARY_DIR}/spirv-cross-glsld.lib
        )
    else()
        target_link_libraries(ORigin
            ${VULKAN_LIBRARY_DIR}/shaderc_shared.lib
            ${VULKAN_LIBRARY_DIR}/spirv-cross-core.lib
            ${VULKAN_LIBRARY_DIR}/spirv-cross-glsl.lib
        )
    endif()
elseif (UNIX AND NOT APPLE)
        target_sources(ORigin PRIVATE
            sources/Platform/Linux/LinuxUtils.cpp
            sources/Platform/Linux/LinuxWindow.cpp
            sources/Platform/Linux/LinuxWindow.h
        )

        target_include_directories(ORigin PRIVATE
            ${THIRD_PARTY_DIR}/mono/include
            /usr/include
        )
        target_link_libraries(ORigin
            vulkan
            shaderc_shared
            spirv-cross-core
            spirv-cross-glsl
            monosgen-2.0
            pthread dl m rt
        )

        target_compile_definitions(ORigin PRIVATE
            OGN_PLATFORM_LINUX
        )
        target_link_directories(ORigin PRIVATE usr/lib)
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set_target_properties(ORigin PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries
    )
    target_compile_definitions(ORigin PRIVATE
        OGN_DEBUG
        _DEBUG
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    set_target_properties(ORigin PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries
    )
    target_compile_definitions(ORigin PRIVATE
        OGN_RELEASE
        _NDEBUG
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Dist)
    set_target_properties(ORigin PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries
    )
    target_compile_definitions(ORigin PRIVATE
        OGN_RELEASE
        OGN_DIST
        _NDEBUG
    )
endif()