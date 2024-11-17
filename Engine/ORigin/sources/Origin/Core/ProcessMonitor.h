#pragma once

namespace origin {
#include <Windows.h>
#include <Psapi.h>
#include <Pdh.h>

#pragma comment(lib, "pdh.lib")

#define HISTORY_SIZE 100

class ProcessMonitor
{
public:
    ProcessMonitor();
    ~ProcessMonitor();
    float GetCpuUsage();
    size_t GetMemoryUsage();

private:
    PDH_HQUERY cpu_query;
    PDH_HCOUNTER cpu_total;
};
}

