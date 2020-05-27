#include <VideoReceiver.hpp>

VideoReceiver::VideoReceiver(const std::string &host, int port) {
    _socket = Socket::connect(host, port);
}

VideoReceiver::~VideoReceiver() {
    close();
}

VideoReceiver &VideoReceiver::operator>>(cv::Mat &frame) {
    read(frame);
    return *this;
}

bool VideoReceiver::read(cv::Mat &frame) {
    uint32_t n = 0;
    if (_socket.read(&n, sizeof(n)) != sizeof(n)) {
        return false;
    }
    n = ntohl(n);
    _buffer.resize(n);
    try {
        if (_socket.read(_buffer.data(), n) != n) {
            return false;
        }
        cv::imdecode(_buffer, cv::IMREAD_COLOR, &frame);
        return true;
    } catch (std::exception &ex) {
        // peer has been disconnected
        close();
    }
    return false;
}

void VideoReceiver::close() {
    _socket.close();
}

bool VideoReceiver::isConnected() const {
    return _socket.is_open();
}
