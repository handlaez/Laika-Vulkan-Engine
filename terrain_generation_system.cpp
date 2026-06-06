#include "terrain_generation_system.hpp"

namespace le {

    float TerrainGenerator::getHeight(float worldX, float worldZ)
    {
        // parametry fali
        float amplitude = 7.0f;   
        float frequency = 0.07f;   

        return amplitude * std::sin(worldX * frequency) +
            amplitude * std::cos(worldZ * frequency);
    }

    glm::vec3 TerrainGenerator::getNormal(float worldX, float worldZ)
    {
        float amplitude = 7.0f;
        float frequency = 0.07f;

        float dx = amplitude * frequency * std::cos(worldX * frequency);
        float dz = amplitude * frequency * -std::sin(worldZ * frequency);

        glm::vec3 normal = glm::vec3(-dx, -1.0f, -dz);
        return glm::normalize(normal);
    }

    MeshData TerrainGenerator::generateChunk(int chunkX, int chunkZ)
    {
        MeshData meshData;
        meshData.vertices.reserve(CHUNK_SIZE * CHUNK_SIZE);
        meshData.indices.reserve((CHUNK_SIZE - 1) * (CHUNK_SIZE - 1) * 6);

        float chunkOffset = (CHUNK_SIZE - 1) * CELL_SIZE;

        // vertices gen
        for (uint32_t z = 0; z < CHUNK_SIZE; ++z) {
            for (uint32_t x = 0; x < CHUNK_SIZE; ++x) {
                Vertex vertex{};

                // World Space
                float worldX = (chunkX * chunkOffset) + (x * CELL_SIZE);
                float worldZ = (chunkZ * chunkOffset) + (z * CELL_SIZE);

                vertex.position = { worldX, getHeight(worldX, worldZ), worldZ };
                vertex.normal = getNormal(worldX, worldZ);

                // mapping collor and height (for funsies, for now)
                float heightNorm = (vertex.position.y + 10.0f) / 20.0f;
                vertex.color = { 0.2f + heightNorm * 0.3f, 0.6f + heightNorm * 0.4f, 0.3f };

                vertex.texCoord = {
                    static_cast<float>(x) / (CHUNK_SIZE - 1),
                    static_cast<float>(z) / (CHUNK_SIZE - 1)
                };

                meshData.vertices.push_back(vertex);
            }
        }

        // index gen
        for (uint32_t z = 0; z < CHUNK_SIZE - 1; ++z) {
            for (uint32_t x = 0; x < CHUNK_SIZE - 1; ++x) {
                uint32_t topLeft = z * CHUNK_SIZE + x;
                uint32_t topRight = topLeft + 1;
                uint32_t bottomLeft = (z + 1) * CHUNK_SIZE + x;
                uint32_t bottomRight = bottomLeft + 1;

                meshData.indices.push_back(topLeft);
                meshData.indices.push_back(bottomLeft);
                meshData.indices.push_back(topRight);

                meshData.indices.push_back(topRight);
                meshData.indices.push_back(bottomLeft);
                meshData.indices.push_back(bottomRight);
            }
        }

        return meshData;
    }
}