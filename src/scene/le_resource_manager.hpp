#ifndef LE_RESOURCE_MANAGER_HPP
#define LE_RESOURCE_MANAGER_HPP

#include <unordered_map>
#include <string>

#include "le_texture.hpp"
#include "le_model.hpp"
#include "i_le_renderable.hpp"

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
		uint32_t loadModel(const std::string& path);

		// adders (for models procedurally generated -- terrain for example)
		uint32_t addModel(const MeshData& meshData);
		uint32_t addModel(std::shared_ptr<LeRenderable> renderable);
		void updateModel(const uint32_t id, const MeshData& newMeshData);

		// accessors
		std::shared_ptr<LeTexture> getTexture(uint32_t id);
		std::shared_ptr<LeRenderable> getModel(uint32_t id);

		// descriptors 
		VkDescriptorSetLayout getTextureDescriptorSetLayout() const { return textureSetLayout; }
		VkSampler getSharedSampler() const { return sharedSampler; }
		VkDescriptorSet getTextureDescriptorSet(uint32_t id) const 
		{
			auto it = textureDescriptorSets.find(id);
			if (it != textureDescriptorSets.end()) {
				return it->second;
			}
			// Fallback to descriptor set 0
			return textureDescriptorSets.at(0);
		}

	private:
		// fallback texture
		const std::string FALLBACK_TEXTURE = "textures/missing_texture.jpg";
		// fallback model will be a cube for now.

		// atomic id counters
		uint32_t nextTextureID = 0;
		uint32_t nextModelID = 0;

		// resource storage
		std::unordered_map<uint32_t, std::shared_ptr<LeTexture>> textures;
		std::unordered_map<uint32_t, std::shared_ptr<LeRenderable>>	models;
		// descriptorSet get their ids from textures
		std::unordered_map<uint32_t, VkDescriptorSet> textureDescriptorSets;

		// fallback texture and model (id = 0)
		void loadFallbackTexture();
		void loadFallbackModel();

		LeDevice& device;

		// Used for all texture descriptor sets
		VkDescriptorPool textureDescriptorPool = VK_NULL_HANDLE;
		VkDescriptorSetLayout textureSetLayout = VK_NULL_HANDLE;
		VkSampler sharedSampler = VK_NULL_HANDLE;

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