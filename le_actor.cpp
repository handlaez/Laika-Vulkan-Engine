#include "le_actor.hpp"

#include "memory"

namespace le {
    void LeActor::addHitbox(const glm::vec3& offset, const glm::vec3 halfExtents)
    {
        AABBHitbox h(offset, halfExtents);
        h.setParent(this->shared_from_this());
        hitboxes.push_back(h);
    }

    bool LeActor::checkCollision(LeActor& other) const
    {
        for (const auto& otherHitbox : other.hitboxes)
        {
            for (const auto& hitbox : hitboxes)
            {
                if (hitbox.intersects(otherHitbox))
                {
                    return true;
                }
            }
        }
        return false;
    }
}
