#include <VideoStreamer.hpp>
#include <ros/

VideoStreamer::VideoStreamer(int port) {
    _server = ServerSocket(port);
}

void VideoStreamer::waitForConnection() {
    _socket = _server.wait_for_connection();
}

VideoStreamer::~VideoStreamer() {
    close();
}

VideoStreamer &VideoStreamer::operator<<(const cv::Mat &frame) {
    write(frame);
    return *this;
}

bool VideoStreamer::write(const cv::Mat &frame) {
    CV_Assert(!frame.empty());
    if (!_socket.is_open()) {
        return false;
    }
    uint32_t n = 0;
    cv::imencode(".jpeg", frame, _buffer);
    n = htonl((uint32_t) _buffer.size());
    return !(_socket.write(&n, sizeof(n)) != sizeof(n) ||
             _socket.write(_buffer.data(), _buffer.size()) != _buffer.size());
}

void VideoStreamer::close() {
    _socket.close();
    _server.close();
}

bool VideoStreamer::isConnected() const {
    return _socket.is_open();
}
