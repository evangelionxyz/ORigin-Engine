// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef PROFILER_H
#define PROFILER_H

#include "optick.h"
#include "Origin/Core/Time.h"

#include <vector>
#include <chrono>
#include <thread>

namespace origin
{
	struct ProfilerResult
	{
		const char *Name;
		float Duration = 0.0;
		int ThreadID = 0;
	};

	template<typename Fn>
	class ProfilerTimer
	{
	public:
		ProfilerTimer(const char *name, Fn&& func)
			: m_Name(name), m_Func(func)
		{
			m_Stopped = false;
			m_StartTimePoint = std::chrono::high_resolution_clock::now();
		}

		~ProfilerTimer()
		{
			if(!m_Stopped)
				Stop();
		}

		void Stop()
		{
			auto endTimePoint = std::chrono::high_resolution_clock::now();
			auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimePoint).time_since_epoch().count();
			auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();
			float duration = (end - start) * 0.001f;
			m_Stopped = true;
			m_Func({ m_Name, duration, 0 });
		}

	private:
		const char *m_Name;
		bool m_Stopped = false;
		Fn m_Func;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimePoint;
		//std::chrono::time_point<std::chrono::steady_clock> m_StartTimePoint;
	};

#ifdef _MSC_VER
	#define __FUNCNAME__ __FUNCSIG__
#endif

#define OGN_ENABLE_PROFILING

#if defined(OGN_ENABLE_PROFILING)
	#define OGN_PROFILER_START(name)        OPTICK_APP(name) OPTICK_START_CAPTURE()
	#define OGN_PROFILER_STOP()             OPTICK_STOP_CAPTURE()
	#define OGN_PROFILER_BEGIN_FRAME(name)	OPTICK_FRAME(name)
	#define OGN_PROFILER_SCOPE(name)        OPTICK_CATEGORY(name, OPTICK_MAKE_CATEGORY(Optick::Filter::GameLogic, Optick::Color::Cyan))
	#define OGN_PROFILER_FUNCTION()         OGN_PROFILER_SCOPE(__FUNCNAME__)
	#define OGN_PROFILER_PHYSICS()          OPTICK_CATEGORY(__FUNCNAME__, OPTICK_MAKE_CATEGORY(Optick::Filter::Physics, Optick::Color::Orange))
	#define OGN_PROFILER_RENDERING()        OPTICK_CATEGORY(__FUNCNAME__, OPTICK_MAKE_CATEGORY(Optick::Filter::Rendering, Optick::Color::Purple))
	#define OGN_PROFILER_INPUT()            OPTICK_CATEGORY(__FUNCNAME__, OPTICK_MAKE_CATEGORY(Optick::Filter::Input, Optick::Color::Sienna))
	#define OGN_PROFILER_LOGIC()            OPTICK_CATEGORY(__FUNCNAME__, OPTICK_MAKE_CATEGORY(Optick::Filter::GameLogic, Optick::Color::DarkRed))
	#define OGN_PROFILER_AUDIO()            OPTICK_CATEGORY(__FUNCNAME__, OPTICK_MAKE_CATEGORY(Optick::Filter::Audio, Optick::Color::ForestGreen)
	#define OGN_PROFILER_SCENE()            OPTICK_CATEGORY(__FUNCNAME__, OPTICK_MAKE_CATEGORY(Optick::Filter::Scene, Optick::Color::Lavender))
#else
	#define OGN_PROFILER_START(name)
	#define OGN_PROFILER_STOP()
	#define OGN_PROFILER_BEGIN_FRAME(name)
	#define OGN_PROFILER_SCOPE(name)
	#define OGN_PROFILER_FUNCTION()
	#define OGN_PROFILER_PHYSICS()
	#define OGN_PROFILER_RENDERING()
	#define OGN_PROFILER_INPUT()
	#define OGN_PROFILER_LOGIC()
	#define OGN_PROFILER_AUDIO()
	#define OGN_PROFILER_SCENE()
#endif
}

#endif