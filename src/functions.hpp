#pragma once
#include "cand.hpp"
#include <iostream>
#include <vector>

REGISTER_FUNCTION(heartbeat, [](const nlohmann::json& jzon) {
    std::cout << "heartbeat" << std::endl;
    return (nlohmann::json) {};
})

REGISTER_FUNCTION(getCurrJson, [](const nlohmann::json& jzon) {
    std::cout << "getCurrJson" << std::endl;
    std::vector<std::uint8_t>
    return (nlohmann::json) {};
})
