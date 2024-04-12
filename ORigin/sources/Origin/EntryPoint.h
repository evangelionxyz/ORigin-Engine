// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Core\Application.h"
#include "Origin\Instrumetation\Instrumentor.h"
#include "Origin\Core\Log.h"

extern origin::Application* origin::CreateApplication(ApplicationCommandLineArgs args);

namespace origin
{
	void Main(int argc, char** argv)
	{
		PROFILER_START("ORiginEngine");
		Log::Init();
		Application* app = CreateApplication({ argc, argv });
		app->Run();
		delete app;
		PROFILER_STOP("ORiginEngine.opt");
	}
}

#if OGN_RELEASE
	#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

int main(int argc, char** argv)
{
	origin::Main(argc, argv);
}