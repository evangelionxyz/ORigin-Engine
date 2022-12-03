// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include <stb_image.h>
#include "Origin\Renderer\Shader.h"
#include "Origin\Renderer\VertexArray.h"
#include "Origin\Renderer\Buffer.h"
#include "Origin\Scene\Component\Camera.h"
#include "Origin\Scene\EditorCamera.h"

namespace Origin
{
	class Skybox
	{
	public:
		static void Init();
		static void LoadTexture();
		static void Draw(const EditorCamera& camera);
		static void Draw(const Camera& camera, glm::mat4& transform);
	};

}

