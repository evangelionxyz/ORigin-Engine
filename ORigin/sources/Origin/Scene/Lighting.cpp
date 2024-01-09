#include "pch.h"
#include "Origin\Renderer\Renderer.h"
#include "Lighting.h"

namespace origin {

	int Lighting::SpotLightCount = 0;
	int Lighting::PointLightCount = 0;

	Lighting::Lighting(LightingType type)
		: Type(type)
	{
		m_ShadowRenderer = ShadowRenderer::Create(type);
	}

	void Lighting::OnUpdate(const TransformComponent& tc)
	{
		static std::string lightUniformName;

		switch (Type)
		{
		case LightingType::Spot:
		{
			SpotLightCount++;
			break;
		}
		case LightingType::Point:
		{
			PointLightCount++;
			break;
		}
		case LightingType::Directional:
		{
			break;
		}
		}

		OGN_CORE_ASSERT(m_ShadowRenderer->GetFramebuffer(), "Lighting: Invalid Shadow's Framebuffer");

		glActiveTexture(m_ShadowRenderer->GetFramebuffer()->GetDepthAttachmentRendererID());
		glBindTexture(GL_TEXTURE_2D, m_ShadowRenderer->GetFramebuffer()->GetDepthAttachmentRendererID());
		//mat->SetInt("uShadowMap", 0);

	}

	void Lighting::OnUpdate(const TransformComponent& tc, const std::shared_ptr<Material>& mat)
	{
		mat->SetMatrix("uLightSpaceMatrix", m_ShadowRenderer->LightSpaceMatrix);

		static std::string lightUniformName;

		switch (Type)
		{
			case LightingType::Spot:
				lightUniformName = std::string("spotLights[" + std::to_string(SpotLightCount) + "].");
				mat->SetInt("spotLightCount", SpotLightCount);
				mat->SetVector(lightUniformName + "Position", tc.Translation);
				mat->SetVector(lightUniformName + "Direction", -tc.GetForward());
				mat->SetVector(lightUniformName + "Color", Color);
				mat->SetFloat(lightUniformName + "InnerConeAngle", InnerConeAngle);
				mat->SetFloat(lightUniformName + "OuterConeAngle", OuterConeAngle);
				mat->SetFloat(lightUniformName + "Exponent", Exponent);

				SpotLightCount++;
				break;
			
			case LightingType::Point:
				lightUniformName = std::string("pointLights[" + std::to_string(PointLightCount) + "].");
				mat->SetInt("pointLightCount", PointLightCount);
				mat->SetVector(lightUniformName + "Position", tc.Translation);
				mat->SetVector(lightUniformName + "Color", Color);
				mat->SetFloat(lightUniformName + "Ambient", Ambient);
				mat->SetFloat(lightUniformName + "Specular", Specular);
				mat->SetFloat(lightUniformName + "Intensity", Intensity);
				mat->SetFloat(lightUniformName + "SpreadSize", SpreadSize);

				PointLightCount++;
				break;
			
			case LightingType::Directional:
				mat->SetVector("dirLight.Direction", -tc.GetForward());
				mat->SetVector("dirLight.Color", Color);
				mat->SetFloat("dirLight.Ambient", Ambient);
				mat->SetFloat("dirLight.Diffuse", Diffuse);
				mat->SetFloat("dirLight.Specular", Specular);
				break;
			
		}

		m_ShadowRenderer->OnAttachTexture(mat);
	}

	void Lighting::SetupShadow(const TransformComponent& tc)
	{
		m_ShadowRenderer->Setup(tc, Size, Near, Far);
	}

	void Lighting::SetType(LightingType type)
	{
		Type = type;
		m_ShadowRenderer->Invalidate(type);
	}

	std::shared_ptr<Lighting> Lighting::Create(LightingType type)
	{
		return std::make_shared<Lighting>(type);
	}

}