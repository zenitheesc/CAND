#include "cand/can.hpp"

int main () {

    struct can_frame frame;
    std::string can_name = "vcan0";

    CAN can(can_name);

    while (1) {

        can<<(&frame);
        std::cout<<can;

    }

    can.socket_close();

    return 0;

}