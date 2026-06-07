#include "terrain_generation_system.hpp"

#include <stdexcept>
#include <array>

namespace le {

    // struct to send the chunk coordinates to the compute shader
    struct ComputePushConstantData {
        glm::ivec2 chunkOffset;
        float seed;
    };

    TerrainGenerator::TerrainGenerator(LeDevice& device, uint32_t chunkSize, float cellSize, int renderDistance)
        : leDevice(device), m_chunkSize(chunkSize), m_cellSize(cellSize), m_renderDistance(renderDistance), m_gridSize(renderDistance * 2 + 1)
    {
        createDescriptorLayout();
        createPipelineLayout();
        
        createDescriptorSets(m_gridSize * m_gridSize);

        computePipeline = std::make_unique<LeComputePipeline>(
            leDevice, "shaders/terrain.comp.spv", pipelineLayout
        );
    }

    TerrainGenerator::~TerrainGenerator()
    {
        vkDestroyDescriptorPool(leDevice.device(), descriptorPool, nullptr);
        vkDestroyPipelineLayout(leDevice.device(), pipelineLayout, nullptr);
        vkDestroyDescriptorSetLayout(leDevice.device(), descriptorSetLayout, nullptr);
    }

    void TerrainGenerator::createDescriptorLayout()
    {
        // binding 0: storage buffer
        VkDescriptorSetLayoutBinding storageLayoutBinding{};
        storageLayoutBinding.binding = 0;
        storageLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        storageLayoutBinding.descriptorCount = 1;
        storageLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &storageLayoutBinding;

        if (vkCreateDescriptorSetLayout(leDevice.device(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create compute descriptor set layout!");
        }

        // max of 1000 chunks for now
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        poolSize.descriptorCount = 1000;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = 1000;

        if (vkCreateDescriptorPool(leDevice.device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create compute descriptor pool!");
        }
    }

    void TerrainGenerator::createPipelineLayout()
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(ComputePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(leDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create compute pipeline layout!");
        }
    }

    void TerrainGenerator::generateChunk(const LeChunk& chunk, VkCommandBuffer cmd, const float seed, int bufferIndex)
    {
        VkDescriptorSet descriptorSet = m_descriptorSets[bufferIndex];

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = chunk.getVertexBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = VK_WHOLE_SIZE;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSet;
        descriptorWrite.dstBinding = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(leDevice.device(), 1, &descriptorWrite, 0, nullptr);

        computePipeline->bind(cmd);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

        ComputePushConstantData pushData{ {chunk.getChunkX(), chunk.getChunkZ()}, seed };
        vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstantData), &pushData);

        vkCmdDispatch(cmd, m_chunkSize / 8, m_chunkSize / 8, 1);
    }

    void TerrainGenerator::createDescriptorSets(int totalChunks) {
        m_descriptorSets.resize(totalChunks);

        std::vector<VkDescriptorSetLayout> layouts(totalChunks, descriptorSetLayout);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(totalChunks);
        allocInfo.pSetLayouts = layouts.data();

        if (vkAllocateDescriptorSets(leDevice.device(), &allocInfo, m_descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate pre-allocated descriptor sets!");
        }
    }
}