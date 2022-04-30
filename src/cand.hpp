#pragma once
#include <map>
#include <nlohmann/json.hpp>
#include <string>

class FunctionHandler {
    static std::map<std::string, nlohmann::json (*)(const nlohmann::json&)> m_functionMap;

public:
    FunctionHandler() = default;
    template <typename T>
    FunctionHandler(T lambda, const std::string& name)
    {
        m_functionMap[name] = lambda;
    }

    [[nodiscard]] auto getFunctionMap() const -> std::map<std::string, nlohmann::json (*)(const nlohmann::json&)>
    {
        return m_functionMap;
    }
};

std::map<std::string, nlohmann::json (*)(const nlohmann::json&)> FunctionHandler::m_functionMap;
#define REGISTER_FUNCTION(name, function) \
    FunctionHandler name(function, #name);

