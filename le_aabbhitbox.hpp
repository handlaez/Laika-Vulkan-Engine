#ifndef LE_AABBHITBOX_HPP
#define LE_AABBHITBOX_HPP

#include <glm/glm.hpp>
#include <memory>

// note: there should be one abstract hitbox class (or inrterface), and different types of hitboxes should derive from that

namespace le {
	class LeActor;

	class AABBHitbox {
	public:
		AABBHitbox();
		AABBHitbox(const glm::vec3& center, const glm::vec3& halfExtents);

		void setCenter(const glm::vec3& center);
		void setHalfExtents(const glm::vec3& halfExtents);

		const glm::vec3& getCenter() const;
		const glm::vec3& getHalfExtents() const;
		// mat4 for rendering purposes
		glm::mat4 mat4(const glm::vec3& globalPos) const;

		glm::vec3 getMin(const glm::vec3& globalPos) const;
		glm::vec3 getMax(const glm::vec3& globalPos) const;

		// intersects will be called from actor's perspective, so thiese vec3's are no problem to get.
		bool intersects(const AABBHitbox& other, const glm::vec3& globalPos, const glm::vec3& otherGlobalPos) const;
		bool contains(const glm::vec3& point, const glm::vec3& globalPos) const;
	private:
		glm::vec3 center_{};       // center described as an offset form actors x, y
		glm::vec3 halfExtents_{};  // half size of the hitbox.
	};
}

#endif