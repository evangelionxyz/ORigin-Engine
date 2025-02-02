#pragma once

namespace origin {

#ifdef OGN_PLATFORM_WINDOWS
#include <Windows.h>
#include <Psapi.h>
#include <Pdh.h>
#pragma comment(lib, "pdh.lib")
#endif

#define HISTORY_SIZE 100

class ProcessMonitor
{
public:
    ProcessMonitor();
    ~ProcessMonitor();
    float GetCpuUsage();
    size_t GetMemoryUsage();

private:
#ifdef OGN_PLATFORM_WINDOWS
    PDH_HQUERY cpu_query;
    PDH_HCOUNTER cpu_total;
#endif
};
}

