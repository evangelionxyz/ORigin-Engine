// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "optick.h"

#pragma once
namespace origin
{
#define PROFILER_START(ENGINE_NAME) OPTICK_APP(ENGINE_NAME)\
	OPTICK_START_CAPTURE()
#define PROFILER_STOP(SAVE_FILE) OPTICK_STOP_CAPTURE()\
	OPTICK_SAVE_CAPTURE(SAVE_FILE)
#define PROFILER_BEGIN_FRAME(FRAME_NAME) OPTICK_FRAME(FRAME_NAME)

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif 

#define PROFILER_SCOPED(ARGS) OPTICK_CATEGORY(ARGS, OPTICK_MAKE_CATEGORY(Optick::Filter::None, Optick::Color::Cyan))
#define PROFILER_FUNCTION() OPTICK_CATEGORY(__PRETTY_FUNCTION__, OPTICK_MAKE_CATEGORY(Optick::Filter::None, Optick::Color::Azure))
#define PROFILER_PHYSICS() OPTICK_CATEGORY(__PRETTY_FUNCTION__, OPTICK_MAKE_CATEGORY(Optick::Filter::Physics, Optick::Color::Orange))
#define PROFILER_RENDERING() OPTICK_CATEGORY(__PRETTY_FUNCTION__, OPTICK_MAKE_CATEGORY(Optick::Filter::Rendering, Optick::Color::Purple))
#define PROFILER_INPUT() OPTICK_CATEGORY(__PRETTY_FUNCTION__, OPTICK_MAKE_CATEGORY(Optick::Filter::Input, Optick::Color::Sienna))
#define PROFILER_LOGIC() OPTICK_CATEGORY(__PRETTY_FUNCTION__, OPTICK_MAKE_CATEGORY(Optick::Filter::GameLogic, Optick::Color::DarkRed))
#define PROFILER_AUDIO() OPTICK_CATEGORY(__PRETTY_FUNCTION__, OPTICK_MAKE_CATEGORY(Optick::Filter::Audio, Optick::Color::ForestGreen)
#define PROFILER_SCENE() OPTICK_CATEGORY(__PRETTY_FUNCTION__, OPTICK_MAKE_CATEGORY(Optick::Filter::Scene, Optick::Color::Lavender))
#define PROFILER_UI() OPTICK_CATEGORY(__PRETTY_FUNCTION__, OPTICK_MAKE_CATEGORY(Optick::Filter::UI, Optick::Color::SeaGreen))

}