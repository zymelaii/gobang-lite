cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(gobang-lite VERSION 0.2.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_CONSOLE "enable console" OFF)
option(STATIC_LINK "enable link staticly" ON)

if (NOT ENABLE_CONSOLE)
	if (MSVC)
		set(LD_WINONLY_FLAG /subsystem:windows /entry:mainCRTStartup)
	else()
		set(LD_WINONLY_FLAG -Wl,--subsystem,windows)
	endif()
endif()

include_directories(./src)
add_subdirectory(./src)