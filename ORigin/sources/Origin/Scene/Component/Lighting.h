#pragma once

#include <memory>
#include <glm\glm.hpp>

namespace Origin
{
	enum class LightingType
	{
		Direct = 0,
		PointLight,
	};

	class Lighting
	{
	public:
		Lighting() = default;
		~Lighting();

		void SetColor(glm::vec4& color);
		void SetAmbient(float ambient) { m_Ambient = ambient; }
		void SetType(LightingType type) { m_Type = type; }

		glm::vec4 GetColor() { return m_Color; }
		float GetAmbient() { return m_Ambient; }
		LightingType& GetType() { return m_Type; }

	private:
		glm::vec4 m_Color;
		float m_Ambient;
		LightingType m_Type;
	};
}