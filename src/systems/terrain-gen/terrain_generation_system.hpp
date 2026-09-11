#ifndef TERRAIN_GENERATION_SYSTEM_HPP
#define TERRAIN_GENERATION_SYSTEM_HPP

#include "i_terrain_generator.hpp"
#include "src/objects/mesh_data.hpp"
#include "src/render/le_compute_pipeline.hpp"

#include <glm/glm.hpp>

namespace le {
    class TerrainGenerator : public ITerrainGenerator {
    public:
        TerrainGenerator(LeDevice& device, uint32_t chunkSize, float cellSize, int renderDistance);
        ~TerrainGenerator();

        void generateChunk(const LeChunk& chunk, VkCommandBuffer cmd, const float seed, int bufferIndex) override;

    private:
        LeDevice& leDevice;

        std::vector<VkDescriptorSet> m_descriptorSets;
        void createDescriptorSets(int totalChunks);

        const uint32_t m_chunkSize;
        const float m_cellSize;
        const int m_renderDistance;
        const int m_gridSize;

        std::unique_ptr<LeComputePipeline> computePipeline;
        VkPipelineLayout pipelineLayout;
        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorPool descriptorPool;

        void createDescriptorLayout();
        void createPipelineLayout();
    };
}

#endif