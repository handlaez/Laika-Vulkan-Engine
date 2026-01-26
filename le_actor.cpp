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
        std::vector<AABBHitbox> collisions;
        if (this->hasBVH()) // if this has BVH
        {
            this->bvh->getPotentialCollisions(0, other.hitboxes.at(0), collisions);
            return !collisions.empty();
        }

        if (other.hasBVH()) // if other has BVH
        {
            other.bvh->getPotentialCollisions(0, this->hitboxes.at(0), collisions);
            return !collisions.empty();
        }

        // no BVH versus BVH collisions implemented

        // classic brute-force collision check
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

    void LeActor::takeBVHOwnership(std::unique_ptr<BVH> newBVH)
    {
        if (!newBVH || newBVH->isEmpty())
        {
            return;
        }

        hitboxes.clear();
        hitboxes.reserve(hitboxes.size() + newBVH->getNodes().size());

        auto self = shared_from_this();

        for (auto& node : newBVH->getNodes()) {
            node.bounds.setParent(shared_from_this());
            if (node.isLeaf())
            {
                hitboxes.push_back(node.bounds);
            }
        }

        bvh = std::move(newBVH);
    }
}
