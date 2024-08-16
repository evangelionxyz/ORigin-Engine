// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <utility>
#include <atomic>
#include <thread>
#include <future>
#include <mutex>
#include <shared_mutex>
#include <queue>
#include <algorithm>
#include <functional>
#include <condition_variable>
#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <stack>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <initializer_list>
#include <cstdio>
#include <chrono>
#include <filesystem>
#include <glm/glm.hpp>

#include "Origin/Core/Base.h"
#include "Origin/Core/Assert.h"
#include "Origin/Core/Buffer.h"
#include "Origin/Core/Log.h"
#include "Origin/Profiler/Profiler.h"

#ifdef OGN_PLATFORM_WINDOWS
	#include <Windows.h>
	#include <shellapi.h>
#endif
