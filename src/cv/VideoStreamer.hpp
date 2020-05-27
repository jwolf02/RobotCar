#ifndef __VIDEOSTREAMER_HPP
#define __VIDEOSTREAMER_HPP

#include <vector>
#include <opencv2/opencv.hpp>
#include <ServerSocket.hpp>
#include <Socket.hpp>

class VideoStreamer {
public:

    VideoStreamer() = default;

    VideoStreamer(int port);

    ~VideoStreamer();

    void waitForConnection();

    VideoStreamer& operator<<(const cv::Mat &frame);

    bool write(const cv::Mat &frame);

    void close();

    bool isConnected() const;

private:

    ServerSocket _server;

    Socket _socket;

    std::vector<unsigned char> _buffer;

};

#endif // __VIDEOSTREAMER_HPP
