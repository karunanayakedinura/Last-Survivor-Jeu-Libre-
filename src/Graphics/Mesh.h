#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Core/Defines.h"
#include "Graphics/GLWrapper.h" // Needed for GLVertexArray, VertexBuffer, etc.

namespace Engine::Graphics {

    struct ENGINE_API Mesh {
        // CPU-side data
        std::vector<float> vertex_data; // (x, y, z, nx, ny, nz, u, v) (8)
        std::vector<uint32_t> indices;
        int vertexCount = 0;
        
        // Bone data
        std::vector<int> bone_ids;         // 4 bone IDs per vertex
        std::vector<float> bone_weights;   // 4 weights per vertex

        // GPU DATA (Wrappers)
        std::shared_ptr<Texture2D> texture; 
        int materialIndex = 0;

        glm::vec3 minBound{FLT_MAX}; 
        glm::vec3 maxBound{-FLT_MAX};
        glm::vec3 centroid{0.0f, 0.0f, 0.0f};

        // OpenGL Wrappers
        GLVertexArray vao;
        VertexBuffer vbo;
        IndexBuffer ibo;
        VertexBuffer bone_vbo;
        VertexBuffer weight_vbo;

        bool Processed = false;

        // Constructors / Destructors
        Mesh() = default;
        explicit Mesh(std::vector<float> data);
        Mesh(const Mesh& other);
        Mesh& operator=(const Mesh& other);
        Mesh(Mesh&&) = default;
        Mesh& operator=(Mesh&&) = default;
        ~Mesh();

        // Methods
        void AddBoneData(unsigned int vertexID, int boneID, float weight);
        void upload();
        void draw() const;
        void ComputeBoundsAndCentroid();
        void Destroy();
    };

} // namespace Engine::Graphics