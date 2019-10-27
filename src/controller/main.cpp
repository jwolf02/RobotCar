#include <ServerSocket.hpp>
#include <util.hpp>
#include <controller.hpp>
#include <iostream>
#include <map>
#include <thread>
#include <Clock.hpp>
#include <opencv2/opencv.hpp>

#define PORT        8225

#define WIDTH       256
#define HEIGHT      256

static void _record(volatile const bool *flag, volatile const char *action) {
    const std::string fname = "actions.txt";
    std::ofstream file(fname, std::ios::ate);
    if (!file) {
        std::cout << "unable to open action file '" << fname << '\'' << std::endl;
        exit(1);
    }

    // create directory images if not already existing
    system("mkdir -p images");

    // wait for first user input
    while (*action == 0x00)
        Clock::sleep_for(250);

    // open camera
    cv::VideoCapture cam(0);
    if (!cam.isOpened()) {
        std::cout << "unable to access camera" << std::endl;
        exit(1);
    }

    // set frame size
    cam.set(cv::CAP_PROP_FRAME_WIDTH, WIDTH);
    cam.set(cv::CAP_PROP_FRAME_HEIGHT, HEIGHT);

    cv::Mat frame;

    // read images and associated actions until program is terminated
    while (*flag) {
        // read (state, action) pair
        cam >> frame;
        char act = *action;

        // get name as the current epoch timestamp in ms
        uint64_t ts = Clock::time_since_epoch<std::chrono::milliseconds>();

        cv::imwrite(std::to_string(ts) + ".jpg", frame);
        file << ts << ':' << act << std::endl;

        // run recorder at 2Hz
        Clock::sleep_for(500);
    }
}

// ./controller [--record]
int main(int argc, const char *argv[]) {

    const std::vector<std::string> args(argv, argv + argc);

    controller::setup();

    std::cout << "waiting for connection..." << std::endl;
    ServerSocket socket;
    try {
        socket = ServerSocket(PORT);
        socket.waitForConnection();
    } catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
        std::cout << "connection failed" << std::endl;
    }
    std::cout << "connection established" << std::endl;

    // map keyboard inputs to actions for controller
	const std::map<char, unsigned int> actions = {
            { 'q', controller::STOP },
            { 'w', controller::DRIVE_FORWARD },
            { 's', controller::DRIVE_BACKWARD },
            { 'a', controller::ROTATE_LEFT },
            { 'd', controller::ROTATE_RIGHT }
	};

    bool terminate = false;
    char c = 0x00;
    bool recording = false;
    char action = 0x00;
    std::thread t;

	while (!terminate) {
	    socket.recv(&c, 1);

	    if (c == 'x') {
            terminate = true;
        } else if (c == 'r'){
	        if (!recording) {
                recording = true;
                t = std::thread(_record, &recording, &action);
                std::cout << "started recording" << std::endl;
            } else {
	            recording = false;
	            t.join();
	            std::cout << "stopped recording" << std::endl;
	        }
	    } else {
	        const auto it = actions.find(c);
	        if (it != actions.end()) {
	            action = c;
	            controller::action(it->second);
	        } else {
                std::cout << "unrecognized action \'" << c << '\'' << std::endl;
            }
	    }
	}

    std::cout << "connection closed" << std::endl;

	socket.close();
	controller::cleanup();
	recording = false;
	t.join();

	return EXIT_SUCCESS;
}

