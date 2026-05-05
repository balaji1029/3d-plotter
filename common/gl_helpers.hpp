#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

namespace gl_helpers {

// Compile a single shader stage from source text. Throws on failure.
GLuint compile_shader(GLenum stage, const std::string& source);

// Read file -> compile_shader. Throws on missing file or compile failure.
GLuint compile_shader_file(GLenum stage, const std::string& path);

// Link a vertex+fragment program from file paths. Throws on failure.
GLuint make_program(const std::string& vs_path, const std::string& fs_path);

// Create a window with GL 3.3 core profile and a sane default. Aborts on failure.
GLFWwindow* create_window(int w, int h, const char* title);

}  // namespace gl_helpers
