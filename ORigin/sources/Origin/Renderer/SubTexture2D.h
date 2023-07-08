// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Renderer/Texture.h"
#include <glm/glm.hpp>

namespace origin {

	class SubTexture2D
	{
	public:
		SubTexture2D(const std::shared_ptr<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);

		const std::shared_ptr<Texture2D> GetTexture() const { return m_Texture; }
		const glm::vec2* GetTexCoords() const { return m_TexCoords; }

		static std::shared_ptr<SubTexture2D> CreateFromCoords(const std::shared_ptr<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize = glm::vec2(1.0f));

	private:
		std::shared_ptr<Texture2D> m_Texture;
		glm::vec2 m_TexCoords[4];
		glm::vec2 m_CellSize = glm::vec2(1.0f);
		glm::vec2 m_SpriteSize = glm::vec2(1.0f);
	};

}
