#pragma once
#include <iostream>
#include <optional>
#include "Core/Engine.h"
#include "Core/ISystem.h"
#include "Core/Defines.h"

// Forward declaration prevents needing to include ECS headers here
namespace Engine::ECS { class Registry; }

namespace Engine::Systems {

    class ENGINE_API InputSystem : public Core::ISystem {
    public:
        InputSystem(ECS::Registry* registry, Core::Engine* engine);
        ~InputSystem() override = default;
        glm::vec2 moveInput = glm::vec2(0.0f);
        glm::vec2 lookInput = glm::vec2(0.0f);
        float moveSpeed = 5.0f;
        float mouseSensitivity = 0.002f;
        GLFWwindow* window = nullptr;
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

        void SetMouseCapture(bool captureState);
        void BindWindow(GLFWwindow* window_arg);
        // --- State Getters (unchanged) ---
        bool GetKeyState(int key);
        bool GetKeyPressed(int key);
        glm::vec2 GetMousePos();
        bool GetMouseButtonState(int button);
        bool GetMouseButtonPressed(int button);

        void OnInit() override;
        void OnFixedUpdate(float deltaTime) override;
        void OnPlayUpdate(float deltaTime) override;

        // Editor specific updates stripped in release builds
        EDITOR_METHOD(void OnEditorUpdate(float deltaTime) override;)
        EDITOR_METHOD(void OnEditorGUI() override;)

    private:
        ECS::Registry* m_Registry;
        Core::Engine* m_Engine;

        std::optional<bool> m_captureStateRequest;
    
        // --- Store window center ---
        glm::vec2 m_windowCenter = glm::vec2(0.0f);
        bool m_mouseCaptured = false; // --- NEW: Track state

        void ApplyMouseCaptureState();
        void ResizeRenderer(int width, int height);

        glm::vec2 mouseDelta = glm::vec2(0.0f);
        glm::vec2 lastMousePos = glm::vec2(0.0f);
        bool firstMouse = true;
        bool keys[1024] = { false };
        bool keysPressed[1024] = { false };
        bool keysReleased[1024] = { false };
        bool mouseButtons[8] = { false };
        bool mouseButtonsPressed[8] = { false };
        bool mouseButtonsReleased[8] = { false };

        // --- Callbacks are now clean ---
        void KeyCallback(GLFWwindow* w, int key, int scancode, int action, int mods);

        // --- ========================================================== ---
        // ---              MODIFIED MOUSE CALLBACK                       ---
        // --- ========================================================== ---
        void mouse_callback(GLFWwindow* w, double xpos, double ypos);

        void MouseButtonCallback(GLFWwindow* w, int button, int action, int mods);

        void process_input(GLFWwindow* w, float deltaTime);

        // --- Static Dispatchers (unchanged) ---
        static void key_callback_dispatch(GLFWwindow* w, int k, int s, int a, int m) {
            static_cast<InputSystem*>(glfwGetWindowUserPointer(w))->KeyCallback(w, k, s, a, m);
        }
        static void mouse_callback_dispatch(GLFWwindow* w, double x, double y) {
            static_cast<InputSystem*>(glfwGetWindowUserPointer(w))->mouse_callback(w, x, y);
        }
        static void mouse_button_callback_dispatch(GLFWwindow* w, int b, int a, int m) {
            static_cast<InputSystem*>(glfwGetWindowUserPointer(w))->MouseButtonCallback(w, b, a, m);
        }
    };

} // namespace Engine::Systems