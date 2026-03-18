#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Blueprints/ReflectionMacros.h"

namespace Engine::Systems { class RenderSystem; class BVHSystem;}

namespace Engine::Components {
    struct Transform {
        REFLECT_CLASS(Transform)
        AUTHORIZE_BLUEPRINT_SCOPE(Transform)

        glm::vec3 Position {0.0f};
        glm::vec3 Rotation {0.0f};
        glm::vec3 Scale {1.0f};

        glm::vec3 Forward {0.0f, 0.0f, -1.0f};
        glm::vec3 Up {0.0f, 1.0f,  0.0f};


        void ResetScale() { Scale = glm::vec3(1.0f); }

        // Compute the right vector
        glm::vec3 Right() const {
            return glm::normalize(glm::cross(Forward, Up));
        }

        // Compute the view matrix (for camera usage)
        glm::mat4 GetViewMatrix() const {
            return glm::lookAt(Position, Position + Forward, Up);
        }

        // Compute the model matrix (for rendering entities)
        glm::mat4 GetModelMatrix() const {
            glm::mat4 model(1.0f);

            // Translate the object in its local space
            model = glm::translate(model, Position);

            // Rotate around the object's own pivot
            glm::mat4 rotationMatrix = glm::mat4(1.0f);
            rotationMatrix = glm::rotate(rotationMatrix, glm::radians(Rotation.y), glm::vec3(0, 1, 0));
            rotationMatrix = glm::rotate(rotationMatrix, glm::radians(Rotation.x), glm::vec3(1, 0, 0));
            rotationMatrix = glm::rotate(rotationMatrix, glm::radians(Rotation.z), glm::vec3(0, 0, 1));
            model = model * rotationMatrix;

            // Scale the object in its local space
            model = glm::scale(model, Scale);
            return model;
        }

    private:
        friend class Engine::Systems::RenderSystem; // Allow RenderSystem to access private members for optimization
        friend class Engine::Systems::BVHSystem; // Allow RenderSystem to access private members for optimization
        
    };

} // namespace Engine::Components