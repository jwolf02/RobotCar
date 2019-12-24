#include <VideoStreamer.hpp>
#include <boost/asio.hpp>
#include <opencv2/core.hpp>
#include <fstream>

#define BUFFER_SIZE         (1024)

VideoStreamer::VideoStreamer(int fourcc, double fps, const cv::Size &framesize, int port, bool isColor) {
    _writer.open("", fourcc, fps, framesize, isColor);
    _thread = std::thread(&VideoStreamer::send_to_receiver, this);
}

VideoStreamer::~VideoStreamer() {
    _flag = false;
    _thread.join();
}

void VideoStreamer::send_to_receiver() {

    std::vector<uint8_t> buffer(BUFFER_SIZE);

    while (_flag) {
        _ipipe.read((char *) buffer.data(), BUFFER_SIZE);
        _socket->send(buffer, buffer.size());
    }
}

VideoStreamer& VideoStreamer::operator<<(const cv::Mat &frame) {
    write(frame);
}

bool VideoStreamer::write(const cv::Mat &frame) {
    _writer.write(frame);
}

bool VideoStreamer::isOpened() const {
    return _writer.isOpened();
}

bool VideoStreamer::isConnected() const {
    return _connected;
}