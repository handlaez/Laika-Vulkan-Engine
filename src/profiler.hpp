#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <iostream>
#include <array>

#include <vulkan/vulkan.h>

#include "le_swapchain.hpp"

struct FrameMetrics {
    double cpuBoidUpdateTimeMs = 0.0;
    double cpuDataFetchTimeMs = 0.0;
    double cpuRenderDispatchTimeMs = 0.0;
    double cpuTotalFrameTimeMs = 0.0;
    double gpuFrameTimeMs = 0.0;
};

struct TimestampQueryResult {
    uint64_t timestamp = 0;
    uint64_t available = 0;
};

namespace Profiler {

    void Initialize(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        size_t expectedFrames = 10000
    );

    void Shutdown();

    void StartFrame();
    void EndFrame();

    void StartBoidUpdate();
    void EndBoidUpdate();

    void StartDataFetch();
    void EndDataFetch();

    void StartRenderDispatch();
    void EndRenderDispatch();

    void ResetQueries(VkCommandBuffer cmd);

    void WriteTimestampStart(VkCommandBuffer cmd);
    void WriteTimestampEnd(VkCommandBuffer cmd);

    void CollectFrameResults();

    void ExportToCSV(const std::string& filename);
}