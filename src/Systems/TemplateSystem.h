#pragma once
#include <iostream>
#include "Core/Engine.h"
#include "Core/ISystem.h"
#include "Core/Defines.h"

// Forward declaration prevents needing to include ECS headers here
namespace Engine::ECS { class Registry; }

namespace Engine::Systems {

    class TemplateSystem : public Core::ISystem {
    public:
        TemplateSystem(ECS::Registry* registry, Core::Engine* engine);
        ~TemplateSystem() override = default;

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
    };

} // namespace Engine::Systems