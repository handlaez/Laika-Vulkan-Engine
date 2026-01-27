#ifndef LE_ACTOR_HPP
#define LE_ACTOR_HPP

#include "le_model.hpp"
#include "le_aabbhitbox.hpp"

#include <glm/gtc/matrix_transform.hpp>
// std
#include <memory>
#include <vector>

namespace le {

	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{};
		
		// Matrix corresponds to translate * Ry * Rx * Rz * scale; 
		// TODO: Remove euler angles, and implement Quaternions!;
		glm::mat4 mat4() {
			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);
			return glm::mat4{
				{
					scale.x * (c1 * c3 + s1 * s2 * s3),
					scale.x * (c2 * s3),
					scale.x * (c1 * s2 * s3 - c3 * s1),
					0.0f,
				},
				{
					scale.y * (c3 * s1 * s2 - c1 * s3),
					scale.y * (c2 * c3),
					scale.y * (c1 * c3 * s2 + s1 * s3),
					0.0f,
				},
				{
					scale.z * (c2 * s1),
					scale.z * (-s2),
					scale.z * (c1 * c2),
					0.0f,
				},
				{
					translation.x,
					translation.y,
					translation.z,
					1.0f}
			};
		}
	};

	class LeActor : public std::enable_shared_from_this<LeActor> {
	public:
		LeActor(uint32_t modelID = 0, uint32_t textureID = 0, const glm::vec3& color = { 1.0f,1.0f,1.0f },
			const TransformComponent& transform = TransformComponent{})
			: modelID(modelID), textureID(textureID), color(color), transform(transform) {
		}

		uint32_t modelID = 0;      // assuming that model 0 is the "missing model" model
		uint32_t textureID = 0;    // assuming that texture 0 is the "missing texture" texture
		glm::vec3 color{};
		TransformComponent transform{};
		std::vector<AABBHitbox> hitboxes;    // most actors will have a single hitbox for now.
		std::unique_ptr<BVH> bvh = nullptr;

		void addHitbox(const glm::vec3& offset, const glm::vec3 halfExtents);
		bool checkCollision(LeActor& other);
		void takeBVHOwnership(std::unique_ptr<BVH> newBVH);
		bool hasBVH() const { return bvh != nullptr; }
	};

}

#endif