#ifndef LE_RESOURCE_MANAGER_HPP
#define LE_RESOURCE_MANAGER_HPP

#include <unordered_map>
#include <string>

#include "le_texture.hpp"
#include "le_model.hpp"

namespace le {
	class LeResourceManager
	{
		// Manager class as described in "Game Engine Architecture ed.3" (chapter 6.1.2)
	public: 
		 LeResourceManager(LeDevice& device); // do nothing
		~LeResourceManager(); // do nothing

		void startUp(); // start up the manager
		void shutDown(); // shut down thy manager

		// loaders (they return an ID to a created item)
		uint32_t loadTexture(const std::string& path);
		uint32_t   loadModel(const std::string& path);

		// accessors
		LeTexture& getTexture(uint32_t id);
		LeModel&     getModel(uint32_t id);

		// descriptors 
		VkDescriptorSetLayout getTextureDescriptorSetLayout() const { return textureSetLayout; }
		VkSampler getSharedSampler() const { return sharedSampler; }
		VkDescriptorSet getTextureDescriptorSet(uint32_t id) const { return textureDescriptorSets.at(id); }

	private:
		// atomic id counters
		uint32_t nextTextureID = 0;
		uint32_t nextModelID = 0;

		// resource storage
		std::unordered_map<uint32_t, LeTexture> textures;
		std::unordered_map<uint32_t, LeModel>     models;
		// descriptorSet get their ids from textures
		std::unordered_map<uint32_t, VkDescriptorSet> textureDescriptorSets;

		LeDevice& device;

		// Used for all texture descriptor sets
		VkDescriptorPool textureDescriptorPool = VK_NULL_HANDLE;
		VkDescriptorSetLayout textureSetLayout = VK_NULL_HANDLE;
		VkSampler                sharedSampler = VK_NULL_HANDLE;

		// removal methods (just in Case)
		void removeTexture(uint32_t id);
		void removeModel(uint32_t id);

		// hleps
		void createTextureDescriptorLayout();
		void createTextureDescriptorPool();
		void createSharedSampler();

		void allocateTextureDescriptor(uint32_t id);
		void writeTextureDescriptor(uint32_t id, LeTexture& tex);
	};
}

#endif