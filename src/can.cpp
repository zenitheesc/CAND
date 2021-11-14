#include "cand/can.hpp"

CAN::CAN(std::string name):interface_name{name} {

    int s; 
    struct sockaddr_can addr;
    struct ifreq ifr;

    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket initialization error");
        return;
    }

    interface_name.copy(ifr.ifr_name, interface_name.length(), 0);
    ioctl(s, SIOCGIFINDEX, &ifr);

    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Binding error");
        return;
    }

    file_descriptor = s;

}
        
int CAN::socket_write (struct can_frame *frame) {

    if (write(file_descriptor, frame, sizeof(struct can_frame)) < 0) {
        perror("Write");
        return 1;
    }

    return 0;

}
CAN& operator<<(CAN& can,struct can_frame *frame) {

    int nbytes;

    nbytes = read(can.file_descriptor, frame, sizeof(struct can_frame));

    if (nbytes < 0) {
        perror("Read error");
        return can;
    }	

    if (nbytes < sizeof(struct can_frame)) {
        fprintf(stderr, "read: incomplete CAN frame\n");
        // callbacks
        return can;
    }
    return can;

}

int CAN::socket_close () {

    if (close(file_descriptor) < 0) {
        perror("Closing error"); // exceptions
        return 1;
    }

    return 0;

}

std::ostream& operator<<(std::ostream& os,struct can_frame& frame){
    os<<std::hex<<std::showbase<<(int)frame.can_id<<" [";
    os<<std::dec<<(int)frame.can_dlc<<"]";
    os<<std::noshowbase;
    for (int i = 0; i < frame.can_dlc; i++) {
        os<<std::uppercase<<std::hex<<" "<<std::setw(2)<< (int)frame.data[i];
    }
    os<<std::nouppercase;

    os<<std::endl;
    return os;
}