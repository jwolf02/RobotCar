
set(DRIVER_SOURCES		${CMAKE_CURRENT_SOURCE_DIR}/L298NHBridge.hpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/HC_SR04.hpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/gpio.hpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/sys_gpio.hpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/L298NHBridge.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/HC_SR04.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/sys_gpio.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/gpio.cpp)

set(Driver_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(driver STATIC ${DRIVER_SOURCES})
target_include_directories(driver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${Util_INCLUDE_DIR})
if (RASPBERRY_PI)
    target_link_libraries(driver PUBLIC wiringPi pthread)
endif()