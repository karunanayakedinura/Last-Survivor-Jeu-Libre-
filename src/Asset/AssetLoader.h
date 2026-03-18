#pragma once
#include "Asset/AssetUtils.h"

namespace Engine::Assets::Runtime {
    class AssetLoader {
    public:
        static AssetsUtils::AssetType GetAssetType(const std::string& path);
        
        // --- File Loading Methods ---
        static bool LoadTexture(const std::string& path, AssetsUtils::CPUTextureData& outData);
        static bool LoadAudio(const std::string& path, AssetsUtils::CPUAudioData& outData);
        static bool LoadMaterial(const std::string& path, AssetsUtils::CPUMaterialData& outData);
        static bool LoadSkeleton(const std::string& path, AssetsUtils::CPUSkeletonData& outData);
        static bool LoadModel(const std::string& path, AssetsUtils::CPUModelData& outData);
        static bool LoadAnimation(const std::string& path, AssetsUtils::CPUAnimationData& outData);

        // --- Memory Loading Methods (Added) ---
        static bool LoadTexture(const std::vector<unsigned char>& data, AssetsUtils::CPUTextureData& outData);
        static bool LoadAudio(const std::vector<unsigned char>& data, AssetsUtils::CPUAudioData& outData);
        static bool LoadMaterial(const std::vector<unsigned char>& data, AssetsUtils::CPUMaterialData& outData);
        static bool LoadSkeleton(const std::vector<unsigned char>& data, AssetsUtils::CPUSkeletonData& outData);
        static bool LoadModel(const std::vector<unsigned char>& data, AssetsUtils::CPUModelData& outData);
        static bool LoadAnimation(const std::vector<unsigned char>& data, AssetsUtils::CPUAnimationData& outData);

        static const std::vector<std::pair<AssetsUtils::AssetType, bool(*)(const std::string&, void*)>> classAssetTypeToString;
    };

}