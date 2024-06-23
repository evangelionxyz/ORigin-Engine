// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Components.h"
#include "Origin\Renderer\Shader.h"

namespace origin
{
	class UIRenderer
	{
	public:
		UIRenderer() = default;

		void CreateFramebuffer(uint32_t vpW, uint32_t vpH, float orthoW, float orthoH);
		void SetViewportSize(uint32_t vpW, uint32_t vpH, float orthoW, float orthoH);
		void Unload();
		void RenderFramebuffer();
		void Render();
		void AddUI(const UIComponent &ui);
		std::vector<UIComponent> &GetUIs() { return m_UIs; }

	private:
		uint32_t m_VAO = 0, m_VBO = 0;
		std::shared_ptr<Shader> m_ScreenShader;
		std::vector<UIComponent> m_UIs;
		glm::mat4 m_Projection;
	};
}


