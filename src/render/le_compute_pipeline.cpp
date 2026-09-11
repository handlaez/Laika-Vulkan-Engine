#include "le_compute_pipeline.hpp"

#include <fstream>
#include <iostream>
#include <cassert>

namespace le 
{
	LeComputePipeline::LeComputePipeline(LeDevice& device, const std::string& shaderPath, VkPipelineLayout layout)
		: device(device)
    {
        std::ifstream file(shaderPath, std::ios::ate | std::ios::binary);
        if (!file.is_open()) throw std::runtime_error("failed to open compute shader: " + shaderPath);

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = buffer.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

        if (vkCreateShaderModule(device.device(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
            throw std::runtime_error("failed to create compute shader module");

        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.layout = layout;
        pipelineInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipelineInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        pipelineInfo.stage.module = shaderModule;
        pipelineInfo.stage.pName = "main";

        if (vkCreateComputePipelines(device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
            throw std::runtime_error("failed to create compute pipeline");
    }

    LeComputePipeline::~LeComputePipeline()
    {
        vkDestroyShaderModule(device.device(), shaderModule, nullptr);
        vkDestroyPipeline(device.device(), pipeline, nullptr);
    }

    void LeComputePipeline::bind(VkCommandBuffer cmd)
    {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
    }
}
