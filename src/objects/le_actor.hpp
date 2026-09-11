#ifndef LE_ACTOR_HPP
#define LE_ACTOR_HPP

#include "src/objects/le_model.hpp"
#include "src/objects/le_aabbhitbox.hpp"
#include "src/objects/le_BVH.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

namespace le {

    struct TransformComponent {
        glm::vec3 translation{};
        glm::vec3 scale{ 1.f, 1.f, 1.f };
        glm::quat rotation{};

        glm::mat4 mat4() const {
            glm::mat4 rot = glm::mat4_cast(rotation);
            return glm::translate(glm::mat4(1.0f), translation) * rot * glm::scale(glm::mat4(1.0f), scale);
        }
    };

    class LeActor : public std::enable_shared_from_this<LeActor> {
    public:
        using id_t = unsigned int;

        static LeActor createGameObject() {
            static id_t currentId = 0;
            return LeActor{ currentId++ };
        }

        LeActor(uint32_t modelID = 0, uint32_t textureID = 0, const glm::vec3& color = { 1.0f, 1.0f, 1.0f },
            const TransformComponent& transform = TransformComponent{})
            : modelID(modelID), textureID(textureID), color(color), transform(transform) {
            static id_t nextId = 0;
            id = nextId++;
        }

        LeActor(const LeActor&) = delete;
        LeActor& operator=(const LeActor&) = delete;
        LeActor(LeActor&&) = default;
        LeActor& operator=(LeActor&&) = default;

        const id_t getId() const { return id; }

        uint32_t modelID = 0;        // assuming model 0 is "missing model"
        uint32_t textureID = 0;      // assuming texture 0 is "missing texture"
        glm::vec3 color{};
        TransformComponent transform{};
        std::vector<AABBHitbox> hitboxes;    // used for drawing or brute-force collision
        std::shared_ptr<BVH> bvh = nullptr;  // one bvh per actor at most

        void addHitbox(const glm::vec3& offset, const glm::vec3 halfExtents);
        bool checkCollision(LeActor& other);
        void setBVH(std::shared_ptr<BVH> newBVH);
        bool hasBVH() const { return bvh != nullptr; }

    private:
        id_t id;
    };

}

#endif