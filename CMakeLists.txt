cmake_minimum_required(VERSION 3.8)
project(MediaIPC)

# Set the C++ standard to C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable all relevant compiler warnings
if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W2 /MD")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

# If we are using the Conan CMake generator, include the generated file
if(EXISTS "${CMAKE_SOURCE_DIR}/conanbuildinfo.cmake")
	include("${CMAKE_SOURCE_DIR}/conanbuildinfo.cmake")
	conan_basic_setup()
endif()

# Locate the Boost headers
find_package(Boost 1.64 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS} SYSTEM)

# Build libMediaIPC
set(LIBRARY_SOURCES
	source/private/ConsumerDelegate.cpp
	source/private/ControlBlock.cpp
	source/private/Formats.cpp
	source/private/IPCUtils.cpp
	source/private/MediaConsumer.cpp
	source/private/MediaProducer.cpp
	source/private/ObjectNames.cpp
	source/private/RingBuffer.cpp
)
add_library(MediaIPC STATIC ${LIBRARY_SOURCES})

# Determine if we are building our example producers and consumers
option(BUILD_EXAMPLES "build example producer and consumer executables" ON)
if (BUILD_EXAMPLES)
	
	# Under Linux and macOS, link against pthreads and librt
	if (UNIX)
		set(LIBRARIES ${LIBRARIES} pthread rt)
	endif()
	
	# Build the examples that only require the Boost headers
	set(EXAMPLES_COMMON examples/common/common.cpp)
	add_executable(procedural_producer examples/producers/procedural_producer.cpp ${EXAMPLES_COMMON})
	add_executable(rawdump_consumer examples/consumers/rawdump_consumer.cpp ${EXAMPLES_COMMON})
	target_link_libraries(procedural_producer MediaIPC ${LIBRARIES})
	target_link_libraries(rawdump_consumer MediaIPC ${LIBRARIES})
	
	# Determine if we have Boost.System
	find_package(Boost 1.64 COMPONENTS system)
	if (Boost_SYSTEM_FOUND)
		
		# Build the examples that require Boost.System
		add_executable(ffmpeg_consumer examples/consumers/ffmpeg_consumer.cpp ${EXAMPLES_COMMON})
		target_link_libraries(ffmpeg_consumer MediaIPC ${LIBRARIES} ${Boost_LIBRARIES})
		
	endif()
	
endif()

# Installation rules
install(DIRECTORY source/public/ DESTINATION include/MediaIPC FILES_MATCHING PATTERN "*.h")
install(DIRECTORY source/public/ DESTINATION include/MediaIPC FILES_MATCHING PATTERN "*.inc")
install(TARGETS MediaIPC ARCHIVE DESTINATION lib)
