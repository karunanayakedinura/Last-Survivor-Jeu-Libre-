#pragma once
#include <memory>
// #include <glm/glm.hpp>
// #include <glm/gtc/type_ptr.hpp>
// #include <glm/gtc/matrix_transform.hpp>
#include "Blueprints/ReflectionMacros.h"
#include "Scripting/NativeScripting.h"
// namespace Engine::Scripting { struct NativeScript; }
namespace Engine::Systems { class NativeScriptSystem; }

namespace Engine::Components {
    struct NativeScriptComponent {
        REFLECT_CLASS(NativeScriptComponent)
        AUTHORIZE_BLUEPRINT_SCOPE(NativeScriptComponent)
        NativeScriptComponent() = default;
        NativeScriptComponent(std::string path) {
            DllPath = path;
        }

        void DestroyScriptInstance() {
            if (m_Instance) {
                m_Instance->OnDestroy();
                m_Instance.reset();
            }
        }
        void* DetachLibrary() {
            void* handle = m_LibraryHandle;
            m_LibraryHandle = nullptr; // Clear internal handle so destructor doesn't free it
            return handle;
        }
        ~NativeScriptComponent() {
            // Unload();
        }

        std::string DllPath;
        std::string TempDiskPath;
        bool Enabled = true;
        bool RunInEditor = false;

        // Called when: registry.CloneEntity(id) -> InsertData(newID, oldComp)
        NativeScriptComponent(const NativeScriptComponent& other) {
            // Copy the configuration data
            this->DllPath = other.DllPath;
            this->Enabled = other.Enabled;

            // CRITICAL: Reset the runtime pointers!
            // We do NOT want the new entity to hijack the old entity's running script.
            this->m_LibraryHandle = nullptr; 
            this->m_Instance = nullptr;      
        }

        // 3. Copy Assignment Operator
        // Called when: std::vector resizes or you assign values
        NativeScriptComponent& operator=(const NativeScriptComponent& other) {
            if (this == &other) return *this;

            this->DllPath = other.DllPath;
            this->Enabled = other.Enabled;

            // Reset runtime pointers
            this->m_LibraryHandle = nullptr;
            this->m_Instance = nullptr;

            return *this;
        }
        
    private:
        friend class Systems::NativeScriptSystem;
        // Opaque handle to the loaded library
        void* m_LibraryHandle = nullptr;
        // Pointer to the script instance created from the DLL
        std::unique_ptr<Scripting::NativeScript> m_Instance = nullptr;
    };

} // namespace Engine::Components