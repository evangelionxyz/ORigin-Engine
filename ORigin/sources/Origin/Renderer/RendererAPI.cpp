// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "RendererAPI.h"

namespace origin
{
	static RendererAPI::API s_API;

	RendererAPI::API RendererAPI::GetAPI()
	{
		return s_API;
	}

	void RendererAPI::SetAPI(API api)
	{
		s_API = api;
	}
}