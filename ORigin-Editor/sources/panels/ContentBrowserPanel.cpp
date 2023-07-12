// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "ContentBrowserPanel.h"
#include "Origin\Project\Project.h"
#include "..\Editor.h"
#include <imgui.h>

namespace origin
{
	namespace Utils
	{
		static void CenteredText(const std::string& text)
		{
			const auto windowWidth = ImGui::GetWindowSize().x;
			const auto textWidth = ImGui::CalcTextSize(text.c_str()).x;
			ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
			ImGui::Text(text.c_str());
		}

		static std::string CapitalizeText(std::string& text)
		{
			for (int i = 0; i < text.length(); i++)
			{
				if (i == 0) text[i] = (int)std::toupper(text[i]);
				else if (text[i - 1] == ' ')
					text[i] = std::toupper(text[i]);
			}

			return text;
		}

		static std::string CapitalizeWholeText(std::string text)
		{
			for (int i = 0; i < (int)text.length(); i++)
				text[i] = std::toupper(text[i]);

			return text;
		}
	}

	ContentBrowserPanel::ContentBrowserPanel()
	{
		m_NavigationIconMap["backward_button"] = Texture2D::Create("Resources/UITextures/backward_icon.png");
		m_NavigationIconMap["forward_button"] = Texture2D::Create("Resources/UITextures/forward_icon.png");

		m_DirectoryIconMap["directory"] = Texture2D::Create("Resources/UITextures/directory_icon.png");
		m_DirectoryIconMap[".unkownfile"] = Texture2D::Create("Resources/UITextures/file_icon.png");
		m_DirectoryIconMap[".org"] = Texture2D::Create("Resources/UITextures/scene_file_icon.png");
		m_DirectoryIconMap[".png"] = Texture2D::Create("Resources/UITextures/png_file_icon.png");
		m_DirectoryIconMap[".jpg"] = Texture2D::Create("Resources/UITextures/jpg_file_icon.png");
		m_DirectoryIconMap[".glsl"] = Texture2D::Create("Resources/UITextures/glsl_file_icon.png");
		m_DirectoryIconMap[".txt"] = Texture2D::Create("Resources/UITextures/txt_file_icon.png");
		m_DirectoryIconMap[".cpp"] = Texture2D::Create("Resources/UITextures/cpp_file_icon.png");
		m_DirectoryIconMap[".cs"] = Texture2D::Create("Resources/UITextures/script_file_icon.png");

		m_RootDirectory = Project::GetAssetDirectory();
		m_CurrentDirectory = m_RootDirectory;
	}

	ContentBrowserPanel::~ContentBrowserPanel()
	{
		m_SubDirectoryMap.clear();
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		FileButton();
		FileArgument();
	}

	std::shared_ptr<Texture2D> ContentBrowserPanel::DirectoryIcon(const std::filesystem::directory_entry& dirEntry)
	{
		auto& fileExtension = dirEntry.path().extension().string();

		if (m_DirectoryIconMap.find(fileExtension) == m_DirectoryIconMap.end() && !dirEntry.is_directory())
			return m_DirectoryIconMap.at(".unkownfile");

		if (dirEntry.is_directory())
			return m_DirectoryIconMap.at("directory");

		return m_DirectoryIconMap.at(fileExtension);
	}

	void ContentBrowserPanel::FileButton()
	{
		ImGui::Begin("Content Browser");
		NavigationButton();

		auto windowSize = ImGui::GetWindowSize();

		// Directory Button
		static float padding = 10.0f;
		static float thumbnailSize = 72.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, nullptr, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			m_DirectoryEntry = directoryEntry;
			bool rootDirectory = m_CurrentDirectory == m_RootDirectory;

			auto relativePath(m_DirectoryEntry.path());
			m_IsDirectory = m_DirectoryEntry.is_directory();

			std::string filenameString = relativePath.filename().string();

			ImGui::PushID(filenameString.c_str());
			// Folder Icon
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
			ImGui::ImageButton((ImTextureID)DirectoryIcon(m_DirectoryEntry)->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			ImGui::PopStyleColor(2);

			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (m_IsDirectory)
				{
					m_SubDirectoryCount++;
					m_CurrentDirectory /= m_DirectoryEntry.path().filename();

					if (rootDirectory)
						m_SubDirectoryMap.clear();
					m_SubDirectoryMap[m_SubDirectoryCount] = m_DirectoryEntry.path();
				}
			}

			if (ImGui::BeginPopupContextItem())
			{
				Utils::CenteredText(Utils::CapitalizeWholeText(filenameString));
				ImGui::Separator();

				if (ImGui::MenuItem("Delete"))
				{
					m_DeleteArgument = true;
					m_DeletePathTarget = m_DirectoryEntry.path();
				}

				ImGui::EndPopup();
			}

			ImGui::TextWrapped(filenameString.c_str());
			ImGui::NextColumn();

			ImGui::PopID();
		}
		ImGui::Columns(1);

		// Right Click Context
		if (ImGui::BeginPopupContextWindow(nullptr, 1, false))
		{
			if (ImGui::BeginMenu("CREATE"))
			{
				ImGui::MenuItem("Animation", nullptr, &Editor::Get().guiAnimationWindow);

				
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void ContentBrowserPanel::NavigationButton()
	{
		// Navigation Button
		bool rootDirectory = m_CurrentDirectory == m_RootDirectory;

		ImVec4 navBtColor;
		uint8_t subDirNumber = m_SubDirectoryCount + 1;

		if (rootDirectory) navBtColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		else navBtColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, navBtColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, navBtColor);

		// Backward Button
		if (ImGui::ImageButton(
			(ImTextureID)m_NavigationIconMap.at("backward_button")->GetRendererID(),
			{ 24, 24 }, { 0, 1 }, { 1, 0 }))
		{
			if (!rootDirectory)
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
				m_SubDirectoryCount--;
			}
		}

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, navBtColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, navBtColor);
		ImGui::ImageButton((ImTextureID)m_NavigationIconMap.at("forward_button")->GetRendererID(), { 24, 24 }, { 0, 1 }, { 1, 0 });
		ImGui::PopStyleColor(3);

		// Forward button
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			if (m_SubDirectoryMap.find(subDirNumber) != m_SubDirectoryMap.end())
			{
				m_SubDirectoryCount++;
				m_CurrentDirectory /= m_SubDirectoryMap.at(subDirNumber).filename();
			}
		}
		ImGui::Separator();
	}

	void ContentBrowserPanel::FileArgument()
	{
		if (m_DeleteArgument)
			DeleteFileArgument();
	}

	void ContentBrowserPanel::DeleteFileArgument()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 8.0f, 7.0f });

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration
			| ImGuiWindowFlags_AlwaysAutoResize
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoFocusOnAppearing
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoNav;

		ImGui::SetNextWindowPos( { ImGui::GetWindowWidth() / 2.0f, ImGui::GetWindowHeight() / 2.0f }, ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background

		if (ImGui::Begin("##delete_argument", nullptr, window_flags))
		{
			Utils::CenteredText("Delete \"" + m_DeletePathTarget.filename().string() + "\"");
			ImGui::Separator();

			Utils::CenteredText("Are you sure ?");

			if (ImGui::Button("Yes", {46.0f, 24.0f}))
			{
				std::filesystem::remove_all(m_DeletePathTarget);
				m_DeleteArgument = false;
			}
			ImGui::SameLine();

			if (ImGui::Button("No", { 46.0f, 24.0f })) m_DeleteArgument = false;
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}
}