#ifndef LE_SCENE_HPP
#define LE_SCENE_HPP

#include "src/objects/le_actor.hpp"
#include "src/objects/le_camera.hpp"
#include "src/core/le_device.hpp"
#include "src/scene/le_resource_manager.hpp"
#include "src/systems/boids/instance_data.hpp"
#include "src/systems/terrain-gen/procedural_terrain.hpp"

// std
#include <vector>
#include <memory>
#include <utility>

namespace le {

    class LeScene {
    public:
        explicit LeScene(LeDevice& device, LeResourceManager& resourceManager);

        const std::string& getName() const;
        void setName(std::string name);

        LeDevice& getDevice();
        LeResourceManager& leResourceManager;

        uint32_t addActor(LeActor actor);
        uint32_t addActor(int32_t model, int32_t texture);

        void toggleRenderHitboxes();
        bool getRenderHitboxes() const;

        LeActor& getActor(size_t index);
        const LeActor& getActor(size_t index) const;
        std::vector<LeActor>& getActors();
        const std::vector<LeActor>& getActors() const;

        LeActor* getActorById(LeActor::id_t id);
        const LeActor* getActorById(LeActor::id_t id) const;
        bool removeActorById(LeActor::id_t id);

        LeCamera& getCamera();
        const LeCamera& getCamera() const;
        LeActor& getCameraObject();
        const LeActor& getCameraObject() const;

        const std::vector<InstanceData>* instanceDataPtr = nullptr;

        std::unique_ptr<LeScene> clone() const;

        void setInstanceData(const std::vector<InstanceData>& data) {
            instanceDataPtr = &data;
        }

        bool setParent(LeActor::id_t child, LeActor::id_t parent);
        void clearParent(LeActor::id_t child);

        std::vector<LeActor::id_t> getRootActors() const;
        std::vector<LeActor::id_t> getChildren(LeActor::id_t parent) const;

        bool isDescendant(LeActor::id_t actor, LeActor::id_t potentialAncestor) const;

        //terrain
        void setTerrain(std::unique_ptr<ProceduralTerrain> terrain) {
            terrain_ = std::move(terrain);
        }

        ProceduralTerrain* getTerrain() const {
            return terrain_.get();
        }

        bool hasTerrain() const {
            return terrain_ != nullptr;
        }

    private:
        void createDefaultCamera();

        bool renderHitboxes_ = false;
        std::unique_ptr<ProceduralTerrain> terrain_ = nullptr;

        LeDevice& leDevice;
        std::vector<LeActor> actors;
        std::unique_ptr<LeActor> cameraObject;
        LeCamera camera;

        std::string name_;
    };

} // namespace le

#endif