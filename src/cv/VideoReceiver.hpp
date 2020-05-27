#ifndef __VIDEORECEIVER_HPP
#define __VIDEORECEIVER_HPP

#include <string>
#include <Socket.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

class VideoReceiver {
public:

    VideoReceiver() = default;

    VideoReceiver(const std::string &host, int port);

    ~VideoReceiver();

    VideoReceiver& operator>>(cv::Mat &frame);

    bool read(cv::Mat &frame);

    void close();

    bool isConnected() const;

private:

    std::string _hostname;

    Socket _socket;

    std::vector<unsigned char> _buffer;

};

#endif // __VIDEORECEIVER_HPP
