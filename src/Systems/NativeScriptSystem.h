#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <mutex>
// Platform-specific headers
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif


#include "Core/Engine.h"
#include "Core/ISystem.h"
#include "Core/Defines.h"
#include "Components/NativeScriptComponent.h"
#include "Scripting/NativeScripting.h"



// Forward declaration prevents needing to include ECS headers here
namespace Engine::ECS { class Registry; }
// namespace Engine::Components { struct NativeScriptComponent; }
// namespace Engine::Scripting { struct NativeScript; }
namespace Engine::Systems { class InputSystem; }


namespace Engine::Systems {
    // Define a function pointer type for our factory function
    using CreateScriptFn = Scripting::NativeScript*(*)();
    enum CompileRequestProvenence { EngineRequested, PackingRequested, UserRequested };

    class NativeScriptSystem : public Core::ISystem {
    public:
        NativeScriptSystem(ECS::Registry* registry, Core::Engine* engine);
        ~NativeScriptSystem() override;

        void OnInit() override;
        void OnFixedUpdate(float deltaTime) override;
        void OnPlayUpdate(float deltaTime) override;

        // Editor specific updates stripped in release builds
        EDITOR_METHOD(void OnEditorUpdate(float deltaTime) override;)
        EDITOR_METHOD(void OnEditorGUI() override;)


        // Checks all active NativeScriptComponents. If their source (.cpp) is newer than the DLL,
        // it unloads the script and adds the script name to the return list.
        std::vector<std::string> CheckForRecompilationAndUnload();

        
        void StartCompilation(const std::vector<std::string>& scripts, CompileRequestProvenence provenence = CompileRequestProvenence::UserRequested);
        
        
        // Getters for UI (Thread-safe)
        bool IsCompiling() const { return m_isCompiling; }
        bool IsCompileDone() const { return m_compileDone; }
        bool WasCompileSuccessful() const { return m_compileSuccess; }
        float GetCompileProgress() const { return m_compileProgress; }
        
        std::string GetStatusMessage();
        std::string GetTimeElapsed();
        std::string GetTimeRemaining();

        // Call this to acknowledge completion (resets flags for next time)
        void ResetCompilationState();

        void SetCompileRequestProvenence(CompileRequestProvenence provenence);
        CompileRequestProvenence GetCompileRequestProvenence();
    private:
        ECS::Registry* m_Registry;
        Core::Engine* m_Engine;

        std::string gen_random_name(const int len);

        void LoadScript(Components::NativeScriptComponent* nsc);

        void UnloadScript(Components::NativeScriptComponent* nsc);

        // Compilation Helpers
        std::string GetVSInstallPath();
        std::string FormatTime(float seconds);

        // Compilation State
        std::atomic<bool> m_isCompiling{false};
        std::atomic<bool> m_compileDone{false};
        std::atomic<bool> m_compileSuccess{false};
        std::atomic<float> m_compileProgress{0.0f};
        CompileRequestProvenence m_compileRequestProvenence = CompileRequestProvenence::EngineRequested;

        std::mutex m_stateMutex; // Protects strings below
        std::string m_compileStatusMsg;
        std::string m_timeElapsedStr = "00:00";
        std::string m_timeRemainingStr = "--:--";

        // --- NEW: Fixed Timestep Variables ---
        float m_FixedTimeStep = 0.02f; // 50 Hz
        float m_Accumulator = 0.0f;
    };

} // namespace Engine::Systems