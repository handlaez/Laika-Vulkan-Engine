#include "le_BVH.hpp"

#include <iostream>   // cerr
#include <algorithm>  // nth_element
#include <functional> // std::function
#include <cfloat>     // FLT_MAX

namespace le {

    struct TriangleMetadata {
        glm::vec3 centroid;
        glm::vec3 min, max;
        uint32_t indexOffset;
    };

    void BVH::build(const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices) {
        nodes_.clear();
        primitiveIndices_.clear();

        if (indices.empty()) return;

        // 1. Gather triangle bounds and centroids
        std::vector<TriangleMetadata> triangles;
        triangles.reserve(indices.size() / 3);

        for (size_t i = 0; i < indices.size(); i += 3) {
            glm::vec3 v0 = vertices[indices[i]];
            glm::vec3 v1 = vertices[indices[i + 1]];
            glm::vec3 v2 = vertices[indices[i + 2]];

            TriangleMetadata tri;
            tri.indexOffset = static_cast<uint32_t>(i);
            tri.centroid = (v0 + v1 + v2) / 3.0f;
            tri.min = glm::min(v0, glm::min(v1, v2));
            tri.max = glm::max(v0, glm::max(v1, v2));
            triangles.push_back(tri);
        }

        // 2. Recursive build lambda
        std::function<int(int, int)> buildRecursive = [&](int start, int end) -> int {
            int nodeIdx = static_cast<int>(nodes_.size());
            nodes_.emplace_back(); // Create node

            // Calculate AABB for this node's range
            glm::vec3 bMin(FLT_MAX), bMax(-FLT_MAX);
            for (int i = start; i < end; ++i) {
                bMin = glm::min(bMin, triangles[i].min);
                bMax = glm::max(bMax, triangles[i].max);
            }

            // Assign bounds using your Center/HalfExtents constructor
            nodes_[nodeIdx].bounds = AABBHitbox((bMin + bMax) * 0.5f, (bMax - bMin) * 0.5f);

            int count = end - start;
            if (count <= 2) { // Leaf condition
                nodes_[nodeIdx].firstPrim = static_cast<int>(primitiveIndices_.size());
                nodes_[nodeIdx].primCount = count;

                // Store indices in the final array
                for (int i = start; i < end; ++i) {
                    primitiveIndices_.push_back(triangles[i].indexOffset);
                }
            }
            else {
                // Split along longest axis
                glm::vec3 extent = bMax - bMin;
                int axis = (extent.x > extent.y && extent.x > extent.z) ? 0 : (extent.y > extent.z ? 1 : 2);
                int mid = (start + end) / 2;

                std::nth_element(triangles.begin() + start, triangles.begin() + mid, triangles.begin() + end,
                    [axis](const TriangleMetadata& a, const TriangleMetadata& b) {
                        return a.centroid[axis] < b.centroid[axis];
                    });

                nodes_[nodeIdx].leftChild = buildRecursive(start, mid);
                nodes_[nodeIdx].rightChild = buildRecursive(mid, end);
            }
            return nodeIdx;
            };

        buildRecursive(0, static_cast<int>(triangles.size()));
    }

    void BVH::getPotentialCollisions(int nodeIdx, const AABBHitbox& testBox, std::vector<AABBHitbox>& outBoxes) const {
        if (nodes_.empty() || nodeIdx == -1)
        {
            return;
        }

        // Use your existing AABBHitbox::intersects method
        if (!nodes_[nodeIdx].bounds.intersects(testBox)) {
            return;
        }

        if (nodes_[nodeIdx].isLeaf()) {
            outBoxes.push_back(nodes_[nodeIdx].bounds);
        }
        else {
            // Recurse down the tree
            getPotentialCollisions(nodes_[nodeIdx].leftChild, testBox, outBoxes);
            getPotentialCollisions(nodes_[nodeIdx].rightChild, testBox, outBoxes);
        }
    }
} // namespace le