#ifndef __SOCKET_HPP
#define __SOCKET_HPP

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Socket {
public:

    constexpr static int SERVER = 0;
    constexpr static int CLIENT = 1;

    constexpr static int IPv4 = 0;
    constexpr static int IPv6 = 1;

    Socket() = default;

    Socket(const std::string &address, unsigned int port) {
        sck_type = CLIENT;
        socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(address), port));
    }

    Socket(int protocol, unsigned int port) {
        sck_type = SERVER;
        switch (protocol) {
            case IPv4: {
                acceptor = tcp::acceptor(io_service, tcp::endpoint(tcp::v4(), port));
                break;
            } case IPv6: {
                acceptor = tcp::acceptor(io_service, tcp::endpoint(tcp::v6(), port));
                break;
            }
            default:
                throw std::runtime_error("invalid protocol");
        }
        try {
            acceptor.accept(socket);
        } catch (std::exception &ex) {
            throw std::runtime_error(ex.what());
        }
    }

    ~Socket() {
        close();
    }

    bool isOpen() const {
        return socket.is_open();
    }

    void send(const void *buf, size_t n) {
        socket.send(boost::asio::const_buffer(buf, n));
    }

    void recv(void *buf, size_t n) {
        socket.receive(boost::asio::buffer(buf, n));
    }

    size_t available() const {
        return socket.available();
    }

    void close() {
        if (isOpen()) {
            socket.close();
        }
    }

    int type() const {
        return sck_type;
    }

private:

    boost::asio::io_service io_service;

    tcp::socket socket = tcp::socket(io_service);

    tcp::acceptor acceptor = tcp::acceptor(io_service);

    int sck_type = SERVER;

};

#endif // __SOCKET_HPP
