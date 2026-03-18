// src/Editor/AssetImporter.h
#pragma once
#include <string>

namespace Engine::Editor {
    class AssetImporter {
    public:
        // Converts source files (.fbx, .png, .wav) to Engine's .pa format
        static bool ImportAsset(const std::string& sourcePath, const std::string& destPath);

    private:
        static bool ImportTexture(const std::string& source, const std::string& dest);
        static bool ImportModel(const std::string& source, const std::string& dest);
        static bool ImportAudio(const std::string& source, const std::string& dest);
    };
}