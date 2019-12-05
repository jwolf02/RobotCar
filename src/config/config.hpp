#ifndef __CONFIG_HPP
#define __CONFIG_HPP

#include <string>
#include <vector>
#include <common.hpp>

namespace config {

    /***
     * load config from file, line must look like KEY=value, whitespace
     * around key and value is trimmed away, lines starting with comment are ignored
     * @param fname
     * @param comment
     */
    void load(const std::string &fname, char comment='#');

    /***
     * parse command line arguments and save them to config
     * it behaves similar to load, but also trims key from '-'
     * @param args
     */
    void parse(const std::vector<std::string> &args);

    inline void parse(int argc, const char **argv) {
        parse(std::vector<std::string>(argv, argv + argc));
    }

    /***
     * checks if config contains key, ignoring case
     * @param key
     * @return
     */
    bool contains(const std::string &key);

    /***
     * override key value pair
     * @param key
     * @param value
     */
    void set(const std::string &key, const std::string &value="");

    template <typename T>
    inline void set(const std::string &key, const T &value) {
        set(key, std::to_string(value));
    }

    /**
     * get value from config
     * @param key
     * @return
     */
    std::string get(const std::string &key);

    /***
     * auto convert value to specicfied type, type must be
     * convertable by string::strto
     * @tparam T
     * @param key
     * @return
     */
    template <typename T>
    inline T get_as(const std::string &key) {
        return string::to<T>(get(key));
    }

    template <>
    inline std::string get_as<std::string>(const std::string &key) {
        return get(key);
    }

    /***
     * overload of get_as but also returns default value if key is not in config
     * @tparam T
     * @param key
     * @param default_value
     * @return
     */
    template <typename T>
    inline T get_or_default(const std::string &key, const T &default_value) {
        return contains(key) ? get_as<T>(key) : default_value;
    }

    template <>
    inline std::string get_or_default<std::string>(const std::string &key, const std::string &default_value) {
        return contains(key) ? get(key) : default_value;
    }

    /***
     * get list of all keys in config
     * @return
     */
    std::vector<std::string> get_keys();

    /***
     * remove key value pair from config
     * @param key
     */
    void remove(const std::string &key);

}

#endif // __CONFIG_HPP
