// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once

namespace Origin
{
	class ScriptEngine
	{
	public:
		static void Init();

		static void Shutdown();
	private:
		static void InitMono();
		static void ShutdownMono();

	};

}

