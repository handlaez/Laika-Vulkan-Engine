#ifndef LE_CHUNK_HPP
#define LE_CHUNK_HPP

#include "src/render/i_le_renderable.hpp"
#include "src/core/le_device.hpp"
#include "src/objects/mesh_data.hpp" 
#include "src/objects/le_buffer.hpp"

// std
#include <memory>

namespace le
{
    class LeChunk : public LeRenderable
    {
    public:
        LeChunk(LeDevice& device, int chunkX, int chunkZ, VkBuffer sharedIndexBuffer, uint32_t sharedIndexCount);
        ~LeChunk();

        // lerenderable
        void bind(VkCommandBuffer commandBuffer) const override;
        void draw(VkCommandBuffer commandBuffer) const override;

        LeChunk(const LeChunk&) = delete;
        LeChunk& operator=(const LeChunk&) = delete;

        LeChunk(LeChunk&& other) noexcept
            : leDevice(other.leDevice),
            chunkX(other.chunkX),
            chunkZ(other.chunkZ),
            vertexBuffer(std::move(other.vertexBuffer)),
            vertexBufferMemory(other.vertexBufferMemory),
            vertexCount(other.vertexCount),
            sharedIndexBuffer(other.sharedIndexBuffer),
            sharedIndexCount(other.sharedIndexCount),
            m_modelID(other.m_modelID)
        {
            other.vertexBuffer = VK_NULL_HANDLE;
            other.vertexBufferMemory = VK_NULL_HANDLE;
            other.vertexCount = 0;
        }

        LeChunk& operator=(LeChunk&&) = delete;

        // getters for the compute shader dispatch pipeline
        VkBuffer getVertexBuffer() const { return vertexBuffer->getBuffer(); }
        int getChunkX() const { return chunkX; }
        int getChunkZ() const { return chunkZ; }

        void setCoordinates(int x, int z) { chunkX = x; chunkZ = z; }

        void setModelID(uint32_t id) { m_modelID = id; }
        uint32_t getModelID() const { return m_modelID; }
    private:
        uint32_t m_modelID;

        LeDevice& leDevice;

        int chunkX;
        int chunkZ;

        std::unique_ptr<LeBuffer> vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;

        VkBuffer sharedIndexBuffer;
        uint32_t sharedIndexCount;
    };
}

#endif