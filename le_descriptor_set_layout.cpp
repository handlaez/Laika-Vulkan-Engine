#include "le_descriptor_set_layout.hpp"

namespace le {

	DescriptorSetLayout::DescriptorSetLayout(LeDevice& device, const std::vector<VkDescriptorSetLayoutBinding>& bindings) : device_(device), layout_(VK_NULL_HANDLE)
	{
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(device_.device(), &layoutInfo, nullptr, &layout_) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout");
		}
	}

	DescriptorSetLayout::~DescriptorSetLayout()
	{
		if (layout_ != VK_NULL_HANDLE) {
			vkDestroyDescriptorSetLayout(device_.device(), layout_, nullptr);
		}
	}
}