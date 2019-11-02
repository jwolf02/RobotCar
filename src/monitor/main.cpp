#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>
#include <Clock.hpp>

using boost::asio::ip::tcp;

#define SEND(sck, ptr, n, err)  boost::asio::write(sck, boost::asio::buffer(ptr, n), err); \
                                if (err) { \
                                    std::cout << err.message() << std::endl; \
                                }

#define RECV(sck, ptr, n, err)  boost::asio::read(sck, boost::asio::buffer(ptr, n), err); \
                                if (err) { \
                                    std::cout << err.message() << std::endl; \
                                }

#define PORT                    8225

int main(int argc, const char *argv[]) {

    const std::vector<std::string> args(argv, argv + argc);
	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " <address>" << std::endl;
		return 1;
	}

    const std::string addr = argv[argc - 1];

	boost::asio::io_service io_service;

	tcp::socket socket(io_service);

    std::cout << "connecting to " << addr << ":" << PORT << std::endl;
    try {
        socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(argv[1]), PORT));
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        exit(1);
    }
    std::cout << "connection established" << std::endl;

    cv::namedWindow("monitor", cv::WINDOW_AUTOSIZE);

    cv::Mat frame;
    std::vector<unsigned char> buffer(256 * 256 * 3);
    int i = 0;
    do {
        Clock clock;
        clock.start();
        boost::system::error_code error;

        // read size of encoded image from socket
        uint32_t n;
        RECV(socket, &n, sizeof(n), error);
        if (error) {
            exit(1);
        }
        buffer.reserve(n);

        // read encoded image from socket
        RECV(socket, buffer.data(), n, error);
        if (error) {
            exit(1);
        }

        // decode and display frame
        cv::imdecode(buffer, cv::IMREAD_COLOR, &frame);
        cv::Mat scaled_frame;
        cv::resize(frame, scaled_frame, cv::Size(), 2, 2);

        cv::imshow("monitor", scaled_frame);

        // read control character from keyboard
        char key = (char) cv::waitKey(1) & 0xff;
        if (key != -1) {
            SEND(socket, &key, 1, error);
            if (error) {
                exit(1);
            }

            if (key == 'x')
                break;
        }
        clock.stop();
        if (i == 5) {
            std::cout << '\r' << (int) ((1.0 / clock.milliseconds()) * 1000.0) << " frames/s, "
                      << (int) ((double(n) / clock.milliseconds())) << " KB/s      " << std::flush;
            i = 0;
        } else {
            ++i;
        }
    } while (true);

    socket.close();
    std::cout << std::endl << "connection closed" << std::endl;

	return 0;
}
