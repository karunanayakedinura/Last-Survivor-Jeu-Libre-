#pragma once
#include "Core/ISystem.h"
#include "Core/Engine.h"
#include <string>

// Macro to easily export the system from the DLL
#ifdef _WIN32
    #define EXPORT_SYSTEM extern "C" __declspec(dllexport)
#else
    #define EXPORT_SYSTEM extern "C"
#endif

// The function signature that our loader will look for
typedef Engine::Core::ISystem* (*CreateSystemFn)();

// Forward declare the loader function so the Engine can use it
namespace Engine::Core {
    void LoadDynamicSystemsFromManifest(Engine* engine, const std::string& manifestPath);
}