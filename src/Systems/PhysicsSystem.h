#pragma once
#include <iostream>
#include <memory>
#include "Core/Engine.h"
#include "Core/ISystem.h"
#include "Core/Defines.h"
#include "Physics/PhysicsUtils.h"
#include <btBulletDynamicsCommon.h> 

// Forward declaration prevents needing to include ECS headers here
namespace Engine::ECS { class Registry; }

namespace Engine::Systems {

    class PhysicsSystem : public Core::ISystem {
    public:
        PhysicsSystem(ECS::Registry* registry, Core::Engine* engine);
        ~PhysicsSystem() override = default;

        void Render();
        void OnStop();
        void RebuildPhysicsWorld();
        void RemoveBody(ECS::Entity entity);

        void AddBody(ECS::Entity entity);
        Systems::PhysicsUtils::RaycastHit Raycast(const glm::vec3& start, const glm::vec3& end, ECS::Entity ignoreEntity = ECS::NULL_ENTITY);


        void OnInit() override;
        void OnShutdown() override;
        void OnFixedUpdate(float deltaTime) override;
        void OnPlayUpdate(float deltaTime) override;

        EDITOR_METHOD(void OnEditorGUI() override;)

        bool showDebugShapes = true;
    private:
        ECS::Registry* m_Registry;
        Core::Engine* m_Engine;
        
        EDITOR_METHOD(void DrawDebugColliders();)

        btBroadphaseInterface* overlappingPairCache;
        btCollisionDispatcher* dispatcher;
        btConstraintSolver* solver;
        btDefaultCollisionConfiguration* collisionConfiguration;
        btDynamicsWorld* dynamicsWorld;
        std::unique_ptr<Systems::PhysicsUtils::PhysicsDebugDrawer> m_DebugDrawer;  
    };

} // namespace Engine::Systems