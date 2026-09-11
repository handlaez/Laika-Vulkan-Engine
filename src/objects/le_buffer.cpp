#include "le_buffer.hpp"

#include <stdexcept>
#include <cstring>

namespace le {

    LeBuffer::LeBuffer(
        LeDevice& device,
        VkDeviceSize instanceSize,
        uint32_t instanceCount,
        VkBufferUsageFlags usageFlags,
        VkMemoryPropertyFlags memoryPropertyFlags
    )
        : leDevice(device),
        instanceSize_(instanceSize),
        instanceCount_(instanceCount)
    {
        bufferSize_ = instanceSize * instanceCount;

        leDevice.createBuffer(bufferSize_, usageFlags, memoryPropertyFlags, buffer_, memory_);
    }

    LeBuffer::~LeBuffer()
    {
        unmap();

        vkDestroyBuffer(leDevice.device(), buffer_,nullptr);

        vkFreeMemory( leDevice.device(), memory_, nullptr);
    }

    void LeBuffer::map(VkDeviceSize size, VkDeviceSize offset)
    {
        if (vkMapMemory(
            leDevice.device(),
            memory_,
            offset,
            size,
            0,
            &mapped_
        ) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to map buffer memory!");
        }
    }

    void LeBuffer::unmap()
    {
        if (mapped_)
        {
            vkUnmapMemory(leDevice.device(), memory_);

            mapped_ = nullptr;
        }
    }

    void LeBuffer::writeToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset)
    {
        if (size == VK_WHOLE_SIZE)
        {
            memcpy(mapped_, data, bufferSize_);
        }
        else
        {
            char* memOffset = (char*)mapped_;
            memOffset += offset;

            memcpy(memOffset, data, size);
        }
    }
}