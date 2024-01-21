// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <utility>
#include <atomic>
#include <thread>
#include <algorithm>
#include <functional>
#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <initializer_list>
#include <cstdio>
#include <chrono>
#include <filesystem>

#include "Origin\Core\Base.h"
#include "Origin\Core\Assert.h"
#include "Origin\Core\Buffer.h"
#include "Origin\Core\Log.h"

#include <glm\glm.hpp>

#ifdef OGN_WINDOWS_PLATFORM
	#include <Windows.h>
#endif