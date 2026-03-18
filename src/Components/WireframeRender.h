#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Blueprints/ReflectionMacros.h"

namespace Engine::Components {
    struct WireframeRender {
        REFLECT_CLASS(WireframeRender)
        AUTHORIZE_BLUEPRINT_SCOPE(WireframeRender)
        bool Active = true;

    private:
        void Toggle() {
            Active = !Active;
        }
        void SetActive(bool state) {
            /* Sets the Visibility state of the WireframeRender (bool) */
            this->Active = state;
        }
        
        bool IsActive() {
            /* Returns the Visibility state of the WireframeRender (bool) */
            return this->Active;
        }
    };

} // namespace Engine::Components