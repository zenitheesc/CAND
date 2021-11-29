#include "cand/can.hpp"

int main () {

    struct can_frame frame;

    std::string can_name = "vcan0";
    CAN can(can_name);

    for (__uint8_t i = 0; i < 20; i++) {

        frame.can_id = 0x500;
        frame.can_dlc = 8;

        for (__uint8_t j = 0; j < 8; j = j + 2) {
        
            frame.data[j] = j + i;
            frame.data[j + 1] = ';';
        
        }

        can.socket_write(frame);

    }

    can.socket_close();

    return 0;

}
