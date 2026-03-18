#pragma once
#include <string>

namespace Engine::Scripting::Helpers {
    enum class ScriptPropType {
        Float, Int, Bool, Vec3, Color
    };

    // Define a structure to hold the property reference
    struct ScriptProp {
        std::string name;
        ScriptPropType type;
        void* data; // Pointer to the actual variable in the child class
    };
} // namespace Engine::Scripting