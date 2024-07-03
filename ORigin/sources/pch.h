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

#ifdef OGN_PLATFORM_WINDOWS
	#include <Windows.h>
	#define OGN_DISABLED_WARNINGS 4244 4312 4267 4005
#endif

