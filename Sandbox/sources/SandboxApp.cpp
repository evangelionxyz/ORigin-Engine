// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine
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
			Application::Get().GetWindow().SetIcon("Resources/UITextures/icon_origin.png");
			PushLayer(new SandboxLayer());
		}
		~SandboxApp() { }
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "Sandbox";
		spec.WorkingDirectory = "../ORigin-Editor";
		spec.CommandLineArgs = args;

		OGN_CORE_INFO(spec.Name);
		return new SandboxApp(spec);
	};
}