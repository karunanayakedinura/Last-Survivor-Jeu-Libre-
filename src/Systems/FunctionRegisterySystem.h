#pragma once
#include <iostream>
#include "Core/Engine.h"
#include "Core/ISystem.h"
#include "Core/Defines.h"
#include "FunctionRegistery/FunctionRegisteryHelper.h"
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <map>


// Forward declaration prevents needing to include ECS headers here
namespace Engine::ECS { class Registry; }

namespace Engine::Systems {

    class ENGINE_API FunctionRegisterySystem : public Core::ISystem {
    public:
        FunctionRegisterySystem(ECS::Registry* registry, Core::Engine* engine);
        ~FunctionRegisterySystem() override = default;

        // Registers a function with a unique string key.
        void Register(const std::string& name, Systems::FunctionRegistery::ReflectedFunction func);

        // Unregisters a function.
        void Unregister(const std::string& name);

        // Calls a function by its key, passing arguments and returning a value.
        std::any Call(const std::string& name, std::vector<std::any> args = {});
        
        bool HasBeenRegistered(const std::string& name, std::vector<std::any> args = {});

        // Clear registry when switching modes to prevent dangling pointers to unloaded DLL code
        void OnStop();

        void OnInit() override;
        void OnShutdown() override;
        void OnFixedUpdate(float deltaTime) override;
        void OnPlayUpdate(float deltaTime) override;

        // Editor specific updates stripped in release builds
        EDITOR_METHOD(void OnEditorUpdate(float deltaTime) override;)
        EDITOR_METHOD(void OnEditorGUI() override;)

    private:
        ECS::Registry* m_Registry;
        Core::Engine* m_Engine;

        std::map<std::string, Systems::FunctionRegistery::ReflectedFunction> m_registry;
        mutable std::shared_mutex m_Mutex; // NEW: Mutex for thread safety
    };

} // namespace Engine::Systems