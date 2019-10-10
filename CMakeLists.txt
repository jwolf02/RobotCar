cmake_minimum_required(VERSION 3.5.1)
project(RobotCar)

set(CMAKE_CXX_STANDARD	11)
set(CMAKE_CXX_FLAGS		"${CMAKE_CXX_FLAGS} -Wall -pedantic -O2 -march=native -mtune=native -fpermissive")

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

set(SOCKET_INCLUDE		src/socket/Socket.hpp
						src/socket/ClientSocket.hpp
						src/socket/ServerSocket.hpp)

set(SOCKET_SOURCES		${SOCKET_INCLUDE}
						src/socket/Socket.cpp
        				src/socket/ServerSocket.cpp
						src/socket/ClientSocket.cpp
						src/socket/socket_wrappers.hpp
						src/socket/socket_wrappers.cpp)

set(SOCKET_INCLUDE_DIR	${CMAKE_CURRENT_SOURCE_DIR}/src/socket)

add_library(socket SHARED ${SOCKET_SOURCES})
target_include_directories(socket PUBLIC ${SOCKET_INCLUDE_DIR})
target_link_libraries(socket PUBLIC bluetooth)

set(TIMER_INCLUDE		src/timer/timer.hpp)

set(TIMER_SOURCES		${TIMER_INCLUDE}
						src/timer/timer.cpp)

set(TIMER_INCLUDE_DIR	${CMAKE_CURRENT_SOURCE_DIR}/src/timer)

add_library(timer STATIC ${TIMER_SOURCES})
target_include_directories(timer PUBLIC ${TIMER_INCLUDE_DIR})
target_link_libraries(timer PUBLIC rt pthread)

set(DRIVER_INCLUDE 		src/driver/L298NHBridge.hpp
						src/driver/HC_SR04.hpp
						src/driver/MPU9250.hpp)

set(DRIVER_SOURCES		${DRIVER_INCLUDE}
						src/driver/L298NHBridge.cpp
						src/driver/HC_SR04.cpp
						src/driver/MPU9250.cpp)

set(DRIVER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/driver)

add_library(driver STATIC ${DRIVER_SOURCES})
target_include_directories(driver PUBLIC ${DRIVER_INCLUDE_DIR} ${UTIL_INCLUDE_DIR})
target_link_libraries(driver PUBLIC util RTIMULib wiringPi)

set(MONITOR_SOURCES     src/monitor/main.cpp)

add_executable(monitor ${MONITOR_SOURCES})
target_include_directories(monitor PUBLIC ${UTIL_INCLUDE_DIR} ${SOCKET_INCLUDE_DIR})
target_link_libraries(monitor socket util)

set(CONTROLLER_SOURCES  src/controller/main.cpp
                        src/controller/RobotCar.cpp
                        src/controller/RobotCar.hpp)

add_executable(controller ${CONTROLLER_SOURCES})
target_include_directories(controller PUBLIC ${SOCKET_INCLUDE_DIR} ${UTIL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src/controller)
target_link_libraries(controller PUBLIC socket util driver)

install(TARGETS monitor DESTINATION /usr/bin OPTIONAL)
install(TARGETS controller DESTINATION /usr/bin OPTIONAL)
install(TARGETS socket DESTINATION /usr/lib)
