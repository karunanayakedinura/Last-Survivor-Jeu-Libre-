#pragma once
#define BUILD_ENCRYPT_KEY "PhotonEngineReleaseKey_2026"

// #ifndef BUILD_ENGINE_EDITOR
//     #define BUILD_ENGINE_EDITOR
// #endif

// When building DLLs (Scripts), we IMPORT symbols from main.exe
#ifdef BUILD_ENGINE
    #define ENGINE_API __declspec(dllexport)
#else
    #define ENGINE_API __declspec(dllimport)
#endif

// Define ENGINE_EDITOR in your CMake/Build system for Editor builds
#if defined(BUILD_ENGINE_EDITOR)
    #define EDITOR_ONLY(...) __VA_ARGS__
    #define EDITOR_METHOD(MethodDef) MethodDef
#else
    #define EDITOR_ONLY(...)
    // #define EDITOR_METHOD(MethodDef)
    #define EDITOR_METHOD(...)
#endif

// ==========================================
// PROFILING MACROS
// ==========================================
// Note: To use these macros in a .cpp file, you must include "Systems/ImGuiSystem.h" 
// so the compiler knows what `Engine::Systems::ScopeTimer` is.

#if defined(BUILD_ENGINE_EDITOR) || defined(ENABLE_PROFILING)
    #define PHOTON_PROFILE_SCOPE(name) Engine::Systems::ScopeTimer timer##__LINE__(name)
    
    // Automatically capture the function signature based on the compiler
    #if defined(_MSC_VER)
        #define PHOTON_PROFILE_METHOD() PHOTON_PROFILE_SCOPE(__FUNCSIG__)
    #else
        #define PHOTON_PROFILE_METHOD() PHOTON_PROFILE_SCOPE(__PRETTY_FUNCTION__)
    #endif
#else
    // Strip profiling out of release builds for maximum performance
    #define PHOTON_PROFILE_SCOPE(name)
    #define PHOTON_PROFILE_METHOD()
#endif