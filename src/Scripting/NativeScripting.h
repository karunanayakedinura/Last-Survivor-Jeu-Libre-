#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "ECS/Entity.h"   // Include the new Entity type
#include "Scripting/NativeScriptingHelpers.h"   // Include ScriptProp and ScriptPropType

// Forward declarations for the new architecture
namespace Engine::ECS { class Registry; }
namespace Engine::Core { class Engine; }
namespace Engine::Systems { class NativeScriptSystem; class InputSystem; class TerminalSystem;}

// (Keep your ScriptPropType and ScriptProp definitions...)

namespace Engine::Scripting {
    
    class NativeScript {
    public:
        virtual ~NativeScript() {}

        virtual void OnCreate() {}
        virtual void OnInit() {}
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnFixedUpdate(float fixedDeltaTime) {}
        virtual void OnPhysicsUpdate(float fixedDeltaTime) {}       
        virtual void OnDestroy() {}
        
        virtual std::string GetName() { return "NativeScript"; }

        // --- Core Dependencies ---
        Core::Engine* engine = nullptr;
        ECS::Registry* registry = nullptr; // NEW: Needed to fetch components
        ECS::Entity entityID = ECS::NULL_ENTITY; // NEW: Replaces Entity* m_Entity

        Systems::InputSystem* InputSysteminstance = nullptr;
        Systems::TerminalSystem* TerminalInstance = nullptr;

        // --- NEW: PROPERTY REGISTRY SYSTEM ---
        std::vector<Helpers::ScriptProp> m_ExposedProperties;

        // Call these in OnCreate() to expose variables
        void Inspect(const std::string& name, float* val) {
            m_ExposedProperties.push_back({name, Helpers::ScriptPropType::Float, val});
        }
        void Inspect(const std::string& name, int* val) {
            m_ExposedProperties.push_back({name, Helpers::ScriptPropType::Int, val});
        }
        void Inspect(const std::string& name, bool* val) {
            m_ExposedProperties.push_back({name, Helpers::ScriptPropType::Bool, val});
        }
        void Inspect(const std::string& name, glm::vec3* val) {
            m_ExposedProperties.push_back({name, Helpers::ScriptPropType::Vec3, val});
        }
        // Helper for Color (treated as Vec3 but drawn differently)
        void InspectColor(const std::string& name, glm::vec3* val) {
            m_ExposedProperties.push_back({name, Helpers::ScriptPropType::Color, val});
        }
        // -------------------------------------

        template<typename T>
        T& GetComponent() {
            return registry->GetComponent<T>(entityID);
        }

        template<typename T>
        bool HasComponent() {
            return registry->HasComponent<T>(entityID);
        }

    private:
        friend class Systems::NativeScriptSystem;
    };

} // namespace Engine::Scripting