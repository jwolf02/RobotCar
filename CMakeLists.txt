cmake_minimum_required(VERSION 3.5.1)
project(RobotCar)

set(CMAKE_CXX_STANDARD	11)
set(CMAKE_CXX_FLAGS	"${CMAKE_CXX_FLAGS} -Wall -pedantic -O2")

set(HEADERS		src/terminal.hpp
			src/L298NHBridge.hpp)

set(SOURCES		src/terminal.cpp
			src/L298NHBridge.cpp
			src/main.cpp)

add_executable(controller ${HEADERS} ${SOURCES})
target_include_directories(controller PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(controller PUBLIC wiringpi)

