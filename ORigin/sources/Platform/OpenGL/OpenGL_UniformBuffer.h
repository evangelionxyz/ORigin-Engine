// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Renderer\UniformBuffer.h"

namespace origin
{
	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		virtual ~OpenGLUniformBuffer();
		void Bind() override;
		void Unbind() override;
		void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
	private:

		uint32_t m_RendererID = 0;
		uint32_t m_Binding;
	};
}


