cmake_minimum_required(VERSION 3.8)
project(MediaIPC)

# Set the C++ standard to C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable all compiler warnings
if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /MD")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
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
	
	# Build the examples
	add_executable(procedural_producer examples/producers/procedural_producer.cpp)
	add_executable(rawdump_consumer examples/consumers/rawdump_consumer.cpp)
	target_link_libraries(procedural_producer MediaIPC ${LIBRARIES})
	target_link_libraries(rawdump_consumer MediaIPC ${LIBRARIES})
	
endif()

# Installation rules
install(DIRECTORY source/public/ DESTINATION include/MediaIPC FILES_MATCHING PATTERN "*.h")
install(DIRECTORY source/public/ DESTINATION include/MediaIPC FILES_MATCHING PATTERN "*.inc")
install(TARGETS MediaIPC ARCHIVE DESTINATION lib)
