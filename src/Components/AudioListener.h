#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Blueprints/ReflectionMacros.h"

namespace Engine::Components {
    struct AudioListener {
        REFLECT_CLASS(AudioListener)
        AUTHORIZE_BLUEPRINT_SCOPE(AudioListener)
        bool Active = true;

    private:
        void SetActive(bool state) {
            /* Sets the Active state of the AudioListener (bool) */
            this->Active = state;
        }
        
        bool IsActive() {
            /* Returns the Active state of the AudioListener (bool) */
            return this->Active;
        }
    };

} // namespace Engine::Components