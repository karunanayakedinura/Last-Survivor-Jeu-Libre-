#pragma once
#include <string>
#include <functional>

namespace Engine::Systems::RessourcePackerUtils {
    using PackProgressCallback = std::function<void(int, int, const std::string&, float, float)>;
} // namespace Engine::Systems::RessourcePackerUtils