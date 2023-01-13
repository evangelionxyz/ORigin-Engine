// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "ContentBrowserPanel.h"
#include <string>
#include <imgui.h>

namespace Origin
{
	extern const std::filesystem::path g_AssetPath = "assets";

	std::shared_ptr<Origin::Texture2D> ContentBrowserPanel::DirectoryIcon(std::filesystem::directory_entry dirEntry)
	{
		auto& fileExtension = dirEntry.path().extension().string();

		if (m_DirectoryIconMap.find(fileExtension) == m_DirectoryIconMap.end() && !dirEntry.is_directory())
			return m_DirectoryIconMap.at(".unkownfile");

		if (dirEntry.is_directory())
			return m_DirectoryIconMap.at("directory");

		return m_DirectoryIconMap.at(fileExtension);
	}

	ContentBrowserPanel::ContentBrowserPanel() : m_CurrentDirectory(g_AssetPath)
	{
		m_NavigationIconMap["backward_button"] = Texture2D::Create("resources/textures/backward_icon.png");
		m_NavigationIconMap["forward_button"] = Texture2D::Create("resources/textures/forward_icon.png");

		m_DirectoryIconMap["directory"] = Texture2D::Create("resources/textures/directory_icon.png");
		m_DirectoryIconMap[".unkownfile"] = Texture2D::Create("resources/textures/file_icon.png");
		m_DirectoryIconMap[".org"] = Texture2D::Create("resources/textures/scene_file_icon.png");
		m_DirectoryIconMap[".png"] = Texture2D::Create("resources/textures/png_file_icon.png");
		m_DirectoryIconMap[".jpg"] = Texture2D::Create("resources/textures/jpg_file_icon.png");
		m_DirectoryIconMap[".glsl"] = Texture2D::Create("resources/textures/glsl_file_icon.png");
		m_DirectoryIconMap[".txt"] = Texture2D::Create("resources/textures/txt_file_icon.png");
		m_DirectoryIconMap[".cpp"] = Texture2D::Create("resources/textures/cpp_file_icon.png");
		m_DirectoryIconMap[".cs"] = Texture2D::Create("resources/textures/script_file_icon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		auto windowSize = ImGui::GetWindowSize();

		// Navigation Button
		bool rootDirectory = m_CurrentDirectory == std::filesystem::path(g_AssetPath);
		ImVec4 navBtColor;

		if (rootDirectory) navBtColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		else navBtColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, navBtColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, navBtColor);

		ImGui::ImageButton((ImTextureID)m_NavigationIconMap.at("backward_button")->GetRendererID(), {24, 24}, {0, 1}, {1, 0});

		ImGui::PopStyleColor(3);
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !rootDirectory)
			m_CurrentDirectory = m_CurrentDirectory.parent_path();

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, navBtColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, navBtColor);

		ImGui::ImageButton((ImTextureID)m_NavigationIconMap.at("forward_button")->GetRendererID(), { 24, 24 }, { 0, 1 }, { 1, 0 });

		ImGui::PopStyleColor(3);
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !rootDirectory)
			m_CurrentDirectory = m_CurrentDirectory.parent_path();


		// Directory Button
		static float padding = 10.0f;
		static float thumbnailSize = 72.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		auto columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, g_AssetPath);
			std::string filenameString = relativePath.filename().string();

			ImGui::PushID(filenameString.c_str());

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::ImageButton((ImTextureID)DirectoryIcon(directoryEntry)->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			ImGui::PopStyleColor();

			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();
			}
			ImGui::TextWrapped(filenameString.c_str());
			ImGui::NextColumn();

			ImGui::PopID();
		}
		ImGui::Columns(1);
		ImGui::End();
	}



}