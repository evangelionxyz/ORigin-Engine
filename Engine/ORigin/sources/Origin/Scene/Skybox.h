// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef SKYBOX_H
#define SKYBOX_H

#include "Origin/Scene/Camera/EditorCamera.h"
#include "Origin/Scene/Camera/SceneCamera.h"

#include "Origin/Core/Types.h"
#include "Origin/Renderer/VertexArray.h"
#include "Origin/Renderer/Buffer.h"

#include <memory>

namespace origin
{
	class Skybox
	{
	public:
		static Ref<Skybox> Create(const std::string& filepath, const std::string& format);

        Ref<VertexArray> vao;
        Ref<VertexBuffer> vbo;
        u32 texture_id;
	};
}

#endif