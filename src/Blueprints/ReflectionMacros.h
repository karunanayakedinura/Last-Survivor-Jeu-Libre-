#pragma once

#include <string>
// --- Profiler Includes ---
#include <chrono>
#include <string>
#include <unordered_map>
#include <mutex>


// 1. Forward-declare the namespace and class so the compiler knows it exists
namespace Engine::Blueprints {
    class BlueprintSystem;
}

// Keep your existing macro structure for blueprint reflection
#define REFLECT_CLASS(ClassName) \
    public: \
    static const char* GetClassName() { return #ClassName; }

#define REFLECT_ENUM(EnumName) \
    public: \
    static const char* GetEnumName() { return #EnumName; }

#define REFLECT_METHOD(MethodName) \
    /* Your macro magic to register the method by name string to the Blueprint system */ \
    Engine::Blueprints::Registry::RegisterMethod(#MethodName, &MethodName);

// // --- NEW: Macro to register class member variables/values ---
// #define REFLECT_VALUE(ValueName) \
//     /* Your macro magic to register the property/value by name string */ \
//     Engine::Blueprints::Registry::RegisterValue(#ValueName, &ValueName);

// #define REFLECT_VALUE_BY_NAME(ClassName, ValueName) \
//     Engine::Blueprints::Registry::RegisterValue(#ValueName, &ClassName::ValueName);

// 2. FIX: Add the ClassName parameter and the reflector friend declaration!
#define AUTHORIZE_BLUEPRINT_SCOPE(ClassName) \
    friend class Engine::Blueprints::BlueprintSystem; \
    friend struct ClassName##_Reflector;

// --- NEW: Method Profiling System ---

struct PhotonProfileData {
    float lastTimeMs = 0.0f;
    float avgTimeMs = 0.0f;
    float maxTimeMs = 0.0f;
    uint64_t callCount = 0;
};

class PhotonProfiler {
public:
    static PhotonProfiler& Get() {
        static PhotonProfiler instance;
        return instance;
    }

    void Record(const char* name, float timeMs) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto& data = m_profiles[name];
        data.lastTimeMs = timeMs;
        data.callCount++;
        
        if (data.callCount == 1) {
            data.avgTimeMs = timeMs;
            data.maxTimeMs = timeMs;
        } else {
            // Apply Exponential Moving Average (EMA) for smoothed average
            float alpha = 0.05f; 
            data.avgTimeMs = (timeMs * alpha) + (data.avgTimeMs * (1.0f - alpha));
            if (timeMs > data.maxTimeMs) {
                data.maxTimeMs = timeMs;
            }
        }
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_profiles.clear();
    }

    // Returns a copy to avoid threading issues when ImGui renders it
    std::unordered_map<std::string, PhotonProfileData> GetProfilesCopy() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_profiles;
    }

private:
    std::unordered_map<std::string, PhotonProfileData> m_profiles;
    std::mutex m_mutex;
};

class PhotonScopedProfiler {
    const char* m_name;
    std::chrono::high_resolution_clock::time_point m_start;
public:
    PhotonScopedProfiler(const char* name) : m_name(name) {
        m_start = std::chrono::high_resolution_clock::now();
    }
    ~PhotonScopedProfiler() {
        auto end = std::chrono::high_resolution_clock::now();
        float ms = std::chrono::duration<float, std::milli>(end - m_start).count();
        PhotonProfiler::Get().Record(m_name, ms);
    }
};

// --- PROFILING MACROS ---
// Drop this at the top of any method to track it (e.g., PHOTON_PROFILE_METHOD();)
#define PHOTON_PROFILE_METHOD() PhotonScopedProfiler _photon_method_profiler(__FUNCTION__)

// Use this for custom scoped blocks
#define PHOTON_PROFILE_SCOPE(Name) PhotonScopedProfiler _photon_scope_profiler(Name)