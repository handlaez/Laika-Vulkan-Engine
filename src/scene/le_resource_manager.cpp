#include "src/scene/le_resource_manager.hpp"

#include <iostream>
#include <filesystem>

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

	void LeResourceManager::reset()
	{
		vkDeviceWaitIdle(device.device());

		textures.clear();
		models.clear();
		textureDescriptorSets.clear();

		nextTextureID = 0;
		nextModelID = 0;

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

	uint32_t LeResourceManager::loadTexture(const std::string& path, const std::string& name)
	{
		uint32_t id = nextTextureID++;

		const std::string resourceName = name.empty() ? std::filesystem::path(path).stem().string() : name;

		auto tex = std::make_shared<LeTexture>(device, path);
		tex->setName(resourceName);
		tex->setPath(path);
		textures[id] = tex;

		allocateTextureDescriptor(id);
		writeTextureDescriptor(id, *tex);

		return id;
	}

	uint32_t LeResourceManager::loadModel(const std::string& path, const std::string& name)
	{
		uint32_t id = nextModelID++;

		const std::string resourceName = name.empty() ? std::filesystem::path(path).stem().string() : name;

		auto model = LeModel::createModelFromFile(device, path);
		model->setName(resourceName);
		model->setPath(path);
		models[id] = model;

		return id;
	}

	uint32_t LeResourceManager::addModel(const MeshData& meshData)
	{
		uint32_t id = nextModelID++;

		models[id] = std::make_shared<LeModel>(device, meshData);

		return id;
	}

	uint32_t LeResourceManager::addModel(std::shared_ptr<LeRenderable> renderable)
	{
		uint32_t id = nextModelID++;
		models[id] = renderable;
		return id;
	}

	void LeResourceManager::updateModel(const uint32_t id, const MeshData& newMeshData)
	{
		auto it = models.find(id);
		if (it != models.end()) 
		{
			if (auto staticModel = std::dynamic_pointer_cast<LeModel>(it->second))
			{
				staticModel->updateGeometry(newMeshData.vertices);
			}
			else
			{
				std::cerr << "Warning: Cannot use CPU update on a procedural model! ID: " << id
					<< ". Use compute shaders to update this buffer." << std::endl;
			}
		}
		else 
		{
			std::cerr << "Tried to update a model that doesn't exist! ID: " << id << std::endl;
		}
	}

	std::shared_ptr<LeTexture> LeResourceManager::getTexture(uint32_t id)
	{
		auto it = textures.find(id);
		if (it != textures.end()) {
			return it->second;
		}
		// fallback to texture 0 if ID not found
		return textures.at(0);
	}

	std::shared_ptr<LeRenderable> LeResourceManager::getModel(uint32_t id)
	{
		auto it = models.find(id);
		if (it != models.end()) {
			return it->second;
		}

		// Fallback to model 0 if ID not found
		return models.at(0);
	}

	std::shared_ptr<LeModel> LeResourceManager::getStaticModel(uint32_t id)
	{
		auto model = getModel(id);

		if (typeid(*model) == typeid(LeModel))
			return std::dynamic_pointer_cast<LeModel>(model);

		return nullptr;
	}

	std::shared_ptr<LeTexture> LeResourceManager::findTexture(uint32_t id) const {
		auto it = textures.find(id);

		if (it == textures.end()) {
			return nullptr;
		}

		return it->second;
	}

	std::shared_ptr<LeRenderable> LeResourceManager::findModel(uint32_t id) const {
		auto it = models.find(id);

		if (it == models.end()) {
			return nullptr;
		}

		return it->second;
	}

	const std::unordered_map<uint32_t, std::shared_ptr<LeTexture>>& LeResourceManager::getTextures() const
	{
		return textures;
	}

	const std::unordered_map<uint32_t, std::shared_ptr<LeRenderable>>& LeResourceManager::getModels() const
	{
		return models;
	}
	
	void LeResourceManager::loadFallbackTexture()
	{
		loadTexture(FALLBACK_TEXTURE, "FALLBACK TEXTURE");
	}

	void LeResourceManager::loadFallbackModel()
	{
		uint32_t id = nextModelID++;
		auto model = LeModel::createCube(device);
		model->setName("FALLBACK MODEL");
		models[id] = model;
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
