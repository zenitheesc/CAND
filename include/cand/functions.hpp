#pragma once
#include "cand/cand.hpp"
#include <cstring>
#include <iostream>
#include <vector>

REGISTER_FUNCTION(heartbeat, [](const nlohmann::json& json) {
    std::cout << "heartbeat" << std::endl;
    return (message_t) {};
})

REGISTER_FUNCTION(request, [](const nlohmann::json& json) {
    std::vector<uint8_t> message = json["data"];

    message_t res = {};

    for (auto& byte : message) {
        std::cout << static_cast<int>(byte) << ",";
    }
    std::cout << "\n";

    size_t size = message.size() / 8 + static_cast<bool>(message.size() % 8);
    res.resize(size);

    for (size_t i = 0; i < message.size(); i += 8) {
        std::cout << i << std::endl;
        std::cout << res[i].size() << std::endl;
        std::memcpy(res[i / 8].data(), message.data() + i, std::min(res[i / 8].max_size(), message.size() - i));
    }

    return res;
})
