#pragma once
#include <map>
#include <vector>
#include <glm/glm.hpp>
#include "Graphics/GLWrapper.h"

namespace Engine::Systems::RenderUtils {
    // Helper for Voxel Grid hashing
    struct VoxelKey {
        int x, y, z;
        bool operator==(const VoxelKey& o) const { return x == o.x && y == o.y && z == o.z; }
    };

    struct VoxelHash {
        size_t operator()(const VoxelKey& k) const {
            return ((std::hash<int>()(k.x) ^ (std::hash<int>()(k.y) << 1)) >> 1) ^ (std::hash<int>()(k.z) << 1);
        }
    };

    struct VoxelData {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
        int count; // For averaging
        uint32_t index; // Store the final mesh index here
    };

    struct SceneSnapshot {
        int width;
        int height;
        std::vector<float> positions;      // RGBA floats (RGB = World Pos, A = Metallic)
        std::vector<float> normals;        // RGBA floats (RGB = World Normal, A = Roughness)
        std::vector<unsigned char> albedo; // RGBA bytes
        std::vector<float> depth;          // Single float depth
    };

    enum class DebugViewMode {
        LIT = 0,                // Final Result
        BASE_COLOR = 1,         // Albedo Texture
        NORMAL = 2,             // World Space Normals
        ROUGHNESS = 3,          // Grey scale roughness
        METALLIC = 4,           // Grey scale metallic
        AMBIENT_OCCLUSION = 5,  // SSAO result
        LIGHTING_ONLY = 6,      // Lighting calculation with grey Albedo (0.5)
        DETAIL_LIGHTING = 7,    // Same as Lighting Only (in Deferred, normals are already baked)
        REFLECTIONS = 8,        // SSR / Indirect only
        UNLIT = 9,              // Just Albedo (no lighting)
        DDGI = 10,
        VOLUMETRIC_FOG = 11,
        TEGI = 12
    };
    
    struct TEGIVolume {
        bool enabled = true;
        GLuint currentTex = 0;
        GLuint historyTex = 0;
        GLuint pingPongTex = 0;
        GLuint fbo = 0;
        GLuint historyFbo = 0;

        // --- NEW: Spatial Light Spread ---
        GLuint spatialTex = 0;
        GLuint spatialFbo = 0;
    };

    struct PreviewBuffer {
        GLuint fbo = 0;
        GLuint texture = 0;
        GLuint rbo = 0; // Renderbuffer for depth testing
        int width = 256; // Resolution of the preview
        int height = 256;

        void Init(int w, int h) {
            width = w;
            height = h;

            // 1. Generate Framebuffer
            glGenFramebuffers(1, &fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

            // 2. Create Texture to hold color data
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

            // 3. Create Renderbuffer for Depth/Stencil
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

            // 4. Check status
            // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                // terminal::Instance().error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

            glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind
        }

        void Cleanup() {
            glDeleteFramebuffers(1, &fbo);
            glDeleteTextures(1, &texture);
            glDeleteRenderbuffers(1, &rbo);
        }
    };

    inline GLuint CreateComputeShaderProgram(const char* computeSrc) {
        GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(shader, 1, &computeSrc, nullptr);
        glCompileShader(shader);
        
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[1024];
            glGetShaderInfoLog(shader, 1024, nullptr, log);
            std::cerr << "Compute Shader Compile Error:\n" << log << std::endl;
            return 0;
        }

        GLuint program = glCreateProgram();
        glAttachShader(program, shader);
        glLinkProgram(program);
        
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            char log[1024];
            glGetProgramInfoLog(program, 1024, nullptr, log);
            std::cerr << "Compute Shader Link Error:\n" << log << std::endl;
        }
        
        glDeleteShader(shader);
        return program;
    }

    inline float lerp(float a, float b, float f) {
        return a + f * (b - a);
    }

    inline float Halton(int index, int base) {
        float f = 1;
        float r = 0;
        while (index > 0) {
            f = f / (float)base;
            r = r + f * (index % base);
            index = index / base;
        }
        return r;
    }

    
} // namespace Engine::Systems::RenderUtils