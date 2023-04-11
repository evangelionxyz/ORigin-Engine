// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Lighting.h"

namespace Origin
{
	void Lighting::SetColor(glm::vec4& color)
	{
		if (m_Type == LightingType::Direct)
		{

		}

		m_Color = color;
	}

	Lighting::~Lighting()
	{
	}
}
