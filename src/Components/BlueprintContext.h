#pragma once
#include <string>
#include "Blueprints/ReflectionMacros.h"

// Forward declare the graph classes inside their proper namespace!
// Adjust "Blueprints" to "GraphEditor" or "Graphs" if they live somewhere else.
namespace Engine::Blueprints {
    struct ExecutionContext;
    struct Graph;
    struct CompiledGraph;
}

namespace Engine::Components {

    struct BlueprintContext {
        REFLECT_CLASS(BlueprintContext)
        AUTHORIZE_BLUEPRINT_SCOPE(BlueprintContext)

        Engine::Blueprints::ExecutionContext* m_ExecutionContext = nullptr;
        Engine::Blueprints::Graph* m_Graph = nullptr;
        Engine::Blueprints::CompiledGraph* m_compiledGraph = nullptr;
        std::string graphPath; 

        // Default constructor
        BlueprintContext() = default;

        // Custom constructor
        BlueprintContext(std::string path) : graphPath(std::move(path)) {}
    };

} // namespace Engine::Components