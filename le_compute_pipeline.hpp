#ifndef LE_COMPUTE_PIPELINE_HPP
#define LE_COMPUTE_PIPELINE_HPP

#include "le_device.hpp"

#include <string>
#include <vector>

namespace le {
    class LeComputePipeline {
    public:
        LeComputePipeline(LeDevice& device, const std::string& shaderPath, VkPipelineLayout layout);
        ~LeComputePipeline();

        void bind(VkCommandBuffer cmd);

    private:
        LeDevice& device;
        VkPipeline pipeline;
        VkShaderModule shaderModule;
    };
}

#endif
