// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Origin/Core/Types.h"

namespace origin {

struct TextVertex
{
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 TexCoord;
    f32 TexIndex;
};

struct QuadVertex
{
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 TexCoord;
    glm::vec2 TilingFactor;
    f32 TexIndex;
};

struct CircleVertex
{
    glm::vec3 TransformedPosition;
    glm::vec3 Position;
    glm::vec4 Color;
    f32 Thickness;
    f32 Fade;
};

struct LineVertex
{
    glm::vec3 Position;
    glm::vec4 Color;
};


}

// font parameter
struct TextParams
{
    glm::vec4 Color = glm::vec4(1.0);
    float Kerning = 0.0f;
    float LineSpacing = 0.0f;
};