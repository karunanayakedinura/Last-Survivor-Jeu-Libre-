#pragma once
#include <iostream>
#include "Core/Engine.h"
#include "Core/ISystem.h"
#include "Core/Defines.h"
#include "BVH\BVHUtils.h"

// Forward declaration prevents needing to include ECS headers here
namespace Engine::ECS { class Registry; }

namespace Engine::Systems {

    class BVHSystem : public Core::ISystem {
    public:
        BVHSystem(ECS::Registry* registry, Core::Engine* engine);
        ~BVHSystem() override = default;

        void OnInit() override;
        void OnShutdown() override;
        void OnFixedUpdate(float deltaTime) override;
        void OnPlayUpdate(float deltaTime) override;

        // Editor specific updates stripped in release builds
        EDITOR_METHOD(void OnEditorUpdate(float deltaTime) override;)
        EDITOR_METHOD(void OnEditorGUI() override;)

        // Builds the initial structure on CPU
        void Process();
        
        // Dispatches Compute Shader to update geometry positions
        void UpdateGPU(); 

        void BindSSBOs(int nodeBindingPoint, int primBindingPoint);
        bool IsReady() const { return bvhReady; }

        void ClearTargetEntities() {
            targetEntities.clear();
        }

    private:
        ECS::Registry* m_Registry;
        Core::Engine* m_Engine;

        std::vector<Systems::BVHUtils::BVHNode> nodes;
        std::vector<Systems::BVHUtils::BVHPrimitive> primitives;
        
        // Tracking entities to rebuild instance buffer
        std::vector<ECS::Entity> targetEntities; 

        // SSBO Handles
        GLuint bvhNodesSSBO = 0;
        GLuint bvhPrimitivesSSBO = 0;
        
        // New Update Buffers
        GLuint bvhSourceSSBO = 0;
        GLuint instanceDataSSBO = 0;
        GLuint boneMatricesSSBO = 0;

        // Helper to compile compute shader directly
        GLuint computeProgramID = 0;

        bool bvhReady = false;

        void UploadToGPU();
        void BuildDebugMesh();
        void AddAABBToBuffer(const Systems::BVHUtils::AABB& box, const glm::vec3& color, std::vector<float>& buffer);
        int SplitPrimitives(int start, int count, const Systems::BVHUtils::AABB& bounds); 
        void UpdateNodeBounds(uint32_t nodeIndex);
        void Subdivide(uint32_t nodeIndex);

        GLuint debugVAO = 0;
        GLuint debugVBO = 0;
        GLuint debugProgram = 0;
        GLsizei debugVertexCount = 0;
    };

} // namespace Engine::Systems