#include "Socket.hpp"

boost::asio::io_service io_service;

Socket* Socket::accept(Socket::protocol_t protocol, unsigned int port) {
    return accept(io_service, protocol, port);
}

Socket* Socket::accept(boost::asio::io_service &service, Socket::protocol_t protocol, unsigned int port) {
    auto socket = new Socket();
    socket->_type = SERVER;
    socket->_protocol = protocol;
    socket->_io_service = &service;
    socket->_acceptor = new tcp::acceptor(service, tcp::endpoint(protocol == IPv4 ? tcp::v4() : tcp::v6(), port));
    boost::asio::socket_base::reuse_address option(true);
    socket->_acceptor->set_option(option);
    socket->_socket = new tcp::socket(service);
    try {
        socket->_acceptor->accept(*socket->_socket);
    } catch (std::exception &ex) {
        throw std::runtime_error(ex.what());
    }
    return socket;
}

Socket* Socket::connect(const std::string &address, unsigned int port) {
    return connect(io_service, address, port);
}

Socket* Socket::connect(boost::asio::io_service &service, const std::string &address, unsigned int port) {
    auto socket = new Socket();
    socket->_type = CLIENT;
    socket->_io_service = &service;
    socket->_socket = new tcp::socket(io_service);
    try {
        socket->_socket->connect(tcp::endpoint(boost::asio::ip::address::from_string(address), port));
    } catch (std::exception &ex) {
        throw std::runtime_error(ex.what());
    }
    return socket;
}

Socket::~Socket() {
    close();
}

bool Socket::isOpen() const {
    return _socket->is_open();
}

size_t Socket::send(const void *buffer, size_t len) {
    if (_socket->is_open()) {
        boost::system::error_code error;
        size_t s = boost::asio::write(*_socket, boost::asio::const_buffer(buffer, len), error);
        if (error) {
            throw std::runtime_error(error.message());
        }
        return s;
    } else {
        throw std::runtime_error("socket not opened");
    }
}

size_t Socket::recv(void *buffer, size_t len) {
    if (_socket->is_open()) {
        boost::system::error_code error;
        size_t s = boost::asio::read(*_socket, boost::asio::buffer(buffer, len), error);
        if (error) {
            throw std::runtime_error(error.message());
        }
        return s;
    } else {
        throw std::runtime_error("socket not opened");
    }
}
void Socket::close() {
    _socket->close();
    delete _acceptor;
    delete _socket;
    _io_service = nullptr;
}

size_t Socket::available() const {
    return _socket->available();
}

int Socket::type() const {
    return _type;
}

Socket::protocol_t Socket::protocol() const {
    return _protocol;
}
