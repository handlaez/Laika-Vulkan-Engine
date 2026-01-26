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
		void setParent(std::shared_ptr<LeActor> p);

		const glm::vec3& getCenter() const;
		glm::vec3 getGlobalCenter() const;
		const glm::vec3& getHalfExtents() const;
		glm::mat4 mat4() const;

		glm::vec3 getMin() const;
		glm::vec3 getMax() const;

		bool intersects(const AABBHitbox& other) const;
		bool contains(const glm::vec3& point) const;
	private:
		glm::vec3 center_{};       // center described as an offset form actors x, y
		glm::vec3 halfExtents_{};  // half size of the hitbox.
		std::weak_ptr<LeActor> parent_;
	};
}

#endif