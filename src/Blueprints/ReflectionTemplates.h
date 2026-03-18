#pragma once

#include "PhotonCommons.h"
#include <functional>
#include <vector>
#include <typeindex>
#include <tuple>
#include <type_traits> // Required for std::is_void_v

namespace PhotonInternal {

    // --- 1. Invoker Generation ---
    
    // Helper that handles both void and non-void return types (Non-Const)
    template <typename ClassType, typename ReturnType, typename... Args, std::size_t... I>
    PhotonAny InvokeHelper(void* instance, ReturnType (ClassType::* func)(Args...), const std::vector<PhotonAny>& args, std::index_sequence<I...>) {
        ClassType* obj = static_cast<ClassType*>(instance);
        if constexpr (std::is_void_v<ReturnType>) {
            (obj->*func)(args[I].Cast<typename std::decay<Args>::type>()...);
            return PhotonAny{}; 
        } else {
            ReturnType result = (obj->*func)(args[I].Cast<typename std::decay<Args>::type>()...);
            
            // NEW: Explicitly convert Enum returns to Int for storage
            // This ensures PhotonAny always holds 'int' for enums, making them compatible with UI input.
            if constexpr (std::is_enum_v<ReturnType>) {
                return PhotonAny{ static_cast<int>(result) };
            } else {
                return PhotonAny{ result };
            }
        }
    }

    // Helper that handles both void and non-void return types (Const Overload)
    template <typename ClassType, typename ReturnType, typename... Args, std::size_t... I>
    PhotonAny InvokeHelper(void* instance, ReturnType (ClassType::* func)(Args...) const, const std::vector<PhotonAny>& args, std::index_sequence<I...>) {
        ClassType* obj = static_cast<ClassType*>(instance);
        if constexpr (std::is_void_v<ReturnType>) {
            (obj->*func)(args[I].Cast<typename std::decay<Args>::type>()...);
            return PhotonAny{};
        } else {
            ReturnType result = (obj->*func)(args[I].Cast<typename std::decay<Args>::type>()...);
            
            // NEW: Explicitly convert Enum returns to Int for storage
            if constexpr (std::is_enum_v<ReturnType>) {
                return PhotonAny{ static_cast<int>(result) };
            } else {
                return PhotonAny{ result };
            }
        }
    }

    // CreateInvoker (Non-Const)
    template <typename ClassType, typename ReturnType, typename... Args>
    std::function<PhotonAny(void*, const std::vector<PhotonAny>&)> CreateInvoker(ReturnType (ClassType::* func)(Args...)) {
        return [func](void* instance, const std::vector<PhotonAny>& args) -> PhotonAny {
            if (args.size() != sizeof...(Args)) {
                std::cerr << "[PhotonReflect] Error: Argument count mismatch during invocation.\n";
                return PhotonAny{};
            }
            return InvokeHelper(instance, func, args, std::index_sequence_for<Args...>{});
        };
    }

    // CreateInvoker (Const Overload)
    template <typename ClassType, typename ReturnType, typename... Args>
    std::function<PhotonAny(void*, const std::vector<PhotonAny>&)> CreateInvoker(ReturnType (ClassType::* func)(Args...) const) {
        return [func](void* instance, const std::vector<PhotonAny>& args) -> PhotonAny {
            if (args.size() != sizeof...(Args)) {
                std::cerr << "[PhotonReflect] Error: Argument count mismatch during invocation.\n";
                return PhotonAny{};
            }
            return InvokeHelper(instance, func, args, std::index_sequence_for<Args...>{});
        };
    }

    // --- 2. Pin Type Extraction ---
    
    template <typename T>
    struct TypeExtractor;

    // Specialization for Non-Const functions
    template <typename ClassType, typename ReturnType, typename... Args>
    struct TypeExtractor<ReturnType(ClassType::*)(Args...)> {
        static std::vector<std::type_index> GetArgTypes() {
            return { std::type_index(typeid(Args))... };
        }
        static std::type_index GetReturnType() {
            return std::type_index(typeid(ReturnType));
        }
    };

    // Specialization for Const functions
    template <typename ClassType, typename ReturnType, typename... Args>
    struct TypeExtractor<ReturnType(ClassType::*)(Args...) const> {
        static std::vector<std::type_index> GetArgTypes() {
            return { std::type_index(typeid(Args))... };
        }
        static std::type_index GetReturnType() {
            return std::type_index(typeid(ReturnType));
        }
    };

    // ExtractArgTypes (Non-Const)
    template <typename ClassType, typename ReturnType, typename... Args>
    std::vector<std::type_index> ExtractArgTypes(ReturnType (ClassType::* func)(Args...)) {
        return TypeExtractor<decltype(func)>::GetArgTypes();
    }

    // ExtractArgTypes (Const Overload)
    template <typename ClassType, typename ReturnType, typename... Args>
    std::vector<std::type_index> ExtractArgTypes(ReturnType (ClassType::* func)(Args...) const) {
        return TypeExtractor<decltype(func)>::GetArgTypes();
    }
    
    // ExtractReturnType (Non-Const)
    template <typename ClassType, typename ReturnType, typename... Args>
    std::type_index ExtractReturnType(ReturnType (ClassType::* func)(Args...)) {
        return std::type_index(typeid(ReturnType));
    }

    // ExtractReturnType (Const Overload)
    template <typename ClassType, typename ReturnType, typename... Args>
    std::type_index ExtractReturnType(ReturnType (ClassType::* func)(Args...) const) {
        return std::type_index(typeid(ReturnType));
    }

    // --- 3. Property Type Extraction ---
    
    template <typename ClassType, typename MemberType>
    std::type_index GetMemberType(MemberType ClassType::* member) {
        return std::type_index(typeid(MemberType));
    }
}