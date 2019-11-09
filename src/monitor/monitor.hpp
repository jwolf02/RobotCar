#ifndef __MONITOR_HPP
#define __MONITOR_HPP

#include <string>
#include <MonitorWindow.hpp>

namespace monitor {

    extern MonitorWindow *window;

    bool connect(const std::string &address, int port);

    void run();

    void send_control(char ctl);

    void disconnect();

}

#endif // __MONITOR_HPP
