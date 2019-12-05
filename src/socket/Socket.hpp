#ifndef __MY_SOCKET_HPP
#define __MY_SOCKET_HPP

#include <boost/asio.hpp>
#include <type_traits>
#include <common.hpp>

using boost::asio::ip::tcp;

class Socket {
public:
    enum {
        CLIENT = 0,
        SERVER = 1
    };

    enum protocol_t {
        IPv4 = 0,
        IPv6 = 1
    };

    typedef boost::system::error_code   error_code;

    static Socket* accept(protocol_t protocol, unsigned int port);

    static Socket* accept(boost::asio::io_service &service, protocol_t protocol, unsigned int port);

    static Socket* connect(const std::string &address, unsigned int port);

    static Socket* connect(boost::asio::io_service &service, const std::string &address, unsigned int port);

    ~Socket();

    bool isOpen() const;

    size_t send(const void *buffer, size_t len);

    template <typename T>
    size_t send(const T &x) {
        static_assert(std::is_integral<T>::value, "can only send integer types directly");
        const auto _x = inet_bswap(x);
        return send(&_x, sizeof(T));
    }

    template <>
    size_t send<float>(const float &x) {
        return send(&x, sizeof(x));
    }

    template <>
    size_t send<double>(const double &x) {
        return send(&x, sizeof(x));
    }

    template <>
    size_t send<std::string>(const std::string &x) {
        const uint32_t str_size = x.size();
        return send(str_size) + send((const void *) x.data(), x.size());
    }

    size_t recv(void *buffer, size_t len);

    template <typename T>
    size_t recv(T &x) {
        static_assert(std::is_integral<T>::value, "can only receive integer types directly");
        auto s = recv(&x, sizeof(T));
        x = inet_bswap(x);
        return s;
    }

    template <>
    size_t recv<float>(float &x) {
        return recv(&x, sizeof(x));
    }

    template <>
    size_t recv<double>(double &x) {
        return recv(&x, sizeof(x));
    }

    template <>
    size_t recv<std::string>(std::string &x) {
        uint32_t str_size;
        size_t c = recv(&str_size, sizeof(str_size));
        x.resize(str_size);
        return c + recv((void *) x.data(), str_size);
    }

    void close();

    size_t available() const;

    int type() const;

    protocol_t protocol() const;

private:

    Socket() = default;

    boost::asio::io_service *_io_service = nullptr;

    tcp::acceptor *_acceptor = nullptr;

    tcp::socket *_socket = nullptr;

    protocol_t _protocol = IPv4;

    int _type = -1;

};

template <typename T>
Socket& operator<<(Socket &socket, const T &x) {
    socket.send<T>(x);
    return socket;
}

template <typename T>
Socket& operator>>(Socket &socket, T &x) {
    socket.recv<T>(x);
    return socket;
}

#endif // __MY_SOCKET_HPP
