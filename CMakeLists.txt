cmake_minimum_required(VERSION 3.5.1)
project(RobotCar)

set(CMAKE_CXX_STANDARD	11)
set(CMAKE_CXX_FLAGS		"${CMAKE_CXX_FLAGS} -Wall -pedantic -O2 -march=native -mtune=native -fpermissive")


set(SOCKET 				src/socket/Socket.hpp
        				src/socket/Socket.cpp
        				src/socket/ServerSocket.hpp
        				src/socket/ServerSocket.cpp
						src/socket/ClientSocket.hpp
						src/socket/ClientSocket.cpp
						src/socket/socket_wrappers.hpp
						src/socket/socket_wrappers.cpp)

set(SOCKET_INCLUDE		src/socket/Socket.hpp
						src/socket/ClientSocket.hpp
						src/socket/ServerSocket.hpp)

set(L298NHBRIDGE 		src/util/L298NHBridge.hpp
						src/util/L298NHBridge.cpp)

set(TERMINAL 			src/util/terminal.hpp
						src/util/terminal.cpp)

set(MONITOR 			src/monitor/main.cpp
						${TERMINAL}
						${SOCKET_INCLUDE})

set(CONTROLLER 			src/controller/main.cpp
						${SOCKET_INCLUDE}
						${L298NHBRIDGE})

set(INCLUDE_DIRECTORIES	${CMAKE_CURRENT_SOURCE_DIR}/src/socket ${CMAKE_CURRENT_SOURCE_DIR}/src/util)

add_library(socket SHARED ${SOCKET})
target_include_directories(socket PUBLIC ${INCLUDE_DIRECTORIES})
target_link_libraries(socket PUBLIC bluetooth)

add_executable(monitor ${MONITOR})
target_include_directories(monitor PUBLIC ${INCLUDE_DIRECTORIES})
target_link_libraries(monitor socket)

add_executable(controller ${CONTROLLER})
target_include_directories(controller PUBLIC ${INCLUDE_DIRECTORIES})
target_link_libraries(controller PUBLIC socket wiringPi pthread)

