#ifndef __CONFIG_HPP
#define __CONFIG_HPP

#include <string>

namespace config {

    void load(const std::string &file);

    void set(const std::string &key, const std::string &value);

    std::string get(const std::string &key);

    void remove(const std::string &key);

}

#endif // __CONFIG_HPP
