// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Renderer2D.hpp"

#include "Origin/Renderer/Renderer.h"
#include "Origin/Asset/AssetManager.h"

namespace origin {

struct Renderer2DData
{
	Shader *render_shader = nullptr;

	Ref<VertexArray> text_vertex_array;
	Ref<VertexBuffer> text_vertex_buffer;
	TextVertex *text_vertex_buffer_base = nullptr;
	TextVertex *text_vertex_buffer_pointer = nullptr;
	std::array<Ref<Texture2D>, 32> font_text_atlas_slots;
	Ref<Shader> text_shader;
	u32 font_atlas_texture_index = 0;
	u32 text_index_count = 0;

	Ref<VertexArray> quad_vertex_array;
	Ref<VertexBuffer> quad_vertex_buffer;
	QuadVertex *quad_vertex_buffer_base = nullptr;
	QuadVertex *quad_vertex_buffer_pointer = nullptr;
	glm::vec4 quad_vertex_positions[4];
	u32 quad_index_count = 0;
	Ref<Shader> quad_shader;

	Ref<VertexArray> circle_vertex_array;
	Ref<VertexBuffer> cirlce_vertex_buffer;
	Ref<Shader> circle_shader;

	u32 circle_index_count = 0;
	CircleVertex *circle_vertex_buffer_base = nullptr;
	CircleVertex *circle_vertex_buffer_pointer = nullptr;
	static const u32 max_line_count = 51200;
	static const u32 max_line_vertices = max_line_count * 2;
	Ref<VertexArray> line_vertex_array;
	Ref<VertexBuffer> line_vertex_buffer;
	Ref<Shader> LineShader;

	u32 line_vertex_count = 0;
	LineVertex *line_vertex_buffer_base = nullptr;
	LineVertex *line_vertex_buffer_pointer = nullptr;

	std::array<Ref<Texture2D>, 32> texture_slots;
	u32 texture_slot_index = 1; // 0 = white texture
};

static Renderer2DData *s_2d_data = nullptr;

void Renderer2D::Init()
{
	OGN_PROFILER_FUNCTION();

	s_2d_data = new Renderer2DData();

	// Quads
	s_2d_data->quad_vertex_array = VertexArray::Create();

	s_2d_data->quad_vertex_buffer = VertexBuffer::Create(Renderer::render_data.max_vertices * sizeof(QuadVertex));
	s_2d_data->quad_vertex_buffer->SetLayout({
		{ ShaderDataType::Float3, "position" },
		{ ShaderDataType::Float4, "color"    },
		{ ShaderDataType::Float2, "texcoord" },
		{ ShaderDataType::Float2, "tiling"   },
		{ ShaderDataType::Float,  "texindex" },
	});

	s_2d_data->quad_vertex_array->AddVertexBuffer(s_2d_data->quad_vertex_buffer);
	s_2d_data->quad_vertex_buffer_base = new QuadVertex[Renderer::render_data.max_vertices];

	u32* quadIndices = new u32[Renderer::render_data.max_quad_indices];

	u32 offset = 0;
	for (u32 i = 0; i < Renderer::render_data.max_quad_indices; i += 6)
	{
		quadIndices[i + 0] = offset + 0;
		quadIndices[i + 1] = offset + 1;
		quadIndices[i + 2] = offset + 2;

		quadIndices[i + 3] = offset + 2;
		quadIndices[i + 4] = offset + 3;
		quadIndices[i + 5] = offset + 0;

		offset += 4;
	}

	Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, Renderer::render_data.max_quad_indices);
	s_2d_data->quad_vertex_array->SetIndexBuffer(quadIB);
	delete[] quadIndices;

	// Text
	s_2d_data->text_vertex_array = VertexArray::Create();
	s_2d_data->text_vertex_buffer = VertexBuffer::Create(Renderer::render_data.max_vertices * sizeof(TextVertex));

	s_2d_data->text_vertex_buffer->SetLayout({
		{ ShaderDataType::Float3, "aPosition" },
		{ ShaderDataType::Float4, "aColor"    },
		{ ShaderDataType::Float2, "aTexCoord" },
		{ ShaderDataType::Float,  "aTexIndex" },
		});

	s_2d_data->text_vertex_array->AddVertexBuffer(s_2d_data->text_vertex_buffer);
	s_2d_data->text_vertex_array->SetIndexBuffer(quadIB);
	s_2d_data->text_vertex_buffer_base = new TextVertex[Renderer::render_data.max_vertices];

	// Circles
	s_2d_data->circle_vertex_array = VertexArray::Create();
	s_2d_data->cirlce_vertex_buffer = VertexBuffer::Create(Renderer::render_data.max_vertices * sizeof(CircleVertex));
	s_2d_data->cirlce_vertex_buffer->SetLayout({
		{ ShaderDataType::Float3, "TransformedPosition"  },
		{ ShaderDataType::Float3, "Position"  },
		{ ShaderDataType::Float4, "Color"     },
		{ ShaderDataType::Float,  "Thickness" },
		{ ShaderDataType::Float,  "Fade"      },
		});
	s_2d_data->circle_vertex_array->AddVertexBuffer(s_2d_data->cirlce_vertex_buffer);
	s_2d_data->circle_vertex_array->SetIndexBuffer(quadIB);
	s_2d_data->circle_vertex_buffer_base = new CircleVertex[Renderer::render_data.max_vertices];

	// Lines
	s_2d_data->line_vertex_array = VertexArray::Create();
	s_2d_data->line_vertex_buffer = VertexBuffer::Create(Renderer::render_data.max_vertices * sizeof(LineVertex));
	s_2d_data->line_vertex_buffer->SetLayout({
		{ ShaderDataType::Float3, "Position" },
		{ ShaderDataType::Float4, "Color"    },
		});

	s_2d_data->line_vertex_array->AddVertexBuffer(s_2d_data->line_vertex_buffer);
	s_2d_data->line_vertex_buffer_base = new LineVertex[Renderer::render_data.max_vertices];

	// Set first texture slot to 0
	s_2d_data->texture_slots[0] = Renderer::white_texture;

	s_2d_data->quad_vertex_positions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
	s_2d_data->quad_vertex_positions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
	s_2d_data->quad_vertex_positions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
	s_2d_data->quad_vertex_positions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

	s_2d_data->quad_shader = Renderer::GetShader("Quad2D");
	s_2d_data->text_shader = Renderer::GetShader("Text");
	s_2d_data->circle_shader = Renderer::GetShader("Circle2D");
	s_2d_data->LineShader = Renderer::GetShader("Line2D");
}

void Renderer2D::Shutdown()
{
	if (s_2d_data)
	{
        delete[] s_2d_data->quad_vertex_buffer_base;
        delete[] s_2d_data->line_vertex_buffer_base;
        delete[] s_2d_data->text_vertex_buffer_base;
        delete[] s_2d_data->circle_vertex_buffer_base;
        delete s_2d_data;
	}
}

void Renderer2D::Begin(Shader *render_shader)
{
	if (render_shader) s_2d_data->render_shader = render_shader;
	StartBatch();
}

void Renderer2D::End()
{
	Flush();
}

void Renderer2D::StartBatch()
{
	OGN_PROFILER_RENDERING();

	s_2d_data->quad_index_count = 0;
	s_2d_data->quad_vertex_buffer_pointer = s_2d_data->quad_vertex_buffer_base;

	s_2d_data->text_index_count = 0;
	s_2d_data->text_vertex_buffer_pointer = s_2d_data->text_vertex_buffer_base;

	s_2d_data->circle_index_count = 0;
	s_2d_data->circle_vertex_buffer_pointer = s_2d_data->circle_vertex_buffer_base;

	s_2d_data->line_vertex_count = 0;
	s_2d_data->line_vertex_buffer_pointer = s_2d_data->line_vertex_buffer_base;

	s_2d_data->texture_slot_index = 1;
	s_2d_data->font_atlas_texture_index = 0;
}

void Renderer2D::Flush()
{
	OGN_PROFILER_RENDERING();

	if (s_2d_data->text_index_count)
	{
		u32 dataSize = (u32)((uint8_t*)s_2d_data->text_vertex_buffer_pointer - (uint8_t*)s_2d_data->text_vertex_buffer_base);
		s_2d_data->text_vertex_buffer->Bind();
		s_2d_data->text_vertex_buffer->SetData(s_2d_data->text_vertex_buffer_base, dataSize);

		for (u32 i = 0; i < s_2d_data->font_atlas_texture_index; i++)
		{
			s_2d_data->font_text_atlas_slots[i]->Bind(i);
		}

		if (!s_2d_data->render_shader)
		{
			s_2d_data->text_shader->Enable();
		}

		RenderCommand::DrawIndexed(s_2d_data->text_vertex_array, s_2d_data->text_index_count);
		Renderer::GetStatistics().draw_calls++;
	}

	if (s_2d_data->circle_index_count)
	{
		u32 dataSize = (u32)((uint8_t*)s_2d_data->circle_vertex_buffer_pointer - (uint8_t*)s_2d_data->circle_vertex_buffer_base);
		s_2d_data->cirlce_vertex_buffer->Bind();
		s_2d_data->cirlce_vertex_buffer->SetData(s_2d_data->circle_vertex_buffer_base, dataSize);

		if (!s_2d_data->render_shader)
		{
			s_2d_data->circle_shader->Enable();
		}

		RenderCommand::DrawIndexed(s_2d_data->circle_vertex_array, s_2d_data->circle_index_count);
		Renderer::GetStatistics().draw_calls++;
	}

	if (s_2d_data->quad_index_count)
	{
		u32 dataSize = (u32)((uint8_t *)s_2d_data->quad_vertex_buffer_pointer - (uint8_t *)s_2d_data->quad_vertex_buffer_base);
		s_2d_data->quad_vertex_buffer->Bind();
		s_2d_data->quad_vertex_buffer->SetData(s_2d_data->quad_vertex_buffer_base, dataSize);

		for (u32 i = 0; i < s_2d_data->texture_slot_index; i++)
		{
			s_2d_data->texture_slots[i]->Bind(i);
		}

		if (!s_2d_data->render_shader)
		{
			s_2d_data->quad_shader->Enable();
		}

		RenderCommand::DrawIndexed(s_2d_data->quad_vertex_array, s_2d_data->quad_index_count);
		Renderer::GetStatistics().draw_calls++;
	}

	if (s_2d_data->line_vertex_count)
	{
		u32 dataSize = (u32)((uint8_t *)s_2d_data->line_vertex_buffer_pointer - (uint8_t *)s_2d_data->line_vertex_buffer_base);
		s_2d_data->line_vertex_buffer->Bind();
		s_2d_data->line_vertex_buffer->SetData(s_2d_data->line_vertex_buffer_base, dataSize);

		if (!s_2d_data->render_shader)
		{
			s_2d_data->LineShader->Enable();
		}

		RenderCommand::DrawLines(s_2d_data->line_vertex_array, s_2d_data->line_vertex_count);
		Renderer::GetStatistics().draw_calls++;
	}
}

void Renderer2D::NextBatch()
{
	Flush();
	StartBatch();
}

void Renderer2D::DrawQuad(const Rect &rect, const glm::vec4 &color)
{
	const glm::vec2 &center = rect.GetCenter();
	const glm::vec2 &size = rect.GetSize();
	DrawQuad({ center.x, center.y, 0.0f }, size, color);
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
	DrawQuad({ position.x, position.y, 0.0f }, size, color);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
	DrawQuad(transform, color);
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec2& tilingFactor, const glm::vec4& tintColor)
{
	DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec2& tilingFactor, const glm::vec4& tintColor)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	DrawQuad(transform, texture, tilingFactor, tintColor);
}

void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
{
	constexpr size_t quadVertexCount = 4;
	const f32 textureIndex = 0.0f; // White Texture
	constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
	const glm::vec2& tilingFactor = glm::vec2(1.0f);

	if (s_2d_data->quad_index_count >= Renderer::render_data.max_quad_indices)
		NextBatch();

	for (size_t i = 0; i < quadVertexCount; i++)
	{
		s_2d_data->quad_vertex_buffer_pointer->Position = transform * s_2d_data->quad_vertex_positions[i];
		s_2d_data->quad_vertex_buffer_pointer->Color = color;
		s_2d_data->quad_vertex_buffer_pointer->TexCoord = textureCoords[i];
		s_2d_data->quad_vertex_buffer_pointer->TexIndex = textureIndex;
		s_2d_data->quad_vertex_buffer_pointer->TilingFactor = tilingFactor;
		s_2d_data->quad_vertex_buffer_pointer++;
	}

	s_2d_data->quad_index_count += 6;
	Renderer::GetStatistics().quad_count++;
}

void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec2& tilingFactor, const glm::vec4& tintColor)
{
	OGN_CORE_ASSERT(texture, "Renderer2D: Invalid texture");

	constexpr size_t quadVertexCount = 4;
	constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

	if (s_2d_data->quad_index_count >= Renderer::render_data.max_quad_indices)
		NextBatch();

	f32 textureIndex = 0.0f;
	for (u32 i = 1; i < s_2d_data->texture_slot_index; i++)
	{
		if (*s_2d_data->texture_slots[i] == *texture)
		{
			textureIndex = (f32)i;
			break;
		}
	}

	if (textureIndex == 0.0f)
	{
		if (s_2d_data->texture_slot_index >= Renderer::render_data.max_texture_slots)
			NextBatch();

		textureIndex = (f32)s_2d_data->texture_slot_index;
		s_2d_data->texture_slots[s_2d_data->texture_slot_index] = texture;
		s_2d_data->texture_slot_index++;
	}

	for (size_t i = 0; i < quadVertexCount; i++)
	{
		s_2d_data->quad_vertex_buffer_pointer->Position = transform * s_2d_data->quad_vertex_positions[i];
		s_2d_data->quad_vertex_buffer_pointer->Color = tintColor;
		s_2d_data->quad_vertex_buffer_pointer->TexCoord = textureCoords[i];
		s_2d_data->quad_vertex_buffer_pointer->TexIndex = textureIndex;
		s_2d_data->quad_vertex_buffer_pointer->TilingFactor = tilingFactor;
		s_2d_data->quad_vertex_buffer_pointer++;
	}
	s_2d_data->quad_index_count += 6;
	Renderer::GetStatistics().quad_count++;
}

void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<SubTexture2D>& subTexture, const glm::vec2& tilingFactor, const glm::vec4& tintColor)
{
	constexpr int QuadVertexCount = 4;
	const glm::vec2* textureCoords = subTexture->GetTexCoords();
	const Ref<Texture2D> texture = subTexture->GetTexture();

	if (s_2d_data->quad_index_count >= Renderer::render_data.max_quad_indices)
		NextBatch();

	f32 textureIndex = 0.0f;
	for (u32 i = 1; i < s_2d_data->texture_slot_index; i++)
	{
		if (*s_2d_data->texture_slots[i] == *texture)
		{
			textureIndex = (f32)i;
			break;
		}
	}

	if (textureIndex == 0.0f)
	{
		if (s_2d_data->texture_slot_index >= Renderer::render_data.max_texture_slots)
			NextBatch();

		textureIndex = (f32)s_2d_data->texture_slot_index;
		s_2d_data->texture_slots[s_2d_data->texture_slot_index] = texture;
		s_2d_data->texture_slot_index++;
	}

	for (size_t i = 0; i < QuadVertexCount; i++)
	{
		s_2d_data->quad_vertex_buffer_pointer->Position = transform * s_2d_data->quad_vertex_positions[i];
		s_2d_data->quad_vertex_buffer_pointer->Color = tintColor;
		s_2d_data->quad_vertex_buffer_pointer->TexCoord = textureCoords[i];
		s_2d_data->quad_vertex_buffer_pointer->TexIndex = textureIndex;
		s_2d_data->quad_vertex_buffer_pointer->TilingFactor = tilingFactor;
		s_2d_data->quad_vertex_buffer_pointer++;
	}
	s_2d_data->quad_index_count += 6;
	Renderer::GetStatistics().quad_count++;
}

void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, f32 rotation, const glm::vec4 &color)
{
	DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, f32 rotation, const glm::vec4& color)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	DrawQuad(transform, color);
}

void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, f32 rotation, const Ref<Texture2D>& texture, const glm::vec2& tilingFactor, const glm::vec4& tintColor)
{
	DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, f32 rotation, const Ref<Texture2D>& texture, const glm::vec2& tilingFactor, const glm::vec4& tintColor)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	DrawQuad(transform, texture, tilingFactor, tintColor);
}

void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
	glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
	glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
	glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
	glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

	DrawLine(p0, p1, color);
	DrawLine(p1, p2, color);
	DrawLine(p2, p3, color);
	DrawLine(p3, p0, color);
}

void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color)
{
	glm::vec3 lineVertices[4];

	for (size_t i = 0; i < 4; i++)
		lineVertices[i] = transform * s_2d_data->quad_vertex_positions[i];

	DrawLine(lineVertices[0], lineVertices[1], color);
	DrawLine(lineVertices[1], lineVertices[2], color);
	DrawLine(lineVertices[2], lineVertices[3], color);
	DrawLine(lineVertices[3], lineVertices[0], color);
}

void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, f32 thickness, f32 fade)
{
	for (size_t i = 0; i < 4; i++)
	{
		s_2d_data->circle_vertex_buffer_pointer->TransformedPosition = transform * s_2d_data->quad_vertex_positions[i];
		s_2d_data->circle_vertex_buffer_pointer->Position = s_2d_data->quad_vertex_positions[i];
		s_2d_data->circle_vertex_buffer_pointer->Color = color;
		s_2d_data->circle_vertex_buffer_pointer->Thickness = thickness;
		s_2d_data->circle_vertex_buffer_pointer->Fade = fade;
		s_2d_data->circle_vertex_buffer_pointer++;
	}

	s_2d_data->circle_index_count += 6;
	Renderer::GetStatistics().circle_count++;
}

void Renderer2D::DrawCircle(const glm::vec3 &position, const glm::vec2 &scale, const glm::vec4 &color, f32 thickness, f32 fade)
{
	const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });
	DrawCircle(transform, color, thickness, fade);
}

void Renderer2D::DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec4 &color)
{
	if (s_2d_data->line_vertex_count >= Renderer2DData::max_line_count)
		NextBatch();

	s_2d_data->line_vertex_buffer_pointer->Position = p0;
	s_2d_data->line_vertex_buffer_pointer->Color = color;
	s_2d_data->line_vertex_buffer_pointer++;

	s_2d_data->line_vertex_buffer_pointer->Position = p1;
	s_2d_data->line_vertex_buffer_pointer->Color = color;
	s_2d_data->line_vertex_buffer_pointer++;

	s_2d_data->line_vertex_count += 2;
	Renderer::GetStatistics().line_count++;
}

void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRenderer2DComponent& src)
{
	const Ref<Texture2D> &texture = AssetManager::GetAsset<Texture2D>(src.Texture);
	if (texture)
	{
		constexpr size_t quadVertexCount = 4;
		glm::vec2 textureCoords[4]
		{
			{ src.UV0.x, src.UV0.y },
			{ src.UV1.x, src.UV0.y },
			{ src.UV1.x, src.UV1.y },
			{ src.UV0.x, src.UV1.y }
		};

		if (src.FlipX)
		{
			textureCoords[0] = { src.UV1.x, src.UV0.y };
			textureCoords[1] = { src.UV0.x, src.UV0.y };
			textureCoords[2] = { src.UV0.x, src.UV1.y };
			textureCoords[3] = { src.UV1.x, src.UV1.y };
		}
		else if (src.FlipY)
		{
			textureCoords[0] = { src.UV0.x, src.UV1.y };
			textureCoords[1] = { src.UV1.x, src.UV1.y };
			textureCoords[2] = { src.UV1.x, src.UV0.y };
			textureCoords[3] = { src.UV0.x, src.UV0.y };
		}

		if (src.FlipX && src.FlipY)
		{
			textureCoords[0] = { src.UV1.x, src.UV1.y };
			textureCoords[1] = { src.UV0.x, src.UV1.y };
			textureCoords[2] = { src.UV0.x, src.UV0.y };
			textureCoords[3] = { src.UV1.x, src.UV0.y };
		}

		if (s_2d_data->quad_index_count >= Renderer::render_data.max_quad_indices)
			NextBatch();

		f32 textureIndex = 0.0f;
		for (u32 i = 1; i < s_2d_data->texture_slot_index; i++)
		{
			if (*s_2d_data->texture_slots[i] == *texture)
			{
				textureIndex = (f32)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_2d_data->texture_slot_index >= Renderer::render_data.max_texture_slots)
				NextBatch();

			textureIndex = (f32)s_2d_data->texture_slot_index;
			s_2d_data->texture_slots[s_2d_data->texture_slot_index] = texture;
			s_2d_data->texture_slot_index++;
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_2d_data->quad_vertex_buffer_pointer->Position = transform * s_2d_data->quad_vertex_positions[i];
			s_2d_data->quad_vertex_buffer_pointer->Color = src.Color;
			s_2d_data->quad_vertex_buffer_pointer->TexCoord = textureCoords[i];
			s_2d_data->quad_vertex_buffer_pointer->TexIndex = textureIndex;
			s_2d_data->quad_vertex_buffer_pointer->TilingFactor = src.TillingFactor;
			s_2d_data->quad_vertex_buffer_pointer++;
		}
		s_2d_data->quad_index_count += 6;
		Renderer::GetStatistics().quad_count++;
	}
	else
	{
		DrawQuad(transform, src.Color);
	}
}

void Renderer2D::DrawString(const std::string& string, Ref<Font> font, const glm::mat4& transform, const TextParams& textParams, glm::vec2 *size)
{
	if (!font)
		return;

	if (!font->IsLoaded || font->GetAtlasTexture() == nullptr)
		return;

	if (s_2d_data->font_atlas_texture_index >= Renderer::render_data.max_texture_slots)
		NextBatch();
		
	const auto& fontGeometry = font->GetData()->FontGeometry;
	const auto& metrics = fontGeometry.getMetrics();

	Ref<Texture2D> fontAtlas = font->GetAtlasTexture();

	f32 textureIndex = 0;
	for (u32 i = 0; i < s_2d_data->font_atlas_texture_index; i++)
	{
		if (*s_2d_data->font_text_atlas_slots[i] == *fontAtlas)
		{
			s_2d_data->font_text_atlas_slots[i] = fontAtlas;
			textureIndex = (f32)i;
			break;
		}
	}

	if(textureIndex == 0.0f)
	{
		if (s_2d_data->font_atlas_texture_index >= Renderer::render_data.max_texture_slots)
			NextBatch();

		textureIndex = (f32)s_2d_data->font_atlas_texture_index;
		s_2d_data->font_text_atlas_slots[s_2d_data->font_atlas_texture_index] = fontAtlas;
		s_2d_data->font_atlas_texture_index++;
	}
		
	double x = 0.0;
	double y = 0.0;
	double max_x = 0.0; // to track the maximum width
	double min_y = 0.0; // to track the minimum y position (since y decrease with line breaks)

	double fs_scale = 1.0 / (metrics.ascenderY - metrics.descenderY);
	const double space_glyph_advance = fontGeometry.getGlyph(' ')->getAdvance();

	for (size_t i = 0; i < string.size(); i++)
	{
		char character = string[i];
		if (character == '\r')
		{
			continue;
		}

		if (character == '\n')
		{
			max_x = std::max(max_x, x);

			x = 0.0;
			y -= fs_scale * metrics.lineHeight + textParams.LineSpacing;
			continue;
		}

		if (character == ' ')
		{
			f32 advance = static_cast<f32>(space_glyph_advance);
			if (i < string.size() - 1)
			{
				char nextCharacter = string[i + 1];
				double dAdvance;
				fontGeometry.getAdvance(dAdvance, character, nextCharacter);
				advance = (f32)advance;
			}

			x += fs_scale * advance + textParams.Kerning;
			continue;
		}

		if (character == '\t')
		{
			x += 4.0f * (fs_scale * space_glyph_advance + textParams.Kerning);
			continue;
		}

		auto glyph = fontGeometry.getGlyph(character);
		if (!glyph)
		{
			glyph = fontGeometry.getGlyph('?');
		}

		double atlas_left, atlas_bottom, atlas_right, atlas_top;
		glyph->getQuadAtlasBounds(atlas_left, atlas_bottom, atlas_right, atlas_top);
		glm::vec2 texCoordMin(static_cast<f32>(atlas_left), static_cast<f32>(atlas_bottom));
		glm::vec2 texCoordMax(static_cast<f32>(atlas_right), static_cast<f32>(atlas_top));

		double plane_left, plane_bottom, plane_right, plane_top;
		glyph->getQuadPlaneBounds(plane_left, plane_bottom, plane_right, plane_top);
		glm::vec2 quad_min(static_cast<f32>(plane_left), static_cast<f32>(plane_bottom));
		glm::vec2 quad_max(static_cast<f32>(plane_right), static_cast<f32>(plane_top));

		quad_min *= fs_scale;
		quad_max *= fs_scale;

		quad_min += glm::vec2(x, y);
		quad_max += glm::vec2(x, y);

		f32 texel_width = 1.0f / s_2d_data->font_text_atlas_slots[static_cast<int>(textureIndex)]->GetWidth();
		f32 texel_height = 1.0f / s_2d_data->font_text_atlas_slots[static_cast<int>(textureIndex)]->GetHeight();

		texCoordMin *= glm::vec2(texel_width, texel_height);
		texCoordMax *= glm::vec2(texel_width, texel_height);

		{
			s_2d_data->text_vertex_buffer_pointer->Position = transform * glm::vec4(quad_min, 0.0f, 1.0f);
			s_2d_data->text_vertex_buffer_pointer->Color = textParams.Color;
			s_2d_data->text_vertex_buffer_pointer->TexCoord = texCoordMin;
			s_2d_data->text_vertex_buffer_pointer->TexIndex = textureIndex;
			s_2d_data->text_vertex_buffer_pointer++;

			s_2d_data->text_vertex_buffer_pointer->Position = transform * glm::vec4(quad_min.x, quad_max.y, 0.0f, 1.0f);
			s_2d_data->text_vertex_buffer_pointer->Color = textParams.Color;
			s_2d_data->text_vertex_buffer_pointer->TexCoord = { texCoordMin.x, texCoordMax.y };
			s_2d_data->text_vertex_buffer_pointer->TexIndex = textureIndex;
			s_2d_data->text_vertex_buffer_pointer++;

			s_2d_data->text_vertex_buffer_pointer->Position = transform * glm::vec4(quad_max, 0.0f, 1.0f);
			s_2d_data->text_vertex_buffer_pointer->Color = textParams.Color;
			s_2d_data->text_vertex_buffer_pointer->TexCoord = texCoordMax;
			s_2d_data->text_vertex_buffer_pointer->TexIndex = textureIndex;
			s_2d_data->text_vertex_buffer_pointer++;

			s_2d_data->text_vertex_buffer_pointer->Position = transform * glm::vec4(quad_max.x, quad_min.y, 0.0f, 1.0f);
			s_2d_data->text_vertex_buffer_pointer->Color = textParams.Color;
			s_2d_data->text_vertex_buffer_pointer->TexCoord = { texCoordMax.x, texCoordMin.y };
			s_2d_data->text_vertex_buffer_pointer->TexIndex = textureIndex;
			s_2d_data->text_vertex_buffer_pointer++;

			s_2d_data->text_index_count += 6;
			Renderer::GetStatistics().quad_count++;
		}

		if (i < string.size())
		{
			double advance = glyph->getAdvance();
			char nextCharacter = string[i + 1];
			fontGeometry.getAdvance(advance, character, nextCharacter);
			x += fs_scale * advance + textParams.Kerning;
		}

		max_x = std::max(max_x, x);
		min_y = std::min(min_y, y);
	}

	if (size)
	{
		double totalHeight = std::abs(min_y) + fs_scale * metrics.lineHeight;
		*size = { static_cast<f32>(max_x), static_cast<f32>(totalHeight) };
	}
}

void Renderer2D::DrawString(const std::string& string, const glm::mat4& transform, TextComponent& component)
{
	Ref<Font> font = AssetManager::GetAsset<Font>(component.FontHandle);
	DrawString(string, font, transform, { component.Color, component.Kerning, component.LineSpacing }, &component.Size);
}
}