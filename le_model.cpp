#include "le_model.hpp"
#include "le_utils.hpp"

//std
#include <cassert>
#include <cstring>
#include <unordered_map>

//temp
#include <iostream>

namespace le {
	LeModel::LeModel(LeDevice& device, const MeshData &meshdata)
		: leDevice( device ),
		vertexBuffer(VK_NULL_HANDLE),
		vertexBufferMemory(VK_NULL_HANDLE),
		vertexCount(0),
		hasIndexBuffer(false),
		indexBuffer(VK_NULL_HANDLE),
		indexBufferMemory(VK_NULL_HANDLE),
		indexCount(0)
	{
		createVertexBuffers(meshdata.vertices);
		createIndexBuffers(meshdata.indices);
	}

	LeModel::~LeModel()
	{
		vkDestroyBuffer(leDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(leDevice.device(), vertexBufferMemory, nullptr);

		if (hasIndexBuffer)
		{
			vkDestroyBuffer(leDevice.device(), indexBuffer, nullptr);
			vkFreeMemory(leDevice.device(), indexBufferMemory, nullptr);
		}
	}

	std::shared_ptr<LeModel> LeModel::createModelFromFile(LeDevice& device, const std::string& filepath, glm::vec3 offset)
	{
		MeshData meshdata{};
		meshdata.loadModel(filepath);

		std::cout << "Model: " << filepath << "\nVertex count: " << meshdata.vertices.size() << "\n";
		auto model = std::make_unique<LeModel>(device, meshdata);

		return model;
	}

	void LeModel::bind(VkCommandBuffer commandBuffer) const
	{
		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void LeModel::draw(VkCommandBuffer commandBuffer) const
	{
		if (hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
		}
	}

	std::shared_ptr<LeModel> LeModel::createCube(LeDevice& device, glm::vec3 offset)
	{
		MeshData modelBuilder{};
		modelBuilder.vertices = {
			// left face (white) — normal (-1, 0, 0)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}, {-1.f,  0.f,  0.f}, {1.f, 0.f}},
			{{-.5f,  .5f,  .5f}, {.9f, .9f, .9f}, {-1.f,  0.f,  0.f}, {0.f, 1.f}},
			{{-.5f, -.5f,  .5f}, {.9f, .9f, .9f}, {-1.f,  0.f,  0.f}, {0.f, 0.f}},
			{{-.5f,  .5f, -.5f}, {.9f, .9f, .9f}, {-1.f,  0.f,  0.f}, {1.f, 1.f}},

			// right face (yellow) — normal (1, 0, 0)
			{{ .5f, -.5f, -.5f}, {.8f, .8f, .1f}, { 1.f,  0.f,  0.f}, {0.f, 0.f}},
			{{ .5f,  .5f,  .5f}, {.8f, .8f, .1f}, { 1.f,  0.f,  0.f}, {1.f, 1.f}},
			{{ .5f, -.5f,  .5f}, {.8f, .8f, .1f}, { 1.f,  0.f,  0.f}, {1.f, 0.f}},
			{{ .5f,  .5f, -.5f}, {.8f, .8f, .1f}, { 1.f,  0.f,  0.f}, {0.f, 1.f}},

			// top face (violet) — normal (0, -1, 0)
			{{-.5f, -.5f, -.5f}, {.4f, .1f, .8f}, { 0.f, -1.f,  0.f}, {0.f, 1.f}},
			{{ .5f, -.5f,  .5f}, {.4f, .1f, .8f}, { 0.f, -1.f,  0.f}, {1.f, 0.f}},
			{{-.5f, -.5f,  .5f}, {.4f, .1f, .8f}, { 0.f, -1.f,  0.f}, {0.f, 0.f}},
			{{ .5f, -.5f, -.5f}, {.4f, .1f, .8f}, { 0.f, -1.f,  0.f}, {1.f, 1.f}},

			// bottom face (red) — normal (0, 1, 0)
			{{-.5f,  .5f, -.5f}, {.8f, .1f, .1f}, { 0.f,  1.f,  0.f}, {1.f, 0.f}},
			{{ .5f,  .5f,  .5f}, {.8f, .1f, .1f}, { 0.f,  1.f,  0.f}, {0.f, 1.f}},
			{{-.5f,  .5f,  .5f}, {.8f, .1f, .1f}, { 0.f,  1.f,  0.f}, {1.f, 1.f}},
			{{ .5f,  .5f, -.5f}, {.8f, .1f, .1f}, { 0.f,  1.f,  0.f}, {0.f, 0.f}},

			// back face (blue) — normal (0, 0, 1)
			{{-.5f, -.5f,  0.5f}, {.1f, .1f, .8f}, { 0.f,  0.f,  1.f}, {1.f, 0.f}},
			{{ .5f,  .5f,  0.5f}, {.1f, .1f, .8f}, { 0.f,  0.f,  1.f}, {0.f, 1.f}},
			{{-.5f,  .5f,  0.5f}, {.1f, .1f, .8f}, { 0.f,  0.f,  1.f}, {1.f, 1.f}},
			{{ .5f, -.5f,  0.5f}, {.1f, .1f, .8f}, { 0.f,  0.f,  1.f}, {0.f, 0.f}},

			// front face (green) — normal (0, 0, -1)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}, { 0.f,  0.f, -1.f}, {0.f, 0.f}},
			{{ .5f,  .5f, -0.5f}, {.1f, .8f, .1f}, { 0.f,  0.f, -1.f}, {1.f, 1.f}},
			{{-.5f,  .5f, -0.5f}, {.1f, .8f, .1f}, { 0.f,  0.f, -1.f}, {0.f, 1.f}},
			{{ .5f, -.5f, -0.5f}, {.1f, .8f, .1f}, { 0.f,  0.f, -1.f}, {1.f, 0.f}},
		};
		for (auto& v : modelBuilder.vertices) {
			v.position += offset;
		}

		modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
								12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

		return std::make_shared<LeModel>(device, modelBuilder);
	}

	void LeModel::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		leDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);
		
		void* data;
		vkMapMemory(leDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(leDevice.device(), stagingBufferMemory);

		leDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			vertexBuffer,
			vertexBufferMemory);

		leDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(leDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(leDevice.device(), stagingBufferMemory, nullptr);
	}

	void LeModel::createIndexBuffers(const std::vector<uint32_t>& indices)
	{
		indexCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = indexCount > 0;
		if (!hasIndexBuffer) {
			return;
		}

		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		leDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);

		void* data;
		vkMapMemory(leDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(leDevice.device(), stagingBufferMemory);

		leDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			indexBuffer,
			indexBufferMemory);

		leDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(leDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(leDevice.device(), stagingBufferMemory, nullptr);
	}

	std::vector<VkVertexInputBindingDescription> Vertex::getBindingDescriptions()
	{
		std::vector< VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(4);
		// position
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		//color
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		//normals
		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, normal);

		//texCoords
		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}

	void LeModel::updateGeometry(const std::vector<Vertex>& newVertices)
	{
		assert(newVertices.size() == vertexCount && "New model gemetry cannot exceed the size of current geometry!");

		VkDeviceSize bufferSize = sizeof(newVertices[0]) * newVertices.size();
		void* data;

		vkMapMemory(leDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);

		memcpy(data, newVertices.data(), (size_t)bufferSize);

		vkUnmapMemory(leDevice.device(), vertexBufferMemory);
	}

	void LeModel::bindIndexBuffer(VkCommandBuffer commandBuffer)
	{
		vkCmdBindIndexBuffer(
			commandBuffer,
			indexBuffer,
			0,
			VK_INDEX_TYPE_UINT32
		);
	}
}