
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

set(CV_SOURCES              ObjectDetector.hpp
                            ObjectDetector.cpp)

set(CV_INCLUDE_DIR          ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

set(CV_LIB                  cv PARENT_SCOPE)

add_library(cv STATIC ${CV_SOURCES})
target_include_directories(cv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cv PUBLIC ${OpenCV_LIBS})