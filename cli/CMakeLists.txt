# JSON
include_directories(../third_party/json/include)

# CXXOPTS
include_directories(../third_party/cxxopts/include)

set(CLI_SOURCES
	scorelisto.cpp
)
add_executable(scorelisto_p ${CLI_SOURCES})
target_link_libraries(scorelisto_p scorelisto)
