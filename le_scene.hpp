#ifndef LE_SCENE_HPP
#define LE_SCENE_HPP

#include "le_actor.hpp"
#include "le_camera.hpp"
#include "le_device.hpp"
#include "le_resource_manager.hpp"
#include "instance_data.hpp"
#include "procedural_terrain.hpp"

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
        void addActor(int32_t model, int32_t texture);

        std::vector<LeActor>& getActors();
        const std::vector<LeActor>& getActors() const;
        LeActor& getActor(int index);

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

        std::unique_ptr<ProceduralTerrain> m_terrain = nullptr;

        LeDevice& leDevice;
        std::vector<LeActor> actors;

        std::unique_ptr<LeActor> cameraObject;
        LeCamera camera;
    };

} // namespace le

#endif
