// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Origin/Renderer/Renderer.h"
#include "Lighting.h"

namespace origin {

	int Lighting::SpotLightCount = 0;
	int Lighting::PointLightCount = 0;

	Lighting::Lighting(LightingType type)
		: Type(type)
	{
		m_TransformUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4), 0);

		// TODO: Apply shader for each lighting type
		std::shared_ptr<Shader> dirLightDepthShader = Shader::Create("Resources/Shaders/SPIR-V/DirectionalLightDepthMap.glsl", true);
		m_ShadowRenderer = ShadowRenderer::Create(dirLightDepthShader, type);

		m_LightingUniformBuffer = UniformBuffer::Create(sizeof(DirLightBufferData), LIGHTING_BINDING);
	}

	Lighting::~Lighting()
	{
	}

	void Lighting::OnRender(const TransformComponent& tc)
	{
		//m_TransformUniformBuffer->Bind();
		//m_TransformUniformBuffer->SetData(&m_ShadowRenderer->ViewProjection, sizeof(glm::mat4));

		switch (Type)
		{
		case LightingType::Spot:
			SpotLightCount++;
			break;

		case LightingType::Point:
			PointLightCount++;
			break;

		case LightingType::Directional:
			m_DirLightData.Direction = glm::vec4(-tc.GetUp(), 1.0f);
			m_LightingUniformBuffer->Bind();
			m_LightingUniformBuffer->SetData(&m_DirLightData, sizeof(m_DirLightData));
			break;
		}

		//m_ShadowRenderer->m_DepthShader->Disable();
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