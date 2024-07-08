// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Core/Application.h"
#include "Origin/Profiler/Profiler.h"
#include "Origin/Core/Log.h"

extern origin::Application* origin::CreateApplication(ApplicationCommandLineArgs args);

namespace origin
{
	int Main(int argc, char** argv)
	{
		Log::Init();
		OGN_PROFILER_START("ORiginEngine");
		Application* app = CreateApplication({ argc, argv });
		app->Run();
		delete app;
		OGN_PROFILER_STOP();
		return 0;
	}
}

int main(int argc, char** argv)
{
	return origin::Main(argc, argv);
}
