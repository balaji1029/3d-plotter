add_executable(week03_cube main.cpp)
target_link_libraries(week03_cube PRIVATE gl_helpers)
target_compile_definitions(week03_cube PRIVATE
    WEEK03_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders")
