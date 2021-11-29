#pragma once
#include <iostream>
#include <iomanip> 
#include <cstring>
#include <string>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

class CAN{
    private:

        int m_fileDescriptor;
        struct can_frame m_frame;
        std::string m_interfaceName;

    public:
        CAN(std::string name);

        void socket_write(struct can_frame&);

        void socket_close();

        struct can_frame getCanFrame();
        int getFileDescriptor();
        void setCanFrame(struct can_frame frame);

};

CAN& operator<<(CAN&, struct can_frame&);
std::ostream& operator<<(std::ostream&,CAN&);