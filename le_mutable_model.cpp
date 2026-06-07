#include "le_mutable_model.hpp"

namespace le {

    LeMutableModel::LeMutableModel(LeDevice& device, VkBuffer vertexBuffer, uint32_t vertexCount, VkBuffer indexBuffer, uint32_t indexCount)
        : leDevice(device),
        m_vertexBuffer(vertexBuffer), m_vertexCount(vertexCount),
        m_indexBuffer(indexBuffer), m_indexCount(indexCount)
    {
    }

    void LeMutableModel::bind(VkCommandBuffer commandBuffer) const
    {
        if (m_vertexBuffer != VK_NULL_HANDLE) {
            VkBuffer buffers[] = { m_vertexBuffer };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        }

        if (m_indexBuffer != VK_NULL_HANDLE) {
            vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void LeMutableModel::draw(VkCommandBuffer commandBuffer) const
    {
        if (m_indexBuffer != VK_NULL_HANDLE && m_indexCount > 0) {
            vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
        }
        else if (m_vertexBuffer != VK_NULL_HANDLE && m_vertexCount > 0) {
            vkCmdDraw(commandBuffer, m_vertexCount, 1, 0, 0);
        }
    }

    void LeMutableModel::updateVertexBuffer(VkBuffer newVertexBuffer, uint32_t newVertexCount)
    {
        m_vertexBuffer = newVertexBuffer;
        m_vertexCount = newVertexCount;
    }

    void LeMutableModel::updateIndexBuffer(VkBuffer newIndexBuffer, uint32_t newIndexCount)
    {
        m_indexBuffer = newIndexBuffer;
        m_indexCount = newIndexCount;
    }

} // namespace le