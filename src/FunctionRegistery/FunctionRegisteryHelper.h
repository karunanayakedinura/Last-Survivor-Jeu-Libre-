#pragma once
#include <functional>
#include <any>

namespace Engine::Systems::FunctionRegistery {
    using ReflectedFunction = std::function<std::any(std::vector<std::any>)>;
} // namespace Engine::Systems::FunctionRegistery