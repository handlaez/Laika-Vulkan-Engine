#ifndef LE_DESCRIPTOR_POOL_HPP
#define LE_DESCRIPTOR_POOL_HPP

#include "le_device.hpp"

namespace le {

    class DescriptorPool {
    public:
        DescriptorPool(LeDevice& device, const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets);
        ~DescriptorPool();
        VkDescriptorPool get() const { return pool_; }

        void allocate(const std::vector<VkDescriptorSetLayout>& layouts, std::vector<VkDescriptorSet>& outSets) const;
        
        DescriptorPool(const DescriptorPool&) = delete;
        DescriptorPool& operator=(const DescriptorPool&) = delete;
    private:
        LeDevice& device_;
        VkDescriptorPool pool_{ VK_NULL_HANDLE };
    };
}
#endif