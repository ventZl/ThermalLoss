set(heating_SRCS heating.cpp)
add_executable(heating ${heating_SRCS})
target_link_libraries(heating utils)
