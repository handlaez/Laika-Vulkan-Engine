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

        LeDevice& getDevice();
        LeResourceManager& leResourceManager;

        uint32_t addActor(LeActor actor);
        std::shared_ptr<LeActor> addActor(int32_t model, int32_t texture);

        void toggleRenderHitboxes();
        bool getRenderHitboxes() const;

        std::vector<std::shared_ptr<LeActor>>& getActors();
        std::shared_ptr<LeActor> getActor(int index);

        LeCamera& getCamera();
        const LeCamera& getCamera() const;

        LeActor& getCameraObject();

        const std::vector<InstanceData>* instanceDataPtr = nullptr;

        void setInstanceData(const std::vector<InstanceData>& data) {
            instanceDataPtr = &data;
        }

        void setTerrain(std::unique_ptr<ProceduralTerrain> terrain) {
            m_terrain = std::move(terrain);
        }

        ProceduralTerrain* getTerrain() const {
            return m_terrain.get();
        }

        bool hasTerrain() const {
            return m_terrain != nullptr;
        }

    private:
        void createDefaultCamera();
        bool renderHitboxes_;

        std::unique_ptr<ProceduralTerrain> m_terrain = nullptr;

        LeDevice& leDevice;
        std::vector<std::shared_ptr<LeActor>> actors;

        std::unique_ptr<LeActor> cameraObject;
        LeCamera camera;
    };

} // namespace le

#endif
