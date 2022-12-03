// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Core\Application.h"

#ifdef OGN_DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <cstdlib>
	#include <crtdbg.h>
#endif

#if OGN_RELEASE
	#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

extern Origin::Application* Origin::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
#ifdef OGN_DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Origin::Log::Init();
	Origin::Application* app = Origin::CreateApplication({argc, argv});
	app->Run();
	delete app;

#ifdef OGN_DEBUG
	_CrtDumpMemoryLeaks();
#endif
}