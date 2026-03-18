#pragma once
#include <iostream>
#include <thread>
#include "Core/Engine.h"
#include "Core/ISystem.h"
#include "Core/Defines.h"
#include "Terminal/TerminalUtils.h"
#include "Terminal/tqdm.h"

// Forward declaration prevents needing to include ECS headers here
namespace Engine::ECS { class Registry; }

namespace Engine::Systems {

    class TerminalSystem : public Core::ISystem {
    public:
        TerminalSystem(ECS::Registry* registry, Core::Engine* engine);
        ~TerminalSystem() override = default;

        void OnInit() override;
        void OnFixedUpdate(float deltaTime) override;
        void OnPlayUpdate(float deltaTime) override;

        // Editor specific updates stripped in release builds
        EDITOR_METHOD(void OnEditorUpdate(float deltaTime) override;)
        EDITOR_METHOD(void OnEditorGUI() override;)
        



        static const size_t MAX_LOG_SIZE = 1000; // Keep only last 1000 logs

        const char* HEADER = "\033[95m";
        const char* BLUE = "\033[94m";
        const char* CYAN = "\033[96m";
        const char* GREEN = "\033[92m";
        const char* WARNING = "\033[93m";
        const char* FAIL = "\033[91m";
        const char* ENDC = "\033[0m";
        const char* BOLD = "\033[1m";
        const char* UNDERLINE = "\033[4m";
        const char* GREY = "\033[90m";
        const char* ITALIC = "\033[3m";
        const char* SURLINE = "\033[4m";
        const char* FILLED = "\033[7m";
        const char* CROSS = "\033[9m";
        const char* DRAKGREY = "\033[30m";
        const char* PURLE = "\033[35m";
        const char* FILLRED = "\033[41m";
        const char* FILLGREEN = "\033[42m";
        const char* FILLYELLOW = "\033[43m";
        const char* FILLBLUE = "\033[44m";
        const char* FILLPURPLE = "\033[45m";
        const char* FILLCYAN = "\033[46m";
        const char* FILLWHITE = "\033[47m";


        static std::vector<TerminalUtils::LogMessage>& GetLogs() {
            static std::vector<TerminalUtils::LogMessage> logs;
            return logs;
        }

        static void ClearLogs() {
            GetLogs().clear();
        }

        void AddLog(const std::string& text, TerminalUtils::LogType type) {
            auto& logs = GetLogs();
            
            // If we reached the limit, remove the oldest one (index 0)
            if (logs.size() >= MAX_LOG_SIZE) {
                logs.erase(logs.begin()); 
            }
            
            logs.push_back({ text, type });
        }

        std::string set_color(std::string text, const char* color) {
            return std::string(color) + text + std::string(ENDC);
        }

        void print(std::string text, std::string end = "\n") {
            std::cout << text << end;
            AddLog(text, TerminalUtils::LogType::Normal); // Changed from direct push_back
        }
        
        void debug(std::string text, const char* color = "\033[35m", std::string end = "\n") {
            std::cout << set_color(set_color("Debug: ", WARNING), BOLD) + set_color(text, PURLE) << end;
            AddLog("Debug: " + text, TerminalUtils::LogType::Debug); // Changed from direct push_back
        }

        void warn(std::string text, const char* color = "\033[93m", std::string end = "\n") {
            std::cout << set_color(set_color("Warn: ", WARNING), BOLD) + set_color(text, WARNING) << end;
            AddLog("Warn: " + text, TerminalUtils::LogType::Warning);
        }
        
        void warning(std::string text, const char* color = "\033[93m", std::string end = "\n") {
            std::cout << set_color(set_color("Warn: ", WARNING), BOLD) + set_color(text, WARNING) << end;
            AddLog("Warn: " + text, TerminalUtils::LogType::Warning);
        }

        void error(std::string text, const char* color = "\033[91m", std::string end = "\n") {
            std::cout << set_color(set_color("Error: ", FAIL), BOLD) + set_color(text, FAIL) << end;
            AddLog("Error: " + text, TerminalUtils::LogType::Error);
        }

        void success(std::string text, const char* color = "\033[92m", std::string end = "\n") {
            std::cout << set_color(set_color("Success: ", GREEN), BOLD) + set_color(text, GREEN) << end;
            AddLog("Success: " + text, TerminalUtils::LogType::Success);
        }

        void info(std::string text, const char* color = "\033[3m", std::string end = "\n") {
            std::cout << set_color(set_color("Info: ", CYAN), BOLD) + set_color(text, CYAN) << end;
            AddLog("Info: " + text, TerminalUtils::LogType::Info);
        }
        
        void test() {
            print("####### Terminal test ########");
            print("This is the print function");
            warn("This is the warn function", WARNING);
            error("This is the error function", FAIL);
            success("This is the success function", GREEN);
            info("This is the info function", CYAN);
            example_description_change();
            print("##############################");
        }
    private:
        ECS::Registry* m_Registry;
        Core::Engine* m_Engine;

        void example_description_change() {
            int total_steps = 120;
            tqdm bar(total_steps);
            for (int i = 0; i < total_steps; ++i) {
                // if (i == total_steps / 2) {
                //     bar.set_description("Halfway there");
                // }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                bar.tick();
            }
            bar.close();
        }
    };

} // namespace Engine::Systems