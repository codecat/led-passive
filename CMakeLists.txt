cmake_minimum_required(VERSION 3.16)
project(led-passive)
set(CMAKE_CXX_STANDARD 17)

add_executable(led-passive
	src/main.cpp
	src/Common.cpp
	src/Buffer.cpp
)

add_subdirectory(thirdparty/serial)

target_link_libraries(led-passive
	serial
)

include_directories(
	src/
	thirdparty/scratch2/
	thirdparty/serial/include/
)
