#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Blueprints/ReflectionMacros.h"

namespace Engine::Components {
    struct Transparent {
        REFLECT_CLASS(Transparent)
        AUTHORIZE_BLUEPRINT_SCOPE(Transparent)
        bool Active = true;

    private:
        void SetActive(bool state) {
            /* Sets the Visibility state of the Transparent (bool) */
            this->Active = state;
        }
        
        bool IsActive() {
            /* Returns the Visibility state of the Transparent (bool) */
            return this->Active;
        }
    };

} // namespace Engine::Components