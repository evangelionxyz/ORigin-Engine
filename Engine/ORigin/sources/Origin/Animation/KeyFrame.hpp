// Copyright (c) Evangelion Manuhutu | Origin Engine

#ifndef KEYFRAME_HPP
#define KEYFRAME_HPP

#include "Origin/Core/Types.h"
#include "Origin/Math/Math.h"

template<typename T>
struct KeyFrame
{
    T Value;
    f32 Timestamp;
};

template<typename T>
using KeyFrames = std::vector<KeyFrame<T>>;

struct TransformKeyFrameBase
{
protected:
    f32 GetScaleFactor(f32 last_time_stamp, f32 next_time_stamp, f32 time)
    {
        f32 midWayLength = time - last_time_stamp;
        f32 framesDiff = next_time_stamp - last_time_stamp;
        f32 scaleFactor = midWayLength / framesDiff;
        return scaleFactor;
    }
};

struct Vec3Key : TransformKeyFrameBase
{
    KeyFrames<glm::vec3> Frames;

    void AddFrame(const KeyFrame<glm::vec3> &keyFrame)
    {
        Frames.push_back(keyFrame);
    }

    i32 GetIndex(f32 animTime)
    {
        for (i32 i = 0; i < Frames.size() - 1; ++i)
        {
            if (animTime < Frames[i + 1].Timestamp)
                return i;
        }

        return 0;
    }

    glm::mat4 InterpolateTranslation(f32 time)
    {
        if (Frames.size() == 1) return glm::translate(glm::mat4(1.0f), Frames[0].Value);
        i32 p0Index = GetIndex(time);
        i32 p1Index = p0Index + 1;
        f32 scaleFactor = GetScaleFactor(Frames[p0Index].Timestamp, Frames[p1Index].Timestamp, time);
        glm::vec3 finalVector = glm::mix(Frames[p0Index].Value, Frames[p1Index].Value, scaleFactor);
        return glm::translate(glm::mat4(1.0f), finalVector);
    }

    glm::mat4 InterpolateScaling(f32 time)
    {
        if (Frames.size() == 1) return glm::scale(glm::mat4(1.0f), Frames[0].Value);
        i32 p0Index = GetIndex(time);
        i32 p1Index = p0Index + 1;
        f32 scaleFactor = GetScaleFactor(Frames[p0Index].Timestamp, Frames[p1Index].Timestamp, time);
        glm::vec3 finalScale = glm::mix(Frames[p0Index].Value, Frames[p1Index].Value, scaleFactor);
        return glm::scale(glm::mat4(1.0f), finalScale);
    }
};

struct QuatKey : public TransformKeyFrameBase
{
    KeyFrames<glm::quat> Frames;

    void AddFrame(const KeyFrame<glm::quat> &keyFrame)
    {
        Frames.push_back(keyFrame);
    }

    i32 GetIndex(f32 time)
    {
        for (i32 i = 0; i < Frames.size() - 1; ++i)
            if (time < Frames[i + 1].Timestamp)
                return i;
        return 0;
    }

    glm::mat4 Interpolate(f32 time)
    {
        if (Frames.size() == 1)
        {
            auto rotation = glm::normalize(Frames[0].Value);
            return glm::toMat4(rotation);
        }

        i32 p0Index = GetIndex(time);
        i32 p1Index = p0Index + 1;
        f32 scaleFactor = GetScaleFactor(Frames[p0Index].Timestamp, Frames[p1Index].Timestamp, time);
        glm::quat finalRotation = glm::slerp(Frames[p0Index].Value, Frames[p1Index].Value, scaleFactor);
        return glm::toMat4(glm::normalize(finalRotation));
    }
};

#endif