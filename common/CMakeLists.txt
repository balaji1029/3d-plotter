add_library(gl_helpers STATIC gl_helpers.cpp)
target_include_directories(gl_helpers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(gl_helpers PUBLIC OpenGL::GL GLEW::GLEW glfw)
