// PhotonCommons.h
#pragma once

#include <any>
#include <iostream>
#include <vector>
#include <string>
#include <type_traits> // Required for smart casting and is_enum_v

// --- GLM SUPPORT ---
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // For to_string

// A generic wrapper for data flowing through the Node Graph wires.
struct PhotonAny {
    std::any value;

    // Helper to cast safely with explicit conversions
    template<typename T>
    T Cast() const {
        if (!value.has_value()) return T{};

        // 1. Exact Match (Fast path)
        if (value.type() == typeid(T)) {
            return std::any_cast<T>(value);
        }

        // 2. Enum Conversions
        if constexpr (std::is_enum_v<T>) {
            if (value.type() == typeid(int)) {
                return static_cast<T>(std::any_cast<int>(value));
            } else if (value.type() == typeid(uint64_t)) {
                return static_cast<T>(std::any_cast<uint64_t>(value));
            }
        }

        // 3. Numeric Conversions (int <-> float <-> bool)
        if constexpr (std::is_arithmetic_v<T> && !std::is_same_v<T, bool> && !std::is_enum_v<T>) {
            if (value.type() == typeid(int))   return static_cast<T>(std::any_cast<int>(value));
            if (value.type() == typeid(uint64_t))   return static_cast<T>(std::any_cast<uint64_t>(value));
            if (value.type() == typeid(float)) return static_cast<T>(std::any_cast<float>(value));
            if (value.type() == typeid(bool))  return static_cast<T>(std::any_cast<bool>(value));
        }
        else if constexpr (std::is_same_v<T, bool>) {
             if (value.type() == typeid(int))   return std::any_cast<int>(value) != 0;
             if (value.type() == typeid(uint64_t))   return std::any_cast<uint64_t>(value) != 0;
             if (value.type() == typeid(float)) return std::any_cast<float>(value) != 0.0f;
        }

        // 4. String Conversions
        if constexpr (std::is_same_v<T, std::string>) {
            if (value.type() == typeid(int)) return std::to_string(std::any_cast<int>(value));
            if (value.type() == typeid(uint64_t)) return std::to_string(std::any_cast<uint64_t>(value));
            if (value.type() == typeid(float)) return std::to_string(std::any_cast<float>(value));
            if (value.type() == typeid(bool)) return std::any_cast<bool>(value) ? "true" : "false";
            
            // Vector/Matrix to String
            if (value.type() == typeid(glm::vec2)) return glm::to_string(std::any_cast<glm::vec2>(value));
            if (value.type() == typeid(glm::vec3)) return glm::to_string(std::any_cast<glm::vec3>(value));
            if (value.type() == typeid(glm::vec4)) return glm::to_string(std::any_cast<glm::vec4>(value));
            if (value.type() == typeid(glm::mat3)) return glm::to_string(std::any_cast<glm::mat3>(value));
            if (value.type() == typeid(glm::mat4)) return glm::to_string(std::any_cast<glm::mat4>(value));
        }

        // --- 5. Vector/Matrix Conversions ---
        if constexpr (std::is_same_v<T, glm::vec2>) {
             if (value.type() == typeid(glm::vec2)) return std::any_cast<glm::vec2>(value);
             return glm::vec2(0.0f);
        }
        if constexpr (std::is_same_v<T, glm::vec3>) {
             if (value.type() == typeid(glm::vec3)) return std::any_cast<glm::vec3>(value);
             return glm::vec3(0.0f);
        }
        if constexpr (std::is_same_v<T, glm::vec4>) {
             if (value.type() == typeid(glm::vec4)) return std::any_cast<glm::vec4>(value);
             return glm::vec4(0.0f);
        }
        if constexpr (std::is_same_v<T, glm::mat3>) {
             if (value.type() == typeid(glm::mat3)) return std::any_cast<glm::mat3>(value);
             return glm::mat3(1.0f); // Identity
        }
        if constexpr (std::is_same_v<T, glm::mat4>) {
             if (value.type() == typeid(glm::mat4)) return std::any_cast<glm::mat4>(value);
             return glm::mat4(1.0f); // Identity
        }

        // 6. Fallback
        try {
            return std::any_cast<T>(value);
        } catch (const std::bad_any_cast& e) {
            std::cerr << "[PhotonAny] Cast Error: Cannot convert " << value.type().name() 
                      << " to " << typeid(T).name() << "\n";
            return T{}; 
        }
    }
};