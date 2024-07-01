// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include <Origin.h>

#include "RuntimeLayer.h"

namespace origin
{
	class RuntimeApp : public Application
	{
	public:
		RuntimeApp(ApplicationSpecification& spec)
			: Application(spec)
		{
			PushLayer(new RuntimeLayer());
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "Runtime";
		spec.WorkingDirectory = "../ORigin-Editor/";
		spec.CommandLineArgs = args;
		spec.Width = 1280;
		spec.Height = 640;
		spec.Maximize = true;
		spec.Runtime = true;

		OGN_CORE_INFO(spec.Name);
		return new RuntimeApp(spec);
	};
}