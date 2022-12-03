// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "pch.h"
#include <GLFW\glfw3.h>

namespace Origin
{
	class Icon
	{
	private:
		unsigned char* m_Data;
		int m_Width, m_Height, m_nChannels;
		std::string m_FilePath;
	public:
		Icon(GLFWwindow* window, const std::string& path);

	};

}