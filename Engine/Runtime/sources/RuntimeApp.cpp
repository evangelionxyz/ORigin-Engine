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
		spec.CommandLineArgs = args;
		spec.Width = 1280;
		spec.Height = 640;
		OGN_CORE_INFO(spec.Name.c_str());
		return new RuntimeApp(spec);
	};
}