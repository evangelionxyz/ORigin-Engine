// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include <glm\glm.hpp>

namespace origin
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			: m_Projection(projection), m_View(glm::mat4(1.0f))
		{
		}
		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_Projection; }
		const glm::mat4& GetViewMatrix() const { return m_View; }
		const glm::mat4 GetViewProjection() const { m_Projection * m_View; }

	protected:
		glm::mat4 m_View;
		glm::mat4 m_Projection;
	};
}