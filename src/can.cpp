#include "cand/can.hpp"

CAN::CAN(const std::string& name)
    : m_interfaceName { name }
{

    int socketCan;
    struct sockaddr_can addr;
    struct ifreq ifr;

    if ((socketCan = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        throw std::runtime_error("Socket initialization error: " + std::string(strerror(errno)));
    }

    strncpy(ifr.ifr_name, m_interfaceName.c_str(), sizeof(ifr.ifr_name));
    ioctl(socketCan, SIOCGIFINDEX, &ifr);

    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(socketCan, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        throw std::runtime_error("Binding error: " + std::string(strerror(errno)));
    }

    m_fileDescriptor = socketCan;
}

CAN::~CAN()
{
    if (close(m_fileDescriptor) < 0) {
        throw std::runtime_error("Closing error");
    }
}

auto CAN::getFileDescriptor() const -> int
{
    return m_fileDescriptor;
}

void CAN::socketWrite(const struct can_frame& frame)
{
    if (write(m_fileDescriptor, &frame, sizeof(struct can_frame)) < 0) {
        throw std::runtime_error("Write error: " + std::string(strerror(errno)));
    }
}

void CAN::setFilter(const struct can_filter& filter)
{
    if (setsockopt(m_fileDescriptor, SOL_CAN_RAW, CAN_RAW_FILTER, &filter, sizeof(filter)) < 0) {
        setsockopt(m_fileDescriptor, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
        throw std::runtime_error("Filter error: " + std::string(strerror(errno)));
    }
}

const struct can_frame& operator>>(CAN& can, struct can_frame& frame)
{

    int nbytes;
    nbytes = read(can.getFileDescriptor(), &frame, sizeof(struct can_frame));
    if (nbytes < 0) {
        throw std::runtime_error("Read error");
    }

    if (nbytes < sizeof(struct can_frame)) {
        throw std::runtime_error("Read: Incomplete CAN frame");
    }

    return frame;
}

const CAN& operator<<(CAN& can, struct can_frame& frame)
{
    can.socketWrite(frame);
    return can;
}

std::ostream& operator<<(std::ostream& os, struct can_frame& frame)
{
    os << std::hex << std::showbase << (int)frame.can_id << " [";
    os << std::dec << (int)frame.can_dlc << "]";
    os << std::noshowbase;
    for (int i = 0; i < frame.can_dlc; i++) {
        os << std::uppercase << std::hex << " " << std::setw(2) << (int)frame.data[i];
    }
    os << std::nouppercase;

    os << std::endl;
    return os;
}