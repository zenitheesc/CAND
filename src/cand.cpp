#include "frameworkd/service-handler/service-handler.hpp"
#include "cand.hpp"
#include "functions.hpp"
#include <cand/can.hpp>
#include <fstream>
#include <functional>
#include <nlohmann/json.hpp>
#include <string>

class MainRoutine : public RoutineService {
public:
    explicit MainRoutine(const std::string& id, const nlohmann::json& jsonConfig)
        : RoutineService { id }
        , m_canBus { jsonConfig["can_bus_id"] }
        , m_configJson { jsonConfig }
    {
    }
    //struct CAN IDS
    nlohmann::json m_servedJson;
    CAN m_canBus;
    nlohmann::json m_configJson;
    std::map<unsigned int, std::map<std::string, nlohmann::json (*)(const nlohmann::json&)>> m_canMap;
    void setup() override
    {
        auto funcHandler = FunctionHandler();

        std::map<std::string, nlohmann::json (*)(const nlohmann::json&)> functionMap = funcHandler.getFunctionMap();
        //struct can_filter filter = {.can_id = 0x400, .can_mask = 0x400};
        //m_canBus.setFilter(filter);
       
        for (auto& [gkay, farofa] : m_configJson.items()) {
            for (auto& [key, value] : farofa["actions"].items()) {
                std::map<std::string, nlohmann::json (*)(const nlohmann::json&)> randomMap;
                std::cout << value["when"]["id"] << "----" << value["do"] << std::endl;
                for (auto& [key1, value1] : value["do"].items()) {
                    randomMap[value1["method"]] = functionMap[value1["method"]];
                }
                m_canMap[value["when"]["id"]] = randomMap;
            }
        }
    }

    void routine() override
    {
        struct can_frame frame;
        m_canBus >> frame;
        for (auto& [struing, func] : m_canMap[frame.can_id]) {
            nlohmann::json answer = func(nlohmann::json {});
        }
    }

    void destroy() override
    {
    }
};

auto main(int argc, char* argv[]) -> int
{
    std::ifstream configJson("config.json");

    nlohmann::json myJson;
    configJson >> myJson;

    MainRoutine mainRoutine("Main Routine", myJson);
    ServiceHandler oneHandler(myJson);
    oneHandler.buildServiceProxy(mainRoutine);
    oneHandler.run();

    //for(auto& [key, value] : myJson["proxys"].items()) {
    //    std::cout << key << ", "<< value << std::endl;
    //}

    while (1) {
    }
}
