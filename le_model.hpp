#ifndef LE_MODEL_HPP
#define LE_MODEL_HPP

#include "le_device.hpp"
#include "le_texture.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <vector>
#include <memory>

namespace le {
	class LeModel {
	public:
		struct Vertex {
			glm::vec3 position;
			glm::vec3 color;
			glm::vec3 normal;
			glm::vec2 texCoord;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const {
				return position == other.position && color == other.color && normal == other.normal && texCoord == other.texCoord;
			}
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
			std::string texturePath{};

			void loadModel(const std::string& filepath, const std::string& texFilepath);
		};

		LeModel(LeDevice &device, const LeModel::Builder &builder);
		~LeModel();

		LeModel(const LeModel&) = delete;
		LeModel& operator=(const LeModel&) = delete;

		static std::unique_ptr<LeModel> createModelFromFile(LeDevice& device, const std::string& filepath, const std::string& texFilepath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

		static std::shared_ptr<LeModel> createCube(LeDevice& device, glm::vec3 offset = {});
	private: 
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

		LeDevice& leDevice;

		std::shared_ptr<LeTexture> texture{};
		
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
		
		bool hasIndexBuffer = false;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		uint32_t indexCount;
	};
}

#endif