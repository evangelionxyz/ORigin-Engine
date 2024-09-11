add_library(BOX2D STATIC
    ${THIRD_PARTY_DIR}/BOX2D/src/aabb.c
    ${THIRD_PARTY_DIR}/BOX2D/src/aabb.h
    ${THIRD_PARTY_DIR}/BOX2D/src/allocate.c
    ${THIRD_PARTY_DIR}/BOX2D/src/allocate.h
    ${THIRD_PARTY_DIR}/BOX2D/src/array.c
    ${THIRD_PARTY_DIR}/BOX2D/src/array.h
    ${THIRD_PARTY_DIR}/BOX2D/src/bitset.c
    ${THIRD_PARTY_DIR}/BOX2D/src/bitset.h
    ${THIRD_PARTY_DIR}/BOX2D/src/block_array.c
    ${THIRD_PARTY_DIR}/BOX2D/src/block_array.h
    ${THIRD_PARTY_DIR}/BOX2D/src/body.c
    ${THIRD_PARTY_DIR}/BOX2D/src/body.h
    ${THIRD_PARTY_DIR}/BOX2D/src/broad_phase.c
    ${THIRD_PARTY_DIR}/BOX2D/src/broad_phase.h
    ${THIRD_PARTY_DIR}/BOX2D/src/constraint_graph.c
    ${THIRD_PARTY_DIR}/BOX2D/src/constraint_graph.h
    ${THIRD_PARTY_DIR}/BOX2D/src/contact.c
    ${THIRD_PARTY_DIR}/BOX2D/src/contact.h
    ${THIRD_PARTY_DIR}/BOX2D/src/contact_solver.c
    ${THIRD_PARTY_DIR}/BOX2D/src/contact_solver.h
    ${THIRD_PARTY_DIR}/BOX2D/src/core.c
    ${THIRD_PARTY_DIR}/BOX2D/src/core.h
    ${THIRD_PARTY_DIR}/BOX2D/src/ctz.h
    ${THIRD_PARTY_DIR}/BOX2D/src/distance.c
    ${THIRD_PARTY_DIR}/BOX2D/src/distance_joint.c
    ${THIRD_PARTY_DIR}/BOX2D/src/dynamic_tree.c
    ${THIRD_PARTY_DIR}/BOX2D/src/geometry.c
    ${THIRD_PARTY_DIR}/BOX2D/src/hull.c
    ${THIRD_PARTY_DIR}/BOX2D/src/id_pool.c
    ${THIRD_PARTY_DIR}/BOX2D/src/id_pool.h
    ${THIRD_PARTY_DIR}/BOX2D/src/island.c
    ${THIRD_PARTY_DIR}/BOX2D/src/island.h
    ${THIRD_PARTY_DIR}/BOX2D/src/joint.c
    ${THIRD_PARTY_DIR}/BOX2D/src/joint.h
    ${THIRD_PARTY_DIR}/BOX2D/src/manifold.c
    ${THIRD_PARTY_DIR}/BOX2D/src/math_functions.c
    ${THIRD_PARTY_DIR}/BOX2D/src/motor_joint.c
    ${THIRD_PARTY_DIR}/BOX2D/src/mouse_joint.c
    ${THIRD_PARTY_DIR}/BOX2D/src/prismatic_joint.c
    ${THIRD_PARTY_DIR}/BOX2D/src/revolute_joint.c
    ${THIRD_PARTY_DIR}/BOX2D/src/shape.c
    ${THIRD_PARTY_DIR}/BOX2D/src/shape.h
    ${THIRD_PARTY_DIR}/BOX2D/src/solver.c
    ${THIRD_PARTY_DIR}/BOX2D/src/solver.h
    ${THIRD_PARTY_DIR}/BOX2D/src/solver_set.c
    ${THIRD_PARTY_DIR}/BOX2D/src/solver_set.h
    ${THIRD_PARTY_DIR}/BOX2D/src/stack_allocator.c
    ${THIRD_PARTY_DIR}/BOX2D/src/stack_allocator.h
    ${THIRD_PARTY_DIR}/BOX2D/src/table.c
    ${THIRD_PARTY_DIR}/BOX2D/src/table.h
    ${THIRD_PARTY_DIR}/BOX2D/src/timer.c
    ${THIRD_PARTY_DIR}/BOX2D/src/types.c
    ${THIRD_PARTY_DIR}/BOX2D/src/weld_joint.c
    ${THIRD_PARTY_DIR}/BOX2D/src/wheel_joint.c
    ${THIRD_PARTY_DIR}/BOX2D/src/world.c
    ${THIRD_PARTY_DIR}/BOX2D/src/world.h
)

target_include_directories(BOX2D PRIVATE
  ${THIRD_PARTY_DIR}/BOX2D/include
  ${THIRD_PARTY_DIR}/BOX2D/src
)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

set(CMAKE_DEBUG_POSTFIX "d")

# Box2D uses C17
set_target_properties(BOX2D PROPERTIES
    C_STANDARD 17
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS YES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

target_compile_definitions(BOX2D PRIVATE BOX2D_ENABLE_SIMD)

if (BOX2D_VALIDATE)
    target_compile_definitions(box2d PRIVATE BOX2D_VALIDATE)
endif()

if (MSVC)
    message(STATUS "Box2D on MSVC")	
    if (BUILD_SHARED_LIBS)
        target_compile_definitions(BOX2D INTERFACE BOX2D_DLL)
    endif()

    target_sources(BOX2D PRIVATE ${THIRD_PARTY_DIR}/BOX2D/src/box2d.natvis)

    target_compile_definitions(BOX2D PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:B2_ENABLE_ASSERT>")

    target_compile_options(BOX2D PRIVATE /experimental:c11atomics)

    if (BOX2D_AVX2)
        message(STATUS "Box2D using AVX2")	
        target_compile_definitions(BOX2D PRIVATE BOX2D_AVX2)
        target_compile_options(BOX2D PRIVATE /arch:AVX2)
    endif()

elseif (MINGW)
    message(STATUS "Box2D on MinGW")
    if (BOX2D_AVX2)
        message(STATUS "Box2D using AVX2")	
        target_compile_definitions(BOX2D PRIVATE BOX2D_AVX2)
        target_compile_options(BOX2D PRIVATE -mavx2)
    else()
    endif()
elseif (APPLE)
    message(STATUS "Box2D on Apple")
elseif (EMSCRIPTEN)
    message(STATUS "Box2D on Emscripten")
    target_compile_options(BOX2D PRIVATE -msimd128 -msse2)
elseif (UNIX)
    message(STATUS "Box2D using Unix")
    if ("${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL "aarch64")
        # raspberry pi
        # -mfpu=neon
        # target_compile_options(box2d PRIVATE)
    else()
        # x64
        if (BOX2D_AVX2)
            message(STATUS "Box2D using AVX2")
            # FMA -mfma -mavx -march=native
            target_compile_definitions(BOX2D PRIVATE BOX2D_AVX2)
            target_compile_options(BOX2D PRIVATE -mavx2)
        else()
        endif()
    endif()
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set_target_properties(BOX2D PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    set_target_properties(BOX2D PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Dist)
    set_target_properties(BOX2D PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
    )
endif()
