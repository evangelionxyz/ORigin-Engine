// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Origin/Core/Types.h"

namespace origin {

struct Vulkan2D
{
    static void Init();
    static void Shutdown();

    static void DrawQuad(const glm::vec3 &position, const glm::vec4 &color);

    static void StartBatch();
    static void Flush();
};

}

