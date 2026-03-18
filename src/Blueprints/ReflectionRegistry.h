#pragma once

#include "Blueprints/PhotonCommons.h"
#include <string>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <typeinfo> 
#include <vector>

// NEW: Enum Metadata
struct EnumValue {
    std::string name;
    int value;
};

struct EnumInfo {
    std::string name;
    std::type_index type = std::type_index(typeid(void));
    std::vector<EnumValue> values;
};

struct PropertyInfo {
    std::string name;
    size_t offset;
    std::type_index type;

    PropertyInfo() : name(""), offset(0), type(typeid(void)) {}

    PropertyInfo(const std::string& n, size_t o, std::type_index t) 
        : name(n), offset(o), type(t) {}
};

struct FunctionInfo {
    std::string name;
    std::vector<std::string> argNames;
    std::vector<std::type_index> argTypes; 
    std::type_index returnType = std::type_index(typeid(void));
    std::function<PhotonAny(void*, const std::vector<PhotonAny>&)> invoker; 
};

struct ClassInfo {
    std::string name;
    std::unordered_map<std::string, FunctionInfo> functions;
    std::unordered_map<std::string, PropertyInfo> properties;

    // NEW: Completely unique registry map ONLY for the ImGui Inspector
    std::unordered_map<std::string, PropertyInfo> guiProperties;

    const FunctionInfo* GetFunction(const std::string& funcName) const {
        if (functions.find(funcName) != functions.end()) return &functions.at(funcName);
        return nullptr;
    }
    
    const PropertyInfo* GetProperty(const std::string& propName) const {
        if (properties.find(propName) != properties.end()) return &properties.at(propName);
        return nullptr;
    }

    const PropertyInfo* GetGuiProperty(const std::string& propName) const {
        if (guiProperties.find(propName) != guiProperties.end()) return &guiProperties.at(propName);
        return nullptr;
    }
};

class PhotonRegistry {
public:
    static PhotonRegistry& Get() { 
        static PhotonRegistry instance; 
        return instance; 
    }
    
    void RegisterClass(const std::string& name, const ClassInfo& info) {
        classes[name] = info;
    }

    // NEW: Register Enum
    void RegisterEnum(std::type_index type, const EnumInfo& info) {
        enums[type] = info;
    }

    const ClassInfo* GetClass(const std::string& name) {
        if (classes.find(name) != classes.end()) return &classes.at(name);
        return nullptr;
    }

    // NEW: Get Enum
    const EnumInfo* GetEnum(std::type_index type) {
        if (enums.find(type) != enums.end()) return &enums.at(type);
        return nullptr;
    }

    const std::unordered_map<std::string, ClassInfo>& GetAllClasses() { return classes; }
    // NEW: Accessor for Graph Editor to list all enums
    const std::unordered_map<std::type_index, EnumInfo>& GetAllEnums() { return enums; }

private:
    std::unordered_map<std::string, ClassInfo> classes;
    // NEW: Enum Storage
    std::unordered_map<std::type_index, EnumInfo> enums;
};