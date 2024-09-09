add_library(BOX2D STATIC
    ${THIRD_PARTY_DIR}/Box2D/src/collision/b2_broad_phase.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/collision/b2_chain_shape.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/collision/b2_circle_shape.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/collision/b2_collide_circle.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/collision/b2_collide_edge.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/collision/b2_collide_polygon.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/collision/b2_collision.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/collision/b2_distance.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/collision/b2_dynamic_tree.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/collision/b2_edge_shape.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/collision/b2_polygon_shape.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/collision/b2_time_of_impact.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/common/b2_block_allocator.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/common/b2_draw.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/common/b2_math.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/common/b2_settings.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/common/b2_stack_allocator.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/common/b2_timer.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_body.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_chain_circle_contact.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_chain_polygon_contact.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_circle_contact.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_contact.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_contact_manager.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_contact_solver.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_distance_joint.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_edge_circle_contact.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_edge_polygon_contact.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_fixture.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_friction_joint.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_gear_joint.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_island.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_joint.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_motor_joint.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_mouse_joint.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_polygon_circle_contact.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_polygon_contact.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_prismatic_joint.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_pulley_joint.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_revolute_joint.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_weld_joint.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_wheel_joint.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_world.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/dynamics/b2_world_callbacks.cpp
    ${THIRD_PARTY_DIR}/Box2D/src/rope/b2_rope.cpp
)

target_include_directories(BOX2D PRIVATE
  ${THIRD_PARTY_DIR}/Box2D/include
  ${THIRD_PARTY_DIR}/Box2D/src
)

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
