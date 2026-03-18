#pragma once
#include "Blueprints/ReflectionRegistry.h"
#include "Blueprints/ReflectionTemplates.h"
#include <cstddef>

#define REGISTER_CLASS_BEGIN(ClassName) \
    struct ClassName##_Reflector { \
        static void Register() { \
            ClassInfo info; \
            info.name = #ClassName; \
            using CurrentClass = ClassName;

#define REGISTER_PROPERTY(MemberName) \
            info.properties[#MemberName] = { \
                #MemberName, \
                offsetof(CurrentClass, MemberName), \
                PhotonInternal::GetMemberType(&CurrentClass::MemberName) \
            };

#define REGISTER_FUNCTION(FuncName, ...) \
            info.functions[#FuncName] = { \
                #FuncName, \
                {__VA_ARGS__}, \
                PhotonInternal::ExtractArgTypes(&CurrentClass::FuncName), \
                PhotonInternal::ExtractReturnType(&CurrentClass::FuncName), \
                PhotonInternal::CreateInvoker(&CurrentClass::FuncName) \
            };

#define REGISTER_CLASS_END(ClassName) \
            PhotonRegistry::Get().RegisterClass(#ClassName, info); \
        } \
    }; \
    // A small trick to auto-call Register() before main() runs, if desired.
    // static struct AutoReg_##ClassName { AutoReg_##ClassName() { ClassName##_Reflector::Register(); } } AutoRegInst_##ClassName;

// --- ENUM REGISTRATION MACROS ---

#define REGISTER_ENUM_BEGIN(EnumName) \
    struct EnumName##_Reflector { \
        static void Register() { \
            EnumInfo info; \
            info.name = #EnumName; \
            info.type = std::type_index(typeid(EnumName)); \
            using CurrentEnum = EnumName;

#define REGISTER_ENUM_VALUE(ValueName) \
            info.values.push_back({ #ValueName, static_cast<int>(CurrentEnum::ValueName) });

#define REGISTER_ENUM_END(EnumName) \
            PhotonRegistry::Get().RegisterEnum(std::type_index(typeid(EnumName)), info); \
        } \
    };
    
// General property for Blueprints
#define REGISTER_PROPERTY(MemberName) \
            info.properties[#MemberName] = { \
                #MemberName, \
                offsetof(CurrentClass, MemberName), \
                PhotonInternal::GetMemberType(&CurrentClass::MemberName) \
            };

// NEW: Completely unique macro ONLY for ImGui Inspector fields
#define REGISTER_GUI_PROPERTY(MemberName) \
            info.guiProperties[#MemberName] = { \
                #MemberName, \
                offsetof(CurrentClass, MemberName), \
                PhotonInternal::GetMemberType(&CurrentClass::MemberName) \
            };