#include <worker.h>
#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>

using boost::asio::ip::tcp;

#define SEND(sck, ptr, n, err)  boost::asio::write(sck, boost::asio::buffer(ptr, n), err)

#define RECV(sck, ptr, n, err)  boost::asio::read(sck, boost::asio::buffer(ptr, n), err)

static cv::Mat frame;
static std::thread t;
MonitorWindow *worker::window;
static std::atomic_char control;
static std::atomic_bool terminate;
static boost::asio::io_service io_service;
static tcp::socket sck(io_service);

// worker thread function
static void func() {
    boost::system::error_code err;
    uint32_t n = 0;
    cv::Mat tmp, scaled;
    std::vector<unsigned char> buffer(640 * 480 * 3);
    MonitorWindow *window = worker::window;
    control = 0x00;
    double fps = 0;
    double data_rate = 0;
    int i = 0;

    while (!terminate) {
        const std::chrono::system_clock::time_point begin = std::chrono::system_clock::now();
        std::chrono::system_clock::time_point end;
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
            cv::cvtColor(scaled, frame, CV_BGR2RGB);
            window->setFrame(frame);
        }

        end = std::chrono::system_clock::now();
        const uint64_t elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

        if (i == 5) {
            window->setFPS(1000 / elapsed_time);
            window->setDataRate(static_cast<unsigned int>(double(n) / double(elapsed_time) * 1000.0));
            i = 0;
        } else {
            i++;
        }
    }
}

bool worker::connect(const std::string &address, int port)
{
    try {
        sck.connect(tcp::endpoint(boost::asio::ip::address::from_string(address), port));
    } catch (std::exception &e) {
        window->setMessage(e.what());
        return false;
    }
    return true;
}

void worker::run()
{
    terminate = false;
    control = 0x00;
    t = std::thread(func);
}

void worker::send_control(char ctl)
{
    control = ctl;
}

void worker::disconnect()
{
    send_control('x');
    sck.close();
    terminate = true;
    t.join();
}
