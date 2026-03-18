#pragma once
#include "Animation/AnimationUtils.h"
#include "Asset/AssetUtils.h"

#include <glm/gtx/matrix_decompose.hpp>


namespace Engine::Systems::Animation {
    class Bone {
    public:
        Bone(const std::string& name, int ID, const aiNodeAnim* channel);

        // --- NEW: Constructor for custom binary format ---
        Bone(const std::string& name, int ID, const Assets::AssetsUtils::CPUAnimationData::Channel& channel);

        void Update(float animationTime);

        glm::mat4 GetLocalTransform();

        std::string GetBoneName() const;

        int GetBoneID();

    private:
        float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
        glm::mat4 InterpolatePosition(float animationTime);
        glm::mat4 InterpolateRotation(float animationTime);
        glm::mat4 InterpolateScaling(float animationTime);

        // --- NEW: Fast index lookups ---
        int GetPositionIndex(float animationTime);
        int GetRotationIndex(float animationTime);
        int GetScaleIndex(float animationTime);
        
        std::vector<AnimationUtils::KeyPosition> m_Positions;
        std::vector<AnimationUtils::KeyRotation> m_Rotations;
        std::vector<AnimationUtils::KeyScale> m_Scales;
        int m_NumPositions;
        int m_NumRotations;
        int m_NumScalings;

        // --- NEW: Cached indices for O(1) lookups ---
        int m_LastPositionIndex = 0;
        int m_LastRotationIndex = 0;
        int m_LastScaleIndex = 0;

        glm::mat4 m_LocalTransform;
        std::string m_Name;
        int m_ID;
    };
}