#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <deque>
#include <memory>
#include "Blueprints/ReflectionMacros.h"

// Forward declare the graphics classes! No gl_wrapper.h include needed!
namespace Engine::Graphics {
    struct Mesh;
    struct Material;
}

namespace Engine::Components {
    struct MeshRenderer {
        REFLECT_CLASS(MeshRenderer)
        AUTHORIZE_BLUEPRINT_SCOPE(MeshRenderer)

        
        // Use shared pointers to reference the graphics assets
        std::vector<std::shared_ptr<Graphics::Mesh>> meshes;
        std::vector<std::shared_ptr<Graphics::Material>> materials;

        bool active = false;
        std::string filePath;   // Model path ("Assets/cube.obj")
        glm::vec3 minBound {FLT_MAX};
        glm::vec3 maxBound {-FLT_MAX};
        glm::vec3 centroid {0.0f};

    private:
        void SetActive(bool state) {
            /* Sets the Visibility state of the 3D model (bool) */
            this->active = state;
        }
        
        bool IsActive() {
            /* Returns the Visibility state of the 3D model (bool) */
            return this->active;
        }
        
        std::string GetModelPath() {
            /* Returns the Path of the 3D model (std::string) */
            return this->filePath;
        }
        
        std::vector<glm::vec3> GetModelbounds() {
            /* Returns the bounds of the 3D model: [Min, Max] (glm::vec3) */
            return {minBound, maxBound};
        }
        
        glm::vec3 GetCentroid() {
            /* Returns the centroid of the 3D model (glm::vec3) */
            return centroid;
        }
        
        int GetModelCount() {
            /* Returns the number of models of the 3D model (int) */
            return static_cast<int>(meshes.size());
        }
        
        int GetMaterialCount() {
            /* Returns the number of materials of the 3D model (int) */
            return static_cast<int>(materials.size());
        }
    };

} // namespace Engine::Components