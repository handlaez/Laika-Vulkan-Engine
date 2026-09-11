#ifndef LE_ACTOR_HPP
#define LE_ACTOR_HPP

#include "src/objects/le_model.hpp"
#include "le_aabbhitbox.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
// std
#include <memory>
#include <vector>

namespace le {

	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::quat rotation{};
		
		// Matrix corresponds to translate * Ry * Rx * Rz * scale; 
		// TODO: Remove euler angles, and implement Quaternions!;
		glm::mat4 mat4() const {
			glm::mat4 rot = glm::mat4_cast(rotation);

			return glm::translate(glm::mat4(1.0f), translation) * rot * glm::scale(glm::mat4(1.0f), scale);
		}
	};

	class LeActor : public std::enable_shared_from_this<LeActor> {
	public:
		LeActor(uint32_t modelID = 0, uint32_t textureID = 0, const glm::vec3& color = { 1.0f,1.0f,1.0f },
			const TransformComponent& transform = TransformComponent{})
			: modelID(modelID), textureID(textureID), color(color), transform(transform) {
		}

		LeActor(const LeActor&) = delete;
		LeActor& operator=(const LeActor&) = delete;
		LeActor(LeActor&&) = default;
		LeActor& operator=(LeActor&&) = default;

		const id_t getId() { return id; }

		uint32_t modelID = 0;		// assuming that model 0 is the "missing model" model
		uint32_t textureID = 0;		// assuming that texture 0 is the "missing texture" texture
		glm::vec3 color{};
		TransformComponent transform{};
		std::vector<AABBHitbox> hitboxes;    // if actor posseses a BVH, this vector is used only for drawing purposes
		std::shared_ptr<BVH> bvh = nullptr;  // one bvh per actor at most (why would you want more?)

		void addHitbox(const glm::vec3& offset, const glm::vec3 halfExtents);
		bool checkCollision(LeActor& other);
		void setBVH(std::shared_ptr<BVH> newBVH);
		bool hasBVH() const { return bvh != nullptr; }
	};

}

#endif