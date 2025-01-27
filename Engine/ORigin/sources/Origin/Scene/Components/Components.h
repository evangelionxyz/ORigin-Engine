// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "Origin/Animation/AnimationBlender.h"
#include "Origin/Animation/AnimationState.h"
#include "Origin/Animation/SpriteAnimation.h"

#include "Origin/Audio/AudioListener.h"
#include "Origin/Math/Math.hpp"
#include "Origin/Core/UUID.h"
#include "Origin/Renderer/Texture.h"
#include "Origin/Renderer/Font.h"
#include "Origin/Renderer/ParticleSystem.h"
#include "Origin/Renderer/Lighting/Lighting.hpp"
#include "Origin/Renderer/Framebuffer.h"
#include "Origin/Renderer/Model/Mesh.hpp"
#include "Origin/Renderer/VertexArray.h"
#include "Origin/Renderer/Buffer.h"
#include "Origin/Scene/Skybox.h"
#include "Origin/Renderer/Renderer.h"

#include "Origin/Scene/Camera/SceneCamera.h"
#include "Origin/Scene/SpriteSheet.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>

#include <box2d/types.h>

namespace origin {
struct Lighting;
class AudioSource;
class SpriteAnimation;
class ScriptableEntity;
class RigidbodyComponent;
class BoxColliderComponent;
class SphereColliderComponent;
class CapsuleColliderComponent;

enum class EntityType
{
    Entity = BIT(1),
    Prefabs = BIT(2),
    Audio = BIT(3),
    UI = BIT(4),
    Mesh = BIT(5),
    Bone = BIT(6),
    Camera = BIT(7),
    Lighting = BIT(8)
};

namespace Utils {

static std::string EntityTypeToString(EntityType type)
{
    switch (type)
    {
    case EntityType::Entity:   return "Entity";
    case EntityType::Prefabs:  return "Prefabs";
    case EntityType::Audio:    return "Audio";
    case EntityType::UI:       return "UI";
    case EntityType::Mesh:     return "Mesh";
    case EntityType::Bone:     return "Bone";
    case EntityType::Camera:   return "Camera";
    case EntityType::Lighting: return "Lighting";
    default:                   return "Invalid";
    }
}

static EntityType EntityTypeStringToType(const std::string &type)
{
    if (R"(Entity)")   return EntityType::Entity;
    if (R"(Prefabs)")  return EntityType::Prefabs;
    if (R"(Audio)")    return EntityType::Audio;
    if (R"(UI)")       return EntityType::UI;
    if (R"(Mesh)")     return EntityType::Mesh;
    if (R"(Bone)")     return EntityType::Bone;
    if (R"(Camera)")   return EntityType::Camera;
    if (R"(Lighting)") return EntityType::Lighting;
    return static_cast<EntityType>(-1);
}
}

class IDComponent
{
public:
    UUID ID;
    UUID Parent = UUID(0);
    EntityType Type = EntityType::Entity;

    IDComponent() = default;
    IDComponent(const IDComponent &) = default;
    IDComponent(UUID id, UUID parent = UUID(0)) : ID(id), Parent(parent) {}

    void SetParent(UUID uuid)
    {
        Parent = uuid;
    }

    void AddChild(UUID childId)
    {
        Children.push_back(childId);
    }

    void RemoveChild(UUID childId)
    {
        Children.erase(std::remove_if(Children.begin(), Children.end(), [childId](const UUID id)
        {
            return id == childId;
        }), Children.end());
    }

    bool HasChild(UUID childId)
    {
        auto it = std::find(Children.begin(), Children.end(), childId);
        return it != Children.end();
    }

    bool HasChild()
    {
        return Children.size() > 0;
    }

    std::vector<UUID> Children;
};

class TagComponent
{
public:
    std::string Tag;
    TagComponent() = default;
    TagComponent(const TagComponent &) = default;
    TagComponent(const std::string &tag) : Tag(tag)
    {
    }
};

class SpriteAnimationComponent
{
public:
    Ref<AnimationState<SpriteAnimation>> State;
    SpriteAnimationComponent()
    {
        State = CreateRef<AnimationState<SpriteAnimation>>();
    }
    SpriteAnimationComponent(const SpriteAnimationComponent &) = default;
    static const AnimationType Type = AnimationType::Sprite;
};

class AudioListenerComponent
{
public:
    AudioListener Listener;
    uint32_t Index = 0;
    bool Enable = true;
    AudioListenerComponent() = default;
    AudioListenerComponent(const AudioListenerComponent &) = default;
};

class EnvironmentMap
{
public:
    EnvironmentMap() = default;
    EnvironmentMap(const EnvironmentMap &) = default;

    Ref<Skybox> skybox;
    glm::vec4 tint_color{ 1.0f, 1.0f, 1.0f, 1.0f };
    f32 blur_factor{ 0.0005f };
};

class AudioComponent
{
public:
    AssetHandle Audio = UUID(0);
    std::string Name;
    float Volume = 1.0f;
    float Panning = 0.0f;
    float Pitch = 1.0f;
    float MinDistance = 10.0f;
    float MaxDistance = 20.0f;
    bool Spatializing = false;
    bool Looping = false;
    bool PlayAtStart = false;
    bool Overlapping = false;
    int OverlapCount = 10;

    AudioComponent() = default;
    AudioComponent(const AudioComponent &) = default;
};

class ParticleComponent
{
public:
    ParticleSystem Particle;

    glm::vec3 Velocity = glm::vec3(0.0f);
    glm::vec3 VelocityVariation = glm::vec3(1.0f);
    glm::vec3 Rotation = glm::vec3(1.0f);

    glm::vec4 ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
    glm::vec4 ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
    uint32_t PoolIndex = 1000;

    float SizeBegin = 0.5f;
    float SizeEnd = 0.0f;
    float SizeVariation = 0.3f;
    float ZAxis = 0.0f;
    float LifeTime = 1.0f;

    ParticleComponent() = default;
    ParticleComponent(const ParticleComponent &) = default;
};

class MeshComponent
{
public:
    std::string Name;
    AssetHandle HModel = UUID(0);
    AnimationBlender blend_space;

    i32 AnimationIndex = 0;

    glm::vec2 blend_position{ 0.0f, 0.0f };
    f32 animation_speed = 1.0f;
    
    MeshComponent() = default;
    MeshComponent(const MeshComponent &) = default;
};

class TextComponent
{
public:
    AssetHandle FontHandle = UUID(0);
    std::string TextString = "This is text component";
    glm::vec4 Color = glm::vec4(1.0f);
    glm::vec2 Size = { 0.0f, 0.0f };
    glm::vec3 Position = { 0.0f, 0.0f, 0.0f };

    float Kerning = 0.0f;
    float LineSpacing = 0.0f;
    bool ScreenSpace = false;

    TextComponent() = default;
    TextComponent(const TextComponent &) = default;
};

class TransformComponent
{
public:
    glm::vec3 WorldTranslation = glm::vec3(0.0f);
    glm::quat WorldRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 WorldScale = glm::vec3(1.0f);
    glm::vec3 Translation = glm::vec3(0.0f);
    glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 Scale = glm::vec3(1.0f);

    bool Visible = true;

    bool Clickable = true;

    TransformComponent() = default;
    TransformComponent(const TransformComponent &) = default;

    TransformComponent(const glm::vec3 &translation)
        : Translation(translation)
    {
    }

    bool operator != (const TransformComponent &other) const
    {
        return WorldTranslation != other.Translation
            || WorldRotation != other.WorldRotation
            || WorldScale != other.WorldScale;
    }

    void SetTransform(glm::mat4 transform)
    {
        Math::DecomposeTransform(transform, WorldTranslation, WorldRotation, WorldScale);
    }

    glm::mat4 GetTransform() const
    {
        return glm::translate(glm::mat4(1.0f), WorldTranslation) * glm::toMat4(WorldRotation) * glm::scale(glm::mat4(1.0f), WorldScale);
    }

    void SetLocalTansform(glm::mat4 transform)
    {
        Math::DecomposeTransform(transform, Translation, Rotation, Scale);
    }

    glm::mat4 GetLocalTransform() const
    {
        return glm::translate(glm::mat4(1.0f), Translation) * glm::toMat4(Rotation) * glm::scale(glm::mat4(1.0f), Scale);
    }

    glm::vec3 GetForward() const
    {
        return glm::normalize(WorldRotation * glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::vec3 GetUp() const
    {
        return glm::normalize(WorldRotation * glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::vec3 GetRight() const
    {
        return glm::normalize(WorldRotation * glm::vec3(1.0f, 0.0f, 0.0f));
    }

    void SetForward(const glm::vec3 &forward)
    {
        glm::vec3 currentForward = GetForward();
        glm::quat rotationQuat = glm::rotation(currentForward, forward);
        WorldRotation = rotationQuat * WorldRotation;
    }

    void SetUp(const glm::vec3 &up)
    {
        glm::vec3 currentUp = GetUp();
        glm::quat rotationQuat = glm::rotation(currentUp, up);
        WorldRotation = rotationQuat * WorldRotation;
    }

    void SetRight(const glm::vec3 &right)
    {
        glm::vec3 currentRight = GetRight();
        glm::quat rotationQuat = glm::rotation(currentRight, right);
        WorldRotation = rotationQuat * WorldRotation;
    }

    void Translate(const glm::vec3 &delta)
    {
        WorldTranslation += delta;
    }

    void Rotate(const glm::quat &delta)
    {
        WorldRotation = glm::normalize(delta * WorldRotation);
    }
};

class SpriteRenderer2DComponent
{
public:
    AssetHandle Texture = UUID(0);

    glm::vec4 Color = glm::vec4(1.0f);

    glm::vec2 UV0 = glm::vec2(0.0f, 0.0f);
    glm::vec2 UV1 = glm::vec2(1.0f, 1.0f);

    glm::vec2 TillingFactor = glm::vec2(1.0f);

    bool FlipX = false;
    bool FlipY = false;

    SpriteRenderer2DComponent() = default;
    SpriteRenderer2DComponent(const SpriteRenderer2DComponent &) = default;
    SpriteRenderer2DComponent(float r, float g, float b, float a)
        : Color(r, g, b, a)
    {
    }
};

#pragma region LIGHTING_COMPONENTS

class LightComponent
{
public:
    Ref<Lighting> Light;
    LightComponent() = default;
    LightComponent(const LightComponent &) = default;

    glm::vec3 color;
};

class DirectionalLightComponent : public LightComponent
{
public:
    DirectionalLightComponent() = default;
    DirectionalLightComponent(const DirectionalLightComponent &) = default;

    glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
    glm::vec4 direction{ 0.0f, 0.0f, 0.0f, 1.0f };
};

class SpotLightComponent : public LightComponent
{
public:
    SpotLightComponent() = default;
    SpotLightComponent(const SpotLightComponent &) = default;
};

#pragma endregion



class CircleRendererComponent
{
public:
    glm::vec4 Color = glm::vec4(1.0f);
    float Thickness = 1.0f;
    float Fade = 0.005f;

    CircleRendererComponent() = default;
    CircleRendererComponent(const CircleRendererComponent &) = default;
};

class CameraComponent
{
public:
    SceneCamera Camera;

    bool Primary = true;
    CameraComponent() = default;
    CameraComponent(const CameraComponent &) = default;
};

class ScriptComponent
{
public:
    std::string ClassName = "None";
    ScriptComponent() = default;
    ScriptComponent(const ScriptComponent &) = default;
};

class NativeScriptComponent
{
public:
    ScriptableEntity *Instance;
    ScriptableEntity *(*InstantiateScript)();

    void (*DestroyScript)(NativeScriptComponent *nsc);

    template <typename T>
    void Bind()
    {
        InstantiateScript = []() { return static_cast<ScriptableEntity *>(new T()); };
        DestroyScript = [](NativeScriptComponent *nsc)
        {
            delete nsc->Instance;
            nsc->Instance = nullptr;
        };
    }
};

class Rigidbody2DComponent
{
public:
    void *RuntimeBody = nullptr;

    enum class BodyType { Static = 0, Dynamic, Kinematic };
    BodyType Type = BodyType::Static;

    glm::vec2 LinearVelocity = glm::vec2(0.0f);
    float  AngularVelocity = 0.0f;
    glm::vec2 MassCenter = glm::vec2(0.0f);

    float Mass = 1.0f;
    float LinearDamping = 0.0f;
    float AngularDamping = 0.01f;
    float RotationalInertia = 0.0f;
    float GravityScale = 1.0f;
    bool FixedRotation = false;
    bool EnableSleep = true;
    bool IsAwake = true;
    bool IsBullet = true;
    bool IsEnabled = true;

    b2BodyId BodyId;

    std::string ContactWith;

    Rigidbody2DComponent() = default;
    Rigidbody2DComponent(const Rigidbody2DComponent &) = default;
};

class BoxCollider2DComponent
{
public:
    glm::vec2 Offset = { 0.0f, 0.0f };
    glm::vec2 Size = { 0.5f, 0.5f };
    glm::vec2 CurrentSize = { 0.0f, 0.0f };

    float Density = 1.0f;
    float Friction = 0.5f;
    float Restitution = 0.0f;
    bool IsSensor = false;

    // All fixtures with the same group index always collide (positive index)
    // or never collide(negative index)
    int Group = 1;
    b2ShapeId ShapeId;

    BoxCollider2DComponent() = default;
    BoxCollider2DComponent(const BoxCollider2DComponent &) = default;
};

class CircleCollider2DComponent
{
public:

    glm::vec2 Offset = { 0.0f, 0.0f };
    float Radius = 0.5f;
    float Density = 1.0f;
    float Friction = 0.5f;
    float Restitution = 0.0f;
    bool IsSensor = false;

    // All fixtures with the same group index always collide(positive index)
    // or never collide(negative index)
    int Group = 1;
    b2ShapeId ShapeId;

    CircleCollider2DComponent() = default;
    CircleCollider2DComponent(const CircleCollider2DComponent &) = default;
};

class RevoluteJoint2DComponent
{
public:
    glm::vec2 AnchorPoint = glm::vec2(0.0f);
    glm::vec2 AnchorPointB = glm::vec2(0.0f);

    // Angles in degrees 
    // (it is converted to radians when creating component)
    bool EnableLimit = false;
    bool EnableMotor = true;
    bool EnableSpring = false;
    bool CollideConnected = false;
    float MaxMotorTorque = 100.0f;
    float MotorSpeed = 0.0f;
    float SpringDampingRatio = 1.0f;
    float LowerAngle = 0.0f;
    float UpperAngle = 0.0f;

    UUID ConnectedBodyID = 0;
    b2JointId JointId;

    RevoluteJoint2DComponent() = default;
    RevoluteJoint2DComponent(const RevoluteJoint2DComponent &) = default;
};

struct BaseUIData
{
    virtual ~BaseUIData() = default;
    enum class Anchor
    {
        Center,
        Left, Right,
        TopLeft, TopRight,
        BottomLeft, BottomRight
    };

    std::string Name;
    Anchor AnchorType;
    TransformComponent Transform;

    BaseUIData() = default;
    BaseUIData(const std::string &name, Anchor anchorType = Anchor::Center)
        : Name(name), AnchorType(anchorType)
    {
    }

};

template<typename T>
struct UIData : public BaseUIData
{
    T Component;

    UIData() = default;
    UIData(const std::string &name, const T &component, BaseUIData::Anchor anchorType = BaseUIData::Anchor::Center)
        : BaseUIData(name, anchorType), Component(component)
    {
    }
};

class UIComponent
{
public:
    UIComponent() = default;

    template<typename T>
    void AddComponent(const std::string &name, UIData<T> component)
    {
        component.Name = GenerateUniqueKey(name);
        Components.push_back(std::make_shared<UIData<T>>(component));
    }

    template<typename T>
    UIData<T> *GetComponent(const std::string &name)
    {
        for (auto &c : Components)
        {
            if (name == c->Name)
                return dynamic_cast<UIData<T>*>(c.get());
        }
        return nullptr;
    }

    bool RenameComponent(int index, const std::string &newName)
    {
        if (newName.empty())
            return false;

        bool foundSameName = false;
        for (auto &d : Components)
        {
            foundSameName = d->Name == newName;
            if (foundSameName)
                break;
        }

        if (foundSameName)
        {
            Components[index]->Name = GenerateUniqueKey(newName);
        }
        else
        {
            Components[index]->Name = newName;
        }

        return true;
    }

    void RemoveComponent(int index)
    {
        Components.erase(Components.begin() + index);
    }

    std::vector<Ref<BaseUIData>> Components;
    std::unordered_map<std::string, int> ComponentCounters;
    Ref<Framebuffer> OFramebuffer;

private:
    std::string GenerateUniqueKey(const std::string &name)
    {
        // check if the name already exists
        bool found = false;
        for (auto &c : Components)
        {
            if (c->Name == name)
            {
                found = true;
                break;
            }
        }

        // if the name does not exist, return it as the unique key
        if (!found)
            return name;

        // generate a unique key if the name already exists
        int counter = 1;
        std::string uniqueKey;
        bool unique = false;

        while (!unique)
        {
            uniqueKey = name + std::to_string(counter);
            unique = true;
            for (auto &c : Components)
            {
                if (c->Name == uniqueKey)
                {
                    unique = false;
                    break;
                }
            }
            counter++;
        }

        // update the counter to reflect the highest number used
        ComponentCounters[name] = counter - 1;

        // clean up counters for names that no longer exist in Components
        for (auto it = ComponentCounters.begin(); it != ComponentCounters.end();)
        {
            bool exists = false;
            for (auto &c : Components)
            {
                if (c->Name.find(it->first) == 0)
                {
                    exists = true;
                    break;
                }
            }

            if (!exists)
                it = ComponentCounters.erase(it);
            else
                ++it;
        }

        return uniqueKey;
    }
};

template <typename... Component>
struct ComponentGroup
{
};

using AllComponents = ComponentGroup <
    TransformComponent,
    CameraComponent,
    UIComponent,
    SpriteAnimationComponent,
    AudioComponent,
    AudioListenerComponent,

    DirectionalLightComponent,
    SpotLightComponent,

    EnvironmentMap,

    SpriteRenderer2DComponent,
    MeshComponent,
    TextComponent,
    CircleRendererComponent,
    ParticleComponent,
    ScriptComponent,
    NativeScriptComponent,
    Rigidbody2DComponent,
    BoxCollider2DComponent,
    CircleCollider2DComponent,
    RevoluteJoint2DComponent,
    RigidbodyComponent,
    BoxColliderComponent,
    SphereColliderComponent,
    CapsuleColliderComponent > ;
}

#endif
