cmake_minimum_required(VERSION 3.16)
project(led-passive)
set(CMAKE_CXX_STANDARD 17)

add_executable(led-passive
	src/main.cpp

	src/Common.cpp
	src/Buffer.cpp
	src/Config.cpp
	src/Easing.cpp

	src/Program.cpp
	src/ScriptFunctions.cpp
	src/Strip.cpp
	src/Scene.cpp
)

add_subdirectory(thirdparty/serial)

set(LUAU_BUILD_CLI OFF CACHE INTERNAL "" FORCE)
set(LUAU_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
add_subdirectory(thirdparty/luau)

target_link_libraries(led-passive
	serial
	Luau.Compiler
	Luau.VM
)

include_directories(
	src/
	thirdparty/scratch2/
	thirdparty/serial/include/
	thirdparty/luau/VM/include/
	thirdparty/luau/Compiler/include/
)
