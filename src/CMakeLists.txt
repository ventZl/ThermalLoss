add_subdirectory(json)
add_subdirectory(model)
add_subdirectory(calc)
add_subdirectory(geom)
add_subdirectory(lib)
add_subdirectory(app)

include_directories(${CMAKE_SRC_DIR} ${CMAKE_SRC_DIR}/src/)

set(app_SRCS app.cpp)
add_executable(tl ${app_SRCS})
target_link_libraries(tl model json calc geom utils)

set(quick_SRCS heating.cpp)
add_executable(heating ${quick_SRCS})
target_link_libraries(heating utils)

set(hydraulic_SRCS hydraulic.cpp)
add_executable(hydraulic ${hydraulic_SRCS})
target_link_libraries(hydraulic utils)
