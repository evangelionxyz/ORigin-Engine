add_executable("Runtime"
  "ORigin-Runtime/sources/RuntimeApp.cpp"
  "ORigin-Runtime/sources/RuntimeLayer.cpp"
  "ORigin-Runtime/sources/RuntimeLayer.h"
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_dependencies("Runtime"
    "ORigin"
    "ASSIMP"
    "BOX2D"
    "GLFW"
    "GLAD"
    "IMGUI"
    "MSDFATLASGEN"
    "MSDFGEN"
    "FreeType"
    "YAMLCPP"
    "JOLT"
  )
  set_target_properties("Runtime" PROPERTIES
    OUTPUT_NAME "Runtime"
    ARCHIVE_OUTPUT_DIRECTORY "D:/Dev/ORigin-Engine//Binaries/Debug/ORigin"
    LIBRARY_OUTPUT_DIRECTORY "D:/Dev/ORigin-Engine//Binaries/Debug/ORigin"
    RUNTIME_OUTPUT_DIRECTORY "D:/Dev/ORigin-Engine//Binaries/Debug/ORigin"
  )
endif()
target_include_directories("Runtime" PRIVATE
  $<$<CONFIG:Debug>:D:/Dev/ORigin-Engine//sources>
  $<$<CONFIG:Debug>:D:/Dev/ORigin-Engine//ORigin/sources>
  $<$<CONFIG:Debug>:D:/Dev/ORigin-Engine//ORigin/vendor/spdlog/include>
  $<$<CONFIG:Debug>:D:/Dev/ORigin-Engine//ORigin/vendor/Assimp/include>
  $<$<CONFIG:Debug>:D:/Dev/ORigin-Engine//ORigin/vendor/origin-glfw/include>
  $<$<CONFIG:Debug>:D:/Dev/ORigin-Engine//ORigin/vendor/glad/include>
  $<$<CONFIG:Debug>:D:/Dev/ORigin-Engine//ORigin/vendor/Box2D/include>
  $<$<CONFIG:Debug>:D:/Dev/ORigin-Engine//ORigin/vendor/entt>
  $<$<CONFIG:Debug>:D:/Dev/ORigin-Engine//ORigin/vendor/miniaudio>
  $<$<CONFIG:Debug>:D:/Dev/ORigin-Engine//ORigin/vendor/imgui>
  $<$<CONFIG:Debug>:D:/Dev/ORigin-Engine//ORigin/vendor/ImGuizmo>
  $<$<CONFIG:Debug>:D:/Dev/ORigin-Engine//ORigin/vendor/glm>
  $<$<CONFIG:Debug>:D:/Dev/ORigin-Engine//ORigin/vendor/yaml-cpp/include>
  $<$<CONFIG:Debug>:D:/Dev/ORigin-Engine//ORigin/vendor/JoltPhysics>
  $<$<CONFIG:Debug>:D:/Dev/ORigin-Engine//ORigin/vendor/msdf-atlas-gen/msdfgen>
  $<$<CONFIG:Debug>:D:/Dev/ORigin-Engine//ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen>
  $<$<CONFIG:Debug>:C:\VulkanSDK\1.3.290.0/Include/>
)
target_compile_definitions("Runtime" PRIVATE
  $<$<CONFIG:Debug>:_CRT_SECURE_NO_WARNINGS>
  $<$<CONFIG:Debug>:NV_USE_STATIC_WINCRT>
  $<$<CONFIG:Debug>:_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS>
  $<$<CONFIG:Debug>:GLFW_INCLUDE_NONE>
  $<$<CONFIG:Debug>:OGN_DEBUG>
  $<$<CONFIG:Debug>:_DEBUG>
)
target_link_directories("Runtime" PRIVATE
)
target_link_libraries("Runtime"
  $<$<CONFIG:Debug>:ORigin>
  $<$<CONFIG:Debug>:ASSIMP>
  $<$<CONFIG:Debug>:BOX2D>
  $<$<CONFIG:Debug>:GLFW>
  $<$<CONFIG:Debug>:GLAD>
  $<$<CONFIG:Debug>:IMGUI>
  $<$<CONFIG:Debug>:MSDFATLASGEN>
  $<$<CONFIG:Debug>:MSDFGEN>
  $<$<CONFIG:Debug>:FreeType>
  $<$<CONFIG:Debug>:YAMLCPP>
  $<$<CONFIG:Debug>:JOLT>
)
target_compile_options("Runtime" PRIVATE
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-g>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-m64>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-g>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-std=c++20>
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set_target_properties("Runtime" PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE False
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()
if(CMAKE_BUILD_TYPE STREQUAL Release)
  add_dependencies("Runtime"
    "ORigin"
    "ASSIMP"
    "BOX2D"
    "GLFW"
    "GLAD"
    "IMGUI"
    "MSDFATLASGEN"
    "MSDFGEN"
    "FreeType"
    "YAMLCPP"
    "JOLT"
  )
  set_target_properties("Runtime" PROPERTIES
    OUTPUT_NAME "Runtime"
    ARCHIVE_OUTPUT_DIRECTORY "D:/Dev/ORigin-Engine//Binaries/Release/ORigin"
    LIBRARY_OUTPUT_DIRECTORY "D:/Dev/ORigin-Engine//Binaries/Release/ORigin"
    RUNTIME_OUTPUT_DIRECTORY "D:/Dev/ORigin-Engine//Binaries/Release/ORigin"
  )
endif()
target_include_directories("Runtime" PRIVATE
  $<$<CONFIG:Release>:D:/Dev/ORigin-Engine//sources>
  $<$<CONFIG:Release>:D:/Dev/ORigin-Engine//ORigin/sources>
  $<$<CONFIG:Release>:D:/Dev/ORigin-Engine//ORigin/vendor/spdlog/include>
  $<$<CONFIG:Release>:D:/Dev/ORigin-Engine//ORigin/vendor/Assimp/include>
  $<$<CONFIG:Release>:D:/Dev/ORigin-Engine//ORigin/vendor/origin-glfw/include>
  $<$<CONFIG:Release>:D:/Dev/ORigin-Engine//ORigin/vendor/glad/include>
  $<$<CONFIG:Release>:D:/Dev/ORigin-Engine//ORigin/vendor/Box2D/include>
  $<$<CONFIG:Release>:D:/Dev/ORigin-Engine//ORigin/vendor/entt>
  $<$<CONFIG:Release>:D:/Dev/ORigin-Engine//ORigin/vendor/miniaudio>
  $<$<CONFIG:Release>:D:/Dev/ORigin-Engine//ORigin/vendor/imgui>
  $<$<CONFIG:Release>:D:/Dev/ORigin-Engine//ORigin/vendor/ImGuizmo>
  $<$<CONFIG:Release>:D:/Dev/ORigin-Engine//ORigin/vendor/glm>
  $<$<CONFIG:Release>:D:/Dev/ORigin-Engine//ORigin/vendor/yaml-cpp/include>
  $<$<CONFIG:Release>:D:/Dev/ORigin-Engine//ORigin/vendor/JoltPhysics>
  $<$<CONFIG:Release>:D:/Dev/ORigin-Engine//ORigin/vendor/msdf-atlas-gen/msdfgen>
  $<$<CONFIG:Release>:D:/Dev/ORigin-Engine//ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen>
  $<$<CONFIG:Release>:C:\VulkanSDK\1.3.290.0/Include/>
)
target_compile_definitions("Runtime" PRIVATE
  $<$<CONFIG:Release>:_CRT_SECURE_NO_WARNINGS>
  $<$<CONFIG:Release>:NV_USE_STATIC_WINCRT>
  $<$<CONFIG:Release>:_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS>
  $<$<CONFIG:Release>:GLFW_INCLUDE_NONE>
  $<$<CONFIG:Release>:OGN_RELEASE>
  $<$<CONFIG:Release>:NDEBUG>
)
target_link_directories("Runtime" PRIVATE
)
target_link_libraries("Runtime"
  $<$<CONFIG:Release>:ORigin>
  $<$<CONFIG:Release>:ASSIMP>
  $<$<CONFIG:Release>:BOX2D>
  $<$<CONFIG:Release>:GLFW>
  $<$<CONFIG:Release>:GLAD>
  $<$<CONFIG:Release>:IMGUI>
  $<$<CONFIG:Release>:MSDFATLASGEN>
  $<$<CONFIG:Release>:MSDFGEN>
  $<$<CONFIG:Release>:FreeType>
  $<$<CONFIG:Release>:YAMLCPP>
  $<$<CONFIG:Release>:JOLT>
)
target_compile_options("Runtime" PRIVATE
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-O2>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-m64>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-O2>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-std=c++20>
)
if(CMAKE_BUILD_TYPE STREQUAL Release)
  set_target_properties("Runtime" PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE False
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()
if(CMAKE_BUILD_TYPE STREQUAL Dist)
  add_dependencies("Runtime"
    "ORigin"
    "ASSIMP"
    "BOX2D"
    "GLFW"
    "GLAD"
    "IMGUI"
    "MSDFATLASGEN"
    "MSDFGEN"
    "FreeType"
    "YAMLCPP"
    "JOLT"
  )
  set_target_properties("Runtime" PROPERTIES
    OUTPUT_NAME "Runtime"
    ARCHIVE_OUTPUT_DIRECTORY "D:/Dev/ORigin-Engine//Binaries/Dist/ORigin"
    LIBRARY_OUTPUT_DIRECTORY "D:/Dev/ORigin-Engine//Binaries/Dist/ORigin"
    RUNTIME_OUTPUT_DIRECTORY "D:/Dev/ORigin-Engine//Binaries/Dist/ORigin"
  )
endif()
target_include_directories("Runtime" PRIVATE
  $<$<CONFIG:Dist>:D:/Dev/ORigin-Engine//sources>
  $<$<CONFIG:Dist>:D:/Dev/ORigin-Engine//ORigin/sources>
  $<$<CONFIG:Dist>:D:/Dev/ORigin-Engine//ORigin/vendor/spdlog/include>
  $<$<CONFIG:Dist>:D:/Dev/ORigin-Engine//ORigin/vendor/Assimp/include>
  $<$<CONFIG:Dist>:D:/Dev/ORigin-Engine//ORigin/vendor/origin-glfw/include>
  $<$<CONFIG:Dist>:D:/Dev/ORigin-Engine//ORigin/vendor/glad/include>
  $<$<CONFIG:Dist>:D:/Dev/ORigin-Engine//ORigin/vendor/Box2D/include>
  $<$<CONFIG:Dist>:D:/Dev/ORigin-Engine//ORigin/vendor/entt>
  $<$<CONFIG:Dist>:D:/Dev/ORigin-Engine//ORigin/vendor/miniaudio>
  $<$<CONFIG:Dist>:D:/Dev/ORigin-Engine//ORigin/vendor/imgui>
  $<$<CONFIG:Dist>:D:/Dev/ORigin-Engine//ORigin/vendor/ImGuizmo>
  $<$<CONFIG:Dist>:D:/Dev/ORigin-Engine//ORigin/vendor/glm>
  $<$<CONFIG:Dist>:D:/Dev/ORigin-Engine//ORigin/vendor/yaml-cpp/include>
  $<$<CONFIG:Dist>:D:/Dev/ORigin-Engine//ORigin/vendor/JoltPhysics>
  $<$<CONFIG:Dist>:D:/Dev/ORigin-Engine//ORigin/vendor/msdf-atlas-gen/msdfgen>
  $<$<CONFIG:Dist>:D:/Dev/ORigin-Engine//ORigin/vendor/msdf-atlas-gen/msdf-atlas-gen>
  $<$<CONFIG:Dist>:C:\VulkanSDK\1.3.290.0/Include/>
)
target_compile_definitions("Runtime" PRIVATE
  $<$<CONFIG:Dist>:_CRT_SECURE_NO_WARNINGS>
  $<$<CONFIG:Dist>:NV_USE_STATIC_WINCRT>
  $<$<CONFIG:Dist>:_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS>
  $<$<CONFIG:Dist>:GLFW_INCLUDE_NONE>
  $<$<CONFIG:Dist>:OGN_DISTRIBUTION>
  $<$<CONFIG:Dist>:NDEBUG>
)
target_link_directories("Runtime" PRIVATE
)
target_link_libraries("Runtime"
  $<$<CONFIG:Dist>:ORigin>
  $<$<CONFIG:Dist>:ASSIMP>
  $<$<CONFIG:Dist>:BOX2D>
  $<$<CONFIG:Dist>:GLFW>
  $<$<CONFIG:Dist>:GLAD>
  $<$<CONFIG:Dist>:IMGUI>
  $<$<CONFIG:Dist>:MSDFATLASGEN>
  $<$<CONFIG:Dist>:MSDFGEN>
  $<$<CONFIG:Dist>:FreeType>
  $<$<CONFIG:Dist>:YAMLCPP>
  $<$<CONFIG:Dist>:JOLT>
)
target_compile_options("Runtime" PRIVATE
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:C>>:-O2>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:CXX>>:-m64>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:CXX>>:-O2>
  $<$<AND:$<CONFIG:Dist>,$<COMPILE_LANGUAGE:CXX>>:-std=c++20>
)
if(CMAKE_BUILD_TYPE STREQUAL Dist)
  set_target_properties("Runtime" PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE False
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()