// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include <Origin.h>

#include "RuntimeLayer.h"

namespace origin
{
	class RuntimeApp : public Application
	{
	public:
		RuntimeApp(const ApplicationSpecification& spec)
			: Application(spec)
		{
			PushLayer(new RuntimeLayer());
		}

		~RuntimeApp()
		{
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "Runtime";
		spec.WorkingDirectory = "../ORigin-Editor";
		spec.CommandLineArgs = args;

		OGN_CORE_INFO(spec.Name);
		return new RuntimeApp(spec);
	};
}