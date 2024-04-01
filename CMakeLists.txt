cmake_minimum_required(VERSION 3.5)

project(renamenoise LANGUAGES C)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(NOT DEFINED RENAMENOISE_DEMO_EXECUTABLE)
    set(RENAMENOISE_DEMO_EXECUTABLE ON)
endif()

if(NOT DEFINED RENAMENOISE_BENCHMARK_EXECUTABLE)
    set(RENAMENOISE_BENCHMARK_EXECUTABLE OFF)
endif()

if (MSVC)
    add_compile_options(/W4)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/Zi)
    else()
        add_compile_options(/Od)
    endif()
else()
    add_compile_options(-Wall -Wextra -Wpedantic)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g)
    else()
        add_compile_options(-O3)
    endif()
endif()

if(WIN32)
	add_library(renamenoise SHARED)
	set_target_properties(renamenoise PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	if(MINGW)
		# Remove "lib" prefix.
		set_target_properties(renamenoise PROPERTIES PREFIX "")
	endif()
	target_compile_definitions(renamenoise
		PRIVATE
			"WIN32"
			"DLL_EXPORT"
	)
else()
	add_library(renamenoise STATIC)
endif()

target_compile_definitions(renamenoise PRIVATE "RENAMENOISE_BUILD")
target_compile_definitions(renamenoise PRIVATE "ENABLE_ASSERTIONS")

target_include_directories(renamenoise
	PRIVATE SYSTEM
		${CMAKE_CURRENT_SOURCE_DIR}
	PUBLIC SYSTEM
		"include"
)

target_sources(renamenoise PRIVATE
	"src/rnn_data.c"
	"src/rnn.c"
	"src/pitch.c"
	"src/renamenoise_fft.c"
	"src/denoise.c"
	"src/renamenoise_lpc.c"
)

if(RENAMENOISE_DEMO_EXECUTABLE)
    add_executable(renamenoise_demo examples/renamenoise_demo.c)

    target_compile_definitions(renamenoise_demo PRIVATE "RENAMENOISE_BUILD")
    target_compile_definitions(renamenoise_demo PRIVATE "ENABLE_ASSERTIONS")

    target_include_directories(renamenoise_demo
        PRIVATE SYSTEM
            ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC SYSTEM
            "include"
    )

    target_sources(renamenoise_demo PRIVATE
        "src/rnn_data.c"
        "src/rnn.c"
        "src/pitch.c"
        "src/renamenoise_fft.c"
        "src/denoise.c"
        "src/renamenoise_lpc.c"
    )

    target_link_libraries(renamenoise_demo PRIVATE m)
    set_target_properties(renamenoise_demo PROPERTIES RUNTIME_OUTPUT_DIRECTORY "examples")
endif()

if(RENAMENOISE_BENCHMARK_EXECUTABLE)
    add_executable(renamenoise_benchmark examples/renamenoise_benchmark.c)

    target_compile_definitions(renamenoise_benchmark PRIVATE "RENAMENOISE_BUILD")
    target_compile_definitions(renamenoise_benchmark PRIVATE "ENABLE_ASSERTIONS")

    target_include_directories(renamenoise_benchmark
        PRIVATE SYSTEM
            ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC SYSTEM
            "include"
    )

    target_sources(renamenoise_benchmark PRIVATE
        "src/rnn_data.c"
        "src/rnn.c"
        "src/pitch.c"
        "src/renamenoise_fft.c"
        "src/denoise.c"
        "src/renamenoise_lpc.c"
    )

    target_link_libraries(renamenoise_benchmark PRIVATE m)
    set_target_properties(renamenoise_benchmark PROPERTIES RUNTIME_OUTPUT_DIRECTORY "examples")
endif()

message(STATUS "===")
message(STATUS "    Building ReNameNoise ${CMAKE_BUILD_TYPE}")
message(STATUS "    RENAMENOISE_DEMO_EXECUTABLE is ${RENAMENOISE_DEMO_EXECUTABLE}")
message(STATUS "    RENAMENOISE_BENCHMARK_EXECUTABLE is ${RENAMENOISE_BENCHMARK_EXECUTABLE}")
get_target_property(RENAMENOISE_CFLAGS renamenoise COMPILE_OPTIONS)
message(STATUS "    CLFAGS = '${RENAMENOISE_CFLAGS}'")
message(STATUS "===")

# target_disable_warnings(renamenoise)
