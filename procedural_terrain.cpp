#include "procedural_terrain.hpp"
#include "le_scene.hpp"

#include <cmath>
#include <vector>

namespace le {

    ProceduralTerrain::ProceduralTerrain(LeDevice& device, int renderDistance, float seed, uint32_t chunkSize, float cellSize)
        : leDevice(device),
        m_renderDistance(renderDistance),
        m_seed(seed),
        m_chunkSize(chunkSize),
        m_cellSize(cellSize),
        m_gridSize(renderDistance * 2 + 1),
        m_lastChunkX(0),
        m_lastChunkZ(0)
    {
        m_physicalChunkSize = (m_chunkSize - 1) * m_cellSize;

        m_generator = std::make_unique<TerrainGenerator>(leDevice, m_chunkSize, m_cellSize, m_renderDistance);

        generateSharedIndexBuffer();
    }

    ProceduralTerrain::~ProceduralTerrain() = default;

    void ProceduralTerrain::generateSharedIndexBuffer()
    {
        const uint32_t CHUNK_SIZE = m_chunkSize;
        std::vector<uint32_t> indices;
        indices.reserve((CHUNK_SIZE - 1) * (CHUNK_SIZE - 1) * 6);

        for (uint32_t z = 0; z < CHUNK_SIZE - 1; ++z) {
            for (uint32_t x = 0; x < CHUNK_SIZE - 1; ++x) {
                uint32_t topLeft = z * CHUNK_SIZE + x;
                uint32_t topRight = topLeft + 1;
                uint32_t bottomLeft = (z + 1) * CHUNK_SIZE + x;
                uint32_t bottomRight = bottomLeft + 1;

                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }

        m_sharedIndexCount = static_cast<uint32_t>(indices.size());
        VkDeviceSize bufferSize = sizeof(indices[0]) * m_sharedIndexCount;

        m_sharedIndexBuffer = std::make_unique<LeBuffer>(
            leDevice,
            sizeof(indices[0]),
            m_sharedIndexCount,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        LeBuffer stagingBuffer(
            leDevice, sizeof(indices[0]), m_sharedIndexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        stagingBuffer.map();
        stagingBuffer.writeToBuffer(indices.data());
        stagingBuffer.unmap();

        leDevice.copyBuffer(stagingBuffer.getBuffer(), m_sharedIndexBuffer->getBuffer(), bufferSize);
    }

    int ProceduralTerrain::getBufferIndex(int chunkX, int chunkZ)
    {
        int wrappedX = ((chunkX % m_gridSize) + m_gridSize) % m_gridSize;
        int wrappedZ = ((chunkZ % m_gridSize) + m_gridSize) % m_gridSize;
        return wrappedX + (wrappedZ * m_gridSize);
    }

    void ProceduralTerrain::init(LeScene& scene, glm::vec3 startPos, VkCommandBuffer computeCmd)
    {
        m_chunkRingBuffer.resize(m_gridSize * m_gridSize);
        m_lastChunkX = std::floor(startPos.x / m_physicalChunkSize);
        m_lastChunkZ = std::floor(startPos.z / m_physicalChunkSize);

        for (int z = -m_renderDistance; z <= m_renderDistance; ++z)
        {
            for (int x = -m_renderDistance; x <= m_renderDistance; ++x)
            {
                int worldChunkX = m_lastChunkX + x;
                int worldChunkZ = m_lastChunkZ + z;

                auto chunk = std::make_unique<LeChunk>(
                    leDevice, worldChunkX, worldChunkZ,
                    m_sharedIndexBuffer->getBuffer(), m_sharedIndexCount
                );

                int index = getBufferIndex(worldChunkX, worldChunkZ);
                m_generator->generateChunk(*chunk, computeCmd, m_seed, index);

                auto mutableModel = std::make_shared<LeMutableModel>(
                    leDevice,
                    chunk->getVertexBuffer(),
                    m_chunkSize * m_chunkSize,
                    m_sharedIndexBuffer->getBuffer(),
                    m_sharedIndexCount
                );

                uint32_t terrainModelID = scene.leResourceManager.addModel(mutableModel);

                LeActor terrainActor = LeActor::createGameObject();
                terrainActor.textureID = 0;
                terrainActor.modelID = terrainModelID;

                float worldX = worldChunkX * m_physicalChunkSize;
                float worldZ = worldChunkZ * m_physicalChunkSize;
                terrainActor.transform.translation = glm::vec3(worldX, 0.0f, worldZ);

                uint32_t actorIndex = scene.addActor(std::move(terrainActor));
                m_chunkRingBuffer[index] = ChunkSlot{ std::move(chunk), actorIndex };
            }
        }
    }

    void ProceduralTerrain::update(LeScene& scene, glm::vec3 currentPos, VkCommandBuffer computeCmd)
    {
        int currentChunkX = std::floor(currentPos.x / m_physicalChunkSize);
        int currentChunkZ = std::floor(currentPos.z / m_physicalChunkSize);

        // ring buffer shift X
        if (currentChunkX != m_lastChunkX) {
            int deltaX = currentChunkX > m_lastChunkX ? 1 : -1;
            int newX = currentChunkX + (m_renderDistance * deltaX);
            int oldX = m_lastChunkX - (m_renderDistance * deltaX);

            for (int z = currentChunkZ - m_renderDistance; z <= currentChunkZ + m_renderDistance; ++z) {
                int bufferIndex = getBufferIndex(oldX, z);
                ChunkSlot& slot = m_chunkRingBuffer[bufferIndex];

                slot.chunk->setCoordinates(newX, z);
                m_generator->generateChunk(*(slot.chunk), computeCmd, m_seed, bufferIndex);

                float worldX = newX * m_physicalChunkSize;
                float worldZ = z * m_physicalChunkSize;
                scene.getActor(slot.actorIndex).transform.translation = glm::vec3(worldX, 0.0f, worldZ);
            }
            m_lastChunkX = currentChunkX;
        }

        // ring buffer shift Z
        if (currentChunkZ != m_lastChunkZ) {
            int deltaZ = currentChunkZ > m_lastChunkZ ? 1 : -1;
            int newZ = currentChunkZ + (m_renderDistance * deltaZ);
            int oldZ = m_lastChunkZ - (m_renderDistance * deltaZ);

            for (int x = currentChunkX - m_renderDistance; x <= currentChunkX + m_renderDistance; ++x) {
                int bufferIndex = getBufferIndex(x, oldZ);
                ChunkSlot& slot = m_chunkRingBuffer[bufferIndex];

                slot.chunk->setCoordinates(x, newZ);
                m_generator->generateChunk(*(slot.chunk), computeCmd, m_seed, bufferIndex);

                float worldX = x * m_physicalChunkSize;
                float worldZ = newZ * m_physicalChunkSize;
                scene.getActor(slot.actorIndex).transform.translation = glm::vec3(worldX, 0.0f, worldZ);
            }
            m_lastChunkZ = currentChunkZ;
        }
    }

    bool ProceduralTerrain::needsUpdate(glm::vec3 currentPos)
    {
        int currentChunkX = std::floor(currentPos.x / m_physicalChunkSize);
        int currentChunkZ = std::floor(currentPos.z / m_physicalChunkSize);
        return (currentChunkX != m_lastChunkX || currentChunkZ != m_lastChunkZ);
    }

} // namespace le