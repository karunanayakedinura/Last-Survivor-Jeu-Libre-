#pragma once
#include <map>
#include <array>
#include <variant>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Blueprints/ReflectionMacros.h"

namespace Engine::Components {
    using AnyUniforms = std::variant<
        int, float, 
        std::array<int, 2>, std::array<int, 3>, std::array<int, 4>,
        std::array<float, 2>, std::array<float, 3>, std::array<float, 4>
    >;
    
    struct MaterialOverride {
        REFLECT_CLASS(MaterialOverride)
        AUTHORIZE_BLUEPRINT_SCOPE(MaterialOverride)
        
        std::string shaderName;
        
        std::map<std::string, AnyUniforms> Uniforms;

    private:
        std::string GetShaderName() {
            /* Returns the ShaderName of the MaterialOverride (std::string) */
            return this->shaderName;
        }

        void SetShaderName(std::string NewName) {
            /* Sets the ShaderName of the MaterialOverride (std::string) */
            this->shaderName = NewName;
        }
        
        void SetUniform(std::string Name, AnyUniforms Uniform) {
            /* Sets the Uniform state of the WireframeRender by Name (std::string, AnyUniforms) */
            Uniforms[Name] = Uniform;
        }
    };

} // namespace Engine::Components