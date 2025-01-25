// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef ANIMATION_H
#define ANIMATION_H

#include "Origin/Core/Base.h"
#include "Origin/Math/Math.h"
#include "KeyFrame.hpp"

#include <assimp/scene.h>

namespace origin {
enum class AnimationType
{
	Sprite = BIT(0),
	SpritesSheet = BIT(1),
	Transform = BIT(2),
	Skeletal = BIT(3)
};

static const char *AnimationTypeToString(AnimationType type)
{
	switch (type)
	{
	case AnimationType::Sprite: return "Sprite";
	case AnimationType::SpritesSheet: return "SpritesSheet";
	case AnimationType::Transform: return "Transform";
	case AnimationType::Skeletal: return "Skeletal";
	default: return "Invalid Type";
	}
}

class Animation
{
public:
	virtual ~Animation() = default;

	bool Looping = false;
	bool Preview = false;

	virtual AnimationType GetType() const = 0;
};


/// <summary>
/// Skeletal animation
/// </summary>

class AnimationNode
{
public:
    AnimationNode() = default;
    AnimationNode(const aiNodeAnim * anim_node);

    void Update(f32 time);

    Vec3Key translation_keys;
    QuatKey rotation_keys;
    Vec3Key scale_keys;

    /// node's local transform
    glm::mat4 transform;

    glm::vec3 translation;
    glm::vec3 scale;
    glm::quat rotation;
};

class SkeletalAnimation : public Animation
{
public:
    SkeletalAnimation() = default;

    SkeletalAnimation(aiAnimation *anim);
    void UpdateTime(f32 delta_time);

    const std::string &GetName() const { return m_Name; }
    const f32 GetDuration() const { return m_Duration; }
    const f32 GetTicksPerSecond() const { return m_TicksPerSecond; }
    const f32 GetTimeInSeconds() const { return m_TimeInSeconds; }
    const f32 GetTimeInTicks() const { return m_TimeInTicks; }

    std::unordered_map<std::string, AnimationNode> &GetChannelMap() { return m_ChannelMap; }

    virtual AnimationType GetType() const override { return AnimationType::Skeletal; }

private:
    void ReadChannels(const aiAnimation *anim);

    std::string m_Name;
    aiAnimation *m_Anim = nullptr;
    f32 m_Duration = 0.0f;
    f32 m_TicksPerSecond = 1.0f;
    f32 m_TimeInSeconds = 0.0f;
    f32 m_TimeInTicks = 0.0f;
    std::unordered_map<std::string, AnimationNode> m_ChannelMap;
};

}



#endif