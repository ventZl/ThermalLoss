add_subdirectory(json)
add_subdirectory(model)
add_subdirectory(calc)

include_directories(${CMAKE_SRC_DIR})

set(app_SRCS app.cpp)
add_executable(tl ${app_SRCS})
target_link_libraries(tl model json calc)
