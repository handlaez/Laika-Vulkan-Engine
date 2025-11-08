#ifndef LE_UNIFORM_BUFFER_HPP
#define LE_UNIFORM_BUFFER_HPP

#include "le_device.hpp"
#include <vulkan/vulkan.h>

namespace le {

    class UniformBuffer {
    public:
        UniformBuffer(LeDevice& device, VkDeviceSize size);
        ~UniformBuffer();
        void* mapped() const { return mapped_; }
        VkBuffer buffer() const { return buffer_; }
        VkDeviceSize size() const { return size_; }

        UniformBuffer(const UniformBuffer&) = delete;
        UniformBuffer& operator=(const UniformBuffer&) = delete;
    private:
        LeDevice& device_;
        VkBuffer buffer_{ VK_NULL_HANDLE };
        VkDeviceMemory memory_{ VK_NULL_HANDLE };
        void* mapped_{ nullptr };
        VkDeviceSize size_;
    };
}

#endif