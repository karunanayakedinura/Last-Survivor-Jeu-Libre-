#pragma once

#include <string>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Blueprints/ReflectionMacros.h"

namespace Engine::Components {

    struct AudioSource {
        // --------------------------------------------------------------------
        // Reflection / Blueprint macros
        // --------------------------------------------------------------------
        REFLECT_CLASS(AudioSource)
        AUTHORIZE_BLUEPRINT_SCOPE(AudioSource)

        // --------------------------------------------------------------------
        // Public data members – visible to the editor / Blueprint system
        // --------------------------------------------------------------------
        std::string name        = "New Sound";
        std::string filePath    = "";
        float       volume      = 1.0f;
        float       pitch       = 1.0f;
        bool        loop        = false;
        bool        playOnAwake = false;   // Playback starts by default
        bool        is3D        = true;
        float       minDistance = 1.0f;
        float       maxDistance = 50.0f;
        uint32_t    handle      = 0;       // Runtime handle (0 = Not Playing)

    private:
        // --------------------------------------------------------------------
        // Getters (const‑correct)
        // --------------------------------------------------------------------
        const std::string& GetName()        const { return name; }
        const std::string& GetFilePath()    const { return filePath; }
        float               GetVolume()      const { return volume; }
        float               GetPitch()       const { return pitch; }
        bool                GetLoop()        const { return loop; }
        bool                GetPlayOnAwake() const { return playOnAwake; }
        bool                GetIs3D()        const { return is3D; }
        float               GetMinDistance() const { return minDistance; }
        float               GetMaxDistance() const { return maxDistance; }
        uint32_t            GetHandle()      const { return handle; }

        // --------------------------------------------------------------------
        // Setters
        // --------------------------------------------------------------------
        void SetName(const std::string& v)        { name = v; }
        void SetFilePath(const std::string& v)    { filePath = v; }
        void SetVolume(float v)                  { volume = v; }
        void SetPitch(float v)                   { pitch = v; }
        void SetLoop(bool v)                     { loop = v; }
        void SetPlayOnAwake(bool v)              { playOnAwake = v; }
        void SetIs3D(bool v)                     { is3D = v; }
        void SetMinDistance(float v)             { minDistance = v; }
        void SetMaxDistance(float v)             { maxDistance = v; }
        void SetHandle(uint32_t v)               { handle = v; }
    };

} // namespace Engine::Components

