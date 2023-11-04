#pragma once
#include "ShadowRenderer.h"
#include "Component.h"

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
	

	class Lighting
	{
	public:

		Lighting(LightingType type);

		void OnUpdate(const TransformComponent& tc);
		void OnUpdate(const TransformComponent& tc, const std::shared_ptr<Material>& mat);
		void SetupShadow(const TransformComponent& tc);
		void SetType(LightingType type);

		std::shared_ptr<ShadowRenderer>& GetShadow() { return m_ShadowRenderer; }

		LightingType Type = LightingType::Directional;
		std::string GetTypeString() { return Utils::LightingTypeToString(Type); }

		static std::shared_ptr<Lighting> Create(LightingType type);

		glm::vec3 Color = glm::vec3(1.0f);
		float Near = -1.0f;
		float Far = 8.0f;
		float Size = 25.0f;
		float Ambient = 0.5f;
		float Diffuse = 0.5f;
		float Specular = 1.0f;

		float InnerConeAngle = 0.5f;
		float OuterConeAngle = 1.0f;
		float Exponent = 1.5f;

		static int SpotLightCount;
		static int PointLightCount;

	private:
		std::shared_ptr<ShadowRenderer> m_ShadowRenderer;
	};
}

