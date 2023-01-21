// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#include "Editor.h"

namespace Origin
{
	void Editor::GameViewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoScrollWithMouse
			| ImGuiWindowFlags_NoCollapse;

		ImGui::Begin("Game", nullptr, window_flags);

		auto viewportHovered = ImGui::IsWindowHovered();
		auto viewportFocused = ImGui::IsWindowFocused();


		ImVec2& viewportMinRegion = ImGui::GetWindowContentRegionMin();
		ImVec2& viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		ImVec2& viewportOffset = ImGui::GetWindowPos();
		m_GameViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_GameViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
		ImVec2& viewportPanelSize = ImGui::GetContentRegionAvail();
		m_GameViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		uint64_t viewportID = m_GameFramebuffer->GetColorAttachmentRendererID(m_RenderTarget);
		ImGui::Image(reinterpret_cast<void*>(viewportID), ImVec2(m_GameViewportSize.x, m_GameViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
		ImGui::PopStyleVar();
	}
}