// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine
#include <Origin.h>
#include "..\Sandbox.h"

namespace origin
{
	class SandboxBuild : public Application
	{
	public:
		SandboxBuild(const ApplicationSpecification spec)
			: Application(spec)
		{
			Application::Get().GetWindow().SetIcon("Resources/UITextures/icon_origin.png");
			PushLayer(new Sandbox());
		}
		~SandboxBuild() { }
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "Sandbox";
		spec.WorkingDirectory = "../ORigin-Editor";
		spec.CommandLineArgs = args;

		OGN_CORE_INFO(spec.Name);
		return new SandboxBuild(spec);
	};
}