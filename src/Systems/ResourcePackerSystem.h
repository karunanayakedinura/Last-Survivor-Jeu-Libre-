#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <iostream>
#include "Core/Engine.h"
#include "Core/ISystem.h"
#include "Core/Defines.h"
#include <json.hpp> 
using json = nlohmann::json;
#include "Package/RessourcePackerUtils.h"

// Forward declaration prevents needing to include ECS headers here
namespace Engine::ECS { class Registry; }

namespace Engine::Systems {
    // // Define the callback type: (processedCount, totalCount, currentFileName)
    // using PackProgressCallback = std::function<void(int, int, const std::string&, float, float)>;

    class ResourcePackerSystem : public Core::ISystem {
    public:
        ResourcePackerSystem(ECS::Registry* registry, Core::Engine* engine);
        ~ResourcePackerSystem() override = default;
        void SetEncryptionKey(const std::string& encryptionKey);
        
        EDITOR_METHOD(void AddFile(const std::string& internalPath, const std::string& diskPath);)
    
        EDITOR_METHOD(bool BuildFromManifest(const std::string& manifestPath, Systems::RessourcePackerUtils::PackProgressCallback onProgress = nullptr);)

        EDITOR_METHOD(bool Build(const std::string& outputPath, Systems::RessourcePackerUtils::PackProgressCallback onProgress = nullptr);)

        void OnInit() override;
        void OnFixedUpdate(float deltaTime) override;
        void OnPlayUpdate(float deltaTime) override;

        // Editor specific updates stripped in release builds
        EDITOR_METHOD(void OnEditorUpdate(float deltaTime) override;)
        EDITOR_METHOD(void OnEditorGUI() override;)

    private:
        ECS::Registry* m_Registry;
        Core::Engine* m_Engine;

        struct FileToPack {
            std::string internalPath;
            std::string diskPath;
        };
        std::vector<FileToPack> m_Files;
        std::string m_Key;
    };

} // namespace Engine::Systems