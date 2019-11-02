cmake_minimum_required(VERSION 3.5.1)
project(RobotCar)

set(CMAKE_CXX_STANDARD	11)
set(CMAKE_CXX_FLAGS		"${CMAKE_CXX_FLAGS} -Wall -pedantic -O2 -march=native -mtune=native -fpermissive")

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

if(RASPBERRY_PI)
	message("-- building for Raspberry Pi")
    add_definitions(-DRASPBERRY_PI)
elseif(JETSON_NANO)
	message("-- building for Jetson Nano")
else()
	message("-- building for default platform")
endif()

set(UTIL_INCLUDE		src/util/config.hpp
						src/util/Tensor.hpp
						src/util/VecND.hpp
						src/util/SpinLock.hpp
						src/util/terminal.hpp
						src/util/util.hpp
						src/util/Clock.hpp)

set(UTIL_SOURCES		${UTIL_INCLUDE}
						src/util/config.cpp
						src/util/terminal.cpp)

set(UTIL_INCLUDE_DIR	${CMAKE_CURRENT_SOURCE_DIR}/src/util)

add_library(util STATIC ${UTIL_SOURCES})
target_include_directories(util PUBLIC ${UTIL_INCLUDE_DIR})

set(DRIVER_INCLUDE 		src/driver/L298NHBridge.hpp
						src/driver/gpio.hpp)

set(DRIVER_SOURCES		${DRIVER_INCLUDE}
						src/driver/L298NHBridge.cpp
						src/driver/gpio.cpp)

set(DRIVER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/driver)

add_library(driver STATIC ${DRIVER_SOURCES})
target_include_directories(driver PUBLIC ${DRIVER_INCLUDE_DIR} ${UTIL_INCLUDE_DIR})
if (RASPBERRY_PI)
	target_link_libraries(driver PUBLIC util RTIMULib wiringPi)
else()
	target_link_libraries(driver PUBLIC util)
endif()

set(MONITOR_SOURCES     src/monitor/main.cpp)

add_executable(monitor ${MONITOR_SOURCES})
target_include_directories(monitor PUBLIC ${UTIL_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
target_link_libraries(monitor util ${OpenCV_LIBS} pthread ${Boost_LIBRARIES})

set(HOST_SOURCES  		src/host/main.cpp
						src/host/controller.hpp
						src/host/controller.cpp)

add_executable(host ${HOST_SOURCES})
target_include_directories(host PUBLIC ${UTIL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src/host ${Boost_INCLUDE_DIR})
target_link_libraries(host PUBLIC util driver pthread ${OpenCV_LIBS} ${Boost_LIBRARIES})

install(TARGETS monitor DESTINATION /usr/bin OPTIONAL)
install(TARGETS host DESTINATION /usr/bin OPTIONAL)
