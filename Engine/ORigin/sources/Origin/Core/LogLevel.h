// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef LOGLEVEL_H
#define LOGLEVEL_H

#include "Base.h"

namespace origin
{
    enum LogLevel
    {
        Info     = BIT(1),
        Warning  = BIT(2),
        Error    = BIT(3),
        Trace    = BIT(4),
        Critical = BIT(5)
    };
}

#endif //LOGLEVEL_H
