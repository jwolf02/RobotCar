#ifndef __VIDEOSTREAMER_HPP
#define __VIDEOSTREAMER_HPP

#include <thread>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>

class VideoStreamer {
public:

    VideoStreamer() = default;

    VideoStreamer(int fourcc, double fps, const cv::Size &framesize, int port, bool isColor=true);

    ~VideoStreamer();

    VideoStreamer& operator<<(const cv::Mat &frame);

    bool write(const cv::Mat &frame);

    bool isOpened() const;

    bool isConnected() const;

private:

    void send_to_receiver();

    std::thread _thread;

    cv::VideoWriter _writer;

    std::ifstream _ipipe;

    boost::asio::ip::tcp::acceptor *_acceptor = nullptr;

    boost::asio::ip::tcp::socket *_socket = nullptr;

    volatile bool _flag = false;

    volatile bool _connected = false;

};

#endif // __VIDEOSTREAMER_HPP
