// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Renderer\Texture.h"
#include <filesystem>
#include <unordered_map>

namespace Origin
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		void OnImGuiRender();

	private:

		std::shared_ptr<Texture2D> m_DirectoryIcon;
		std::shared_ptr<Texture2D> m_BackButtonIcon;

		std::unordered_map <std::string, std::shared_ptr<Texture2D>> m_DirectoryIconMap;
		std::shared_ptr<Texture2D> DirectoryIcon(std::filesystem::directory_entry dirEntry);

		std::filesystem::path m_CurrentDirectory;
	};
}