#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Blueprints/ReflectionMacros.h"

namespace Engine::Components {
    struct AudioSource;
}


namespace Engine::Components {
    struct AudioEmitter {
        REFLECT_CLASS(AudioEmitter)
        AUTHORIZE_BLUEPRINT_SCOPE(AudioEmitter)
        std::vector<std::shared_ptr<Components::AudioSource>> sources;

    private:
        int GetAudioSourcesCount() {
            /* Returns the number of Audio sources of the AudioEmitter (int) */
            return static_cast<int>(this->sources.size());
        }
    };

} // namespace Engine::Components