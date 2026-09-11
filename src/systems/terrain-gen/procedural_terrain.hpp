#ifndef PROCEDURAL_TERRAIN_HPP
#define PROCEDURAL_TERRAIN_HPP

#include "terrain_generation_system.hpp"
#include "le_chunk.hpp"
#include "le_buffer.hpp"
#include "le_mutable_model.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace le {
    struct ChunkSlot
    {
        std::unique_ptr<LeChunk> chunk;
        uint32_t actorIndex;
    };

    class LeScene;

    class ProceduralTerrain
    {
    public:
        ProceduralTerrain(LeDevice& device, int renderDistance, float seed, uint32_t chunkSize, float cellSize);
        ~ProceduralTerrain();

        void init(LeScene& scene, glm::vec3 startPos, VkCommandBuffer computeCmd);
        void update(LeScene& scene, glm::vec3 currentPos, VkCommandBuffer computeCmd);

        bool needsUpdate(glm::vec3 currentPos);

    private:
        int getBufferIndex(int chunkX, int chunkZ);
        void generateSharedIndexBuffer(); // generates the 64x64 grid indices once (it's the same for all chunks!)

        LeDevice& leDevice;
        std::unique_ptr<TerrainGenerator> m_generator;

        std::unique_ptr<LeBuffer> m_sharedIndexBuffer;
        uint32_t m_sharedIndexCount;

        uint32_t m_chunkSize;
        float m_cellSize;
        float m_seed;
        int m_renderDistance;
        int m_gridSize;
        int m_lastChunkX;
        int m_lastChunkZ;
        float m_physicalChunkSize;

        std::vector<ChunkSlot> m_chunkRingBuffer;
    };
}

#endif