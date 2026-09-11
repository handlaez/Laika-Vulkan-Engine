#ifndef LE_MUTABLE_MODEL_HPP
#define LE_MUTABLE_MODEL_HPP

#include "i_le_renderable.hpp"
#include "le_device.hpp"

namespace le {
    class LeMutableModel : public LeRenderable {

        // this class works very differently than LeModel -- it doesn't own the model, it's just a handler for the resource manager, and the renderer.
        // some other object owns the model itself -- like chunk from the world generator for example.

    public:
        // supports both indexed and non-indexed drawing
        LeMutableModel(LeDevice& device,
            VkBuffer vertexBuffer, uint32_t vertexCount,
            VkBuffer indexBuffer = VK_NULL_HANDLE, uint32_t indexCount = 0);

        ~LeMutableModel() override = default; // do NOT destroy the buffers here!

        void bind(VkCommandBuffer commandBuffer) const override;
        void draw(VkCommandBuffer commandBuffer) const override;

        // update the pointers without reallocating
        void updateVertexBuffer(VkBuffer newVertexBuffer, uint32_t newVertexCount);
        void updateIndexBuffer(VkBuffer newIndexBuffer, uint32_t newIndexCount);

    private:
        LeDevice& leDevice;

        VkBuffer m_vertexBuffer;
        uint32_t m_vertexCount;

        VkBuffer m_indexBuffer;
        uint32_t m_indexCount;
    };
}

#endif