#pragma once
#include "Components.h"
#include "Origin\Renderer\Shader.h"

namespace origin
{
	class UIRenderer
	{
	public:
		UIRenderer() = default;

		void CreateFramebuffer(uint32_t width, uint32_t height);
		void Unload();

		void RenderFramebuffer();
		void Render();

		void AddUI(const UIComponent &ui);

		void OnResizeViewport(uint32_t width, uint32_t height);
		std::vector<UIComponent> &GetUIs() { return m_UIs; }

	private:
		uint32_t m_VAO, m_VBO;
		std::shared_ptr<Shader> m_ScreenShader;

		std::vector<UIComponent> m_UIs;
	};
}


