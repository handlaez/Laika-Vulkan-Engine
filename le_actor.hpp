#ifndef LE_ACTOR_HPP
#define LE_ACTOR_HPP

#define GLM_ENABLE_EXPERIMENTAL

#include "le_model.hpp"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
// std
#include <memory>

namespace le {

	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::quat rotation{};
		
		// Matrix corresponds to translate * Ry * Rx * Rz * scale; 
		// TODO: Remove euler angles, and implement Quaternions!;
		glm::mat4 mat4() const {
			glm::mat4 rot = glm::mat4_cast(rotation);

			return glm::translate(glm::mat4(1.0f), translation)
				* rot
				* glm::scale(glm::mat4(1.0f), scale);
		}
	};

	class LeActor {
	public:
		using id_t = unsigned int;

		static LeActor createGameObject() {
			static id_t currentId = 0;
			return LeActor{ currentId++ };
		}

		LeActor(uint32_t modelID, uint32_t textureID, const glm::vec3& color = { 1.0f,1.0f,1.0f },
			const TransformComponent& transform = TransformComponent{})
			: id(nextId++), modelID(modelID), textureID(textureID), color(color), transform(transform) {
		}

		LeActor(const LeActor&) = delete;
		LeActor& operator=(const LeActor&) = delete;
		LeActor(LeActor&&) = default;
		LeActor& operator=(LeActor&&) = default;

		const id_t getId() { return id; }

		uint32_t modelID = 0;        // assuming that model 0 is the "missing model" model
		uint32_t textureID = 0;    // assuming that texture 0 is the "missing texture" texture
		glm::vec3 color{};
		TransformComponent transform{};

	private:
		LeActor(id_t objId) : id{ objId } {}

		id_t id;
		inline static id_t nextId = 1;
	};

}

#endif