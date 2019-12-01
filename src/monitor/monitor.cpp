#include <monitor.hpp>
#include <boost/asio.hpp>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/dnn.hpp>

using boost::asio::ip::tcp;

#define SEND(sck, ptr, n, err)  if (sck.is_open()) boost::asio::write(sck, boost::asio::const_buffer(ptr, n), err)

#define RECV(sck, ptr, n, err)  if (sck.is_open()) boost::asio::read(sck, boost::asio::buffer(ptr, n), err)

static cv::Mat frame;
static std::thread t;
MonitorWindow *monitor::window;
static std::atomic_char control;
static std::atomic_bool terminate;
static boost::asio::io_service io_service;
static tcp::socket sck(io_service);
static bool connected = false;

// monitor thread function
static void transceiver() {
    using namespace monitor;
    boost::system::error_code err;
    uint32_t n = 0;
    cv::Mat tmp, scaled;
    std::vector<unsigned char> buffer(640 * 480 * 3);
    control = 0x00;
    int i = 0;

    while (!terminate) {
        const std::chrono::system_clock::time_point begin = std::chrono::system_clock::now();
        // if user has entered a control command
        // send it to host
        const char c = control.exchange(0x00);
        if (c != 0x00) {
            SEND(sck, &c, 1, err);
            if (err) {
                window->setMessage(err.message());
            }
        }

        // read image from network if camera enabled
        if (window->cameraEnabled()) {
            // read size of compressed image from frame
            RECV(sck, &n, sizeof(n), err);
            if (err) {
                window->setMessage(err.message());
            }
            n = ntohl(n);

            // read image from network
            buffer.reserve(n);
            RECV(sck, buffer.data(), n, err);
            if (err) {
                window->setMessage(err.message());
            }

            cv::imdecode(buffer, cv::IMREAD_COLOR, &tmp);
            window->setFrameSize(tmp.size[1], tmp.size[0]);
            cv::resize(tmp, scaled, cv::Size(640, 480));
            cv::cvtColor(scaled, frame, cv::COLOR_RGB2BGR);
            window->setFrame(frame);
        }

        const std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        const uint64_t elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

        if (i >= 5 && window->cameraEnabled()) {
            window->setFPS(static_cast<int>(UINT64_C(1000) / elapsed_time));
            const int data_rate = static_cast<unsigned int>(double(n) / double(elapsed_time) * 1000.0);
            window->setDataRate(data_rate);
            window->setPing((n * UINT32_C(1000)) / data_rate);
            i = 0;
        } else {
            i++;
        }
    }
}

bool monitor::connect(const std::string &address, int port) {
    if (!connected) {
        try {
            sck.connect(tcp::endpoint(boost::asio::ip::address::from_string(address), port));
        } catch (std::exception &e) {
            window->setMessage(e.what());
            return false;
        }
        connected = true;
        return true;
    } else {
        return false;
    }
}

void monitor::start_transceiver() {
    terminate = false;
    control = 0x00;
    t = std::thread(transceiver);
}

void monitor::send_control(char ctl) {
    control = ctl;
}

void monitor::disconnect() {
    if (connected) {
        if (sck.is_open())
            sck.close();
        terminate = true;
        t.join();
    }
    connected = false;
}
