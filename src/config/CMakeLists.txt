
set(CONFIG_SOURCES      ${CMAKE_CURRENT_SOURCE_DIR}/config.hpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/config.cpp)

set(Config_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR})

set(Config_LIB          config)

add_library(config STATIC ${CONFIG_SOURCES})
target_include_directories(config PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${Util_INCLUDE_DIR})