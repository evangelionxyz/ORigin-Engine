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

class Lighting
{
public:
    virtual ~Lighting() = default;
    Lighting() = default;
    glm::vec3 color = glm::vec3(1.0f);

    template<typename LightType>
    static Ref<Lighting> Create() { return CreateRef<LightType>(); }

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
};

class DirectionalLight final : public Lighting
{
public:
    DirectionalLight();
    ~DirectionalLight() override;

    void Bind() const override;
    void Unbind() const override;
    
    glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
    glm::vec4 direction{ 0.0f, 0.0f, 0.0f, 1.0 };
};



}


