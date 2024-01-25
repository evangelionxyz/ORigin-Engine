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

#define OGN_DISABLED_WARNINGS                         \
    4005 4180 4412 4455 4494 4514 4574 4582 4583 4587 \
    4588 4619 4623 4625 4626 4643 4648 4702 4793 4820 \
    4988 5026 5027 5045 6294 4244 4312 4267


