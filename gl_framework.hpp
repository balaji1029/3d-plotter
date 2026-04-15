#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

// extern GLfloat xrot, yrot, zrot;

void initGL(void);
void error_callback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

GLuint LoadShaderGL(GLenum eShaderType, const std::string& strFilename);

GLuint CreateShaderGL(GLenum eShaderType, const std::string& strShaderFile);

GLuint CreateProgramGL(const std::vector<GLuint>& shaderList);