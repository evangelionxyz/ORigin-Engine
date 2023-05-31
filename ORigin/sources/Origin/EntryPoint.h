// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Core\Application.h"
#include "Origin\Core\Log.h"

#if OGN_RELEASE
	#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

extern origin::Application* origin::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	origin::Log::Init();
	origin::Application* app = origin::CreateApplication({argc, argv});
	app->Run();
	delete app;
}