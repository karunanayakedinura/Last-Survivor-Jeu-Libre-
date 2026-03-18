#pragma once

#include "Core/ISystem.h"
#include "ECS/Registry.h"
#include "Systems/AnimatorSystem.h"
#include <vector>
#include <string>

namespace Engine::Systems {

    struct ContactPoint {
        glm::vec3 position;
        float time;
        std::string boneName;
    };

    class MotionMatchingSystem : public Core::ISystem {
    public:
        MotionMatchingSystem(ECS::Registry* registry, Core::Engine* engine);
        ~MotionMatchingSystem() override = default;

        void OnInit() override {}
        void OnPlayUpdate(float deltaTime) override {}
        void OnFixedUpdate(float deltaTime) override;

        // Analyzes an animation to find where feet touch the ground
        void ComputeAndVisualizeFootsteps(ECS::Entity entity, const std::string& animationName);

    private:
        // Helper to sample a bone's world position at a specific time without affecting runtime playback
        // glm::vec3 SampleBonePosition(Components::Animator& animator, Systems::Animation::Animation* anim, float time, const std::string& boneName);
        
        // Recursive helper to compute global transforms for sampling
        // void BuildSampleTransform(Systems::Animation::Animation* anim, const Systems::Animation::AnimationUtils::AssimpNodeData* node, 
        //                           float time, const glm::mat4& parentTransform, 
        //                           std::map<std::string, glm::mat4>& outTransforms);
        void BuildSampleTransform(Systems::Animation::Animation* anim, const Systems::Animation::AnimationUtils::AssimpNodeData* node, 
                              float time, const glm::mat4& parentTransform, 
                              std::unordered_map<std::string, glm::mat4>& outTransforms);

        ECS::Registry* m_Registry;
        Core::Engine* m_Engine;
        AnimatorSystem* m_AnimatorSystem;

        const std::vector<std::string> m_FootBoneNames = { "LeftFoot", "RightFoot", "mixamorig:LeftFoot", "mixamorig:RightFoot", "Mimic_foot_l", "Mimic_foot_r", "foot_l", "foot_r", "ik_foot_l", "ik_foot_r" };
    };

} // namespace Engine::Systems