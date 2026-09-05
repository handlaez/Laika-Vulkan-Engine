#include "src/scene/le_resource_manager.hpp"
#include <iostream>

namespace le {
	LeResourceManager::LeResourceManager(LeDevice& device) : device{ device }
	{
		// do nothing
	}

	LeResourceManager::~LeResourceManager()
	{
		// do nothing
	}

	void LeResourceManager::startUp()
	{
		createTextureDescriptorLayout();
		createTextureDescriptorPool();
		createSharedSampler();

		loadFallbackTexture();
		loadFallbackModel();
	}

	void LeResourceManager::shutDown()
	{
		vkDestroySampler(device.device(), sharedSampler, nullptr);
		vkDestroyDescriptorPool(device.device(), textureDescriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(device.device(), textureSetLayout, nullptr);

		textures.clear();
		models.clear();
	}

	uint32_t LeResourceManager::loadTexture(const std::string& path)
	{
		uint32_t id = nextTextureID++;

		auto tex = std::make_shared<LeTexture>(device, path);
		textures[id] = tex;

		allocateTextureDescriptor(id);
		writeTextureDescriptor(id, *tex);

		return id;
	}

	uint32_t LeResourceManager::loadModel(const std::string& path)
	{
		uint32_t id = nextModelID++;

		auto model = LeModel::createModelFromFile(device, path);
		models[id] = model;

		return id;
	}

	std::shared_ptr<LeTexture> LeResourceManager::getTexture(uint32_t id)
	{
		return textures.at(id);
	}

	std::shared_ptr<LeModel> LeResourceManager::getModel(uint32_t id)
	{
		return models.at(id);
	}
	
	void LeResourceManager::loadFallbackTexture()
	{
		loadTexture(FALLBACK_TEXTURE);
	}

	void LeResourceManager::loadFallbackModel()
	{
		uint32_t id = nextModelID++;
		models[id] = LeModel::createCube(device);
	}

	// assuming here that texture and model class' destructors will handle the rest (as they should)
	void LeResourceManager::removeTexture(uint32_t id)
	{
		if (textures.find(id) != textures.end()) {
			textures.erase(id);
			textureDescriptorSets.erase(id);
		}
	}

	void LeResourceManager::removeModel(uint32_t id)
	{
		if (models.find(id) != models.end()) {
			models.erase(id);
		}
	}

	void LeResourceManager::createTextureDescriptorLayout()
	{
		VkDescriptorSetLayoutBinding samplerBinding{};
		samplerBinding.binding = 0;
		samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerBinding.descriptorCount = 1;
		samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &samplerBinding;

		if (vkCreateDescriptorSetLayout(device.device(), &layoutInfo, nullptr, &textureSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create texture descriptor set layout!");
		}
	}

	void LeResourceManager::createTextureDescriptorPool()
	{
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize.descriptorCount = 250;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = 250;

		if (vkCreateDescriptorPool(device.device(), &poolInfo, nullptr, &textureDescriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create texture descriptor pool!");
		}
	}

	void LeResourceManager::createSharedSampler()
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 4.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		if (vkCreateSampler(device.device(), &samplerInfo, nullptr, &sharedSampler) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create shared sampler!");
		}
	}

	void LeResourceManager::allocateTextureDescriptor(uint32_t id)
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = textureDescriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &textureSetLayout;

		VkDescriptorSet descriptorSet{};

		if (vkAllocateDescriptorSets(device.device(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate texture descriptor set!");
		}

		textureDescriptorSets[id] = descriptorSet;
	}

	void LeResourceManager::writeTextureDescriptor(uint32_t id, LeTexture& tex)
	{
		VkDescriptorSet descriptorSet = textureDescriptorSets.at(id);
		
		VkDescriptorImageInfo imageInfo = tex.getDescriptorInfo(sharedSampler);

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptorSet;
		write.dstBinding = 0;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.descriptorCount = 1;
		write.pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device.device(), 1, &write, 0, nullptr);
	}
}
