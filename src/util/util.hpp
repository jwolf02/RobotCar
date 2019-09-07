#ifndef __UTIL_HPP
#define __UTIL_HPP

#include <vector>
#include <string>
#include <sstream>
#include <type_traits>

namespace util {

    /***
     * split string by the specified delimiter
     * @param str the string to split
     * @param delim the delimiter, by default a single space
     * @return a vector of strings
     */
    inline std::vector<std::string> split(const std::string &str, const std::string &delim=" ") {
        std::vector<std::string> tokens;
        size_t last = 0;
        size_t next = 0;
        while ((next = str.find(delim, last)) != std::string::npos) {
            tokens.emplace_back(str.substr(last, next - last));
            last = next + 1;
        }
        tokens.emplace_back(str.substr(last));
        return tokens;
    }

    /***
     * case string to arithmetic type specified by the template argument
     * @tparam T type to case to
     * @param str string to be casted
     * @return arithmetic type
     */
    template <typename T>
    inline T strto(const std::string &str) {
        static_assert(std::is_fundamental<T>::value && std::is_arithmetic<T>::value, "cannot convert string to non-arithmetic type");

        // signed types
        if (std::is_same<T, char>::value || std::is_same<T, short>::value || std::is_same<T, int>::value ||
            std::is_same<T, long>::value || std::is_same<T, long long>::value) {
            return (T) strtoll(str.c_str(), nullptr, 10);
        } // unsigned types
        else if (std::is_same<T, unsigned char>::value || std::is_same<T, unsigned short>::value || std::is_same<T, unsigned int>::value ||
                 std::is_same<T, unsigned long>::value || std::is_same<T, unsigned long long>::value) {
            return (T) strtoull(str.c_str(), nullptr, 10);
        } // double
        else if (std::is_same<T, double>::value) {
            return strtod(str.c_str(), nullptr);
        } // float
        else if (std::is_same<T, float>::value) {
            return strtof(str.c_str(), nullptr);
        } // long double
        else if (std::is_same<T, long double>::value) {
            return strtold(str.c_str(), nullptr);
        } else {
            throw std::domain_error("cannot convert string to specified type");
        }
    }

    /***
     * remove leading and trailing whitespace characters (can be any characters) from string
     * @param str
     * @param whitespace
     * @return
     */
    inline std::string trim(const std::string &str, const std::string &whitespace=" \t") {
        const size_t begin = str.find_first_not_of(whitespace);
        const size_t end = str.find_last_not_of(whitespace);
        return str.substr(begin, end - begin + 1);
    }

    template <typename T>
    inline constexpr bool is_integer_type() {
        return std::is_same<T, char>::value || std::is_same<T, unsigned char>::value ||
        std::is_same<T, short>::value || std::is_same<T, unsigned short>::value ||
        std::is_same<T, int>::value || std::is_same<T, unsigned int>::value ||
        std::is_same<T, long>::value || std::is_same<T, unsigned long>::value ||
        std::is_same<T, long long>::value || std::is_same<T, unsigned long long>::value;
    }

    template <typename T>
    inline constexpr bool is_floating_point_type() {
        return std::is_same<T, long double>::value || std::is_same<T, double>::value || std::is_same<T, float>::value;
    }

    template <typename T>
    inline std::pair<T, char> convert_unit(T value) {
        if (value >= 1000000000) {
            return std::make_pair(value / 1000000000, 'G');
        } else if (value >= 1000000) {
            return std::make_pair(value / 1000000, 'M');
        } else if (value >= 1000) {
            return std::make_pair(value / 1000, 'K');
        }

        if (is_floating_point_type<T>() && value < 1.0) {
            if (value >= 0.001) {
                return std::make_pair(value * 1000, 'm');
            } else if (value >= 0.000001) {
                return std::make_pair(value * 1000000, 'u');
            } else {
                return std::make_pair(value * 1000000000, 'n');
            }
        }

        return std::make_pair(value, ' ');
    }
}

#endif // __UTIL_HPP
