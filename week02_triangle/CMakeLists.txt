add_executable(week02_triangle main.cpp)
target_link_libraries(week02_triangle PRIVATE gl_helpers)
target_compile_definitions(week02_triangle PRIVATE
    WEEK02_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders")
