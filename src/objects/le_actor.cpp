#include "src/objects/le_actor.hpp"

#include "memory"
#include <iostream> // temp

namespace le {

    bool LeActor::hasParent() const
    {
        return parentId.has_value();
    }

    void LeActor::setParentId(id_t id)
    {
        parentId = id;
    }

    void LeActor::clearParent()
    {
        parentId.reset();
    }

    // clone for a runtime scene;
    LeActor LeActor::clone() const
    {
        LeActor result{ modelID, textureID, color, transform };

        result.id = id;
        result.hitboxes = hitboxes;
        result.bvh = bvh;
        result.scripts_ = scripts_;

        return result;
    }

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

    void LeActor::clearBVH()
    {
        bvh.reset();
    }

    const std::vector<LeScript>& LeActor::getScripts() const
    {
        return scripts_;
    }

    std::vector<LeScript>& LeActor::getScripts()
    {
        return scripts_;
    }

    void LeActor::addScript(LeScript script)
    {
        scripts_.push_back(std::move(script));
    }

    bool LeActor::removeScript(size_t index)
    {
        if (index >= scripts_.size())
        {
            return false;
        }

        scripts_.erase(scripts_.begin() + static_cast<std::ptrdiff_t>(index));
        return true;
    }

    bool LeActor::hasScripts() const
    {
        return !scripts_.empty();
    }


}
