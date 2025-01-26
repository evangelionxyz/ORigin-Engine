// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef SKYBOX_H
#define SKYBOX_H

#include "Origin/Scene/Camera/EditorCamera.h"
#include "Origin/Scene/Camera/SceneCamera.h"

#include <memory>

namespace origin
{
	class Skybox
	{
	public:
		virtual void Draw(const glm::mat4& viewProjection) = 0;
		virtual void Draw(const SceneCamera& camera) = 0;
		virtual void Draw(const EditorCamera& camera) = 0;
		virtual void SetBlur(float blur) = 0;
		static std::shared_ptr<Skybox> Create(const std::string& filepath, const std::string& format);
	};
}

#endif