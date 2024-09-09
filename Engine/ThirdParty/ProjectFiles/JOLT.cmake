add_library(JOLT STATIC
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/AABBTree/AABBTreeBuilder.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Core/Color.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Core/Factory.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Core/IssueReporting.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Core/JobSystemSingleThreaded.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Core/JobSystemThreadPool.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Core/JobSystemWithBarrier.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Core/LinearCurve.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Core/Memory.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Core/Profiler.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Core/RTTI.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Core/Semaphore.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Core/StringTools.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Core/TickCounter.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Geometry/ConvexHullBuilder.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Geometry/ConvexHullBuilder2D.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Geometry/Indexify.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Geometry/OrientedBox.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Math/Vec3.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/ObjectStream/ObjectStream.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/ObjectStream/ObjectStreamBinaryIn.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/ObjectStream/ObjectStreamBinaryOut.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/ObjectStream/ObjectStreamIn.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/ObjectStream/ObjectStreamOut.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/ObjectStream/ObjectStreamTextIn.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/ObjectStream/ObjectStreamTextOut.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/ObjectStream/SerializableObject.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/ObjectStream/TypeDeclarations.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Body/Body.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Body/BodyAccess.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Body/BodyCreationSettings.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Body/BodyInterface.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Body/BodyManager.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Body/MassProperties.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Body/MotionProperties.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Character/Character.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Character/CharacterBase.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Character/CharacterVirtual.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/BroadPhase/BroadPhase.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/BroadPhase/BroadPhaseBruteForce.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/BroadPhase/BroadPhaseQuadTree.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/BroadPhase/QuadTree.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/CastConvexVsTriangles.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/CastSphereVsTriangles.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/CollideConvexVsTriangles.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/CollideSphereVsTriangles.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/CollisionDispatch.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/CollisionGroup.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/EstimateCollisionResponse.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/GroupFilter.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/GroupFilterTable.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/ManifoldBetweenTwoFaces.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/NarrowPhaseQuery.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/NarrowPhaseStats.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/PhysicsMaterial.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/PhysicsMaterialSimple.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/Shape/BoxShape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/Shape/CapsuleShape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/Shape/CompoundShape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/Shape/ConvexHullShape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/Shape/ConvexShape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/Shape/CylinderShape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/Shape/DecoratedShape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/Shape/HeightFieldShape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/Shape/MeshShape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/Shape/MutableCompoundShape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/Shape/RotatedTranslatedShape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/Shape/ScaledShape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/Shape/Shape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/Shape/SphereShape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/Shape/StaticCompoundShape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/Shape/TaperedCapsuleShape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/Shape/TriangleShape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Collision/TransformedShape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/ConeConstraint.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/Constraint.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/ConstraintManager.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/ContactConstraintManager.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/DistanceConstraint.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/FixedConstraint.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/GearConstraint.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/HingeConstraint.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/MotorSettings.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/PathConstraint.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/PathConstraintPath.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/PathConstraintPathHermite.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/PointConstraint.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/PulleyConstraint.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/RackAndPinionConstraint.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/SixDOFConstraint.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/SliderConstraint.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/SpringSettings.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/SwingTwistConstraint.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Constraints/TwoBodyConstraint.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/DeterminismLog.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/IslandBuilder.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/LargeIslandSplitter.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/PhysicsLock.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/PhysicsScene.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/PhysicsSystem.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/PhysicsUpdateContext.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Ragdoll/Ragdoll.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/SoftBody/SoftBodyCreationSettings.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/SoftBody/SoftBodyMotionProperties.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/SoftBody/SoftBodyShape.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/SoftBody/SoftBodySharedSettings.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/StateRecorderImpl.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Vehicle/MotorcycleController.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Vehicle/TrackedVehicleController.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Vehicle/VehicleAntiRollBar.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Vehicle/VehicleCollisionTester.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Vehicle/VehicleConstraint.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Vehicle/VehicleController.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Vehicle/VehicleDifferential.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Vehicle/VehicleEngine.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Vehicle/VehicleTrack.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Vehicle/VehicleTransmission.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Vehicle/Wheel.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Physics/Vehicle/WheeledVehicleController.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/RegisterTypes.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Renderer/DebugRenderer.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Renderer/DebugRendererPlayback.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Renderer/DebugRendererRecorder.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Renderer/DebugRendererSimple.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Skeleton/SkeletalAnimation.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Skeleton/Skeleton.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Skeleton/SkeletonMapper.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/Skeleton/SkeletonPose.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/TriangleGrouper/TriangleGrouperClosestCentroid.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/TriangleGrouper/TriangleGrouperMorton.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/TriangleSplitter/TriangleSplitter.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/TriangleSplitter/TriangleSplitterBinning.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/TriangleSplitter/TriangleSplitterFixedLeafSize.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/TriangleSplitter/TriangleSplitterLongestAxis.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/TriangleSplitter/TriangleSplitterMean.cpp
    ${THIRD_PARTY_DIR}/JoltPhysics/Jolt/TriangleSplitter/TriangleSplitterMorton.cpp
)

target_include_directories(JOLT PRIVATE ${THIRD_PARTY_DIR}/JoltPhysics)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set_target_properties(JOLT PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Debug/Binaries/ThirdParty
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    set_target_properties(JOLT PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Release/Binaries/ThirdParty
    )
elseif (CMAKE_BUILD_TYPE STREQUAL Dist)
    set_target_properties(JOLT PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/Dist/Binaries/ThirdParty
    )
endif()