// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Renderer\Texture.h"
#include <filesystem>

namespace Origin
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		void OnImGuiRender();

	private:
		std::shared_ptr<Texture2D> m_DirectoryIcon;
		std::shared_ptr<Texture2D> m_FileIcon;
		std::shared_ptr<Texture2D> m_BackButtonIcon;
		std::filesystem::path m_CurrentDirectory;
	};
}