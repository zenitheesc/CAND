#include "cand/can.hpp"

int main () {

    struct can_frame frame;

    CAN can("vcan0");

    while (1) {

        can>>frame;
        std::cout<<frame;

    }

    return 0;

}