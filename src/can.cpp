#include "cand/can.hpp"

CAN::CAN(std::string name):m_interfaceName{name} {

    int socketCan; 
    struct sockaddr_can addr;
    struct ifreq ifr;

    if ((socketCan = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        throw std::runtime_error("Socket initialization error");
    }

    m_interfaceName.copy(ifr.ifr_name, m_interfaceName.length(), 0);
    ioctl(socketCan, SIOCGIFINDEX, &ifr);

    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(socketCan, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        throw std::runtime_error("Binding error");
    }

    m_fileDescriptor = socketCan;

}
struct can_frame CAN::getCanFrame(){
    return m_frame;
}
int CAN::getFileDescriptor(){
    return m_fileDescriptor;
}
void CAN::setCanFrame(struct can_frame frame){
    this->m_frame=frame;
}
        
void CAN::socket_write (struct can_frame& frame) {

    int vl = write(m_fileDescriptor, &frame, sizeof(struct can_frame));
    if(vl<0){
        throw std::runtime_error("Write error");
    }

}
CAN& operator<<(CAN& can,struct can_frame& frame) {

    int nbytes;
    nbytes = read(can.getFileDescriptor(), &frame, sizeof(struct can_frame));
    if (nbytes < 0) {
        throw std::runtime_error("Read error");
    }	

    if (nbytes < sizeof(struct can_frame)) {
        throw std::runtime_error("Read: Incomplete CAN frame");
    }

    can.setCanFrame(frame);

    return can;

}

void CAN::socket_close () {

    if(close(m_fileDescriptor)<0){
        throw std::runtime_error("Closing error");
    }

}

std::ostream& operator<<(std::ostream& os,CAN& can){
    os<<std::hex<<std::showbase<<(int)can.getCanFrame().can_id<<" [";
    os<<std::dec<<(int)can.getCanFrame().can_dlc<<"]";
    os<<std::noshowbase;
    for (int i = 0; i < can.getCanFrame().can_dlc; i++) {
        os<<std::uppercase<<std::hex<<" "<<std::setw(2)<< (int)can.getCanFrame().data[i];
    }
    os<<std::nouppercase;

    os<<std::endl;
    return os;
}