// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include <chrono>

namespace origin
{
	class Timestep
	{
	public:
		Timestep(double time = 0.0)
			: m_Time(time), m_DeltaTime(0.0)
{
			s_Instance = this;
		}

		operator double() const { return m_Time; }
		static double Seconds() { return s_Instance->m_Time; }
		static double MilliSeconds() { return s_Instance->m_Time * 1000.0; }
		static double DeltaTime() { return s_Instance->m_DeltaTime; }

		void SetDeltaTime(double deltaTime)
		{
			m_DeltaTime = deltaTime;
		}

		static Timestep& Get() { return *s_Instance; }

	private:
		static Timestep* s_Instance;
		double m_Time;
		double m_DeltaTime;
	};

	class Timer
	{
	public:
		Timer() {
			Reset();
		}

		void Timer::Reset()
		{
			m_Start = std::chrono::high_resolution_clock::now();
		}

		double Timer::Elapsed()
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001 * 0.001 * 0.001;
		}

		double Timer::ElapsedMillis()
		{
			return Elapsed() * 1000.0;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
	};

}