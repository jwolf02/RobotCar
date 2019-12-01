#include <iostream>
#include <map>
#include <atomic>
#include <functional>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>
#include <L298NHBridge.hpp>
#include <common.hpp>
#include "../config/config.hpp"
#include <ObjectDetector.hpp>
#include <fstream>

using boost::asio::ip::tcp;
using namespace cv;
using namespace cv::dnn;

#define SEND(sck, ptr, n, err)  if (sck.is_open()) { \
                                    boost::asio::write(sck, boost::asio::buffer(ptr, n), err); \
                                    if (err) \
                                        std::cout << err.message() << std::endl; \
                                }

#define RECV(sck, ptr, n, err)  if (sck.is_open()) { \
                                    boost::asio::read(sck, boost::asio::buffer(ptr, n), err); \
                                    if (err) \
                                        std::cout << err.message() << std::endl; \
                                }

int main(int argc, const char *argv[]) {
    const std::vector<std::string> args(argv, argv + argc);
    if (args.size() > 1 && string::starts_with(args[1], "--config=")) {
        auto tokens = string::split(args[1], "=");
        if (tokens.size() >= 2) {
            try {
                config::load(tokens[1]);
            } catch (std::exception &ex) {
                std::cout << ex.what() << std::endl;
                exit(1);
            }
        }
    }
    // parse command line arguments, override config file parameters
    config::parse(args);

    // load variables from config
    const int port = config::get_as<int>("PORT");
    const int width = config::get_as<int>("WIDTH");
    const int height = config::get_as<int>("HEIGHT");
    const int r_speed = config::get_as<int>("ROTATION_SPEED");
    const int d_speed = config::get_as<int>("DRIVE_SPEED");

    const int ENA = config::get_as<int>("ENA");
    const int IN1 = config::get_as<int>("IN1");
    const int IN2 = config::get_as<int>("IN2");
    const int IN3 = config::get_as<int>("IN3");
    const int IN4 = config::get_as<int>("IN4");
    const int ENB = config::get_as<int>("ENB");

    const std::string model = config::get("MODEL");
    const std::string config = config::get("CONFIG");
    const std::string framework = config::get("FRAMEWORK");

    const int backend = config::get_as<int>("BACKEND");
    const int target = config::get_as<int>("TARGET");
    const float threshold = config::get_as<float>("THRESHOLD");
    const float nms_threshold = config::get_as<float>("NMS_THRESHOLD");

    L298NHBridge bridge(ENA, IN1, IN2, IN3, IN4, ENB);

    // open camera
    cv::VideoCapture camera(0);
    if (!camera.isOpened()) {
        std::cout << "unable to access camera" << std::endl;
        exit(1);
    }

    // set camera properties
    camera.set(cv::CAP_PROP_FRAME_WIDTH, width);
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, height);

    std::cout << "frame_size=(" << camera.get(cv::CAP_PROP_FRAME_WIDTH) << ", "
                << camera.get(cv::CAP_PROP_FRAME_HEIGHT)
                << "), FPS=" << camera.get(cv::CAP_PROP_FPS) << std::endl;

    // load object detector
    ObjectDetector detector(model, config, framework);
    detector.setBackend(cv::dnn::DNN_BACKEND_VKCOM);
    detector.setTarget(cv::dnn::DNN_TARGET_VULKAN);
    detector.getOutputLayers();
    detector.setConfidenceThreshold(threshold);
    detector.setNMSThreshold(nms_threshold);
    detector.setDetectableClasses(2);

    // check if list of classes can be loaded
    std::vector<std::string> classes;
    if (config::contains("CLASSES")) {
        std::ifstream file(config::get("CLASSES"));
        if (file) {
            std::string line;
            while (std::getline(file, line, '\n')) {
                classes.push_back(line);
            }
            file.close();
        }
    }
    detector.setClasses(classes);

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

    // map keyboard inputs to actions for host
	const std::map<char, std::function<void (void)>> actions = {
            { 'q', [&]{ bridge.stop_motors(); }},
            { 'w', [&]{ bridge.set_motors(-d_speed, d_speed); }},
            { 's', [&]{ bridge.set_motors(d_speed, -d_speed); }},
            { 'a', [&]{ bridge.set_motors(r_speed, r_speed); }},
            { 'd', [&]{ bridge.set_motors(-r_speed, -r_speed); }}
	};

    bool terminated = false;
    char c = 0x00;
    cv::Mat frame(width, height, CV_8SC3);
    std::vector<unsigned char> buffer(width * height * 3);
    boost::system::error_code error;
    bool camera_enabled = true;
    int i = 0;
    std::vector<ObjectDetector::Prediction> predictions;

    // main control loop
	do {
	    const std::chrono::system_clock::time_point begin = std::chrono::system_clock::now();
	    // read controls if available
	    if (socket.available()) {
            RECV(socket, &c, 1, error);
            if (error) {
                std::cout << "unable to read data from socket" << std::endl;
                break;
            } else {
                if (c == 'x') {
                    std::cout << "connection terminated by peer" << std::endl;
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

	    // YOLOv3 needs input to be either (320, 320) or (416, 416)
	    cv::resize(frame, frame, Size(320, 320));


	    // run frame through detector
	    predictions = detector.run(frame, true);

        // send frame over network
	    uint32_t n = 0;
	    if (camera_enabled) {
            cv::imencode(".jpeg", frame, buffer);

            n = htonl((uint32_t) buffer.size());
            SEND(socket, &n, sizeof(n), error);
            if (error) {
                break;
            }
            SEND(socket, buffer.data(), buffer.size(), error);
            if (error) {
                break;
            }
        }

	    const uint64_t elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - begin).count();
	    const uint64_t fps = UINT64_C(1000) / elapsed_time;
    } while (!terminated);
    std::cout << std::endl << "connection closed" << std::endl;

    if (socket.is_open()) {
        socket.close();
    }

	return EXIT_SUCCESS;
}

