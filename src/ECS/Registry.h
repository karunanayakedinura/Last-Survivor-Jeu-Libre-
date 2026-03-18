#pragma once
#include <queue>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include "ECS/Entity.h"
#include "ECS/ComponentArray.h"

namespace Engine::ECS {

    class Registry {
    public:
        Registry();

        // Entity Management
        Entity CreateEntity();
        void DestroyEntity(Entity entity);

        // --- NEW: Wipes all entities and resets the registry ---
        void Clear();

        // --- NEW: Entity Naming ---
        void SetEntityName(Entity entity, const std::string& name);
        std::string GetEntityName(Entity entity) const;

        bool IsValid(Entity entity) const;

        // Component Management
        template <typename T>
        void RegisterComponent() {
            std::type_index typeName = typeid(T);
            assert(m_ComponentArrays.find(typeName) == m_ComponentArrays.end() && "Registering component type more than once.");
            m_ComponentArrays[typeName] = std::make_shared<ComponentArray<T>>();
        }

        template <typename T>
        void AddComponent(Entity entity, T component) {
            GetComponentArray<T>()->InsertData(entity, component);
        }

        template <typename T>
        void RemoveComponent(Entity entity) {
            GetComponentArray<T>()->RemoveData(entity);
        }

        template <typename T>
        T& GetComponent(Entity entity) {
            return GetComponentArray<T>()->GetData(entity);
        }

        template <typename T>
        bool HasComponent(Entity entity) {
            return GetComponentArray<T>()->HasData(entity);
        }

        // Gets the component if it exists, otherwise creates it, adds it, and returns it
        template <typename T, typename... Args>
        T& GetOrAddComponent(Entity entity, Args&&... args) {
            if (HasComponent<T>(entity)) {
                return GetComponent<T>(entity);
            }

            // Construct the component using the provided arguments (if any)
            T newComponent{std::forward<Args>(args)...};
            AddComponent<T>(entity, newComponent);
            
            return GetComponent<T>(entity);
        }
        
        // Returns all entities that have a specific component
        template <typename T>
        const std::vector<Entity>& View() {
            return GetComponentArray<T>()->GetEntities();
        }

    private:
        std::queue<Entity> m_AvailableEntities;
        uint32_t m_LivingEntityCount = 0;

        // --- Active Entity Tracker ---
        std::unordered_set<Entity> m_ActiveEntities;

        // --- Map for storing entity names ---
        std::unordered_map<Entity, std::string> m_EntityNames;
        std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> m_ComponentArrays;

        template <typename T>
        std::shared_ptr<ComponentArray<T>> GetComponentArray() {
            std::type_index typeName = typeid(T);
            
            // Auto-register the component if it hasn't been registered yet (Lazy Initialization)
            if (m_ComponentArrays.find(typeName) == m_ComponentArrays.end()) {
                RegisterComponent<T>();
            }
            
            return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[typeName]);
        }
    };

} // namespace Engine::ECS