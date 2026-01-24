#ifndef LE_AABBHITBOX_HPP
#define LE_AABBHITBOX_HPP

#include <glm/glm.hpp>

// note: there should be one abstract hitbox class (or inrterface), and different types of hitboxes should derive from that

namespace le {
	class AABBHitbox
	{
	public:
		AABBHitbox();
		AABBHitbox(const glm::vec3& center, const glm::vec3& halfExtents);

		void setCenter(const glm::vec3& center);
		void setHalfExtents(const glm::vec3& halfExtents);

		const glm::vec3& getCenter() const;
		const glm::vec3& getHalfExtents() const;
		glm::mat4 mat4() const;

		glm::vec3 getMin() const;
		glm::vec3 getMax() const;

		bool intersects(const AABBHitbox& other) const;
		bool contains(const glm::vec3& point) const;

	private:
		glm::vec3 center_{};
		glm::vec3 halfExtents_{};
	};

}

#endif