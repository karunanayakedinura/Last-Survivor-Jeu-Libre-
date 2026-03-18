#pragma once

#include "Core/Engine.h"
#include "Core/ISystem.h"
#include "ECS/Registry.h"
#include "Components/Animator.h"
#include "Asset/AssetUtils.h"
#include "Animation/Animation.h"

namespace Engine::Systems {

    class AnimatorSystem : public Core::ISystem {
    public:
        AnimatorSystem(ECS::Registry* registry, Core::Engine* engine);
        ~AnimatorSystem() override = default;

        void OnInit() override;
        void OnPlayUpdate(float deltaTime) override;

        EDITOR_METHOD(void OnEditorUpdate(float deltaTime) override;)

        // --- Skeleton Management ---
        void SetSkeleton(ECS::Entity entity, const Assets::AssetsUtils::CPUSkeletonData& skeleton);
        const Assets::AssetsUtils::CPUSkeletonData* GetSkeleton(ECS::Entity entity);
        bool HasSkeleton(ECS::Entity entity);

        // --- Animation Control ---
        void AddAnimation(ECS::Entity entity, const std::string& name, std::shared_ptr<Systems::Animation::Animation> animation);
        void PlayAnimation(ECS::Entity entity, const std::string& name, bool loop = true);
        void SetAnimationWeight(ECS::Entity entity, const std::string& name, float weight);
        void SetBonesToLock(ECS::Entity entity, const std::string& animationName, const std::vector<std::string>& bones);
        
        // --- State Accessors (Missing Methods Ported) ---
        const std::vector<glm::mat4>& GetFinalBoneMatrices(ECS::Entity entity);
        
        // Returns a pointer to the active state for a specific animation name, or nullptr if not active/found
        Systems::Animation::AnimationUtils::AnimationState* FindActiveState(ECS::Entity entity, const std::string& animationName);
        
        // Alternative: Find by Animation pointer if you have the resource
        Systems::Animation::AnimationUtils::AnimationState* FindActiveState(ECS::Entity entity, Systems::Animation::Animation* anim);

        // --- Motion Matching / Global Accessors ---
        glm::vec3 GetBoneGlobalPos(ECS::Entity entity, const std::string& name);
        glm::quat GetBoneGlobalRot(ECS::Entity entity, const std::string& name);
        
        void SetBoneGlobalPos(ECS::Entity entity, const std::string& name, glm::vec3 pos);
        void SetBoneGlobalRot(ECS::Entity entity, const std::string& name, glm::quat rot);

    private:
        ECS::Registry* m_Registry;
        Core::Engine* m_Engine;

        // Helper to keep references valid; returns static empty vector if entity invalid
        const std::vector<glm::mat4>& GetEmptyMatrices(); 

        void UpdateAnimator(Components::Animator& animator, float dt);
        void CalculateBoneTransform(Components::Animator& animator, const Systems::Animation::AnimationUtils::AssimpNodeData* node, const glm::mat4& parentTransform);
        void CalculateGlobalInverseBindPose(Components::Animator& animator, int nodeIndex, const glm::mat4& parentTransform, const std::vector<std::vector<int>>& childrenMap, const Assets::AssetsUtils::CPUSkeletonData& skeletonData);
        void NormalizeWeights(Components::Animator& animator);
        
        static constexpr int MAX_BONES = 250;
    };

} // namespace Engine::Systems