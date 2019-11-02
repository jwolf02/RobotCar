#include <ServerSocket.hpp>
#include <util.hpp>
#include <controller.hpp>
#include <iostream>
#include <map>
#include <thread>
#include <Clock.hpp>
#include <opencv2/opencv.hpp>
#include <atomic>
#include <boost/asio.hpp>

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

#define WIDTH       320
#define HEIGHT      240

int main(int argc, const char *argv[]) {

    const std::vector<std::string> args(argv, argv + argc);

    controller::setup();

    // setup boost socket
    boost::asio::io_service io_service;
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), PORT));
    tcp::socket socket(io_service);

    // connect to client
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

    // map keyboard inputs to actions for host
	const std::map<char, unsigned int> actions = {
            { 'q', controller::STOP },
            { 'w', controller::DRIVE_FORWARD },
            { 's', controller::DRIVE_BACKWARD },
            { 'a', controller::ROTATE_LEFT },
            { 'd', controller::ROTATE_RIGHT }
	};

    bool terminated = false;
    char c = 0x00;
    cv::Mat frame(WIDTH, HEIGHT, CV_8SC3);
    std::vector<unsigned char> buffer(WIDTH * HEIGHT * 3);
    boost::system::error_code error;

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
                } else {
                    const auto it = actions.find(c);
                    if (it != actions.end()) {
                        controller::action(it->second);
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

	    cv::imencode(".jpeg", frame, buffer);

	    const uint32_t n = buffer.size();
	    SEND(socket, &n, sizeof(n), error);
	    if (error) {
	        break;
	    }
	    SEND(socket, buffer.data(), buffer.size(), error);
	    if (error) {
	        break;
	    }
	} while (true);
    std::cout << "connection closed" << std::endl;

    if (terminated)
        socket.close();
    controller::cleanup();

	return EXIT_SUCCESS;
}
