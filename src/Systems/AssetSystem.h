#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <future>
#include <mutex>
#include <queue>
#include <variant>

#include "Core/ISystem.h"
#include "Core/Engine.h"
#include "ECS/Registry.h"
#include "Asset/AssetUtils.h"

#include "Animation/Bone.h"

// #include "Graphics/GLWrapper.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Texture.h"

// // Forward Declarations
// namespace Engine::Graphics {
//     struct Material;
//     class Texture2D;
//     struct Mesh;
// }

namespace Engine::Components {
    struct Animator;
}

namespace Engine::Systems {

    // Result structure passed from Worker Thread -> Main Thread
    struct AssetLoadResult {
        Assets::AssetsUtils::AssetType type;
        std::string path;
        ECS::Entity targetEntity = ECS::NULL_ENTITY;
        
        // Data Containers
        Assets::AssetsUtils::CPUTextureData textureData;
        Assets::AssetsUtils::CPUAudioData audioData;
        Assets::AssetsUtils::CPUMaterialData materialData;
        Assets::AssetsUtils::CPUSkeletonData skeletonData;
        Assets::AssetsUtils::CPUAnimationData animationData;
        Assets::AssetsUtils::CPUModelData modelData;

        // Config
        std::string nameOverride;
        float weight = 0.0f;
        bool looping = true;
        bool success = false;
        std::string errorMessage;
    };

    // Internal State Tracking
    struct AssetState {
        Assets::AssetsUtils::AssetType type = Assets::AssetsUtils::AssetType::None;
        int lockCount = 0;
        float lastAccessedTime = 0.0f;
        bool isLoaded = false;
        bool isLoading = false;
        bool loadFailed = false;

        // We use a variant or void* to hold the actual resource in the registry
        // ensuring the System keeps it alive.
        std::shared_ptr<void> strongRef = nullptr;
        // We also keep a weak ref to check if it's still used externally after we prune it
        std::weak_ptr<void> weakRef; 
    };

    struct LoadRequest {
        std::string path;
        ECS::Entity target = ECS::NULL_ENTITY;
        std::string animName;
        float weight = 0.0f;
        bool looping = true;
        float deadline = 0.0f; // Seconds until needed. Lower = Higher Priority.

        std::vector<std::string> dependencies; // <-- List of required asset paths

        // Operator for Priority Queue (Min-Heap behavior needed for deadline)
        bool operator>(const LoadRequest& other) const {
            return deadline > other.deadline;
        }
    };

    class AssetSystem : public Core::ISystem {
    public:
        AssetSystem(ECS::Registry* registry, Core::Engine* engine);
        ~AssetSystem() override;

        void OnInit() override;
        void OnPlayUpdate(float deltaTime) override;
        void OnFixedUpdate(float deltaTime) override;
        EDITOR_METHOD(void OnEditorUpdate(float deltaTime) override;)
        void OnShutdown() override;

        // --- State Management ---

        /**
         * @brief Locks an asset in memory, preventing garbage collection.
         * If the asset is not loaded, it triggers a high-priority load.
         */
        void LockAsset(const std::string& path);

        /**
         * @brief Decrements the lock count. If 0, the asset becomes eligible for GC.
         */
        void UnlockAsset(const std::string& path);

        /**
         * @brief Hints the system that an asset will be needed soon.
         * @param path The file path.
         * @param timeToNeed Estimated time in seconds until needed. 0.0f = Immediate/ASAP.
         */
        void HintAsset(const std::string& path, float timeToNeed);

        // --- Request API ---

        /**
         * @brief Request a model. Equivalent to Hint(path, 0) + Entity Assignment.
         */
        void RequestModel(const std::string& path, ECS::Entity target, 
                          const std::string& animationName = "", float weight = 0.0f, bool looping = true, 
                          const std::vector<std::string>& dependencies = {});

        // --- NEW: FETCH PREVIEW DATA ---
        bool GetAssetPreview(const std::string& path, int& width, int& height, std::vector<unsigned char>& outPixels);

        std::shared_ptr<Graphics::Texture2D> GetTexture(const std::string& path);
        std::shared_ptr<Graphics::Material> GetMaterial(const std::string& path);

        // --- NEW GETTERS FOR UI PROFILING ---
        size_t GetActiveTasksCount() const { return m_ActiveTasks.size(); }
        size_t GetLoadQueueSize() const { return m_LoadQueue.size(); }

        // --- Garbage Collection ---
        
        /**
         * @brief Configures the time (in seconds) an unlocked asset stays in memory after last access.
         */
        void SetPruneTimeout(float seconds);
        
        /**
         * @brief Manually triggers garbage collection of unused assets.
         */
        void PruneAssets();

        void ClearCache();
        
        /**
         * @brief Forces an immediate garbage collection, ignoring the prune timeout.
         * Useful for instantly freeing memory when unloading a scene.
         */
        void ForcePrune();
    private:
        ECS::Registry* m_Registry;
        Core::Engine* m_Engine;

        // Config
        float m_PruneTimeout = 10.0f; // 10 Seconds default
        float m_TimeSinceLastPrune = 0.0f;
        const int MAX_CONCURRENT_LOADS = 4;

        // State Registry
        std::unordered_map<std::string, AssetState> m_AssetRegistry;
        std::recursive_mutex m_RegistryMutex;

        // Async Management
        std::priority_queue<LoadRequest, std::vector<LoadRequest>, std::greater<LoadRequest>> m_LoadQueue;
        std::vector<LoadRequest> m_WaitingQueue;
        
        struct ActiveTask {
            std::future<AssetLoadResult> future;
            std::string path;
        };
        std::vector<ActiveTask> m_ActiveTasks;

        // Helpers
        void UpdateLoading(float deltaTime);
        void ProcessQueue();
        void FinalizeLoad(AssetLoadResult& result);
        
        // Worker
        AssetLoadResult LoadAssetWorker(LoadRequest request);

        // Type-Specific Generators
        std::shared_ptr<Graphics::Texture2D> UploadTexture(Assets::AssetsUtils::CPUTextureData& data);

        // Cache Accessors (Typed Wrappers around Registry)
        template<typename T>
        std::shared_ptr<T> GetCachedResource(const std::string& path, Assets::AssetsUtils::AssetType type);

        void UpdateAccessTime(const std::string& path);

        std::vector<std::weak_ptr<Graphics::Material>> m_PendingMaterials;
        void ResolvePendingMaterialTextures();
    };

} // namespace Engine::Systems