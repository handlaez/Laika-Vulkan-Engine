#include "le_uniform_buffer_pool.hpp"

namespace le {
    UniformBufferPool::UniformBufferPool(LeDevice& device, size_t framesInFlight, VkDeviceSize singleBufferSize)
    {
        buffers_.resize(framesInFlight);
        for (size_t i = 0; i < framesInFlight; ++i) {
            buffers_[i] = std::make_unique<UniformBuffer>(device, singleBufferSize);
        }
    }

    UniformBuffer& UniformBufferPool::getForFrame(size_t frameIndex)
    {
        return *buffers_.at(frameIndex);
    }
}