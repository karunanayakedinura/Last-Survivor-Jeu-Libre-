#pragma once
#include <iostream>
#include <random>
#include "Core/Engine.h"
#include "Core/ISystem.h"
#include "Core/Defines.h"
#include "Core/EngineConfig.h"
#include "Graphics/RenderUtils.h"
#include "Graphics/Material.h"
#include "Graphics\Shaders.h"


// Forward declaration prevents needing to include ECS headers here
namespace Engine::ECS { class Registry; using Entity = uint32_t;}
namespace Engine::Components { enum class LightType;}


namespace Engine::Systems {
    class ENGINE_API RenderSystem : public Core::ISystem {
    public:
        RenderSystem(ECS::Registry* registry, Core::Engine* engine);
        ~RenderSystem() override = default;

        void OnInit() override;
        void OnShutdown() override;
        void OnFixedUpdate(float deltaTime) override;
        void OnPlayUpdate(float deltaTime) override;

        EDITOR_METHOD(void OnEditorGUI() override;)

        // Add these function declarations inside the RenderSystem class
        void InitTEGI();
        void TEGI_Pass();

        RenderUtils::DebugViewMode currentDebugMode = RenderUtils::DebugViewMode::LIT;
        void SetDebugViewMode(RenderUtils::DebugViewMode mode) { currentDebugMode = mode; }

        // void OnStop();

        void Initialize();

        // void Update(float deltaTime);

        void Render();

        void RenderNanoLODDebug();

        void Shutdown();

        void AddShader(const std::string& name, const char* vertSrc, const char* fragSrc);
        // --- OVERLOAD FOR GEOMETRY SHADERS ---
        void AddShader(const std::string& name, const char* vertSrc, const char* geomSrc, const char* fragSrc);

        GLuint GetShader(const std::string& name);

        bool HasShader(const std::string& name);

        void RenderDebugBoxes();
        
        // --- Helpers for other Systems ---
        void DrawFullScreenQuad(); // Expose this for PostProcessingSystem
        GLuint GetQuadVAO() const { return quadVAO; }


        GLuint GetLitSceneTexture();
        GLuint GetDepthTexture();
        GLuint GetPositionTexture();
        GLuint GetNormalTexture();
        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix() const;
        glm::mat4 GetPreviousViewProjectionMatrix() const;

        // void ClearMainCamera() { mainCamera = ECS::NULL_ENTITY; }
        void SetMainCamera(ECS::Entity camera) { mainCamera = camera; }

        int GetEntityIDAt(int x, int y);

        void OnResize(int width, int height);

        void CleanupShadowCasters();

        void RenderMeshPreview(Mesh* mesh, RenderUtils::PreviewBuffer& buffer, glm::vec3 minBound, glm::vec3 maxBound);

        void RenderMaterialPreview(Graphics::Material* material, RenderUtils::PreviewBuffer& buffer);

        // --- NanoLOD Integration ---
        RenderUtils::SceneSnapshot CaptureScene(); // Default capture (fullscreen)
        RenderUtils::SceneSnapshot CaptureSceneIsolated(int width, int height); // Capture from specific FBO
        void SaveSceneToPLY(const std::string& filename, const RenderUtils::SceneSnapshot& snapshot);
        
        // Triggers the 360 capture and mesh generation for a NanoLOD component
        // void BakeNanoLOD(Entity* entity, NanoLOD* lodComponent);

        // Debug Visualization
        // void RenderMotionMatchingDebug();

        GLuint debugVAO = 0;
        GLuint debugVBO = 0;

        int GetGBufferWidth() const { return gBufferWidth; }
        int GetGBufferHeight() const { return gBufferHeight; }
        // void SetGridSpacing(float spacing) { m_gridSpacing = spacing; }

        // --- NEW: TAA & Jitter Helpers ---
        glm::vec2 GetJitter() const { return currentJitter; }
        glm::mat4 GetCleanProjectionMatrix() const { return cleanProjectionMatrix; }
        
        // --- NEW: Toggle Flags ---
        bool enableSSGI = false;
        bool enableVolumetricFog = true;
        bool enableTAA = false; // Handled by PostProcessing, but flag stored here for logic

        GLuint GetLitSceneFBO() const { return litSceneFBO; } // <--- ADD THIS

        void RenderProbeDebug();

    private:
        ECS::Registry* m_Registry;
        Core::Engine* m_Engine;
        
        RenderUtils::TEGIVolume tegi;

        // --- NEW: TAA State ---
        int frameIndex = 0;
        glm::vec2 currentJitter = glm::vec2(0.0f);
        glm::mat4 cleanProjectionMatrix; // Unjittered (for culling/UI)

        // --- NEW: SSGI Resources ---
        GLuint ssgiFBO = 0;
        GLuint ssgiRawTexture = 0; // The raw noisy result
        GLuint ssgiBlurFBO = 0;
        GLuint ssgiBlurTexture = 0; // Filtered result
        
        // --- NEW: Volumetric Fog Resources ---
        GLuint volFogFBO = 0;
        GLuint volFogTexture = 0;

        // --- Selection Mask Buffers ---
        GLuint selectionMaskFBO = 0;
        GLuint selectionMaskTex = 0;

        // --- NEW: Passes ---
        void SSGI_Pass();
        void SSGI_Denoise_Pass();
        void VolumetricFog_Pass();

        void UpdateJitter();


        // Helper to register built-in shaders (moved from PP system)
        void LoadPostProcessShaders();
        
        GLuint cubeVAO = 0;
        GLuint cubeVBO;

        void ShadowPass();
        void GeometryPass();
        void SSAO_Pass();
        void SSAO_Blur_Pass();
        void LightingPass();
        void TransparencyPass(); 

        // --- Helper Functions ---
        GLuint CompileShader(GLenum type, const char* src);
        GLuint CreateShaderProgram(const char* vertSrc, const char* fragSrc);

        // --- NEW OVERLOAD FOR GEOMETRY SHADERS ---
        GLuint CreateShaderProgram(const char* vertSrc, const char* geomSrc, const char* fragSrc);
        
        void GenerateSSAOKernel();
        void GenerateSSAONoiseTexture();
        
        // --- Helper functions for shadow setup (NEW) ---
        void SetupShadowFBO_2D(GLuint& fbo, GLuint& tex);
        void SetupShadowFBO_Cube(GLuint& fbo, GLuint& cubemap);
        void SetupShadowCasters();
        
        GLuint bakeFBO = 0;
        GLuint bakePosTex = 0;
        GLuint bakeNormTex = 0;
        GLuint bakeAlbedoTex = 0;
        GLuint bakeDepthTex = 0;
        void InitBakeResources(int width, int height);

        // --- Core Engine Data ---
        ECS::Entity mainCamera = ECS::NULL_ENTITY;
        std::map<std::string, GLuint> shaders;

        // --- Matrices ---
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
        glm::mat4 previousViewProjectionMatrix; // For Motion Blur

        // --- MODIFIED Shadow Mapping ---
        struct ShadowCaster {
            GLuint shadowFBO;
            GLuint shadowMap;           // Texture ID
            GLenum shadowMapType;       // GL_TEXTURE_2D or GL_TEXTURE_CUBE_MAP
            glm::mat4 lightSpaceMatrix; // For Dir/Spot
            float shadowFarPlane;       // For Point
            ECS::Entity lightEntity;
            Components::LightType lightType; // Store light type
            int shadowIndex; // Index in the sampler array
        };
        
        std::vector<ShadowCaster> shadowCasters;
        
        // --- NEW: Separate resolutions for 2D and Cube maps
        const int SHADOW_WIDTH_2D = Core::Config::EngineConfig::Instance().GetRenderSettings().SHADOW_WIDTH_2D;
        const int SHADOW_HEIGHT_2D = Core::Config::EngineConfig::Instance().GetRenderSettings().SHADOW_HEIGHT_2D;
        const int SHADOW_WIDTH_CUBE = Core::Config::EngineConfig::Instance().GetRenderSettings().SHADOW_WIDTH_CUBE;
        const int SHADOW_HEIGHT_CUBE = Core::Config::EngineConfig::Instance().GetRenderSettings().SHADOW_HEIGHT_CUBE;
        const int MAX_SHADOW_CASTERS = Core::Config::EngineConfig::Instance().GetRenderSettings().MAX_SHADOW_CASTERS;
        const int MAX_LIGHTS = Core::Config::EngineConfig::Instance().GetRenderSettings().MAX_LIGHTS;

        // --- ID Picking Buffers ---
        GLuint pickingFBO = 0;
        GLuint pickingTexture = 0;
        GLuint pickingDepthTexture = 0; // We can reuse gDepthTexture or make a new one

        GLuint pickingDepthRBO = 0; // Dedicated depth buffer for picking
        
        // --- G-Buffer (Deferred Shading) ---
        GLuint gBufferFBO;
        GLuint gPositionMetallic;
        GLuint gNormalRoughness;
        GLuint gAlbedo;
        GLuint gDepthTexture;
        int gBufferWidth = Core::Config::EngineConfig::Instance().GetScreenWidth();
        int gBufferHeight = Core::Config::EngineConfig::Instance().GetScreenHeight();

        // --- SSAO Buffers ---
        GLuint ssaoFBO;
        GLuint ssaoColorBuffer;
        GLuint ssaoBlurFBO;
        GLuint ssaoBlurColorBuffer;
        std::vector<glm::vec3> ssaoKernel;
        GLuint ssaoNoiseTexture;

        // --- Lit Scene Buffer (Output of RenderSystem) ---
        GLuint litSceneFBO;
        GLuint litSceneColorBuffer;
        // --------------------------------------------------

        GLuint quadVAO = 0;
        GLuint quadVBO;

        // 1. ADD THIS STRUCT INSIDE THE RenderSystem CLASS (e.g., under private or public section)
        struct DDGIVolume {
            // glm::ivec3 probeCounts = glm::ivec3(16, 4, 16); 
            // glm::vec3 startPosition = glm::vec3(-12.0f, 0.0f, -12.0f);
            // glm::vec3 probeStep = glm::vec3(1.5f, 1.0f, 1.5f);

            glm::ivec3 probeCounts = glm::ivec3(22, 4, 22);     
            glm::vec3 startPosition = glm::vec3(-2.2f, 0.0f, -2.2f); // Center on 0
            glm::vec3 probeStep = glm::vec3(0.2f, 0.2f, 0.2f);
            
            // Resources
            GLuint irradianceTex = 0;   // GL_RGB10_A2 or GL_RGBA16F
            GLuint distanceTex = 0;     // GL_RG16F
            GLuint rayDataTex = 0;      // GL_RGBA16F (Radiance + Distance)
            
            // Shaders
            GLuint traceShader = 0;
            GLuint updateIrradianceShader = 0;
            GLuint updateDistanceShader = 0;

            bool enabled = true;
            bool debugProbes = false;

            // --- NEW: Temporal Accumulator ---
            int accumulatorIndex = 0;
        } ddgi;

        // 2. ADD THESE FUNCTION DECLARATIONS TO RenderSystem CLASS
        void InitDDGI();
        void UpdateDDGI();

        // --- NEW: Helper Declarations ---
        void BindShadowMapsToShader(GLuint shader);
        void UploadLightsToShader(GLuint shader);
        // EDITOR_METHOD(void DrawDebugColliders();)
    };

} // namespace Engine::Systems