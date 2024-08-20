// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef RANDOM_H
#define RANDOM_H

#include <random>

namespace origin {

	class Random
	{
	public:
		static void Init()
		{
			s_RandomEngine.seed(std::random_device()());
		}

		static float Float()
		{
			return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
		}

		static float Float(float low, float high)
		{
			float r = (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
			r = (high - low) * r + low;
			return r;
		}

	private:
		static std::mt19937 s_RandomEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
	};
}

#endif