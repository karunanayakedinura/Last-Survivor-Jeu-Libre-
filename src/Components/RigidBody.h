#pragma once

#include <string>
#include <map>
#include <variant>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <btBulletDynamicsCommon.h>
#include "Blueprints/ReflectionMacros.h"

namespace Engine::Components {

    enum class CollisionShapeType {
        BOX,
        SPHERE,
        CAPSULE,
        MESH_CONVEX
    };

    struct RigidBody {
        // --------------------------------------------------------------------
        // Reflection / Blueprint macros (unchanged)
        // --------------------------------------------------------------------
        REFLECT_CLASS(RigidBody)
        AUTHORIZE_BLUEPRINT_SCOPE(RigidBody)

        // --------------------------------------------------------------------
        // Public data members – exposed to the editor / Blueprint system
        // --------------------------------------------------------------------
        bool dirty = true;                     // Flag to recreate the rigid body when needed
        btRigidBody* body = nullptr;           // Owned by this struct (deleted in destructor)
        btCollisionShape* shape = nullptr;     // Owned by this struct (deleted in destructor)
        float mass = 1.0f;
        CollisionShapeType shapeType = CollisionShapeType::BOX;
        bool isStatic = true;

        // Collider‑specific settings
        glm::vec3 boxSize = glm::vec3(1.0f);
        float radius = 0.5f;
        float height = 1.0f;

        // Collider offset
        glm::vec3 offset = glm::vec3(0.0f);

        // Linear locks (position)
        bool lockPosX = false;
        bool lockPosY = false;
        bool lockPosZ = false;

        // Angular locks (rotation)
        bool lockRotX = false;
        bool lockRotY = false;
        bool lockRotZ = false;

        // --------------------------------------------------------------------
        // Special member functions – defaulted for simple copy/move semantics
        // --------------------------------------------------------------------
        RigidBody() = default;
        RigidBody(const RigidBody&) = default;
        RigidBody(RigidBody&&) noexcept = default;
        RigidBody& operator=(const RigidBody&) = default;
        RigidBody& operator=(RigidBody&&) noexcept = default;
        ~RigidBody() {
            // Clean up Bullet resources if we own them.
            delete body;
            delete shape;
        }

    private:
        // --------------------------------------------------------------------
        // Getters (const‑correct)
        // --------------------------------------------------------------------
        bool GetDirty() const { return dirty; }
        btRigidBody* GetBody() const { return body; }
        btCollisionShape* GetShape() const { return shape; }
        float GetMass() const { return mass; }
        CollisionShapeType GetShapeType() const { return shapeType; }
        bool GetIsStatic() const { return isStatic; }
        const glm::vec3& GetBoxSize() const { return boxSize; }
        float GetRadius() const { return radius; }
        float GetHeight() const { return height; }
        const glm::vec3& GetOffset() const { return offset; }
        bool GetLockPosX() const { return lockPosX; }
        bool GetLockPosY() const { return lockPosY; }
        bool GetLockPosZ() const { return lockPosZ; }
        bool GetLockRotX() const { return lockRotX; }
        bool GetLockRotY() const { return lockRotY; }
        bool GetLockRotZ() const { return lockRotZ; }

        // --------------------------------------------------------------------
        // Setters
        // --------------------------------------------------------------------
        void SetDirty(bool v) { dirty = v; }
        void SetBody(btRigidBody* v) { body = v; }
        void SetShape(btCollisionShape* v) { shape = v; }
        void SetMass(float v) { mass = v; }
        void SetShapeType(CollisionShapeType v) { shapeType = v; }
        void SetIsStatic(bool v) { isStatic = v; }
        void SetBoxSize(const glm::vec3& v) { boxSize = v; }
        void SetRadius(float v) { radius = v; }
        void SetHeight(float v) { height = v; }
        void SetOffset(const glm::vec3& v) { offset = v; }
        void SetLockPosX(bool v) { lockPosX = v; }
        void SetLockPosY(bool v) { lockPosY = v; }
        void SetLockPosZ(bool v) { lockPosZ = v; }
        void SetLockRotX(bool v) { lockRotX = v; }
        void SetLockRotY(bool v) { lockRotY = v; }
        void SetLockRotZ(bool v) { lockRotZ = v; }

        // --------------------------------------------------------------------
        // Optional helper – rebuild the Bullet body when `dirty` is true
        // --------------------------------------------------------------------
        void RebuildIfNeeded() {
            if (!dirty) return;

            // Clean up any existing Bullet objects
            delete body;
            delete shape;
            body = nullptr;
            shape = nullptr;

            // Create the appropriate collision shape
            switch (shapeType) {
                case CollisionShapeType::BOX:
                    shape = new btBoxShape(btVector3(boxSize.x, boxSize.y, boxSize.z));
                    break;
                case CollisionShapeType::SPHERE:
                    shape = new btSphereShape(radius);
                    break;
                case CollisionShapeType::CAPSULE:
                    shape = new btCapsuleShape(radius, height);
                    break;
                case CollisionShapeType::MESH_CONVEX:
                    // Real mesh loading would be inserted here
                    shape = new btConvexHullShape();
                    break;
            }

            // Compute local inertia (only for dynamic bodies)
            btVector3 localInertia(0, 0, 0);
            if (!isStatic && shape) {
                shape->calculateLocalInertia(mass, localInertia);
            }

            // Motion state – initialise with the offset transform
            btDefaultMotionState* motionState = new btDefaultMotionState(
                btTransform(btQuaternion(0,0,0,1),
                            btVector3(offset.x, offset.y, offset.z))
            );

            // Construction info and actual rigid body
            btRigidBody::btRigidBodyConstructionInfo ci(mass, motionState, shape, localInertia);
            body = new btRigidBody(ci);

            // Apply linear/angular lock flags
            body->setLinearFactor(btVector3(
                lockPosX ? 0.f : 1.f,
                lockPosY ? 0.f : 1.f,
                lockPosZ ? 0.f : 1.f
            ));
            body->setAngularFactor(btVector3(
                lockRotX ? 0.f : 1.f,
                lockRotY ? 0.f : 1.f,
                lockRotZ ? 0.f : 1.f
            ));

            dirty = false;
        }
    }; // struct RigidBody

} // namespace Engine::Components