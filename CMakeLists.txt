cmake_minimum_required(VERSION 3.5.1)
project(RobotCar)

set(CMAKE_CXX_STANDARD	14)
set(CMAKE_CXX_FLAGS		"${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -O3 -mtune=native")

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

if(RASPBERRY_PI)
	# building rchost for a Raspberry Pi based system using wiringPi
	message("-- Building for Raspberry Pi")
	message("-- Using WiringPi as GPIO library")
    add_definitions(-DWIRING_PI)
elseif(JETSON_NANO)
	# building rchost for NVIDIA Jetson Nano
	message("-- Building for Jetson Nano")
	message("-- Using custom sys_gpio as GPIO library")
	add_definitions(-DSYS_GPIO)
else()
	# building rcmonitor-ui and rchost using Qt5
	message("-- Building for default platform")
	message("-- No GPIO library support")
endif()

add_subdirectory(src/util)
add_subdirectory(src/socket)
add_subdirectory(src/config)
add_subdirectory(src/driver)
add_subdirectory(src/timer)
add_subdirectory(src/monitor)
add_subdirectory(src/host)

install(TARGETS rcmonitor-ui DESTINATION /usr/bin OPTIONAL)
install(TARGETS rchost DESTINATION /usr/bin OPTIONAL)
