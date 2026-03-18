#pragma once
#include <glm/glm.hpp>
#include "Asset\AssetUtils.h"
#include "BVH\AABB.h"

namespace Engine::Graphics {struct Mesh;}

namespace Engine::Systems::BVHUtils {
    // STD430 Aligned Node (64 bytes)
    struct GPUBVHNode {
        glm::vec3 aabbMin; float pad1;   
        glm::vec3 aabbMax; float pad2;   
        glm::uvec4 children;             
        uint32_t childCount;
        uint32_t firstPrim;
        uint32_t primCount;
        int32_t parent; // Changed pad3 to parent index for potential refitting
    }; 

    // STD430 Aligned Primitive (48 bytes)
    // Stores the Result (World Space) for raytracing
    struct GPUBVHPrimitive {
        glm::vec3 v0; float pad1;
        glm::vec3 v1; float pad2;
        glm::vec3 v2; float pad3;
    }; 

    // --- NEW: Source Data for GPU Updates ---
    // This allows us to re-transform the original mesh data every frame
    struct GPUSourcePrimitive {
        glm::vec4 v0_local; 
        glm::vec4 v1_local;
        glm::vec4 v2_local;
        
        // Bone Indices and Weights for skinning (4 per vertex)
        glm::uvec4 v0_bones; glm::vec4 v0_weights;
        glm::uvec4 v1_bones; glm::vec4 v1_weights;
        glm::uvec4 v2_bones; glm::vec4 v2_weights;
        
        uint32_t instanceID; // Index into the Instance Buffer
        uint32_t pad[3];
    };

    // Per-Entity data sent to GPU every frame
    struct GPUInstanceData {
        glm::mat4 modelMatrix;
        int32_t boneOffset; // Index into global bone buffer
        int32_t hasBones;   // Boolean flag
        int32_t pad[2];
    };

    // Internal CPU structures
    struct BVHPrimitive {
        Systems::BVHUtils::AABB bounds;
        glm::vec3 centroid;
        glm::vec3 v0, v1, v2; 
        uint32_t globalMeshID; 
        uint32_t triangleIndex; 
        
        // For source data generation
        uint32_t originalEntityIndex; 
        const Graphics::Mesh* sourceMesh;
    };

    struct BVHNode {
        Systems::BVHUtils::AABB bounds;
        uint32_t children[4] = {0, 0, 0, 0};
        uint32_t childCount = 0;
        uint32_t firstPrim = 0; 
        uint32_t primCount = 0; 
        int32_t parent = -1; 
        bool isLeaf() const { return primCount > 0; }
    };
}