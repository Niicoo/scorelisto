cmake_minimum_required(VERSION 3.10)

# The C++ standard
set(CXX_STANDARD 11)

# Project name
project(scorelisto CXX)

# The version number
set(scorelisto_VERSION_MAJOR 1)
set(scorelisto_VERSION_MINOR 0)

# FFMPEG
include_directories(${CMAKE_SOURCE_DIR}/third_party/ffmpeg/include)
link_directories(${CMAKE_SOURCE_DIR}/third_party/ffmpeg/lib)

# SCORELISTO
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/1_PitchDetector)
include_directories(${CMAKE_SOURCE_DIR}/include/2_StepDetector)
include_directories(${CMAKE_SOURCE_DIR}/include/3_NoteDetector)
include_directories(${CMAKE_SOURCE_DIR}/include/3_NoteDetector/HeightDetector)
include_directories(${CMAKE_SOURCE_DIR}/include/3_NoteDetector/RhythmDetector)

# Linking FFMPEG
if(UNIX)
	link_libraries(pthread)
endif(UNIX)
link_libraries(avformat)
link_libraries(avcodec)
link_libraries(avutil)
link_libraries(swresample)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	# using GCC
	add_definitions(
		-Wall
		-Wextra
		-Wfatal-errors
		-Wconversion
	)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	# using Visual Studio C++
endif()


# SHARED LIBRARY
add_subdirectory(src)

# CLI
option(BUILD_CLI "Build CLI program" ON)
if(${BUILD_CLI})
	add_subdirectory(cli)
endif()

# TEST
option(BUILD_TESTING "Build test programs" OFF)
if(${BUILD_TESTING})
	enable_testing()
	add_subdirectory(tests)	
endif()

# GUI
option(BUILD_GUI "Build GUI program" ON)
if(${BUILD_GUI})
	add_subdirectory(gui)
endif()