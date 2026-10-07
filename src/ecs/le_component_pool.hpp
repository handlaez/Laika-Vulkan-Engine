#ifndef LE_COMPONENT_POOL_HPP
#define LE_COMPONENT_POOL_HPP

#include <cstddef>
#include <unordered_map>
#include <utility>
#include <vector>

namespace le
{
    template <typename EntityId, typename T>
    class LeComponentPool
    {
    public:
        template <typename... Args>
        T& add(EntityId entity, Args&&... args)
        {
            const auto [it, inserted] = indices_.try_emplace(entity, components_.size());

            if (!inserted)
            {
                return components_[it->second];
            }

            entities_.push_back(entity);
            components_.emplace_back(std::forward<Args>(args)...);

            return components_.back();
        }

        bool has(EntityId entity) const
        {
            return indices_.contains(entity);
        }

        T* get(EntityId entity)
        {
            const auto it = indices_.find(entity);

            if (it == indices_.end())
            {
                return nullptr;
            }

            return &components_[it->second];
        }

        const T* get(EntityId entity) const
        {
            const auto it = indices_.find(entity);

            if (it == indices_.end())
            {
                return nullptr;
            }

            return &components_[it->second];
        }

        bool remove(EntityId entity)
        {
            const auto it = indices_.find(entity);

            if (it == indices_.end())
            {
                return false;
            }

            const size_t index = it->second;
            const size_t lastIndex = components_.size() - 1;

            if (index != lastIndex)
            {
                components_[index] = std::move(components_[lastIndex]);

                const EntityId movedEntity = entities_[lastIndex];
                entities_[index] = movedEntity;
                indices_[movedEntity] = index;
            }

            components_.pop_back();
            entities_.pop_back();
            indices_.erase(it);

            return true;
        }

        template <typename Function>
        void forEach(Function&& function)
        {
            for (size_t i = 0; i < components_.size(); ++i)
            {
                function(entities_[i], components_[i]);
            }
        }

        template <typename Function>
        void forEach(Function&& function) const
        {
            for (size_t i = 0; i < components_.size(); ++i)
            {
                function(entities_[i], components_[i]);
            }
        }

        const std::vector<EntityId>& getEntities() const
        {
            return entities_;
        }

    private:
        std::vector<EntityId> entities_;
        std::vector<T> components_;
        std::unordered_map<EntityId, size_t> indices_;
    };
}

#endif