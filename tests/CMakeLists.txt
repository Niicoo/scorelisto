# GOOGLETEST
enable_testing()
find_package(GTest)

if(NOT GTest_FOUND)
    set(GTEST_ROOT "${CMAKE_SOURCE_DIR}/third_party/googletest/" CACHE PATH "path to gtest")
    find_package(GTest REQUIRED)
endif()
include_directories(${GTEST_INCLUDE_DIRS})

set(TEST_SOURCES
    common_toolsTest.cpp
    1_PitchDetector/FFmpegAudioReaderTest.cpp
    1_PitchDetector/McLeodPitchExtractorMethodTest.cpp
    1_PitchDetector/PitchDetectorTest.cpp
    2_StepDetector/HysteresisThresholdTest.cpp
    2_StepDetector/CumulativeSumThresholdTest.cpp
    2_StepDetector/HistogramStepDetectorTest.cpp
    3_NoteDetector/HeightDetectorTest.cpp
    3_NoteDetector/DijkstraTest.cpp
    3_NoteDetector/RhythmDetectorTest.cpp
    maintests.cpp
)
add_executable(run_tests ${TEST_SOURCES})
target_link_libraries(run_tests scorelisto)
target_link_libraries(run_tests ${GTEST_LIBRARIES})
gtest_discover_tests(run_tests)
