#include "pch.h"
#include "ProcessMonitor.h"

namespace origin {

ProcessMonitor::ProcessMonitor()
{
    PdhOpenQuery(NULL, 0, &cpu_query);
    PdhAddCounter(cpu_query, TEXT("\\Processor(_Total)\\% Processor Time"), 0, &cpu_total);
    PdhCollectQueryData(cpu_query);
}

ProcessMonitor::~ProcessMonitor()
{
    PdhCloseQuery(cpu_query);
}

float ProcessMonitor::GetCpuUsage()
{
    PDH_FMT_COUNTERVALUE counter_val;
    PdhCollectQueryData(cpu_query);
    PdhGetFormattedCounterValue(cpu_total, PDH_FMT_DOUBLE, NULL, &counter_val);
    return static_cast<float>(counter_val.doubleValue);
}

size_t ProcessMonitor::GetMemoryUsage()
{
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
        return pmc.WorkingSetSize;
    return 0;
}
}
