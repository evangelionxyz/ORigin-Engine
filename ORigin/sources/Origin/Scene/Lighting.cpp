// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Origin/Renderer/Renderer.h"
#include "Lighting.h"

namespace origin {

	int Lighting::SpotLightCount = 0;
	int Lighting::PointLightCount = 0;

#define LIGHTSPACE_BINDING 3

	Lighting::Lighting(LightingType type)
		: Type(type)
	{
		LightingUBO = UniformBuffer::Create(sizeof(DirLightBufferData), LIGHTING_BINDING);
		m_ShadowRenderer.OnCreate(type);
	}

	Lighting::~Lighting()
	{
	}

	std::shared_ptr<Lighting> Lighting::Create(LightingType type)
	{
		return std::make_shared<Lighting>(type);
	}

}