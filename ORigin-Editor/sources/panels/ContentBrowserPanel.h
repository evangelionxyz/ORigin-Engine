// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Renderer\Texture.h"

#include <string>
#include <filesystem>
#include <unordered_map>

namespace origin
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		~ContentBrowserPanel();
		void OnImGuiRender();

	private:
		void FileButton();
		void NavigationButton();
		void FileArgument();
		void DeleteFileArgument();

		std::shared_ptr<Texture2D> DirectoryIcon(const std::filesystem::directory_entry& dirEntry);

		std::unordered_map <std::string, std::shared_ptr<Texture2D>> m_DirectoryIconMap;
		std::unordered_map <std::string, std::shared_ptr<Texture2D>> m_NavigationIconMap;

		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_DeletePathTarget;
		std::filesystem::directory_entry m_DirectoryEntry;

		std::unordered_map<uint8_t, std::filesystem::path> m_SubDirectoryMap;

		uint8_t m_SubDirectoryCount = 0;
		bool m_IsDirectory;
		bool m_DeleteArgument = false;
	};
}