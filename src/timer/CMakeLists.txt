
set(TIMER_SOURCES       Timer.hpp
                        Timer.cpp)

set(Timer_INCLUDE_DIR   ${CMAKE_CURRENT_SOURCE_DIR})

set(Timer_LIB           timer)

add_library(timer STATIC ${TIMER_SOURCES})
target_include_directories(timer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(timer PUBLIC rt)