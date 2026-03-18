#pragma once
#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <filesystem>
#include <chrono>

#include "Core/Engine.h"
#include "Core/ISystem.h"
#include "Core/Defines.h"

// Third-party UI integrations
#include "TextEditor.h"
#include "ImGuizmo.h"

namespace Engine::ECS { class Registry; }

namespace Engine::Systems {

    struct ProfileResult {
        std::string Name;
        float Time;
    };

    class ImGuiSystem : public Core::ISystem {
    public:
        ImGuiSystem(ECS::Registry* registry, Core::Engine* engine);
        ~ImGuiSystem() override = default;

        void OnInit() override;
        void OnShutdown() override;
        void OnFixedUpdate(float deltaTime) override;
        void OnPlayUpdate(float deltaTime) override;

        void DrawReleaseUI();

        EDITOR_METHOD(void OnEditorUpdate(float deltaTime) override;)
        EDITOR_METHOD(void OnEditorGUI() override;)

        // --- Profiling API ---
        static void PushProfileResult(const ProfileResult& result);

        std::optional<ECS::Entity> m_SelectedEntity;

    private:
        ECS::Registry* m_Registry;
        Core::Engine* m_Engine;

        // --- Core UI Methods ---
        void SetupImGuiStyle();
        void DrawDockSpace();
        void DrawToolbar();
        void DrawHierarchy();
        void DrawInspector();
        void DrawComponents(ECS::Entity entity);
        void DrawAddComponentPopup(ECS::Entity entity);

        template<typename T, typename UIFunction>
        void DrawComponent(const std::string& name, ECS::Entity entity, UIFunction uiFunction);
        
        template<typename T>
        void DrawAddComponentMenu(const std::string& name, ECS::Entity entity);

        template<typename T>
        void DrawReflectedComponent(const std::string& name, ECS::Entity entity);

        // --- Feature Panels ---
        void DrawImporterUI();
        void DrawGameView();
        void DrawAssetBrowser();
        void DrawConsole();
        void DrawTextEditor();
        void DrawProfiler();
        void DrawLoadingPopup();
        void DrawPackingModal();
        void DrawCompilingModal();
        void DrawMaterialEditor();

        // --- Helper Methods ---
        GLuint CopyThumbnailFromFramebuffer(int w, int h);
        void CalculateMeshBounds(const std::vector<float>& vertices, glm::vec3& minB, glm::vec3& maxB);
        GLuint GetOrCreateThumbnail(const std::string& fullPath, const std::string& ext);
        std::string FormatTime(float seconds);

        // ==========================================
        // STATE VARIABLES
        // ==========================================
        
        // Window Toggles
        bool m_showHierarchy = true;
        bool m_showInspector = true;
        bool m_showAssetBrowser = true;
        bool m_showConsole = true;
        bool m_showProfiler = true;
        bool m_showTextEditor = true;
        bool m_showNodeEditor = false;
        bool m_showNodePalette = false;
        bool m_showNodeProperties = false;
        bool m_openSavePopup = false;
        bool m_viewDebugLogs = false;
        bool m_nodeEditorActive = false;
        bool m_DrawGrid = true;
        bool m_showMaterialEditor = false;

        // Importer
        bool m_ShowImportModal = false;
        char m_SourcePath[256] = "Assets/Mimic/Mimic.fbx";
        char m_DestPath[256] = "Assets/Mimic/model.pa";
        char m_sceneNameBuffer[128] = "NewScene";

        // Gizmo
        ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGuizmo::MODE mCurrentGizmoMode = ImGuizmo::WORLD;
        float editor_translation_grid_size = 0.1f;
        float editor_rotation_grid_size = 0.1f;
        float editor_scale_grid_size = 0.001f;

        // Preview & Caching
        std::string m_lastPreviewedFile = "";
        std::unordered_map<std::string, GLuint> m_thumbnailCache;
        GLuint m_defaultWhiteTexture = 0;
        const void* m_lastPreviewedMeshId = nullptr;
        const void* m_lastInspectorMeshId = nullptr;
        GLuint m_cachedInspectorMeshTex = 0;
        std::string m_lastInspectorMatName = "";
        GLuint m_cachedInspectorMatTex = 0;

        // Asset Browser
        float m_thumbnailSize = 80.0f;
        float m_thumbnailPadding = 12.0f;
        std::filesystem::path m_assetBrowserCurrentPath = "Assets";
        std::string m_assetBrowserSelectedFile = "";

        // Text Editor
        TextEditor m_textEditor;
        std::string m_currentFileOpen;
        bool m_isForcedReadOnly = false;

        // Profiler
        float m_frameTimes[1000] = {0};
        int m_frameTimeIndex = 0;
        static std::vector<ProfileResult> s_ProfileResults;
        static std::vector<ProfileResult> s_LastFrameProfileResults;
        static std::mutex s_ProfileMutex;

        // Resource Packer Threading
        bool m_openPackingPopup = false;
        bool m_isPacking = false;
        std::atomic<float> m_packProgress{0.0f};
        std::atomic<bool> m_packDone{false};
        std::atomic<bool> m_packSuccess{false};
        std::string m_packStatusMsg = "";
        std::mutex m_uiMutex;
        std::string m_timeElapsedStr = "00:00";
        std::string m_timeRemainingStr = "--:--";

        // Compiling Modal
        bool m_openCompilingModal = false;
        bool m_isCompiling = false;
        std::atomic<float> m_compileProgress{0.0f};
        std::atomic<bool> m_compileDone{false};
        std::atomic<bool> m_compileSuccess{false};
        std::string m_compileStatusMsg = "";
        std::vector<std::string> m_compileQueue;
        std::string m_compileTimeElapsedStr = "00:00";
        std::string m_compileTimeRemainingStr = "--:--";

        // Material Editor State
        char m_matEdName[128] = "NewMaterial";
        char m_matEdSaveDir[256] = "Assets/Materials";
        glm::vec4 m_matEdBaseColor{1.0f};
        float m_matEdMetallic = 0.0f;
        float m_matEdRoughness = 0.5f;
        float m_matEdEmissive = 0.0f;
        std::string m_matEdAlbedoPath = "";
        std::string m_matEdNormalPath = "";
        std::string m_matEdMetallicRoughnessPath = "";

        // --- NEW: Track Game View Hover State ---
        bool m_isGameViewHovered = false;
    };

    class ScopeTimer {
    public:
        ScopeTimer(const char* name) : m_Name(name) {
            m_StartTimepoint = std::chrono::high_resolution_clock::now();
        }
        
        ~ScopeTimer() {
            auto endTimepoint = std::chrono::high_resolution_clock::now();
            long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
            long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();
            float duration = (end - start) * 0.001f;
            ImGuiSystem::PushProfileResult({m_Name, duration});
        }
    private:
        const char* m_Name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
    };

} // namespace Engine::Systems

// Utility Macro to automatically generate a unique variable name for the timer
#define PROFILE_SCOPE(name) Engine::Systems::ScopeTimer timer##__LINE__(name)