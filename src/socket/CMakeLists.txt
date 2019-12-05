
find_package(Boost REQUIRED COMPONENTS system)

set(SOCKET_SOURCES		${CMAKE_CURRENT_SOURCE_DIR}/Socket.hpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/Socket.cpp)

set(Socket_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

set(Socket_LIB          socket PARENT_SCOPE)

add_library(socket STATIC ${SOCKET_SOURCES})
target_include_directories(socket PUBLIC ${Util_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIR})
target_link_libraries(socket PUBLIC ${Boost_LIBRARIES})