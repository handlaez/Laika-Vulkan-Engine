#include "le_chunk.hpp"

namespace le {

    constexpr uint32_t CHUNK_VERTEX_COUNT = 64 * 64;

    LeChunk::LeChunk(LeDevice& device, int chunkX, int chunkZ, VkBuffer sharedIndexBuffer, uint32_t sharedIndexCount)
        : leDevice(device), chunkX(chunkX), chunkZ(chunkZ), vertexCount(CHUNK_VERTEX_COUNT),
        sharedIndexBuffer(sharedIndexBuffer), sharedIndexCount(sharedIndexCount)
    {
        vertexBuffer = std::make_unique<LeBuffer>(
            leDevice,
            sizeof(Vertex),
            vertexCount,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );
    }

    LeChunk::~LeChunk() = default; // LeBuffer cleans itself up automatically!

    void LeChunk::bind(VkCommandBuffer commandBuffer) const
    {
        VkBuffer buffers[] = { vertexBuffer->getBuffer() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, sharedIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }

    void LeChunk::draw(VkCommandBuffer commandBuffer) const
    {
        vkCmdDrawIndexed(commandBuffer, sharedIndexCount, 1, 0, 0, 0);
    } 
}