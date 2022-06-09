file(GLOB_RECURSE SOURCE_FILES . *.cpp)
file(GLOB_RECURSE HEADER_FILES . *.h)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_link_options(${PROJECT_NAME} PRIVATE ${LD_WINONLY_FLAG})
target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)

if (MSVC)
	# target_compile_options(${PROJECT_NAME} PRIVATE /O2)
else()
	if (STATIC_LINK)
		target_link_options(${PROJECT_NAME} PRIVATE -static)
	endif()
	target_compile_options(${PROJECT_NAME} PRIVATE -O3)
endif()
