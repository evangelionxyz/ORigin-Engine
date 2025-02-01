// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Origin/Core/Base.h"
#include "Origin/Renderer/UniformBuffer.h"
#include <glm/glm.hpp>

namespace origin {

enum LightingType : u8
{
    LightingType_Spot        = BIT(0),
    LightingType_Point       = BIT(1),
    LightingType_Directional = BIT(2),
    LightingType_Area        = BIT(3),
};

static std::string_view LightingTypeToString(LightingType type)
{
    switch(type)
    {
    case LightingType_Spot:
        return "Spot";
    case LightingType_Point:
        return "Point";
    case LightingType_Directional:
        return "Directional";
    case LightingType_Area:
        return "Area";
    }
    return "Invalid";
}

struct Lighting
{
    virtual ~Lighting() = default;
    Lighting() = default;

    template<typename LightType>
    static Ref<Lighting> Create() { return CreateRef<LightType>(); }

    virtual void Bind() const {};
    virtual void Unbind() const {};

    i32 index = -1;
};

struct DirectionalLight final : public Lighting
{
    DirectionalLight();
    ~DirectionalLight() override;

    void Bind() const override;
    void Unbind() const override;

    struct Data
    {
        glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
        glm::vec4 direction{ 0.0f, 0.0f, 0.0f, 1.0 };
    } data;
};

struct PointLight final : public Lighting
{
    PointLight();
    ~PointLight() override;

    struct Data
    {
        glm::vec4 position;
        glm::vec4 intensity;
        glm::vec4 color;
        glm::vec4 falloff;
    } data;
};

struct SpotLight final : public Lighting
{
    SpotLight();
    ~SpotLight() override;

    struct Data
    {
        glm::vec4 position;
        glm::vec4 direction;
        glm::vec4 color;
        glm::vec4 cutt_off;
    } data;
};

}


