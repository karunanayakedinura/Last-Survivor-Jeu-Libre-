#pragma once
#include <map>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Blueprints/ReflectionMacros.h"
#include "Animation/AnimationUtils.h"
#include "Asset\AssetUtils.h"

namespace Engine::Components {
    struct Animator {
        REFLECT_CLASS(Animator)
        AUTHORIZE_BLUEPRINT_SCOPE(Animator)
        
        std::string m_SkeletonPath = "";
        
        std::map<std::string, Systems::Animation::AnimationUtils::BoneInfo> boneInfoMap;

        int m_BoneCounter = 0;

        std::map<std::string, std::shared_ptr<Systems::Animation::Animation>> m_Animations;

        Assets::AssetsUtils::CPUSkeletonData m_SkeletonData;
        bool m_HasSkeleton = false;

        std::vector<glm::mat4> m_FinalBoneMatrices;
    
        // Stores the raw global transform (Object Space) of each bone, without offset/inverseBind
        std::vector<glm::mat4> m_GlobalMatrices; 

        std::vector<Systems::Animation::AnimationUtils::AnimationState> m_ActiveAnimations;

    private:
        int GetBoneCount() {
            /* Gets the BoneCount of the Animator (int) */
            return static_cast<int>(boneInfoMap.size());
        }
        
        float HasSkeleton() {
            /* Gets the HasSkeleton state of the Animator (float) */
            return this->m_HasSkeleton;
        }
        
        int GetAnimationCount() {
            /* Gets the AnimationCount of the Animator (int) */
            return static_cast<int>(m_Animations.size());
        }
        
        std::vector<std::string> GetAnimationNames() {
            /* Gets the AnimationNames of the Animator (std::string) */
            std::vector<std::string> Names;
            for (auto it = m_Animations.begin(); it != m_Animations.end(); ++it) {
                Names.push_back(it->first);
            }
            return Names;
        }
    };

} // namespace Engine::Components