
set(CONFIG_SOURCES      config.hpp
                        config.cpp)

set(Config_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

set(Config_LIB          config PARENT_SCOPE)

add_library(config STATIC ${CONFIG_SOURCES})
target_include_directories(config PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${Util_INCLUDE_DIR})