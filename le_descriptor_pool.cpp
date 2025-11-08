#include "le_descriptor_pool.hpp"

namespace le {

    DescriptorPool::DescriptorPool(LeDevice& device, const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets)
        : device_(device)
    {
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = maxSets;

        if (vkCreateDescriptorPool(device_.device(), &poolInfo, nullptr, &pool_) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool!");
        }
    }

    DescriptorPool::~DescriptorPool()
    {
        if (pool_ != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(device_.device(), pool_, nullptr);
        }
    }

    void DescriptorPool::allocate(const std::vector<VkDescriptorSetLayout>& layouts, std::vector<VkDescriptorSet>& outSets) const
    {
        outSets.resize(layouts.size());
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = pool_;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
        allocInfo.pSetLayouts = layouts.data();

        if (vkAllocateDescriptorSets(device_.device(), &allocInfo, outSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }
    }
}