// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Core\Application.h"
#include "Origin\Profiler\Profiler.h"
#include "Origin\Core\Log.h"

extern origin::Application* origin::CreateApplication(ApplicationCommandLineArgs args);

namespace origin
{
	void Main(int argc, char** argv)
	{
		Log::Init();
		OGN_PROFILER_START("ORiginEngine");
		Application* app = CreateApplication({ argc, argv });
		app->Run();
		delete app;
		OGN_PROFILER_STOP();
	}
}

#ifdef OGN_PLATFORM_WINDOWS

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{ 
	origin::Main(0, nullptr);
	return 0;
}

#else
int main(int argc, char** argv)
{
	origin::Main(argc, argv);
}
#endif