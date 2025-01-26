// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include <cstdint>
#include <memory>

namespace origin
{
	class UniformBuffer
	{
	public:
        virtual ~UniformBuffer() {}

		virtual void SetData(const void* data, u32 size, u32 offset = 0) = 0;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual u32 Get() = 0;

		static std::shared_ptr<UniformBuffer> Create(u32 size, u32 binding);
	};

}

#endif