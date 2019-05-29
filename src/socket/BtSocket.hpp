#ifndef __BTSOCKET_HPP
#define __BTSOCKET_HPP

#include <unordered_map>

namespace BtSocket {

std::unordered_map<std::string, std::string> scan_for_devices();

}

#endif // __BTSOCKET_HPP
