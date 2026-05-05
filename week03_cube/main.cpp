// Week 3 — colored cube + MVP + orbit camera + keyboard.
//
// Controls:
//   Arrow keys   — orbit (yaw/pitch)
//   = / -        — zoom in / out
//   ESC          — quit
//
// Things to try:
//   - Replace per-face colors with per-vertex normals (Week 5 setup).
//   - Switch between perspective and orthographic projection.
//   - Use RenderDoc to inspect the MVP uniform on the draw call.

#include "gl_helpers.hpp"
#include "orbit_camera.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <cstdio>

// 24 vertices: 4 per face so each face gets a distinct color.
// Layout per vertex: x, y, z, r, g, b
static const GLfloat CUBE_VERTS[] = {
    // -Z face (red)
    -1,-1,-1, 1,0,0,   1,-1,-1, 1,0,0,   1, 1,-1, 1,0,0,  -1, 1,-1, 1,0,0,
    // +Z face (green)
    -1,-1, 1, 0,1,0,   1,-1, 1, 0,1,0,   1, 1, 1, 0,1,0,  -1, 1, 1, 0,1,0,
    // -Y face (blue)
    -1,-1,-1, 0,0,1,   1,-1,-1, 0,0,1,   1,-1, 1, 0,0,1,  -1,-1, 1, 0,0,1,
    // +Y face (yellow)
    -1, 1,-1, 1,1,0,   1, 1,-1, 1,1,0,   1, 1, 1, 1,1,0,  -1, 1, 1, 1,1,0,
    // -X face (magenta)
    -1,-1,-1, 1,0,1,  -1, 1,-1, 1,0,1,  -1, 1, 1, 1,0,1,  -1,-1, 1, 1,0,1,
    // +X face (cyan)
     1,-1,-1, 0,1,1,   1, 1,-1, 0,1,1,   1, 1, 1, 0,1,1,   1,-1, 1, 0,1,1,
};

static const GLushort CUBE_INDICES[] = {
     0, 1, 2,  2, 3, 0,
     4, 6, 5,  6, 4, 7,
     8, 9,10, 10,11, 8,
    12,14,13, 14,12,15,
    16,17,18, 18,19,16,
    20,22,21, 22,20,23,
};

int main() {
    GLFWwindow* win = gl_helpers::create_window(800, 800, "Week 3 — Cube");
    glfwSwapInterval(1);

    GLuint program = gl_helpers::make_program(
        WEEK03_SHADER_DIR "/cube.vert",
        WEEK03_SHADER_DIR "/cube.frag");

    GLuint vao = 0, vbo = 0, ebo = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTS), CUBE_VERTS, GL_STATIC_DRAW);

    GLint pos = glGetAttribLocation(program, "in_pos");
    GLint col = glGetAttribLocation(program, "in_color");
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(col);
    glVertexAttribPointer(col, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CUBE_INDICES), CUBE_INDICES, GL_STATIC_DRAW);
    glBindVertexArray(0);

    GLint u_mvp = glGetUniformLocation(program, "u_mvp");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.08f, 0.08f, 0.12f, 1.0f);

    OrbitCamera cam;

    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();
        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(win, GLFW_TRUE);

        const float STEP = glm::radians(2.0f);
        if (glfwGetKey(win, GLFW_KEY_LEFT)  == GLFW_PRESS) cam.rotate(-STEP, 0);
        if (glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS) cam.rotate( STEP, 0);
        if (glfwGetKey(win, GLFW_KEY_UP)    == GLFW_PRESS) cam.rotate(0,  STEP);
        if (glfwGetKey(win, GLFW_KEY_DOWN)  == GLFW_PRESS) cam.rotate(0, -STEP);
        if (glfwGetKey(win, GLFW_KEY_EQUAL) == GLFW_PRESS) cam.zoom(0.97f);
        if (glfwGetKey(win, GLFW_KEY_MINUS) == GLFW_PRESS) cam.zoom(1.03f);

        int w, h;
        glfwGetFramebufferSize(win, &w, &h);
        glViewport(0, 0, w, h);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view  = cam.view();
        glm::mat4 proj  = glm::perspective(glm::radians(45.0f),
                                           (float)w / (float)h, 0.1f, 100.0f);
        glm::mat4 mvp   = proj * view * model;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program);
        glUniformMatrix4fv(u_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, sizeof(CUBE_INDICES) / sizeof(CUBE_INDICES[0]),
                       GL_UNSIGNED_SHORT, nullptr);

        glfwSwapBuffers(win);
    }

    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
    glfwDestroyWindow(win);
    glfwTerminate();
}
