#ifndef __WORKER_H
#define __WORKER_H

#include <string>
#include <monitorwindow.h>

namespace worker {

    extern MonitorWindow *window;

    bool connect(const std::string &address, int port);

    void run();

    void send_control(char ctl);

    void disconnect();

}

#endif // __WORKER_H
