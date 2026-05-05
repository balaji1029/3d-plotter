add_executable(week04_rubiks main.cpp rubiks.cpp)
target_link_libraries(week04_rubiks PRIVATE gl_helpers)
target_compile_definitions(week04_rubiks PRIVATE
    WEEK04_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders")
