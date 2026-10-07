#ifndef LE_SCENE_HPP
#define LE_SCENE_HPP

#include "src/objects/le_actor.hpp"
#include "src/objects/le_camera.hpp"
#include "src/core/le_device.hpp"
#include "src/scene/le_resource_manager.hpp"
#include "src/systems/boids/instance_data.hpp"
#include "src/ecs/le_component_pool.hpp"
#include "src/systems/terrain-gen/procedural_terrain.hpp"

// std
#include <vector>
#include <memory>
#include <utility>
#include <typeindex>
#include <unordered_map>

namespace le {

    struct TestComponent
    {
        int value = 42;
    };

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

        //ecs
        template <typename T, typename... Args>
        T& addComponent(LeActor::id_t entity, Args&&... args)
        {
            auto& pool = getOrCreateComponentPool<T>();
            return pool.storage.add(entity, std::forward<Args>(args)...);
        }

        template <typename T>
        T* getComponent(LeActor::id_t entity)
        {
            auto* pool = getComponentPool<T>();
            if (!pool)
            {
                return nullptr;
            }

            return pool->storage.get(entity);
        }

        template <typename T>
        const T* getComponent(LeActor::id_t entity) const
        {
            const auto* pool = getComponentPool<T>();
            if (!pool)
            {
                return nullptr;
            }

            return pool->storage.get(entity);
        }

        template <typename T>
        bool hasComponent(LeActor::id_t entity) const
        {
            const auto* pool = getComponentPool<T>();
            return pool && pool->storage.has(entity);
        }

        template <typename T>
        bool removeComponent(LeActor::id_t entity)
        {
            auto* pool = getComponentPool<T>();
            return pool && pool->storage.remove(entity);
        }

        template <typename T, typename Function>
        void forEachComponent(Function&& function)
        {
            auto* pool = getComponentPool<T>();
            if (!pool)
            {
                return;
            }

            pool->storage.forEach(std::forward<Function>(function));
        }

        template <typename T, typename Function>
        void forEachComponent(Function&& function) const
        {
            const auto* pool = getComponentPool<T>();
            if (!pool)
            {
                return;
            }

            pool->storage.forEach(std::forward<Function>(function));
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

        struct IComponentPool
        {
            virtual ~IComponentPool() = default;
            virtual void remove(LeActor::id_t entity) = 0;
            virtual std::unique_ptr<IComponentPool> clone() const = 0;
        };

        template <typename T>
        struct ComponentPool final : IComponentPool
        {
            LeComponentPool<LeActor::id_t, T> storage;

            void remove(LeActor::id_t entity) override
            {
                storage.remove(entity);
            }

            std::unique_ptr<IComponentPool> clone() const override
            {
                return std::make_unique<ComponentPool<T>>(*this);
            }
        };

        template <typename T>
        ComponentPool<T>& getOrCreateComponentPool()
        {
            const std::type_index type = typeid(T);
            const auto it = componentPools_.find(type);

            if (it != componentPools_.end())
            {
                return *static_cast<ComponentPool<T>*>(it->second.get());
            }

            auto pool = std::make_unique<ComponentPool<T>>();
            auto* result = pool.get();

            componentPools_.emplace(type, std::move(pool));

            return *result;
        }

        template <typename T>
        ComponentPool<T>* getComponentPool()
        {
            const auto it = componentPools_.find(typeid(T));

            if (it == componentPools_.end())
            {
                return nullptr;
            }

            return static_cast<ComponentPool<T>*>(it->second.get());
        }

        template <typename T>
        const ComponentPool<T>* getComponentPool() const
        {
            const auto it = componentPools_.find(typeid(T));

            if (it == componentPools_.end())
            {
                return nullptr;
            }

            return static_cast<const ComponentPool<T>*>(it->second.get());
        }

        std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> componentPools_;
    };

} // namespace le

#endif