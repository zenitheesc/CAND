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

    try{
        int vl = write(file_descriptor, frame, sizeof(struct can_frame));
        if(vl<0){
            throw vl;
        }
    }catch(int ex){
        std::cerr<<"Write error";
        return 1;
    }

    return 0;

}
CAN& operator<<(CAN& can,struct can_frame *frame) {

    int nbytes;
    try{
        nbytes = read(can.file_descriptor, frame, sizeof(struct can_frame));
        throw nbytes;
    }catch(int ex){
        if (nbytes < 0) {
            std::cerr<<"Read error";
        }	

        if (nbytes < sizeof(struct can_frame)) {
            std::cerr<<"Read: incomplete CAN frame"<<std::endl;
        }
    }
    return can;

}

int CAN::socket_close () {

    try{
        int vl = close(file_descriptor);
        if(vl<0){
            throw vl;
        }
    }catch(int ex){
        std::cerr<<"Closing error";
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