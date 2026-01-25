#include "le_aabbhitbox.hpp"
#include "le_actor.hpp"

#include <glm/gtc/quaternion.hpp>

namespace le {
	AABBHitbox::AABBHitbox() : center_(0.0f), halfExtents_(0.5f) {}

	AABBHitbox::AABBHitbox(const glm::vec3& center, const glm::vec3& halfExtents)
		: center_(center), halfExtents_(halfExtents) {}


	void AABBHitbox::setCenter(const glm::vec3& center) 
	{
		center_ = center;
	}

	void AABBHitbox::setHalfExtents(const glm::vec3& halfExtents)
	{
		halfExtents_ = halfExtents;
	}

	void AABBHitbox::setParent(std::shared_ptr<LeActor> p)
	{
		parent_ = p;
	}

	const glm::vec3& AABBHitbox::getCenter() const
	{
		return center_;
	}

	glm::vec3 AABBHitbox::getGlobalCenter() const
	{
		if (auto p = parent_.lock()) {
			return p->transform.translation + center_; // parent exists
		}
		else {
			return center_; // fallback if parent destroyed
		}
	}

	const glm::vec3& AABBHitbox::getHalfExtents() const
	{
		return halfExtents_;
	}

	glm::mat4 AABBHitbox::mat4() const
	{
		glm::vec3 globalPos = getGlobalCenter();

		return glm::mat4{
			{ 2.0f * halfExtents_.x, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 2.0f * halfExtents_.y, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 2.0f * halfExtents_.z, 0.0f },
			{ globalPos, 1.0f }
		};
	}

	glm::vec3 AABBHitbox::getMin() const
	{
		glm::vec3 globalCenter = getGlobalCenter();
		return globalCenter - halfExtents_;
	}

	glm::vec3 AABBHitbox::getMax() const
	{
		glm::vec3 globalCenter = getGlobalCenter();
		return globalCenter + halfExtents_;
	}

	bool AABBHitbox::intersects(const AABBHitbox& other) const
	{
		const glm::vec3 minA = getMin();
		const glm::vec3 maxA = getMax();
		const glm::vec3 minB = other.getMin();
		const glm::vec3 maxB = other.getMax();

		return (minA.x <= maxB.x && maxA.x >= minB.x) &&
			   (minA.y <= maxB.y && maxA.y >= minB.y) &&
			   (minA.z <= maxB.z && maxA.z >= minB.z);
	}

	bool AABBHitbox::contains(const glm::vec3& point) const
	{
		const glm::vec3 min = getMin();
		const glm::vec3 max = getMax();

		return (point.x >= min.x && point.x <= max.x) &&
			   (point.y >= min.y && point.y <= max.y) &&
			   (point.z >= min.z && point.z <= max.z);
	}
}
