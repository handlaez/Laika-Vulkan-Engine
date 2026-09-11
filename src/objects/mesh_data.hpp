#ifndef MESH_DATA_HPP
#define MESH_DATA_HPP

#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>

#include "src/core/le_utils.hpp"

namespace le {

	struct Vertex 
	{
		alignas(16) glm::vec3 position;
		alignas(16) glm::vec3 normal;
		alignas(16) glm::vec3 color;
		alignas(8) glm::vec2 texCoord;
		glm::vec2 padding;

		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

		bool operator==(const Vertex& other) const {
			return position == other.position && color == other.color && normal == other.normal && texCoord == other.texCoord;
		}
	};

	struct MeshData
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		void loadModel(const std::string& filepath);
	};
}

namespace std {
	template <>
	struct hash<le::Vertex> {
		size_t operator()(le::Vertex const& vertex) const {
			size_t seed = 0;
			Utils::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.texCoord);
			return seed;
		}
	};
}

#endif