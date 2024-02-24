cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(DiscordRichPresence VERSION 1.0.0)

file(GLOB SOURCES
	src/*.cpp
    src/*.hpp
    include/*.hpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (PROJECT_IS_TOP_LEVEL)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _DRPC_EXPORTING)
endif()

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)
target_include_directories(${PROJECT_NAME} PRIVATE 
    libs/discord-rpc/include
)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libs/extensions/libdiscord-rpc.a)
else()
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libs/extensions/discord-rpc.lib)
endif()

setup_geode_mod(${PROJECT_NAME})
