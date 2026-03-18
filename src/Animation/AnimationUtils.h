#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <assimp/scene.h>


namespace Engine::Systems::Animation {
    class Animation;
}

namespace Engine::Systems::Animation::AnimationUtils {
    struct AnimationState {
        Animation* animation = nullptr;
        float currentTime = 0.0f;
        float weight = 0.0f;
        bool isLooping = true;
    };

    struct BoneInfo {
        int id;
        glm::mat4 offset;
    };

    static inline glm::mat4 AssimpToGLM(const aiMatrix4x4& from) {
        glm::mat4 to;
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
        return to;
    };

    static inline glm::vec3 AssimpToGLM(const aiVector3D& vec) {
        return glm::vec3(vec.x, vec.y, vec.z);
    };

    static inline glm::quat AssimpToGLM(const aiQuaternion& p) {
        return glm::quat(p.w, p.x, p.y, p.z);
    };

    struct KeyPosition {
        glm::vec3 position;
        float timeStamp;
    };

    struct KeyRotation {
        glm::quat orientation;
        float timeStamp;
    };

    struct KeyScale {
        glm::vec3 scale;
        float timeStamp;
    };

    struct AssimpNodeData {
        glm::mat4 transformation;
        std::string name;
        int childrenCount;
        std::vector<AssimpNodeData> children;
    };
}