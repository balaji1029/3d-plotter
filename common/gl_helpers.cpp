#include "gl_helpers.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace gl_helpers {

GLuint compile_shader(GLenum stage, const std::string& source) {
    GLuint sh = glCreateShader(stage);
    const char* src = source.c_str();
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);

    GLint ok = 0;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetShaderInfoLog(sh, len, nullptr, log.data());
        glDeleteShader(sh);
        throw std::runtime_error("shader compile failed:\n" + log);
    }
    return sh;
}

GLuint compile_shader_file(GLenum stage, const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("cannot open shader: " + path);
    std::stringstream ss;
    ss << f.rdbuf();
    return compile_shader(stage, ss.str());
}

GLuint make_program(const std::string& vs_path, const std::string& fs_path) {
    GLuint vs = compile_shader_file(GL_VERTEX_SHADER, vs_path);
    GLuint fs = compile_shader_file(GL_FRAGMENT_SHADER, fs_path);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetProgramInfoLog(prog, len, nullptr, log.data());
        throw std::runtime_error("program link failed:\n" + log);
    }
    glDetachShader(prog, vs);
    glDetachShader(prog, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

GLFWwindow* create_window(int w, int h, const char* title) {
    glfwSetErrorCallback([](int, const char* d) { std::cerr << "GLFW: " << d << "\n"; });
    if (!glfwInit()) std::exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* win = glfwCreateWindow(w, h, title, nullptr, nullptr);
    if (!win) { glfwTerminate(); std::exit(EXIT_FAILURE); }
    glfwMakeContextCurrent(win);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed\n";
        std::exit(EXIT_FAILURE);
    }
    return win;
}

}  // namespace gl_helpers
