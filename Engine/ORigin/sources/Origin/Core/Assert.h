// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef ASSERT_H
#define ASSERT_H

#include "Base.h"
#include "Log.h"

#include <filesystem>

#ifdef OGN_DEBUG
    // Assert with condition and additional arguments
#define OGN_CORE_ASSERT(check, ...) \
        do { \
            if(!(check)) { \
                OGN_CORE_ERROR(__VA_ARGS__); \
                OGN_DEBUGBREAK(); \
            } \
        } while(0)

    // Assert with only condition
#define OGN_CORE_ASSERT_SIMPLE(check) \
        do { \
            if(!(check)) { \
                OGN_CORE_ERROR("Assertion '{}' failed at {}:{}", #check, __FILE__, __LINE__); \
                OGN_DEBUGBREAK(); \
            } \
        } while(0)
#else
#define OGN_CORE_ASSERT(...)
#define OGN_CORE_ASSERT_SIMPLE(check)
#endif

#endif