#include "le_actor.hpp"

#include "memory"
#include <iostream> // temp

namespace le {
    void LeActor::addHitbox(const glm::vec3& offset, const glm::vec3 halfExtents)
    {
        AABBHitbox h(offset, halfExtents);
        hitboxes.push_back(h);
    }

    bool LeActor::checkCollision(LeActor& other)
    {
        std::vector<AABBHitbox> collisions;
        if (this->hasBVH()) // if this has BVH
        {
            this->bvh->getPotentialCollisions(0, other.hitboxes.at(0), collisions, this->transform.translation, other.transform.translation);

            // draw currently colliding hitboxes (test)
            this->hitboxes.clear();
            for (auto& h : collisions)
            {
                this->hitboxes.push_back(h);
            }

            return !collisions.empty();
        }

        if (other.hasBVH()) // if other has BVH
        {
            // draw currently colliding hitboxes (test)
            other.hitboxes.clear();
            for (auto& h : collisions)
            {
                other.hitboxes.push_back(h);
            }

            other.bvh->getPotentialCollisions(0, this->hitboxes.at(0), collisions, other.transform.translation, this->transform.translation);
            return !collisions.empty();
        }

        // no BVH versus BVH collisions implemented

        // classic brute-force collision check
        for (const auto& otherHitbox : other.hitboxes)
        {
            for (const auto& hitbox : hitboxes)
            {
                if (hitbox.intersects(otherHitbox, this->transform.translation, other.transform.translation))
                {
                    return true;
                }
            }
        }
        return false;
    }

    void LeActor::setBVH(std::shared_ptr<BVH> newBVH)
    {
        if (!newBVH || newBVH->isEmpty())
        {
            return;
        }

        /* is all leaf hitboxes are to be drawn, uncomment this.
        // for now, only the currently colliding ones are being drawn
        
        hitboxes.clear();
        hitboxes.reserve(hitboxes.size() + newBVH->getNodes().size());
        for (auto& node : newBVH->getNodes()) {
            if (node.isLeaf())
            {
                hitboxes.push_back(node.bounds);
            }
        }
        */

        bvh = newBVH;
    }
}
