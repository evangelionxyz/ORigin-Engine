// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Core\Application.h"

#if OGN_RELEASE
	#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

extern Origin::Application* Origin::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	Origin::Log::Init();
	Origin::Application* app = Origin::CreateApplication({argc, argv});
	app->Run();
	delete app;
}