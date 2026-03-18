#pragma once
#include <glm/glm.hpp>
#include "Blueprints/ReflectionMacros.h"

namespace Engine::Components {
    struct DebugBox {
        REFLECT_CLASS(DebugBox)
        AUTHORIZE_BLUEPRINT_SCOPE(DebugBox)
        glm::vec3 Extents = glm::vec3(1.0f);           // Size of the box
        glm::vec3 Color = glm::vec3(1.0f, 0.0f, 1.0f); // Default Magenta
        bool Wireframe = true;                         // Draw as lines or solid
    };
}