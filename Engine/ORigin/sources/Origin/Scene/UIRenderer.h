// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef UI_RENDERER_H
#define UI_RENDERER_H

#include "Components/Components.h"
#include "Origin/Renderer/Shader.h"

namespace origin
{
	class UIRenderer
	{
	public:
		UIRenderer() = default;

		void CreateFramebuffer(f32 width, f32 height);
		void SetViewportSize(f32 width, f32 height);

		void Unload();
		void RenderFramebuffer();
		void Render();
		void AddUI(const UIComponent &ui);

		std::vector<UIComponent> &GetUIs() { return m_UIs; }

		glm::vec3 clear_color = glm::vec3(0.1f);

	private:
		uint32_t m_VAO = 0, m_VBO = 0;
		std::shared_ptr<Shader> m_ScreenShader;
		std::vector<UIComponent> m_UIs;
		glm::mat4 m_Projection;
	};
}
#endif