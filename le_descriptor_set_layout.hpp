#ifndef LE_DESCRIPTOR_SET_LAYOUT_HPP
#define LE_DESCRIPTOR_SET_LAYOUT_HPP

#include "le_device.hpp"

namespace le {

	class DescriptorSetLayout {
	public:
        DescriptorSetLayout(LeDevice& device, const std::vector<VkDescriptorSetLayoutBinding>& bindings);
        ~DescriptorSetLayout();
        VkDescriptorSetLayout get() const { return layout_; }

        DescriptorSetLayout(const DescriptorSetLayout&) = delete;
        DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
    private:
        LeDevice& device_;
        VkDescriptorSetLayout layout_{ VK_NULL_HANDLE };
	};
}

#endif