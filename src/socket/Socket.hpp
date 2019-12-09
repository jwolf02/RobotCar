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
        static_assert(std::is_fundamental<T>::value || std::is_arithmetic<T>::value, "can only send basic types directly");
        if (std::is_integral<T>::value) {
            const auto _x = inet_bswap(x);
            return send(&_x, sizeof(T));
        } else if (std::is_floating_point<T>::value || std::is_same<T, bool>::value) {
            return send((const void *) &x, sizeof(x));
        }
    }

    size_t recv(void *buffer, size_t len);

    template <typename T>
    size_t recv(T &x) {
        static_assert(std::is_fundamental<T>::value || std::is_arithmetic<T>::value, "can only receive basic types directly");
        if (std::is_integral<T>::value) {
            auto s = recv(&x, sizeof(T));
            x = inet_bswap(x);
            return s;
        } else if (std::is_floating_point<T>::value || std::is_same<T, bool>::value) {
            return recv((void *) &x, sizeof(x));
        }
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

Socket& operator<<(Socket &socket, const std::string &x) {
    const uint32_t s = x.size();
    socket.send<uint32_t>(s);
    socket.send((const void *) x.data(), x.size());
    return socket;
}

Socket& operator>>(Socket &socket, std::string &x) {
    uint32_t s = 0;
    socket.recv<uint32_t>(s);
    x.resize(s);
    socket.recv(const_cast<char*>(x.data()), s);
    return socket;
}

#endif // __MY_SOCKET_HPP
