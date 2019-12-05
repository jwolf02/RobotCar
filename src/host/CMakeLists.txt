
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

set(HOST_SOURCES 		${CMAKE_CURRENT_SOURCE_DIR}/ObjectDetector.hpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/ObjectDetector.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# host executable
add_executable(rchost ${HOST_SOURCES})
target_include_directories(rchost PUBLIC ${Driver_INCLUDE_DIR} ${Util_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIR} ${Config_INCLUDE_DIR})
target_link_libraries(rchost PUBLIC pthread ${Driver_LIB} ${Config_LIB} ${OpenCV_LIBS} ${Boost_LIBRARIES})