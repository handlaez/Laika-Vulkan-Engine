#ifndef LE_UNIFORM_BUFFER_POOL
#define LE_UNIFORM_BUFFER_POOL

#include "le_uniform_buffer.hpp"
#include <memory>

namespace le {
	class UniformBufferPool {
    public:
        UniformBufferPool(LeDevice& device, size_t framesInFlight, VkDeviceSize singleBufferSize);
        ~UniformBufferPool() = default;
        UniformBuffer& getForFrame(size_t frameIndex);
        size_t frames() const { return buffers_.size(); }
    private:
        std::vector<std::unique_ptr<UniformBuffer>> buffers_;
	};
}

#endif
