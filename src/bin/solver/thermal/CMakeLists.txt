set(solver_SRCS solver.cpp)
add_executable(solver ${solver_SRCS})
target_link_libraries(solver utils)
