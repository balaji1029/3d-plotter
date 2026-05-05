// Week 4 — Rubik's cube with per-face rotation.
//
// What works out of the box:
//   - 27 cubelets are drawn at integer positions with proper face colors.
//   - Camera orbit (arrows / =/-) and ESC to quit work.
//   - A face-turn animation interpolates from 0 to 90 degrees.
//
// What is intentionally TODO (the learning):
//   - RubiksCube::commit_turn — permute cubelet positions and orientations
//     after the 90 degree animation finishes. Right now turns animate but
//     don't "stick", so the cube snaps back at the end of each turn.
//   - Optional: queue moves while one is animating instead of dropping them.
//
// Controls:
//   U / D — top / bottom face          (hold Shift for counter-clockwise)
//   L / R — left / right face
//   F / B — front / back face
//   Arrows / = / -  — camera orbit / zoom
//   ESC — quit

#include "gl_helpers.hpp"
#include "rubiks.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <cstdio>
#include <vector>

// ---- Geometry: a unit cubelet (slightly inset) with one color attribute per
// vertex. We pick the color per face when building the mesh and reuse the same
// VAO for all 27 cubelets — only the model matrix changes per draw call.

struct Vertex { float x, y, z, r, g, b; };

static void push_face(std::vector<Vertex>& out,
                      glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d,
                      std::array<float, 3> color) {
    auto v = [&](glm::vec3 p) {
        out.push_back({p.x, p.y, p.z, color[0], color[1], color[2]});
    };
    // Two CCW triangles, viewed from outside.
    v(a); v(b); v(c);
    v(a); v(c); v(d);
}

static std::vector<Vertex> build_cubelet_mesh() {
    const float s = 0.45f;  // half-side, leaves a small gap between cubelets
    glm::vec3 P[8] = {
        {-s,-s,-s}, { s,-s,-s}, { s, s,-s}, {-s, s,-s},
        {-s,-s, s}, { s,-s, s}, { s, s, s}, {-s, s, s},
    };
    std::vector<Vertex> out;
    out.reserve(36);
    push_face(out, P[4], P[5], P[6], P[7], face_color::GREEN);   // +Z front
    push_face(out, P[1], P[0], P[3], P[2], face_color::BLUE);    // -Z back
    push_face(out, P[3], P[7], P[6], P[2], face_color::WHITE);   // +Y up
    push_face(out, P[1], P[5], P[4], P[0], face_color::YELLOW);  // -Y down
    push_face(out, P[5], P[1], P[2], P[6], face_color::RED);     // +X right
    push_face(out, P[0], P[4], P[7], P[3], face_color::ORANGE);  // -X left
    return out;
}

// ---- Camera: copied minimal orbit camera so this directory is self-contained.
class OrbitCamera {
   public:
    void rotate(float dyaw, float dpitch) {
        yaw_ += dyaw; pitch_ += dpitch;
        const float lim = glm::radians(89.0f);
        if (pitch_ >  lim) pitch_ =  lim;
        if (pitch_ < -lim) pitch_ = -lim;
    }
    void zoom(float k) { radius_ = std::max(2.0f, radius_ * k); }
    glm::mat4 view() const {
        glm::vec3 eye = radius_ * glm::vec3(
            std::cos(pitch_) * std::sin(yaw_),
            std::sin(pitch_),
            std::cos(pitch_) * std::cos(yaw_));
        return glm::lookAt(eye, glm::vec3(0), glm::vec3(0, 1, 0));
    }
   private:
    float yaw_ = 0.6f, pitch_ = 0.4f, radius_ = 8.0f;
};

// ---- Keyboard -> face turn binding.
static FaceTurn key_to_turn(int key, bool shift) {
    int dir = shift ? -1 : +1;
    switch (key) {
        case GLFW_KEY_R: return {0, +1, dir};
        case GLFW_KEY_L: return {0, -1, -dir};
        case GLFW_KEY_U: return {1, +1, dir};
        case GLFW_KEY_D: return {1, -1, -dir};
        case GLFW_KEY_F: return {2, +1, dir};
        case GLFW_KEY_B: return {2, -1, -dir};
    }
    return {0, 0, 0};
}

int main() {
    GLFWwindow* win = gl_helpers::create_window(900, 900, "Week 4 — Rubik's");
    glfwSwapInterval(1);

    GLuint program = gl_helpers::make_program(
        WEEK04_SHADER_DIR "/cubelet.vert",
        WEEK04_SHADER_DIR "/cubelet.frag");

    auto mesh = build_cubelet_mesh();
    GLuint vao = 0, vbo = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(Vertex), mesh.data(), GL_STATIC_DRAW);

    GLint pos = glGetAttribLocation(program, "in_pos");
    GLint col = glGetAttribLocation(program, "in_color");
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(col);
    glVertexAttribPointer(col, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    GLint u_mvp = glGetUniformLocation(program, "u_mvp");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.07f, 0.07f, 0.10f, 1.0f);

    RubiksCube cube;
    OrbitCamera cam;

    // Edge-detected keys for face turns; held arrows = camera orbit.
    int turn_keys[] = {GLFW_KEY_U, GLFW_KEY_D, GLFW_KEY_L, GLFW_KEY_R,
                       GLFW_KEY_F, GLFW_KEY_B};
    int prev[6] = {0, 0, 0, 0, 0, 0};

    double t_prev = glfwGetTime();
    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();
        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(win, GLFW_TRUE);

        double t_now = glfwGetTime();
        float dt = float(t_now - t_prev);
        t_prev = t_now;

        // Camera (held)
        const float STEP = glm::radians(2.0f);
        if (glfwGetKey(win, GLFW_KEY_LEFT)  == GLFW_PRESS) cam.rotate(-STEP, 0);
        if (glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS) cam.rotate( STEP, 0);
        if (glfwGetKey(win, GLFW_KEY_UP)    == GLFW_PRESS) cam.rotate(0,  STEP);
        if (glfwGetKey(win, GLFW_KEY_DOWN)  == GLFW_PRESS) cam.rotate(0, -STEP);
        if (glfwGetKey(win, GLFW_KEY_EQUAL) == GLFW_PRESS) cam.zoom(0.97f);
        if (glfwGetKey(win, GLFW_KEY_MINUS) == GLFW_PRESS) cam.zoom(1.03f);

        // Face turns (edge-detected)
        bool shift = glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                     glfwGetKey(win, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        for (int i = 0; i < 6; ++i) {
            int s = glfwGetKey(win, turn_keys[i]);
            if (s == GLFW_PRESS && prev[i] == GLFW_RELEASE) {
                cube.queue_turn(key_to_turn(turn_keys[i], shift));
            }
            prev[i] = s;
        }

        cube.update(dt);

        int w, h;
        glfwGetFramebufferSize(win, &w, &h);
        glViewport(0, 0, w, h);
        glm::mat4 view = cam.view();
        glm::mat4 proj = glm::perspective(glm::radians(40.0f),
                                          (float)w / (float)h, 0.1f, 100.0f);
        glm::mat4 vp = proj * view;
        glm::mat4 layer_xform = cube.anim_layer_matrix();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program);
        glBindVertexArray(vao);

        for (const Cubelet& c : cube.cubelets()) {
            glm::mat4 model = c.model_matrix();
            if (cube.cubelet_in_active_layer(c)) model = layer_xform * model;
            glm::mat4 mvp = vp * model;
            glUniformMatrix4fv(u_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);
        glfwSwapBuffers(win);
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
    glfwDestroyWindow(win);
    glfwTerminate();
}
