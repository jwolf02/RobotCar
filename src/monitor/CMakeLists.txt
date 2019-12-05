
if (NOT (RASPBERRY_PI OR JETSON_NANO))
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
    find_package(OpenCV REQUIRED)
    find_package(Boost REQUIRED COMPONENTS system)

    # many Qt specific commands
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
    include_directories(${Qt5Widgets_INCLUDES})
    add_definitions(${Qt5Widgets_DEFINITIONS})

    set(MONITOR_SOURCES		${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/MonitorWindow.hpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/MonitorWindow.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/MonitorWindow.ui
                            ${CMAKE_CURRENT_SOURCE_DIR}/monitor.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/monitor.hpp)

    # monitor executable
    add_executable(rcmonitor-ui ${MONITOR_SOURCES})
    target_include_directories(rcmonitor-ui PUBLIC ${UTIL_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(rcmonitor-ui pthread ${OpenCV_LIBS} ${Boost_LIBRARIES})
    target_link_libraries(rcmonitor-ui Qt5::Widgets Qt5::Core)
endif()