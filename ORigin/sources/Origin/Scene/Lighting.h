#pragma once
#include "ShadowRenderer.h"
#include "Origin/Renderer/UniformBuffer.h"
#include "Components/Components.h"

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

	struct DirLightBufferData
	{
		glm::mat4 LightSpaceMat = glm::mat4(1.0f);
		glm::vec4 Position = glm::vec4(0.0f);
		glm::vec4 Color = glm::vec4(1.0f);
		glm::vec4 Ambient = glm::vec4(0.2f);
		float Diffuse = 1.0f;
		float Specular = 1.0f;
	};
	
	class Lighting
	{
	public:
		Lighting(LightingType type);
		~Lighting();
		ShadowRenderer& GetShadow() { return m_ShadowRenderer; }
		std::string GetTypeString() { return Utils::LightingTypeToString(Type); }

		LightingType Type = LightingType::Directional;
		DirLightBufferData DirLightData;
		std::shared_ptr<UniformBuffer> LightingUBO;
		float NearPlane = 1.0f, FarPlane = 50.0f;
		float OrthoSize = 20.0f;
		static int SpotLightCount;
		static int PointLightCount;

		static std::shared_ptr<Lighting> Create(LightingType type);

	private:
		ShadowRenderer m_ShadowRenderer;
	};
}

