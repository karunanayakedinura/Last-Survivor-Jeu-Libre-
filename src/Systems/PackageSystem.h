#pragma once
#include <iostream>
#include "Core/Engine.h"
#include "Core/ISystem.h"
#include "Core/Defines.h"
#include "Package/PackageUtils.h"

// Forward declaration prevents needing to include ECS headers here
namespace Engine::ECS { class Registry; }

namespace Engine::Systems {

    class PackageSystem : public Core::ISystem {
    public:
        PackageSystem(ECS::Registry* registry, Core::Engine* engine);
        ~PackageSystem() override = default;

        // --- Add Getters for UI ---
        bool IsMounted() const;
        const std::string& GetPackPath() const;
        const std::unordered_map<std::string, PackageUtils::PakEntry>& GetIndex() const;
        // --------------------------

        // Helper to ensure paths match regardless of slashes
        std::string NormalizePath(std::string path);

        // --- DEBUG & UTILITY METHODS ---

        /**
         * @brief Prints all files currently indexed in the .pak to the terminal with metadata.
         */
        void DebugPrintFiles();

        /**
         * @brief Returns a vector containing all internal file paths in the mounted .pak.
         */
        std::vector<std::string> GetFileList();

        bool Mount(const std::string& path, const std::string& key);

        std::vector<unsigned char> GetFileData(std::string path);

        // NEW: Open a stream for a file
        std::unique_ptr<PackageUtils::IFileStream> OpenStream(const std::string& path);

        void OnInit() override;
        void OnFixedUpdate(float deltaTime) override;
        void OnPlayUpdate(float deltaTime) override;

        // Editor specific updates stripped in release builds
        EDITOR_METHOD(void OnEditorUpdate(float deltaTime) override;)
        EDITOR_METHOD(void OnEditorGUI() override;)

    private:
        ECS::Registry* m_Registry;
        Core::Engine* m_Engine;

        std::string m_PackPath;
        std::string m_Key;
        std::unordered_map<std::string, PackageUtils::PakEntry> m_Index;
        std::ifstream m_PackFile;
        bool m_isMounted = false;

    };

} // namespace Engine::Systems