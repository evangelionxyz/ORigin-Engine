#include "pch.h"
#include "Origin\Renderer\Renderer.h"
#include "Lighting.h"

namespace origin {

	int Lighting::SpotLightCount = 0;
	int Lighting::PointLightCount = 0;

	Lighting::Lighting(LightingType type)
		: Type(type)
	{
		m_UniformBuffer = UniformBuffer::Create(sizeof(LightBufferData), 2);

		// TODO: Apply shader for each lighting type
		std::shared_ptr<Shader> dirLightDepthShader = Shader::Create("Resources/Shaders/SPIR-V/DirectionalLightDepthMap.glsl", true, true);
		m_ShadowRenderer = ShadowRenderer::Create(dirLightDepthShader, type);
		m_DirectionalUniformBuffer = UniformBuffer::Create(sizeof(DirectionalLightBufferData), 3);
	}

	Lighting::~Lighting()
	{
	}

	void Lighting::OnRender(const TransformComponent& tc)
	{
		m_UniformBuffer->Bind();
		m_UniformBuffer->SetData(&m_ShadowRenderer->m_LightViewProjection, sizeof(glm::mat4));

		switch (Type)
		{
		case LightingType::Spot:
			SpotLightCount++;
			break;

		case LightingType::Point:
			PointLightCount++;
			break;

		case LightingType::Directional:
			m_DirLightData.Direction = glm::vec4(tc.GetUp(), 1.0f);
			m_DirectionalUniformBuffer->Bind();
			m_DirectionalUniformBuffer->SetData(&m_DirLightData, sizeof(m_DirLightData));
			break;
		}
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