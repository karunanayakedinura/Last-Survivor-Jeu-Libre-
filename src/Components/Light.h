#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <deque>
#include <memory>
#include "Blueprints/ReflectionMacros.h"
#include "Blueprints/ReflectionRegistryMacros.h"

namespace Engine::Components {
    enum class LightType { 
        DIRECTIONAL, POINT, SPOT 
    };

    struct Light {
        REFLECT_CLASS(Light)
        AUTHORIZE_BLUEPRINT_SCOPE(Light)
        LightType Type = LightType::POINT;
        // glm::vec3 position {0.0f, 0.0f, 0.0f};
        glm::vec3 Direction {0.0f, -1.0f, 0.0f};
        glm::vec3 Color {1.0f};
        float Intensity = 1.0f;
        float Range = 5.0f;
        float SpotAngle = 45.0f;
        bool CastShadows = true;
        
        float ShadowNear = 0.1f;
        float ShadowFar = 30.0f;
        float OrthoSize = 15.0f;

        /* Gets the light type of the light (LightType) */
        LightType GetLightType() const {
            return Type;
        }

        /* Gets the direction of the light (glm::vec3) */
        const glm::vec3& GetDirection() const {
            return Direction;
        }

        /* Gets the color of the light (glm::vec3) */
        const glm::vec3& GetColor() const {
            return Color;
        }

        /* Gets the intensity of the light (float) */
        float GetIntensity() const {
            return Intensity;
        }

        /* Gets the range of the light (float) */
        float GetRange() const {
            return Range;
        }

        /* Gets the spot angle of the light (float) */
        float GetSpotAngle() const {
            return SpotAngle;
        }

        /* Gets whether the light casts shadows (bool) */
        bool GetCastShadows() const {
            return CastShadows;
        }

        /* Gets the near plane of the light's shadow frustum (float) */
        float GetShadowNear() const {
            return ShadowNear;
        }

        /* Gets the far plane of the light's shadow frustum (float) */
        float GetShadowFar() const {
            return ShadowFar;
        }

        /* Gets the orthographic size for directional light shadow map (float) */
        float GetOrthoSize() const {
            return OrthoSize;
        }

    private:
        /* Sets the light type of the light (LightType) */
        void SetLightType(LightType val) {
            this->Type = val;
        }
        
        /* Sets the Direction of the light (glm::vec3) */
        void SetDirection(glm::vec3 dir) {
            this->Direction = dir;
        }

        /* Sets the color of the light (glm::vec3) */
        void SetColor(glm::vec3 col) {
            this->Color = col;
        }

        /* Sets the intensity of the light (float) */
        void SetIntensity(float val) {
            this->Intensity = val;
        }

        /* Sets the range of the light (float) */
        void SetRange(float val) {
            this->Range = val;
        }

        /* Sets the spot angle of the light (float) */
        void SetSpotAngle(float val) {
            this->SpotAngle = val;
        }

        /* Sets whether the light casts shadows (bool) */
        void SetCastShadows(bool val) {
            this->CastShadows = val;
        }

        /* Sets the near plane of the light's shadow frustum (float) */
        void SetShadowNear(float val) {
            this->ShadowNear = val;
        }

        /* Sets the far plane of the light's shadow frustum (float) */
        void SetShadowFar(float val) {
            this->ShadowFar = val;
        }

        /* Sets the orthographic size for directional light shadow map (float) */
        void SetOrthoSize(float val) {
            this->OrthoSize = val;
        }
    };
} // namespace Engine::Components