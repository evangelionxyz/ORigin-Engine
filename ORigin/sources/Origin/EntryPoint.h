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

#if OGN_RELEASE
	#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

int main(int argc, char** argv)
{
	origin::Main(argc, argv);
}