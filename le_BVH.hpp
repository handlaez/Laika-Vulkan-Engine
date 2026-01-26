#ifndef LE_BVH_HPP
#define LE_BVH_HPP

#include "le_aabbhitbox.hpp"

#include <vector>
#include <glm/glm.hpp>

namespace le {

    class BVH {
    public:
        struct BVHNode {
            AABBHitbox bounds;      // the bounding box for this node
            int leftChild = -1;     // index in the nodes_ vector
            int rightChild = -1;
            int firstPrim = -1;     // start index in primitiveIndices_ (leaf only)
            int primCount = 0;      // number of triangles (leaf only)

            bool isLeaf() const { return primCount > 0; }
        };

        // build the hierarchy from mesh data
        void build(const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices);

        void getPotentialCollisions(int nodeIdx, const AABBHitbox& testBox, std::vector<AABBHitbox>& outBoxes) const; // returns all boxes that collide
        
        std::vector<BVHNode> getNodes() { return nodes_; } // again, terrible way to do this :)

        // helper to check if the BVH has been initialized
        bool isEmpty() const { return nodes_.empty(); }

    private:
        std::vector<BVHNode> nodes_;
        std::vector<uint32_t> primitiveIndices_; // Stores sorted triangle index offsets
    };

}

#endif