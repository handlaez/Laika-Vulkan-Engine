#include "profiler.hpp"

#include "le_swapchain.hpp"

namespace 
{
    using Clock = std::chrono::high_resolution_clock;

    struct ProfilerState 
    {
        VkDevice device = VK_NULL_HANDLE;
        VkQueryPool queryPool = VK_NULL_HANDLE;

        uint32_t currentFrame = 0;

        float timestampPeriod = 0.0f;

        std::vector<FrameMetrics> frames;
        std::vector<FrameMetrics> perFrameMetrics;

        Clock::time_point frameStart;
        Clock::time_point boidStart;
        Clock::time_point dataStart;
        Clock::time_point renderStart;
    };

    ProfilerState gProfiler;

    double GetElapsedMs(Clock::time_point start) 
    {
        auto end = Clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        return elapsed.count();
    }

    uint32_t GetQueryBase(uint32_t frameIndex) 
    {
        return frameIndex * 2;
    }

    FrameMetrics& CurrentMetrics() 
    {
        return gProfiler.perFrameMetrics[gProfiler.currentFrame];
    }

    void CreateQueryPool() 
    {
        VkQueryPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        info.queryType = VK_QUERY_TYPE_TIMESTAMP;
        info.queryCount = le::LeSwapchain::MAX_FRAMES_IN_FLIGHT * 2;

        if (vkCreateQueryPool(
            gProfiler.device,
            &info,
            nullptr,
            &gProfiler.queryPool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create query pool");
        }
    }
}

namespace Profiler 
{
    void Initialize(VkDevice device, VkPhysicalDevice physicalDevice, size_t expectedFrames)
    {
        gProfiler.device = device;
        gProfiler.frames.reserve(expectedFrames);
        gProfiler.perFrameMetrics.resize(le::LeSwapchain::MAX_FRAMES_IN_FLIGHT);

        VkPhysicalDeviceProperties props{};
        vkGetPhysicalDeviceProperties(physicalDevice, &props);

        gProfiler.timestampPeriod = props.limits.timestampPeriod;
        gProfiler.currentFrame = UINT32_MAX;

        CreateQueryPool();
    }

    void Shutdown() 
    {
        if (gProfiler.queryPool != VK_NULL_HANDLE) {

            vkDestroyQueryPool(
                gProfiler.device,
                gProfiler.queryPool,
                nullptr
            );

            gProfiler.queryPool = VK_NULL_HANDLE;
        }
    }

    void StartFrame() 
    {
        gProfiler.currentFrame = (gProfiler.currentFrame + 1) % le::LeSwapchain::MAX_FRAMES_IN_FLIGHT;
        gProfiler.frameStart = Clock::now();
    }

    void EndFrame() 
    {
        CurrentMetrics().cpuTotalFrameTimeMs = GetElapsedMs(gProfiler.frameStart);
    }

    void StartBoidUpdate() 
    {
        gProfiler.boidStart = Clock::now();
    }

    void EndBoidUpdate() 
    {
        CurrentMetrics().cpuBoidUpdateTimeMs = GetElapsedMs(gProfiler.boidStart);
    }

    void StartDataFetch() 
    {
        gProfiler.dataStart = Clock::now();
    }

    void EndDataFetch() 
    {
        CurrentMetrics().cpuDataFetchTimeMs = GetElapsedMs(gProfiler.dataStart);
    }

    void StartRenderDispatch() 
    {
        gProfiler.renderStart = Clock::now();
    }

    void EndRenderDispatch() 
    {
        CurrentMetrics().cpuRenderDispatchTimeMs = GetElapsedMs(gProfiler.renderStart);
    }

    void ResetQueries(VkCommandBuffer cmd) 
    {
        uint32_t queryBase = GetQueryBase(gProfiler.currentFrame);

        vkCmdResetQueryPool(
            cmd,
            gProfiler.queryPool,
            queryBase,
            2
        );
    }

    void WriteTimestampStart(VkCommandBuffer cmd) 
    {
        vkCmdWriteTimestamp(
            cmd,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            gProfiler.queryPool,
            GetQueryBase(gProfiler.currentFrame)
        );
    }

    void WriteTimestampEnd(VkCommandBuffer cmd) 
    {
        vkCmdWriteTimestamp(
            cmd,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            gProfiler.queryPool,
            GetQueryBase(gProfiler.currentFrame) + 1
        );
    }

    void CollectFrameResults() 
    {
        uint32_t queryBase = GetQueryBase(gProfiler.currentFrame);

        std::array<TimestampQueryResult, 2> results{};

        VkResult result = vkGetQueryPoolResults(
            gProfiler.device,
            gProfiler.queryPool,
            queryBase,
            2,
            sizeof(results),
            results.data(),
            sizeof(TimestampQueryResult),
            VK_QUERY_RESULT_64_BIT |
            VK_QUERY_RESULT_WITH_AVAILABILITY_BIT
        );

        if (result != VK_SUCCESS)
            return;

        if (!results[0].available || !results[1].available)
            return;

        uint64_t delta = results[1].timestamp - results[0].timestamp;

        double gpuTimeNs = static_cast<double>(delta) * gProfiler.timestampPeriod;

        gProfiler.perFrameMetrics[gProfiler.currentFrame].gpuFrameTimeMs = gpuTimeNs / 1'000'000.0;
        gProfiler.frames.push_back(gProfiler.perFrameMetrics[gProfiler.currentFrame]);
    }

    void ExportToCSV(const std::string& filename) {

        std::ofstream file(filename);

        if (!file.is_open()) {
            std::cerr << "Failed to open profiler output file\n";
            return;
        }

        file << "Frame\t" << "CPU Total (ms)\t" << "CPU Boids (ms)\t" << "CPU Data Update (ms)\t"
            << "CPU Render Dispatch (ms)\t" << "GPU Render (ms)\n";

        for (size_t i = 0; i < gProfiler.frames.size(); ++i) {

            const auto& f = gProfiler.frames[i];

            file << i << "\t"<< f.cpuTotalFrameTimeMs << "\t" << f.cpuBoidUpdateTimeMs << "\t" << f.cpuDataFetchTimeMs
                << "\t" << f.cpuRenderDispatchTimeMs << "\t" << f.gpuFrameTimeMs << "\n";
        }

        std::cout << "Saved " << gProfiler.frames.size() << " frames to " << filename << std::endl;
    }
}