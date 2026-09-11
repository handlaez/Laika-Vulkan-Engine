#ifndef LE_MODEL_HPP
#define LE_MODEL_HPP

#include "src/core/le_device.hpp"
#include "src/objects/le_texture.hpp"
#include "src/objects/le_BVH.hpp"
#include "src/render/i_le_renderable.hpp"
#include "src/objects/mesh_data.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <vector>
#include <memory>

namespace le {
	class BVH;

	class LeModel : public LeRenderable 
	{
	public:
		LeModel(LeDevice &device, const MeshData &meshdata);
		~LeModel();

		void bind(VkCommandBuffer commandBuffer) const override;
		void draw(VkCommandBuffer commandBuffer) const override;

		static std::shared_ptr<LeModel> createModelFromFile(LeDevice& device, const std::string& filepath, glm::vec3 offset = {});
		static std::shared_ptr<LeModel> createCube(LeDevice& device, glm::vec3 offset = {});

		// copying is forbidden
		LeModel(const LeModel&) = delete;
		LeModel& operator=(const LeModel&) = delete;

		// moving is okay (constructor only)
		LeModel(LeModel&& other) noexcept
		  : leDevice(other.leDevice),
			vertexBuffer(other.vertexBuffer),
			vertexBufferMemory(other.vertexBufferMemory),
			vertexCount(other.vertexCount),
			hasIndexBuffer(other.hasIndexBuffer),
			indexBuffer(other.indexBuffer),
			indexBufferMemory(other.indexBufferMemory),
			indexCount(other.indexCount)
		{
			// Null out other's resources
			other.vertexBuffer = VK_NULL_HANDLE;
			other.vertexBufferMemory = VK_NULL_HANDLE;
			other.indexBuffer = VK_NULL_HANDLE;
			other.indexBufferMemory = VK_NULL_HANDLE;
			other.hasIndexBuffer = false;
			other.vertexCount = 0;
			other.indexCount = 0;
		}

		// disabling move assignment (because leDevice& cannot be reassigned)
		LeModel& operator=(LeModel&&) = delete;

		//BVH
		const std::shared_ptr<BVH> getBVH() const { return std::make_shared<BVH>(bvh); }
		const std::vector<glm::vec3>& getPositions() const { return positions; }
		const std::vector<uint32_t>& getIndices() const { return indices; }

		// modifying is sometimes cool
		void updateGeometry(const std::vector<Vertex>& newVertices);

		VkBuffer getVertexBuffer() const { return vertexBuffer; }
		VkBuffer getIndexBuffer() const { return indexBuffer; }
		uint32_t getIndexCount() const { return indexCount; }

		void bindIndexBuffer(VkCommandBuffer commandBuffer);

	private: 
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

		LeDevice& leDevice;

		std::shared_ptr<LeTexture> texture{};
		
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;

		//BVH
		BVH bvh;
		std::vector<glm::vec3> positions; // CPU copy for physics/BVH
		std::vector<uint32_t> indices;    // CPU copy for physics/BVH
		
		bool hasIndexBuffer = false;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		uint32_t indexCount;
	};
}

#endif