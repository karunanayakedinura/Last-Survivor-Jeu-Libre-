#pragma once
#include "Core/Defines.h"
#include <string>

namespace Engine::Core {

    class ISystem {
    public:
        std::string m_Name = "UnknownSystem";
        // Add a flag to identify systems loaded from an external DLL
        bool m_IsDynamic = false;
        virtual ~ISystem() = default;

        // Lifecycle
        virtual void OnInit() {}
        virtual void OnShutdown() {}

        // Runtime updates
        virtual void OnPlayUpdate(float deltaTime) {}
        virtual void OnFixedUpdate(float deltaTime) {} // For physics/networking

        // Editor specific updates stripped in release builds
        EDITOR_METHOD(virtual void OnEditorUpdate(float deltaTime) {})
        EDITOR_METHOD(virtual void OnEditorGUI() {})
    };

} // namespace Engine::Core