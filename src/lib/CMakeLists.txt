set(utils_SRCS solver.cpp thermal.cpp named.cpp reynolds.cpp report.cpp)
add_library(utils ${utils_SRCS})
target_link_libraries(utils json)
