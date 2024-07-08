// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once

#include <cstdint>
#include <memory>

namespace origin
{
	class UniformBuffer
	{
	public:
        virtual ~UniformBuffer() {}

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual uint32_t Get() = 0;

		static std::shared_ptr<UniformBuffer> Create(uint32_t size, uint32_t binding);
	};

}

