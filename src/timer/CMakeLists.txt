
set(TIMER_SOURCES       ${CMAKE_CURRENT_SOURCE_DIR}/Timer.hpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/Timer.cpp RingBuffer.hpp)

set(Timer_INCLUDE_DIR   ${CMAKE_CURRENT_SOURCE_DIR})

set(Timer_LIB           timer)

add_library(timer STATIC ${TIMER_SOURCES})
target_include_directories(timer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(timer PUBLIC rt)