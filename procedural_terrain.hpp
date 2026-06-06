#ifndef PROCEDURAL_TERRAIN_HPP
#define PROCEDURAL_TERRAIN_HPP

#include "le_scene.hpp"
#include "terrain_generation_system.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace le {
    struct ChunkSlot 
    {
        uint32_t modelID;
        uint32_t actorIndex;
    };

    class ProceduralTerrain 
    {
    public:
        ProceduralTerrain(int renderDistance);

        void init(LeScene& scene, glm::vec3 startPos);
        void update(LeScene& scene, glm::vec3 currentPos);

    private:
        int getBufferIndex(int chunkX, int chunkZ);

        int m_renderDistance;
        int m_gridSize;
        int m_lastChunkX;
        int m_lastChunkZ;
        float m_physicalChunkSize;

        std::vector<ChunkSlot> m_chunkRingBuffer;
    };
}

#endif