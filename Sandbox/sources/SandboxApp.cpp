// Copyright (c) Evangelion Manuhutu | ORigin Engine
#include <Origin.h>
#include "SandboxLayer.h"

namespace origin
{
	class SandboxApp : public Application
	{
	public:
		SandboxApp(const ApplicationSpecification spec)
			: Application(spec)
		{
			PushLayer(new SandboxLayer());
		}

		~SandboxApp() { }
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "Sandbox";
		spec.Width = 600;
		spec.Height = 420;
		spec.WorkingDirectory = "../ORigin-Editor";
		spec.CommandLineArgs = args;

		OGN_CORE_INFO(spec.Name);
		return new SandboxApp(spec);
	};
}