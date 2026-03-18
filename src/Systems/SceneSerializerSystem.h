#pragma once
#include <iostream>
#include "Core/Engine.h"
#include "Core/ISystem.h"
#include "Core/Defines.h"

// Forward declaration prevents needing to include ECS headers here
namespace Engine::ECS { class Registry; }

#include <fstream>
#include <iostream>
#include <json.hpp>
using json = nlohmann::json;

namespace Engine::Systems {

    class SceneSerializerSystem : public Core::ISystem {
    public:
        SceneSerializerSystem(ECS::Registry* registry, Core::Engine* engine);
        ~SceneSerializerSystem() override = default;

        void OnInit() override;
        void OnShutdown() override;
        void OnFixedUpdate(float deltaTime) override;
        void OnPlayUpdate(float deltaTime) override;

        // Editor specific updates stripped in release builds
        EDITOR_METHOD(void OnEditorUpdate(float deltaTime) override;)
        EDITOR_METHOD(void OnEditorGUI() override;)

        void SerializeScene(const std::string& filepath);

        void DeserializeScene(const std::string& filepath);

    private:
        ECS::Registry* m_Registry;
        Core::Engine* m_Engine;
    };

} // namespace Engine::Systems