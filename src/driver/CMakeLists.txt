
set(DRIVER_SOURCES		L298NHBridge.hpp
                        L298NHBridge.cpp
                        HC_SR04.hpp
                        HC_SR04.cpp
                        gpio.hpp
                        gpio.cpp
                        sys_gpio.hpp
                        sys_gpio.cpp
                        SPIDev.hpp
                        SPIDev.cpp)

set(Driver_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

set(Driver_LIB          driver PARENT_SCOPE)

add_library(driver STATIC ${DRIVER_SOURCES})
target_include_directories(driver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${Util_INCLUDE_DIR})
if (RASPBERRY_PI)
    target_link_libraries(driver PUBLIC wiringPi pthread)
endif()