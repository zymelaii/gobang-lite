cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(gobang-lite VERSION 0.5.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_CONSOLE "enable console" OFF)
option(STATIC_LINK "enable link staticly" ON)

if (NOT ENABLE_CONSOLE)
	if (MSVC)
		set(LD_WINONLY_FLAG /subsystem:windows /entry:mainCRTStartup)
	else()
		set(LD_WINONLY_FLAG -Wl,--subsystem,windows)
	endif()
endif()

include_directories(src)
add_subdirectory(src)

install(
	TARGETS ${PROJECT_NAME}
	DESTINATION ${CMAKE_BINARY_DIR}
)
install(
	DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets
	DESTINATION ${CMAKE_BINARY_DIR}
)
