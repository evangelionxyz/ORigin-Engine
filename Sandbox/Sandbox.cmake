add_executable("Sandbox"
  "Sandbox/sources/SandboxApp.cpp"
  "Sandbox/sources/SandboxLayer.cpp"
  "Sandbox/sources/SandboxLayer.h"
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_dependencies("Sandbox"
    "ORigin"
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
  set_target_properties("Sandbox" PROPERTIES
    OUTPUT_NAME "Sandbox"
    ARCHIVE_OUTPUT_DIRECTORY "/home/evangelion/dev/ORigin-Engine//Binaries/Debug/Sandbox"
    LIBRARY_OUTPUT_DIRECTORY "/home/evangelion/dev/ORigin-Engine//Binaries/Debug/Sandbox"
    RUNTIME_OUTPUT_DIRECTORY "/home/evangelion/dev/ORigin-Engine//Binaries/Debug/Sandbox"
  )
endif()
target_include_directories("Sandbox" PRIVATE
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//sources>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/sources>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/spdlog/include>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Assimp/include>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/origin-glfw/include>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/glad/include>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Box2D/include>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/entt/>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/miniaudio>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/imgui>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/ImGuizmo>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/glm>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/yaml-cpp/include>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Optick/src/>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/JoltPhysics>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/msdf-atlas-gen/msdfgen>
  $<$<CONFIG:Debug>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen>
  $<$<CONFIG:Debug>:/usr/include>
  $<$<CONFIG:Debug>:/usr/include/vulkan>
)
target_compile_definitions("Sandbox" PRIVATE
  $<$<CONFIG:Debug>:GLFW_INCLUDE_NONE>
  $<$<CONFIG:Debug>:_CRT_SECURE_NO_WARNINGS>
  $<$<CONFIG:Debug>:_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING>
  $<$<CONFIG:Debug>:OGN_DEBUG>
  $<$<CONFIG:Debug>:_DEBUG>
)
target_link_directories("Sandbox" PRIVATE
  $<$<CONFIG:Debug>:/usr/lib>
)
target_link_libraries("Sandbox"
  $<$<CONFIG:Debug>:ORigin>
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
  $<$<CONFIG:Debug>:vulkan>
  $<$<CONFIG:Debug>:shaderc_shared>
  $<$<CONFIG:Debug>:spirv-cross-core>
  $<$<CONFIG:Debug>:spirv-cross-glsl>
  $<$<CONFIG:Debug>:monosgen-2.0>
  $<$<CONFIG:Debug>:pthread>
  $<$<CONFIG:Debug>:dl>
  $<$<CONFIG:Debug>:m>
  $<$<CONFIG:Debug>:rt>
)
target_compile_options("Sandbox" PRIVATE
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-fPIC>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-g>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-m64>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-fPIC>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-g>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-std=c++20>
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set_target_properties("Sandbox" PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE True
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()
if(CMAKE_BUILD_TYPE STREQUAL Release)
  add_dependencies("Sandbox"
    "ORigin"
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
  set_target_properties("Sandbox" PROPERTIES
    OUTPUT_NAME "Sandbox"
    ARCHIVE_OUTPUT_DIRECTORY "/home/evangelion/dev/ORigin-Engine//Binaries/Release/Sandbox"
    LIBRARY_OUTPUT_DIRECTORY "/home/evangelion/dev/ORigin-Engine//Binaries/Release/Sandbox"
    RUNTIME_OUTPUT_DIRECTORY "/home/evangelion/dev/ORigin-Engine//Binaries/Release/Sandbox"
  )
endif()
target_include_directories("Sandbox" PRIVATE
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//sources>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/sources>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/spdlog/include>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Assimp/include>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/origin-glfw/include>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/glad/include>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Box2D/include>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/entt/>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/miniaudio>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/imgui>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/ImGuizmo>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/glm>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/yaml-cpp/include>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Optick/src/>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/JoltPhysics>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/msdf-atlas-gen/msdfgen>
  $<$<CONFIG:Release>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen>
  $<$<CONFIG:Release>:/usr/include>
  $<$<CONFIG:Release>:/usr/include/vulkan>
)
target_compile_definitions("Sandbox" PRIVATE
  $<$<CONFIG:Release>:GLFW_INCLUDE_NONE>
  $<$<CONFIG:Release>:_CRT_SECURE_NO_WARNINGS>
  $<$<CONFIG:Release>:_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING>
  $<$<CONFIG:Release>:OGN_RELEASE>
  $<$<CONFIG:Release>:NDEBUG>
)
target_link_directories("Sandbox" PRIVATE
  $<$<CONFIG:Release>:/usr/lib>
)
target_link_libraries("Sandbox"
  $<$<CONFIG:Release>:ORigin>
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
  $<$<CONFIG:Release>:vulkan>
  $<$<CONFIG:Release>:shaderc_shared>
  $<$<CONFIG:Release>:spirv-cross-core>
  $<$<CONFIG:Release>:spirv-cross-glsl>
  $<$<CONFIG:Release>:monosgen-2.0>
  $<$<CONFIG:Release>:pthread>
  $<$<CONFIG:Release>:dl>
  $<$<CONFIG:Release>:m>
  $<$<CONFIG:Release>:rt>
)
target_compile_options("Sandbox" PRIVATE
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-O2>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-fPIC>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-m64>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-O2>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-fPIC>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-std=c++20>
)
if(CMAKE_BUILD_TYPE STREQUAL Release)
  set_target_properties("Sandbox" PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE True
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()
if(CMAKE_BUILD_TYPE STREQUAL Dist)
  add_dependencies("Sandbox"
    "ORigin"
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
  set_target_properties("Sandbox" PROPERTIES
    OUTPUT_NAME "Sandbox"
    ARCHIVE_OUTPUT_DIRECTORY "/home/evangelion/dev/ORigin-Engine//Binaries/Dist/Sandbox"
    LIBRARY_OUTPUT_DIRECTORY "/home/evangelion/dev/ORigin-Engine//Binaries/Dist/Sandbox"
    RUNTIME_OUTPUT_DIRECTORY "/home/evangelion/dev/ORigin-Engine//Binaries/Dist/Sandbox"
  )
endif()
target_include_directories("Sandbox" PRIVATE
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//sources>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/sources>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/spdlog/include>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Assimp/include>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/origin-glfw/include>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/glad/include>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Box2D/include>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/entt/>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/miniaudio>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/imgui>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/ImGuizmo>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/glm>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/yaml-cpp/include>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/Optick/src/>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/JoltPhysics>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/msdf-atlas-gen/msdfgen>
  $<$<CONFIG:Dist>:/home/evangelion/dev/ORigin-Engine//ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen>
  $<$<CONFIG:Dist>:/usr/include>
  $<$<CONFIG:Dist>:/usr/include/vulkan>
)
target_compile_definitions("Sandbox" PRIVATE
  $<$<CONFIG:Dist>:GLFW_INCLUDE_NONE>
  $<$<CONFIG:Dist>:_CRT_SECURE_NO_WARNINGS>
  $<$<CONFIG:Dist>:_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING>
  $<$<CONFIG:Dist>:OGN_DISTRIBUTION>
  $<$<CONFIG:Dist>:NDEBUG>
)
target_link_directories("Sandbox" PRIVATE
  $<$<CONFIG:Dist>:/usr/lib>
)
target_link_libraries("Sandbox"
  $<$<CONFIG:Dist>:ORigin>
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
  $<$<CONFIG:Dist>:vulkan>
  $<$<CONFIG:Dist>:shaderc_shared>
  $<$<CONFIG:Dist>:spirv-cross-core>
  $<$<CONFIG:Dist>:spirv-cross-glsl>
  $<$<CONFIG:Dist>:monosgen-2.0>
  $<$<CONFIG:Dist>:pthread>
  $<$<CONFIG:Dist>:dl>
  $<$<CONFIG:Dist>:m>
  $<$<CONFIG:Dist>:rt>
)
target_compile_options("Sandbox" PRIVATE
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:C>>:-O2>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:C>>:-fPIC>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:CXX>>:-m64>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:CXX>>:-O2>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:CXX>>:-fPIC>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:CXX>>:-std=c++20>
)
if(CMAKE_BUILD_TYPE STREQUAL Dist)
  set_target_properties("Sandbox" PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE True
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()