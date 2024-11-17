// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Origin/Core/Log.h"

namespace origin {
static Log *s_Instance = nullptr;
Log::Log()
{
    s_Instance = this;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
}

Log *Log::GetInstance()
{
    return s_Instance;
}

}
