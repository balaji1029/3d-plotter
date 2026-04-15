#include "gl_framework.hpp"

extern float xrot, yrot, zrot;

//! Initialize GL State
void initGL(void) {
    //Set framebuffer clear color
    glClearColor(0.12f, 0.12f, 0.18f, 1.0f);
    //Set depth buffer furthest depth
    glClearDepth(1.0);
    //Set depth test to less-than
    glDepthFunc(GL_LESS);
    //Enable depth testing
    glEnable(GL_DEPTH_TEST);
}

//!GLFW Error Callback
void error_callback(int error, const char* description) {
    std::cerr << description << std::endl;
}

//!GLFW framebuffer resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    //!Resize the viewport to fit the window size - draw to entire window
    glViewport(0, 0, width, height);
}

//!GLFW keyboard callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    //!Close the window if the ESC key was pressed
    if ((action == GLFW_PRESS || action == GLFW_REPEAT)) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        if (key == GLFW_KEY_UP) {
            xrot += 5.0f;
        }
        if (key == GLFW_KEY_DOWN) {
            xrot -= 5.0f;
        }
        if (key == GLFW_KEY_LEFT) {
            yrot += 5.0f;
        }
        if (key == GLFW_KEY_RIGHT) {
            yrot -= 5.0f;
        }
    }
}


GLuint LoadShaderGL(GLenum eShaderType, const std::string& strFilename) {
    std::ifstream shaderFile(strFilename.c_str());
    if (!shaderFile.is_open())
        throw std::runtime_error("Cannot find file: " + strFilename);

    std::stringstream shaderData;
    shaderData << shaderFile.rdbuf();
    shaderFile.close();

    try {
        return CreateShaderGL(eShaderType, shaderData.str());
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        throw;
    }
}

GLuint CreateShaderGL(GLenum eShaderType, const std::string& strShaderFile) {
    GLuint shader = glCreateShader(eShaderType);
    const char* strFileData = strShaderFile.c_str();
    glShaderSource(shader, 1, &strFileData, NULL);

    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar* strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

        const char* strShaderType = NULL;
        switch (eShaderType) {
        case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
        case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
        case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
        }

        std::cerr << "Compile failure in " << strShaderType << " shader:" << std::endl << strInfoLog << std::endl;
        delete[] strInfoLog;
    }

    return shader;
}

GLuint CreateProgramGL(const std::vector<GLuint>& shaderList) {
    GLuint program = glCreateProgram();

    for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
        glAttachShader(program, shaderList[iLoop]);

    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar* strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
        std::cerr << "GLSL Linker failure: " << strInfoLog << std::endl;
        delete[] strInfoLog;
    }

    for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
        glDetachShader(program, shaderList[iLoop]);

    return program;
}