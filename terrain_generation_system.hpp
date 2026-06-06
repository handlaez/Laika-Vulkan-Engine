#ifndef TERRAIN_GENERATION_SYSTEM_HPP
#define TERRAIN_GENERATION_SYSTEM_HPP

#include "mesh_data.hpp"
#include <glm/glm.hpp>

namespace le {

    class TerrainGenerator {
    public:
        static constexpr uint32_t CHUNK_SIZE = 64;
        static constexpr float CELL_SIZE = 1.0f;

        static MeshData generateChunk(int chunkX, int chunkZ);

    private:
        static float getHeight(float worldX, float worldZ);

        static glm::vec3 getNormal(float worldX, float worldZ);
    };

}

#endif