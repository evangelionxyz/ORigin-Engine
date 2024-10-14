// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef ENTRY_POINT_H
#define ENTRY_POINT_H

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

#ifdef OGN_PLATFORM_WINDOWS
    #pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

int main(const int argc, char **argv)
{
#ifdef OGN_PLATFORM_WINDOWS
    #ifndef OGN_DISTRIBUTION
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        freopen("CONIN$", "r", stdin);
    #endif
#endif

    origin::Main(argc, argv);

#ifdef OGN_PLATFORM_WINDOWS
    #ifndef OGN_DISTRIBUTION
        FreeConsole();
    #endif
#endif

    return 0;
}

#endif