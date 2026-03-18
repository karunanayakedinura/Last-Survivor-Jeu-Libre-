#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "Core/Defines.h"
#include "Core/ISystem.h"
#include "ECS/Registry.h" // Include your new ECS
#include "Graphics/GLWrapper.h"

#include "Components/Transform.h"
#include "Components/Camera.h"

// extern ENGINE_API bool g_MouseCapturedByUI;

namespace Engine::Core {
    // A clean, exported wrapper for UI state
    class ENGINE_API UIState {
    public:
        static bool IsMouseCaptured();
        static void SetMouseCaptured(bool state);
    };

    enum class EngineState {
        Edit,
        Play,
        Pause
    };

    class Engine {
    public:
        Engine();
        int Initialize();
        void Run();
        void Shutdown();

        // System Registration now easily passes the registry pointer automatically
        template<typename T, typename... Args>
        void AddSystem(Args&&... args) {
            m_Systems.push_back(std::make_unique<T>(&m_Registry, std::forward<Args>(args)...));
        }

        // Expose registry for editor / debugging
        ECS::Registry& GetRegistry() { return m_Registry; }

        // --- NEW: Helper Methods for Systems/Scripts ---

        EngineState GetState() const {
            return m_Current_State;
        }

        // Safely retrieves a dynamically loaded system by its string name
        Core::ISystem* GetDynamicSystem(const std::string& name) {
            for (auto& sys : m_Systems) {
                if (sys->m_IsDynamic && sys->m_Name == name) {
                    return sys.get();
                }
            }
            return nullptr;
        }

        // Retrieves a previously registered system (useful for grabbing the InputSystem)
        template<typename T>
        T* GetSystem() {
            for (auto& sys : m_Systems) {
                if (sys->m_IsDynamic) continue; // CRITICAL: Skip DLL boundary RTTI checks

                if (T* target = dynamic_cast<T*>(sys.get())) {
                    return target;
                }
            }
            return nullptr;
        }

        void OnWindowResize(int width, int height);

        void SetPlayMode();
        void SetStopMode();
        void SetPauseMode(bool state);

        // --- NEW: Editor Camera Management ---
        ECS::Entity m_EditorCamera = ECS::NULL_ENTITY;

        // -------------------------------------
        Components::Transform m_EditorCameraTransform;

        Components::Camera m_EditorCameraCameraComponent;
        
        void DestroyEditorCamera();
        ECS::Entity GetEditorCamera();
        void CreateEditorCamera();

        void SetCurrentScenePath(const std::string& path) { m_CurrentScenePath = path; }
        std::string GetCurrentScenePath() const { return m_CurrentScenePath; }

        void LoadScene(const std::string& path);

        void AddDynamicSystem(std::unique_ptr<ISystem> system) {
            m_Systems.push_back(std::move(system));
            // If the engine is already running, initialize it immediately
            if (m_Current_State != EngineState::Edit && m_Current_State != EngineState::Play) {
                // Safe fallback, though you ideally load these during Engine::Initialize
            }
        }

    private:
        ECS::Registry m_Registry; // <-- Central ECS Registry
        std::vector<std::unique_ptr<ISystem>> m_Systems;
        
        #if defined(BUILD_ENGINE_EDITOR)
        EngineState m_Current_State = EngineState::Edit;
        #else
        EngineState m_Current_State = EngineState::Play;
        #endif

        float lastTime = (float)glfwGetTime();
        float delay = 0.0f;
        GLFWwindow* window;

        // Track the current scene loaded from disk
        std::string m_CurrentScenePath = "Assets/Scenes/DefaultScene.pscene";
    };

} // namespace Engine::Core