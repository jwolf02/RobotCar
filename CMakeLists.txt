cmake_minimum_required(VERSION 3.5.1)
project(RobotCar)

set(CMAKE_CXX_STANDARD	11)
set(CMAKE_CXX_FLAGS		"${CMAKE_CXX_FLAGS} -Wall -pedantic -O2 -march=native -mtune=native")

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
	find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

	# many Qt specific commands
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTORCC ON)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
	include_directories(${Qt5Widgets_INCLUDES})
	add_definitions(${Qt5Widgets_DEFINITIONS})
endif()

set(UTIL_INCLUDE_DIR	${CMAKE_CURRENT_SOURCE_DIR}/src/util)

set(DRIVER_INCLUDE 		src/driver/L298NHBridge.hpp
						src/driver/HC_SR04.hpp
		src/driver/gpio.hpp
		src/driver/sys_gpio.hpp)

set(DRIVER_SOURCES		${DRIVER_INCLUDE}
						src/driver/L298NHBridge.cpp
						src/driver/HC_SR04.cpp
		src/driver/sys_gpio.cpp
		src/driver/gpio.cpp)

set(DRIVER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/driver ${CMAKE_CURRENT_SOURCE_DIR}/src/socket)

add_library(driver STATIC ${DRIVER_SOURCES})
target_include_directories(driver PUBLIC ${DRIVER_INCLUDE_DIR} ${UTIL_INCLUDE_DIR})
if (RASPBERRY_PI)
	target_link_libraries(driver PUBLIC wiringPi pthread)
endif()

set(MONITOR_SOURCES		src/monitor/main.cpp
						src/monitor/MonitorWindow.hpp
						src/monitor/MonitorWindow.cpp
						src/monitor/MonitorWindow.ui
						src/monitor/monitor.cpp
						src/monitor/monitor.hpp)

# monitor executable
add_executable(rcmonitor-ui ${MONITOR_SOURCES})
target_include_directories(rcmonitor-ui PUBLIC ${UTIL_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src/monitor)
target_link_libraries(rcmonitor-ui pthread ${OpenCV_LIBS} ${Boost_LIBRARIES})
if (NOT (RASPBERRY_PI OR JETSON_NANO))
	target_link_libraries(rcmonitor-ui Qt5::Widgets Qt5::Core)
endif()

# host executable
add_executable(rchost src/host/main.cpp)
target_include_directories(rchost PUBLIC ${UTIL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src/host ${Boost_INCLUDE_DIR})
target_link_libraries(rchost PUBLIC driver pthread ${OpenCV_LIBS} ${Boost_LIBRARIES})

install(TARGETS rcmonitor-ui DESTINATION /usr/bin OPTIONAL)
install(TARGETS rchost DESTINATION /usr/bin OPTIONAL)
