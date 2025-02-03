// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef RENDER_BUFFER_H
#define RENDER_BUFFER_H

#include "Origin/Core/Assert.h"

namespace origin {
enum class ShaderDataType
{
	None = 0,
	Int, Int2, Int3, Int4,
	Float, Float2, Float3, Float4,
	Mat2, Mat3, Mat4,
	Boolean
};

static u32 ShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Int:    return 4;
	case ShaderDataType::Int2:   return 4 * 2;
	case ShaderDataType::Int3:   return 4 * 3;
	case ShaderDataType::Int4:   return 4 * 4;
	case ShaderDataType::Float:  return 4;
	case ShaderDataType::Float2: return 4 * 2;
	case ShaderDataType::Float3: return 4 * 3;
	case ShaderDataType::Float4: return 4 * 4;
	case ShaderDataType::Mat2:   return 4 * 2 * 2;
	case ShaderDataType::Mat3:   return 4 * 3 * 3;
	case ShaderDataType::Mat4:   return 4 * 4 * 4;
	case ShaderDataType::Boolean: return 1;
	}

	OGN_CORE_ASSERT(false, "Unknown Shader Data Type");
	return 0;
}

struct BufferElement
{
	std::string Name;
	ShaderDataType Type = ShaderDataType::None;
	u32 Size = 0;
	u32 Offset = 0;
	bool Normalized = false;

	BufferElement()
	{
	}

	BufferElement(ShaderDataType type, const std::string &name, bool normalize = false)
		: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalize)
	{
	}

	uint8_t GetComponentCount() const
	{
		switch (Type)
		{
		case ShaderDataType::Int:     return 1;
		case ShaderDataType::Int2:    return 2;
		case ShaderDataType::Int3:    return 3;
		case ShaderDataType::Int4:    return 4;
		case ShaderDataType::Float:   return 1;
		case ShaderDataType::Float2:  return 2;
		case ShaderDataType::Float3:  return 3;
		case ShaderDataType::Float4:  return 4;
		case ShaderDataType::Mat2:    return 2;
		case ShaderDataType::Mat3:    return 3;
		case ShaderDataType::Mat4:    return 4;
		case ShaderDataType::Boolean: return 1;
		}

		OGN_CORE_ASSERT(false, "Unknown Shader Data Type");
		return 0;
	}
};

class BufferLayout
{
public:
	BufferLayout() = default;

	BufferLayout(std::initializer_list<BufferElement> elements)
		: m_Elements(elements)
	{
		CalculateOffsetAndStride();
	}

	u32 GetStride() const { return m_Stride; }
	const std::vector<BufferElement> &GetElements() { return m_Elements; }

	std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
	std::vector<BufferElement>::iterator end() { return m_Elements.end(); }

private:
	void CalculateOffsetAndStride()
	{
		u32 offset = 0;
		m_Stride = 0;

		for (auto &element : m_Elements)
		{
			element.Offset = offset;
			offset += element.Size;
			m_Stride += element.Size;
		}
	}

	std::vector<BufferElement> m_Elements;
	u32 m_Stride = 0;
};

class VertexBuffer
{
public:
	virtual ~VertexBuffer() {};

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;
	virtual void SetData(const void *data, u32 size, u32 offset = 0) = 0;
	virtual void SetLayout(const BufferLayout &layout) = 0;
	virtual const BufferLayout &GetLayout() const = 0;

	static Ref<VertexBuffer> Create(u32 size);
	static Ref<VertexBuffer> Create(void *vertices, u32 size);
};

class IndexBuffer
{
public:
	virtual ~IndexBuffer() {};

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;
	virtual u32 GetCount() const = 0;
	virtual u32 GetBufferID() const = 0;

	static Ref<IndexBuffer> Create(u32 size);
	static Ref<IndexBuffer> Create(void *indices, u32 count);
};
}

#endif