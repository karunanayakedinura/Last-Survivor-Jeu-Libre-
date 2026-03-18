#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <deque>
#include <memory>
#include "Blueprints/ReflectionMacros.h"
#include "Blueprints/ReflectionRegistryMacros.h"
#include "Graphics\Texture.h"
#include "Graphics\GLWrapper.h"

// Forward declare the graphics classes! No gl_wrapper.h include needed!
namespace Engine::Graphics {
    struct Mesh;
    // class Texture2D;
}

namespace Engine::Components {
    struct NanoLOD {
        REFLECT_CLASS(NanoLOD)
        AUTHORIZE_BLUEPRINT_SCOPE(NanoLOD)

        bool IsBaked = false;
        float SwitchDistance = 1.0f; // Distance to swap to LOD
        int CaptureResolution = 512; // Resolution of the capture
        int MeshStep = 3;            // Grid simplification (higher = fewer polys)
        
        // The generated LOD mesh (stores geometry + baked texture logic implied)
        std::shared_ptr<Graphics::Mesh> LodMesh; 
        
        // NEW: Store the result of the capture here
        std::shared_ptr<Graphics::Texture2D> BakedTexture = nullptr;

    private:
        bool GetBakedState() {
            /* Returns the IsBaked state of the NanoLOD (bool) */
            return this->IsBaked;
        }

        void SetSwitchDistance(float NewDistance) {
            /* Sets the Visibility state of the Transparent (float) */
            this->SwitchDistance = NewDistance;
        }

        float GetSwitchDistance() {
            /* Fets the Visibility state of the Transparent (float) */
            return this->SwitchDistance;
        }
    };
} // namespace Engine::Components