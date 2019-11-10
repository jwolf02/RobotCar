#ifndef __MONITOR_HPP
#define __MONITOR_HPP

#include <string>
#include <MonitorWindow.hpp>

namespace monitor {

    extern MonitorWindow *window;

    bool connect(const std::string &address, int port);

    bool is_connected();

    void start_transceiver();

    void send_control(char ctl);

    void disconnect();

}

#endif // __MONITOR_HPP
