#include <iostream>
#include <iomanip> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#ifndef LINUX_CAN_H
#define LINUX_CAN_H

class CAN{
    private:

        int file_descriptor;
        struct can_frame frame;
        std::string interface_name;

        friend CAN& operator<<(CAN&, struct can_frame *);


    public:
        CAN(std::string name);

        int socket_write(struct can_frame *);

        int socket_close();

};
std::ostream& operator<<(std::ostream&,struct can_frame&);

#endif 