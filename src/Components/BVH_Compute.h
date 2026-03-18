#pragma once
#include "Blueprints/ReflectionMacros.h"

namespace Engine::Components {

    struct BVH_Compute {
        REFLECT_CLASS(BVH_Compute)
        AUTHORIZE_BLUEPRINT_SCOPE(BVH_Compute)

        bool IsDirty = false;
    };

} // namespace Engine::Components