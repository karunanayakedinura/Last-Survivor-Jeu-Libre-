#pragma once
#include "Core/Engine.h"
#include "Core/ISystem.h"
#include "Core/Defines.h"
#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>

namespace Engine::Systems::PhysicsUtils {
    struct RaycastHit {
        bool hasHit = false;
        glm::vec3 hitPoint = glm::vec3(0.0f);
        glm::vec3 hitNormal = glm::vec3(0.0f);
        ECS::Entity hitEntity = ECS::NULL_ENTITY;
    };

    class PhysicsDebugDrawer : public btIDebugDraw {
    public:
        PhysicsDebugDrawer();

        void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

        void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
        void reportErrorWarning(const char* warningString);
        void draw3dText(const btVector3& location, const char* textString);
        
        void setDebugMode(int debugMode);
        int getDebugMode() const;

        void Flush(const glm::mat4& view, const glm::mat4& proj);

    private:
        std::unique_ptr<ShaderProgram> m_Shader;
        GLVertexArray m_VAO;
        VertexBuffer m_VBO;
        std::vector<float> m_LineData; 
        int m_DebugMode = 0;
    };

} // Engine::Core::Systems::PhysicsUtils