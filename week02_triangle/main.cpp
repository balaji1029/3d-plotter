// Week 2 — first triangle.
//
// Goal: a complete, working pipeline you can break and rebuild. The code
// below should compile and draw a colored triangle. Use RenderDoc to capture
// a frame and inspect the vertex inputs, draw call, and final framebuffer.
//
// Things to try once it works:
//   - Change the triangle's vertex colors and confirm via RenderDoc.
//   - Add a 4th vertex and switch to GL_TRIANGLE_STRIP.
//   - Swap the fragment shader to output a uniform color, set via uniform.

#include "gl_helpers.hpp"

#include <iostream>

int main() {
    GLFWwindow* win = gl_helpers::create_window(800, 600, "Week 2 — Triangle");

    glfwSetKeyCallback(win, [](GLFWwindow* w, int key, int, int action, int) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(w, GLFW_TRUE);
    });

    GLuint program = gl_helpers::make_program(
        WEEK02_SHADER_DIR "/triangle.vert",
        WEEK02_SHADER_DIR "/triangle.frag");

    // Interleaved: x, y, r, g, b
    GLfloat verts[] = {
        -0.7f, -0.6f, 1.0f, 0.2f, 0.2f,
         0.7f, -0.6f, 0.2f, 1.0f, 0.2f,
         0.0f,  0.7f, 0.2f, 0.4f, 1.0f,
    };

    GLuint vao = 0, vbo = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    GLint pos_loc = glGetAttribLocation(program, "in_pos");
    GLint col_loc = glGetAttribLocation(program, "in_color");
    glEnableVertexAttribArray(pos_loc);
    glVertexAttribPointer(pos_loc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(col_loc);
    glVertexAttribPointer(col_loc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    glBindVertexArray(0);

    glClearColor(0.10f, 0.10f, 0.14f, 1.0f);

    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(win);
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
    glfwDestroyWindow(win);
    glfwTerminate();
    std::cout << "bye\n";
}
