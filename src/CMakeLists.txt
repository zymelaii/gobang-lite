file(GLOB_RECURSE SOURCE_FILES . *.cpp)
file(GLOB_RECURSE HEADER_FILES . *.h)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_link_options(${PROJECT_NAME} PRIVATE ${LD_WINONLY_FLAG})

if (STATIC_LINK)
	if (MSVC)
		target_link_options(${PROJECT_NAME} PRIVATE /MD)
	else()
		target_link_options(${PROJECT_NAME} PRIVATE -static)
	endif()
endif()
