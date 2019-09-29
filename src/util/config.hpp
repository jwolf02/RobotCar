#ifndef __CONFIG_HPP
#define __CONFIG_HPP

#include <string>
#include <util.hpp>

namespace config {

    extern const std::string DEFAULT_PATH;

    void load(const std::string &file, char comment='#');

    bool contains(const std::string &key);

    void set(const std::string &key, const std::string &value);

    std::string get(const std::string &key);

    template <typename T>
    inline T get_as(const std::string &key) {
        return util::strto<T>(get(key));
    }

    template <>
    inline std::string get_as<std::string>(const std::string &key) {
        return get(key);
    }

    template <typename T>
    inline T get_or_default(const std::string &key, T default_value) {
        return contains(key) ? get_as<T>(key) : default_value;
    }

    void remove(const std::string &key);

}

#endif // __CONFIG_HPP
