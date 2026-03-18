#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>

// Forward declare Texture2D (assuming it's in your gl_wrapper / GLWrapper.h)
class Texture2D; 

namespace Engine::Graphics {

    struct Material {
        std::string name = "Default Material";
        
        // PBR Textures
        std::shared_ptr<Texture2D> albedoMap = nullptr;
        std::shared_ptr<Texture2D> normalMap = nullptr;
        std::shared_ptr<Texture2D> metallicRoughnessMap = nullptr; // R=Occ, G=Rough, B=Metal

        // --- ADD THESE TO REMEMBER PENDING ASYNC TEXTURES ---
        std::string albedoPath = "";
        std::string normalPath = "";
        std::string metallicRoughnessPath = "";
        // ---------------------------------------------------- 
        
        // Factors
        glm::vec4 albedoColor{1.0f};
        float metallic = 0.0f;
        float roughness = 0.5f;
        float normalStrength = 1.0f;
        
        // Method declaration only! No OpenGL code here.
        void Bind(uint32_t shaderID) const;
    };

} // namespace Engine::Graphics