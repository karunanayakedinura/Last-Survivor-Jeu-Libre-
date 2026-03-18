#pragma once
#include <json.hpp>
#include <fstream>
#include <iostream>
using json = nlohmann::json;

namespace Engine::Core::Config {
    struct RenderSystemSettings {
        int SHADOW_WIDTH_2D = 8192;
        int SHADOW_HEIGHT_2D = 8192;
        int SHADOW_WIDTH_CUBE = 8192;
        int SHADOW_HEIGHT_CUBE = 8192;
        int MAX_SHADOW_CASTERS = 8;
        int MAX_LIGHTS = 64;
    };
    class EngineConfig {
    public:
        static EngineConfig& Instance() {
            static EngineConfig instance;
            return instance;
        }
        
        int LoadEngineSettings() {
            json APP_CONFIG;

            std::ifstream f(EngineSettingsPath.c_str());
            if (f.is_open()) {
                f >> APP_CONFIG;
            } else {
                std::cout << "[CRITICAL] " << EngineSettingsPath << " not found! Exiting." << std::endl;
                return 0;
            }

            ScreenWidth = (int) APP_CONFIG["ScreenWidth"];
            ScreenHeight = (int) APP_CONFIG["ScreenHeight"];
            VSyncEnableState = (bool) APP_CONFIG["VSync"];

            // terminal::Instance().info("Width: " + std::to_string((int) APP_CONFIG["ScreenWidth"]) + ", Height: " + std::to_string((int) APP_CONFIG["ScreenHeight"]));

            renderSystemSettings.SHADOW_WIDTH_2D = (int) APP_CONFIG["SHADOWS_SETTINGS"]["SHADOW_WIDTH_2D"];
            renderSystemSettings.SHADOW_HEIGHT_2D = (int) APP_CONFIG["SHADOWS_SETTINGS"]["SHADOW_HEIGHT_2D"];
            renderSystemSettings.SHADOW_WIDTH_CUBE = (int) APP_CONFIG["SHADOWS_SETTINGS"]["SHADOW_WIDTH_CUBE"];
            renderSystemSettings.SHADOW_HEIGHT_CUBE = (int) APP_CONFIG["SHADOWS_SETTINGS"]["SHADOW_HEIGHT_CUBE"];
            renderSystemSettings.MAX_SHADOW_CASTERS = (int) APP_CONFIG["SHADOWS_SETTINGS"]["MAX_SHADOW_CASTERS"];
            renderSystemSettings.MAX_LIGHTS = (int) APP_CONFIG["SHADOWS_SETTINGS"]["MAX_LIGHTS"];
            std::cout << "Engine Configuration File loaded successfully." << std::endl;
            
            return 1;
        }

        int GetScreenWidth() {
            return ScreenWidth;
        }
        
        int GetScreenHeight() {
            return ScreenHeight;
        }

        bool GetVSyncEnableState() {
            return VSyncEnableState;
        }

        void SetWindowSize(int width, int height) {
            ScreenWidth = width;
            ScreenHeight = height;
        }

        RenderSystemSettings GetRenderSettings() {
            return renderSystemSettings;
        }
    private:
        int ScreenWidth = 1280;
        int ScreenHeight = 720;
        bool VSyncEnableState = false;
        RenderSystemSettings renderSystemSettings;

        std::string EngineSettingsPath = "Engine/EngineSettings.json";
    };
} // namespace Engine::Core::Config