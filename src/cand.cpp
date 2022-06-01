#include "cand/cand.hpp"
#include "cand/functions.hpp"
#include <array>
#include <cand/can.hpp>
#include <cstring>
#include <frameworkd/classes/daemon/daemon.hpp>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <nlohmann/json.hpp>
#include <sdbus-c++/Error.h>
#include <string>

auto hex2string(const unsigned int i) -> std::string
{
    std::stringstream s;
    s << "0X" << std::uppercase << std::setw(3) << std::setfill('0') << std::hex << i;
    return s.str();
}

class CandService : public RoutineService {
public:
    explicit CandService(const std::string& id, const std::string& busName, const nlohmann::json actions)
        : RoutineService { id }
        , m_canBus { busName }
    {
        m_actions = actions;
    }
    nlohmann::json m_actions;
    CAN m_canBus;

    std::map<unsigned int, std::vector<std::array<std::uint8_t, 8>> (*)(const nlohmann::json&)> m_canMap;
    std::map<unsigned int, DBusHandler::Path> adresses;

    void setup() override
    {
        std::map<std::string, message_t (*)(const nlohmann::json&)> functionMap = FunctionHandler::getFunctionMap();
        std::cout << m_actions << std::endl;

        for (auto& [id, service] : m_actions.items()) {
            std::cout << id << service << std::endl;
            auto idNum = static_cast<unsigned int>(std::stoul(id, nullptr, 16));
            std::string name = service["who"];
            m_canMap[idNum] = functionMap[service["method"]];
            adresses[idNum] = DBusHandler::Path {
                "zfkd.dbus." + name,
                "/zfkd/dbus/" + name,
                "zfkd.dbus." + name,
                service["method"]
            };
        }
    }

    void routine() override
    {
        struct can_frame recFrame;
        m_canBus >> recFrame;

        std::cout << "received id:" << hex2string(recFrame.can_id) << std::endl;

        std::cout << m_actions << std::endl;
        nlohmann::json serviceJson = m_actions[hex2string(recFrame.can_id)];
        std::cout << serviceJson << std::endl;

        if (adresses.count(recFrame.can_id) == 0) {
            return;
        }

        nlohmann::json recJson;
        try {
            nlohmann::json object;
            // TODO(grillo): Enable calling a Method with zeros arguments
            object["a"] = 0;
            recJson = DBusHandler::callMethod(adresses[recFrame.can_id], object);
            std::cout << recJson << std::endl;
        } catch (sdbus::Error) {
            std::cout << "invalid ID" << std::endl;
            return;
        }

        auto dataFrames = m_canMap[recFrame.can_id](recJson);

        std::cout << "can_frames:" << dataFrames.size() << std::endl;

        for (auto& frameData : dataFrames) {
            for (auto& byte : frameData) {
                std::cout << static_cast<int>(byte);
                std::cout << ",";
            }
            std::cout << "/";
        }

        std::cout << std::endl;

        unsigned int startID = static_cast<unsigned int>(std::strtoul(static_cast<std::string>(serviceJson["answer"]["start_id"]).c_str(), nullptr, 16));
        unsigned int endID = static_cast<unsigned int>(std::strtoul(static_cast<std::string>(serviceJson["answer"]["end_id"]).c_str(), nullptr, 16));

        for (unsigned int i = startID; i <= endID && !dataFrames[i - startID].empty(); i++) {

            struct can_frame frame;

            frame.len = static_cast<unsigned char>(dataFrames[i - startID].size());
            std::memcpy(frame.data, dataFrames[i - startID].data(), dataFrames[i - startID].size());

            frame.can_id = i;
            m_canBus << frame;
        }
    }

    void destroy() override
    {
    }
};

auto main() -> int
{
    Daemon daemon("cand.json");
    std::cout << daemon.m_configHandler.getAllConfig() << std::endl;
    std::cout << daemon.m_configHandler["actions"] << std::endl;
    CandService candService("cand", "vcan0", daemon.m_configHandler["actions"]);

    daemon.deploy(candService);
    daemon.run();
}
