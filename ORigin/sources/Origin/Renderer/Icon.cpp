// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "icon.h"
#include "stb_image.h"

namespace Origin
{
	Icon::Icon(GLFWwindow* window, const std::string& path)
		: m_Data(nullptr), m_FilePath(path), m_Width(0), m_Height(0), m_nChannels(0)
	{
		stbi_set_flip_vertically_on_load(false);
		m_Data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_nChannels, 4);

		GLFWimage icon[1];
		icon[0].width = m_Width;
		icon[0].height = m_Height;
		icon[0].pixels = m_Data;

		glfwSetWindowIcon(window, 1, icon);

		if (m_Data) {
			stbi_image_free(icon[0].pixels);
		}
	}
}