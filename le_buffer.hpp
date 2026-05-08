#ifndef LE_BUFFER_HPP
#define LE_BUFFER_HPP

#include "le_device.hpp"

namespace le {

    class LeBuffer {
    public:
        LeBuffer(
            LeDevice& device,
            VkDeviceSize instanceSize,
            uint32_t instanceCount,
            VkBufferUsageFlags usageFlags,
            VkMemoryPropertyFlags memoryPropertyFlags
        );

        ~LeBuffer();

        LeBuffer(const LeBuffer&) = delete;
        LeBuffer& operator=(const LeBuffer&) = delete;

        VkBuffer getBuffer() const { return buffer_; }

        void map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        void unmap();

        void writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    private:
        LeDevice& leDevice;

        VkBuffer buffer_{};
        VkDeviceMemory memory_{};

        void* mapped_ = nullptr;

        VkDeviceSize bufferSize_;
        VkDeviceSize instanceSize_;
        uint32_t instanceCount_;
    };

}

#endif