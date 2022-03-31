target_sources(lk PRIVATE
	debug.c
	init.c
)

message(STATUS "Selected platform: ${LK_PLATFORM}")
add_subdirectory(${LK_PLATFORM})
