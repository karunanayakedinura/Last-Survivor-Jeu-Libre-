#pragma once
#include <vector>
#include <unordered_map>
#include <cassert>
#include "ECS/Entity.h"

namespace Engine::ECS {

    // Interface so the Registry can hold a collection of different array types
    class IComponentArray {
    public:
        virtual ~IComponentArray() = default;
        virtual void EntityDestroyed(Entity entity) = 0;
    };

    template <typename T>
    class ComponentArray : public IComponentArray {
    public:
        void InsertData(Entity entity, T component) {
            assert(m_EntityToIndexMap.find(entity) == m_EntityToIndexMap.end() && "Component added to same entity more than once.");

            size_t newIndex = m_Size;
            m_EntityToIndexMap[entity] = newIndex;
            
            // Safely add to the new dense entity vector
            if (newIndex >= m_IndexToEntityMap.size()) {
                m_IndexToEntityMap.push_back(entity);
            } else {
                m_IndexToEntityMap[newIndex] = entity;
            }

            if (newIndex >= m_ComponentArray.size()) {
                m_ComponentArray.push_back(component);
            } else {
                m_ComponentArray[newIndex] = component;
            }
            m_Size++;
        }

        void RemoveData(Entity entity) {
            assert(m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end() && "Removing non-existent component.");

            // To keep the array packed, move the last element into the deleted element's spot
            size_t indexOfRemovedEntity = m_EntityToIndexMap[entity];
            size_t indexOfLastElement = m_Size - 1;
            
            // Move the component
            m_ComponentArray[indexOfRemovedEntity] = m_ComponentArray[indexOfLastElement];

            // Move the entity in the dense array
            Entity entityOfLastElement = m_IndexToEntityMap[indexOfLastElement];
            m_IndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

            // Update the sparse map to point the moved entity to its new index
            m_EntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;

            // Remove from sparse map
            m_EntityToIndexMap.erase(entity);
            
            // Remove the duplicate last entry in the dense array
            m_IndexToEntityMap.pop_back(); 
            
            m_Size--;
        }

        T& GetData(Entity entity) {
            assert(m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end() && "Retrieving non-existent component.");
            return m_ComponentArray[m_EntityToIndexMap[entity]];
        }

        bool HasData(Entity entity) const {
            return m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end();
        }

        void EntityDestroyed(Entity entity) override {
            if (m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end()) {
                RemoveData(entity);
            }
        }

        // Expose an iterator for systems
        // std::vector<Entity> GetEntities() const {
        //     std::vector<Entity> entities;
        //     for (auto const& [entity, index] : m_EntityToIndexMap) {
        //         entities.push_back(entity);
        //     }
        //     return entities;
        // }
        const std::vector<Entity>& GetEntities() const {
            return m_IndexToEntityMap;
        }
    private:
        // The packed array of components (cache-friendly)
        std::vector<T> m_ComponentArray;
        
        // Sparse set mappings
        std::unordered_map<Entity, size_t> m_EntityToIndexMap;
        // std::unordered_map<size_t, Entity> m_IndexToEntityMap;
        std::vector<Entity> m_IndexToEntityMap;
        size_t m_Size = 0;
    };

} // namespace Engine::ECS