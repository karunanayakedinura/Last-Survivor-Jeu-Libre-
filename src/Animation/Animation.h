#pragma once
#include "Animation/AnimationUtils.h"
#include "Asset/AssetUtils.h"

namespace Engine::Systems::Animation {
    class Bone;
    class Animator;
}

namespace Engine::Systems::Animation {
    class Animation {
    public:
        Animation() = default;
        
        std::vector<std::string> BonesToLoc;
        std::string m_Path;

        Animation(const aiScene* scene, int animIndex, std::map<std::string, AnimationUtils::BoneInfo>& boneInfoMap);

        // --- NEW: Constructor for custom binary format ---
        Animation(const Assets::AssetsUtils::CPUAnimationData& animData, 
                const Assets::AssetsUtils::CPUSkeletonData& skeletonData, 
                std::map<std::string, AnimationUtils::BoneInfo>& boneInfoMap);

        Animation::Bone* FindBone(const std::string& name);

        float GetTicksPerSecond();
        float GetDuration();
        const AnimationUtils::AssimpNodeData& GetRootNode();
        const glm::mat4& GetGlobalInverseTransform() const;

    private:
        void ReadBones(const aiAnimation* animation, std::map<std::string, AnimationUtils::BoneInfo>& boneInfoMap);
        void ReadHierarchyData(AnimationUtils::AssimpNodeData& dest, const aiNode* src);

        // --- NEW: Helpers for binary format ---
        void BuildHierarchyFromSkeleton(AnimationUtils::AssimpNodeData& dest, const Assets::AssetsUtils::CPUSkeletonData& skeleton, int nodeIndex, const std::vector<std::vector<int>>& childrenMap);

        float m_Duration;
        float m_TicksPerSecond;
        std::vector<Bone> m_Bones;
        AnimationUtils::AssimpNodeData m_RootNode;
        glm::mat4 m_GlobalInverseTransform;
    };
}