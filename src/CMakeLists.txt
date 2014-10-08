add_subdirectory(json)

set(model_SRCS model.cpp)
add_library(model ${model_SRCS})

set(app_SRCS app.cpp)
add_executable(tl ${app_SRCS})
target_link_libraries(tl model json)
