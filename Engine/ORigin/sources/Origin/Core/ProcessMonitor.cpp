#include "pch.h"
#include "ProcessMonitor.h"

namespace origin {

ProcessMonitor::ProcessMonitor()
{
#ifdef OGN_PLATFORM_WINDOWS
    PdhOpenQuery(NULL, 0, &cpu_query);
    PdhAddCounter(cpu_query, TEXT("\\Processor(_Total)\\% Processor Time"), 0, &cpu_total);
    PdhCollectQueryData(cpu_query);
#endif
}

ProcessMonitor::~ProcessMonitor()
{
#ifdef OGN_PLATFORM_WINDOWS
    PdhCloseQuery(cpu_query);
#endif
}

float ProcessMonitor::GetCpuUsage()
{
#ifdef OGN_PLATFORM_WINDOWS
    PDH_FMT_COUNTERVALUE counter_val;
    PdhCollectQueryData(cpu_query);
    PdhGetFormattedCounterValue(cpu_total, PDH_FMT_DOUBLE, NULL, &counter_val);
    return static_cast<float>(counter_val.doubleValue);
#endif
    return 0.0f;
}

size_t ProcessMonitor::GetMemoryUsage()
{
#ifdef OGN_PLATFORM_WINDOWS
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
        return pmc.WorkingSetSize;
#endif
    return 0;
}
}
