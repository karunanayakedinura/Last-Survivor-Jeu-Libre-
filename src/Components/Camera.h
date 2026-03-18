#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Blueprints/ReflectionMacros.h"

namespace Engine::Components {
    struct Camera {
        REFLECT_CLASS(Camera)
        AUTHORIZE_BLUEPRINT_SCOPE(Camera)

        float Fov = 80.0f;
        float NearPlane = 0.01f;
        float FarPlane = 1000.0f;

    private:
        void SetFov(float NewFov) {
            /* Sets the Fov state of the Camera (float) */
            this->Fov = NewFov;
        }
        
        float GetFov() {
            /* Gets the Fov state of the Camera (float) */
            return this->Fov;
        }
        
        void SetNearPlane(float NewNearPlane) {
            /* Sets the NearPlane state of the Camera (float) */
            this->NearPlane = NewNearPlane;
        }
        
        float GetNearPlane() {
            /* Gets the NearPlane state of the Camera (float) */
            return this->NearPlane;
        }
        
        void SetFarPlane(float NewFarPlane) {
            /* Sets the FarPlane state of the Camera (float) */
            this->FarPlane = NewFarPlane;
        }
        
        float GetFarPlane() {
            /* Gets the FarPlane state of the Camera (float) */
            return this->FarPlane;
        }

    };

} // namespace Engine::Components