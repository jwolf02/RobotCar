cmake_minimum_required(VERSION 3.5.1)
project(RobotCar)

set(CMAKE_CXX_STANDARD	11)
set(CMAKE_CXX_FLAGS		"${CMAKE_CXX_FLAGS} -Wall -pedantic -O2 -march=native -mtune=native")

set(l298NHBRIDGE		src/L298NHBridge.hpp
						src/L298NHBridge.cpp)

set(SERVER_SOCKET src/socket/Socket.hpp
        src/socket/Socket.cpp
        src/socket/ServerSocket.hpp
        src/socket/ServerSocket.cpp)

set(CLIENT_SOCKET src/socket/Socket.hpp
        src/socket/Socket.cpp
        src/socket/ClientSocket.hpp
        src/socket/ClientSocket.cpp)

set(TERMINAL			src/terminal.hpp
						src/terminal.cpp)

set(CLIENT				src/client.cpp
						${TERMINAL}
						${CLIENT_SOCKET})

set(CONTROLLER			src/controller.cpp
						${TERMINAL}
						${SERVER_SOCKET})

add_executable(client ${CLIENT})
target_include_directories(client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(controller ${CONTROLLER})
target_include_directories(controller PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
#target_link_libraries(controller PUBLIC wiringPi pthread)

