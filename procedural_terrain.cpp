#include "procedural_terrain.hpp"
#include <cmath>

namespace le {

    ProceduralTerrain::ProceduralTerrain(int renderDistance)
        : m_renderDistance(renderDistance),
        m_gridSize(renderDistance * 2 + 1),
        m_lastChunkX(0), m_lastChunkZ(0)
    {
        m_physicalChunkSize = (TerrainGenerator::CHUNK_SIZE - 1) * TerrainGenerator::CELL_SIZE;
    }

    int ProceduralTerrain::getBufferIndex(int chunkX, int chunkZ)
    {
        int wrappedX = ((chunkX % m_gridSize) + m_gridSize) % m_gridSize;
        int wrappedZ = ((chunkZ % m_gridSize) + m_gridSize) % m_gridSize;
        return wrappedX + (wrappedZ * m_gridSize);
    }

    void ProceduralTerrain::init(LeScene& scene, glm::vec3 startPos)
    {
        m_chunkRingBuffer.resize(m_gridSize * m_gridSize);

        m_lastChunkX = std::floor(startPos.x / m_physicalChunkSize);
        m_lastChunkZ = std::floor(startPos.z / m_physicalChunkSize);

        for (int z = -m_renderDistance; z <= m_renderDistance; ++z) {
            for (int x = -m_renderDistance; x <= m_renderDistance; ++x) {

                int worldChunkX = m_lastChunkX + x;
                int worldChunkZ = m_lastChunkZ + z;

                MeshData chunkData = TerrainGenerator::generateChunk(worldChunkX, worldChunkZ);
                uint32_t modelID = scene.leResourceManager.addModel(chunkData);

                LeActor terrainActor = LeActor::createGameObject();
                terrainActor.modelID = modelID;
                terrainActor.textureID = 0;

                float worldX = worldChunkX * m_physicalChunkSize;
                float worldZ = worldChunkZ * m_physicalChunkSize;
                terrainActor.transform.translation = glm::vec3(worldX, 0.0f, worldZ);

                int index = getBufferIndex(worldChunkX, worldChunkZ);
                m_chunkRingBuffer[index] = { modelID };

                uint32_t actorIndex = scene.addActor(std::move(terrainActor));
                m_chunkRingBuffer[index] = { modelID, actorIndex };
            }
        }
    }

    void ProceduralTerrain::update(LeScene& scene, glm::vec3 currentPos)
    {
        int currentChunkX = std::floor(currentPos.x / m_physicalChunkSize);
        int currentChunkZ = std::floor(currentPos.z / m_physicalChunkSize);

        if (currentChunkX != m_lastChunkX) {
            int deltaX = currentChunkX > m_lastChunkX ? 1 : -1;

            int newX = currentChunkX + (m_renderDistance * deltaX);
            int oldX = m_lastChunkX - (m_renderDistance * deltaX);

            for (int z = currentChunkZ - m_renderDistance; z <= currentChunkZ + m_renderDistance; ++z) {
                int bufferIndex = getBufferIndex(oldX, z);

                MeshData newChunkData = TerrainGenerator::generateChunk(newX, z);
                ChunkSlot& slot = m_chunkRingBuffer[bufferIndex];

                scene.leResourceManager.updateModel(slot.modelID, newChunkData);

                float newWorldX = newX * m_physicalChunkSize;
                float newWorldZ = z * m_physicalChunkSize;
                scene.getActor(slot.actorIndex).transform.translation = glm::vec3(newWorldX, 0.0f, newWorldZ);
            }
            m_lastChunkX = currentChunkX;
        }

        if (currentChunkZ != m_lastChunkZ) {
            int deltaZ = currentChunkZ > m_lastChunkZ ? 1 : -1;

            int newZ = currentChunkZ + (m_renderDistance * deltaZ);
            int oldZ = m_lastChunkZ - (m_renderDistance * deltaZ);

            for (int x = currentChunkX - m_renderDistance; x <= currentChunkX + m_renderDistance; ++x) {
                int bufferIndex = getBufferIndex(x, oldZ);

                MeshData newChunkData = TerrainGenerator::generateChunk(x, newZ);
                ChunkSlot& slot = m_chunkRingBuffer[bufferIndex];

                scene.leResourceManager.updateModel(slot.modelID, newChunkData);

                float newWorldX = x * m_physicalChunkSize;
                float newWorldZ = newZ * m_physicalChunkSize;
                scene.getActor(slot.actorIndex).transform.translation = glm::vec3(newWorldX, 0.0f, newWorldZ);
            }
            m_lastChunkZ = currentChunkZ;
        }
    }

} // namespace le