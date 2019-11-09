#include <iostream>
#include <map>
#include <atomic>
#include <functional>
#include <vector>
#include <string>
#include <csignal>
#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>
#include <L298NHBridge.hpp>
#include <util.hpp>

using boost::asio::ip::tcp;

#define SEND(sck, ptr, n, err)  boost::asio::write(sck, boost::asio::buffer(ptr, n), err); \
                                if (err) { \
                                    std::cout << err.message() << std::endl; \
                                }

#define RECV(sck, ptr, n, err)  boost::asio::read(sck, boost::asio::buffer(ptr, n), err); \
                                if (err) { \
                                    std::cout << err.message() << std::endl; \
                                }

#define PORT        8225

#define FPS         10

// frame size
#define WIDTH       320
#define HEIGHT      240

// rotation speed
#define R_SPEED     0.6
// driving speed
#define D_SPEED     0.8

static void signal_handler(int signum) {
    exit(1);
}

int main(int argc, const char *argv[]) {

    const std::vector<std::string> args(argv, argv + argc);
    const int port = args.size() >= 2 ? util::strto<int>(args[1]) : PORT;

    L298NHBridge bridge(20, 6, 13, 19, 26, 21);

    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        std::cout << "cannot setup signal handler" << std::endl;
        exit(1);
    }

    // setup boost socket
    boost::asio::io_service io_service;
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));
    tcp::socket socket(io_service);

    // connect to client
    std::cout << "listening on port " << port << std::endl;
    std::cout << "waiting for connection..." << std::endl;
    try {
        acceptor.accept(socket);
    } catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
        std::cout << "connection failed" << std::endl;
        exit(1);
    }
    std::cout << "connection established" << std::endl;

    // open camera
    cv::VideoCapture camera(0);
    if (!camera.isOpened()) {
        std::cout << "unable to access camera" << std::endl;
        socket.close();
        exit(1);
    }

    // set camera properties
    camera.set(cv::CAP_PROP_FRAME_WIDTH, WIDTH);
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, HEIGHT);
    camera.set(cv::CAP_PROP_FPS, FPS);

    std::cout << "frame_size=(" << camera.get(cv::CAP_PROP_FRAME_WIDTH) << ", "
                << camera.get(cv::CAP_PROP_FRAME_HEIGHT)
                << "), FPS=" << camera.get(cv::CAP_PROP_FPS) << std::endl;

    // map keyboard inputs to actions for host
	const std::map<char, std::function<void (void)>> actions = {
            { 'q', [&]{ bridge.stop_motors(); }},
            { 'w', [&]{ bridge.set_motors(-D_SPEED, D_SPEED); }},
            { 's', [&]{ bridge.set_motors(D_SPEED, -D_SPEED); }},
            { 'a', [&]{ bridge.set_motors(R_SPEED, R_SPEED); }},
            { 'd', [&]{ bridge.set_motors(-R_SPEED, -R_SPEED); }}
	};

    bool terminated = false;
    char c = 0x00;
    cv::Mat frame(WIDTH, HEIGHT, CV_8SC3);
    std::vector<unsigned char> buffer(WIDTH * HEIGHT * 3);
    boost::system::error_code error;
    bool camera_enabled = true;

    // main control loop
	do {
	    // read controls if available
	    if (socket.available()) {
            RECV(socket, &c, 1, error);
            if (error) {
                std::cout << "unable to read data from socket" << std::endl;
                break;
            } else {
                if (c == 'x') {
                    std::cout << "connection terminated by peer" << std::endl;
                    terminated = true;
                    break;
                } else if (c == 'c') {
                    camera_enabled = !camera_enabled;
                    std::cout << "camera turned " << (camera_enabled ? "ON" : "OFF") << std::endl;
                } else {
                    const auto it = actions.find(c);
                    if (it != actions.end()) {
                        const auto &func = it->second;
                        func();
                    } else {
                        std::cout << "unrecognized action \'" << c << '\'' << std::endl;
                    }
                }
            }
	    }

	    if (!camera.read(frame)) {
	        std::cout << "unable to acquire camera image" << std::endl;
	        break;
	    }

	    if (camera_enabled) {
            cv::imencode(".jpeg", frame, buffer);

            const uint32_t n = htonl((uint32_t) buffer.size());
            SEND(socket, &n, sizeof(n), error);
            if (error) {
                break;
            }
            SEND(socket, buffer.data(), buffer.size(), error);
            if (error) {
                break;
            }
        }
	} while (true);
    std::cout << "connection closed" << std::endl;

    if (socket.is_open()) {
        socket.close();
    }

	return EXIT_SUCCESS;
}

