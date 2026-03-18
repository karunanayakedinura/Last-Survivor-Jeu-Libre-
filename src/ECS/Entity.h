#pragma once
#include <cstdint>

namespace Engine::ECS {
    
    // An Entity is just a unique 32-bit integer
    using Entity = uint32_t;
    
    // A constant to represent an invalid or null entity
    constexpr Entity MAX_ENTITIES = 500000;
    constexpr Entity NULL_ENTITY = MAX_ENTITIES + 1;

} // namespace Engine::ECS