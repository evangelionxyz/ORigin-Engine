// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef ENTRY_PINT_H
#define ENTRY_PINT_H

#include "Origin/Core/Application.h"
#include "Origin/Profiler/Profiler.h"
#include "Origin/Core/Log.h"

namespace origin
{
	int Main(const int argc, char** argv)
	{
		auto logger = Log();
		OGN_PROFILER_START("ORiginEngine");
		Application* app = CreateApplication({ argc, argv });
		app->Run();
		delete app;
		OGN_PROFILER_STOP();
		return 0;
	}
}

#ifdef OGN_DISTRIBUTION
	#ifdef OGN_PLATFORM_WINDOWS
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

int main(const int argc, char **argv)
{
	return origin::Main(argc, argv);
}

#endif