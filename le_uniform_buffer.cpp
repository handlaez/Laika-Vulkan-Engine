#include "le_uniform_buffer.hpp"

namespace le {
    UniformBuffer::UniformBuffer(LeDevice& device, VkDeviceSize size) : device_(device), size_(size)
    {
        device_.createBuffer(size_, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            buffer_, memory_);

        vkMapMemory(device_.device(), memory_, 0, size_, 0, &mapped_);
    }

    UniformBuffer::~UniformBuffer()
    {
        if (mapped_) {
            vkUnmapMemory(device_.device(), memory_);
            mapped_ = nullptr;
        }
        if (buffer_ != VK_NULL_HANDLE) {
            vkDestroyBuffer(device_.device(), buffer_, nullptr);
        }
        if (memory_ != VK_NULL_HANDLE) {
            vkFreeMemory(device_.device(), memory_, nullptr);
        }
    }
}