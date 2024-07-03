#pragma once
#include "ShadowRenderer.h"
#include "Origin/Renderer/UniformBuffer.h"
#include "Components.h"

namespace origin {

	enum class LightingType
	{
		Spot, Point, Directional, None
	};

	namespace Utils
	{
		static std::string LightingTypeToString(LightingType type)
		{
			switch (type)
			{
				case LightingType::Spot: return "SPOT";
				case LightingType::Point: return "POINT";
				case LightingType::Directional: return "DIRECTIONAL";
				default: return "";
			}
			return "Invalid Type";
		}

		static LightingType LightTypeStringToType(std::string type)
		{
			if(type == "SPOT") return LightingType::Spot;
			if(type == "POINT") return LightingType::Point;
			if(type == "DIRECTIONAL") return LightingType::Directional;
			return LightingType::None;
		}
	}

	struct LightBufferData
	{
		glm::mat4 Transform;
	};

	struct DirectionalLightBufferData
	{
		glm::vec4 Direction = glm::vec4(0.0f);
		glm::vec4 Color = glm::vec4(1.0f);
		float Strength = 1.0f;
		float Diffuse = 1.0f;
		float Specular = 1.0f;
	};
	
	class Lighting
	{
	public:
		Lighting(LightingType type);
		~Lighting();

		void OnRender(const TransformComponent& lightDirection);
		void SetType(LightingType type);

		LightBufferData m_LightData;
		DirectionalLightBufferData m_DirLightData;

		const std::shared_ptr<ShadowRenderer>& GetShadow() const { return m_ShadowRenderer; }

		LightingType Type = LightingType::Directional;
		std::string GetTypeString() { return Utils::LightingTypeToString(Type); }

		static int SpotLightCount;
		static int PointLightCount;

		static std::shared_ptr<Lighting> Create(LightingType type);

	private:
		std::shared_ptr<ShadowRenderer> m_ShadowRenderer;
		std::shared_ptr<UniformBuffer> m_UniformBuffer;
		std::shared_ptr<UniformBuffer> m_DirectionalUniformBuffer;
	};
}

