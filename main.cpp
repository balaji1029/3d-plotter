#include "gl_framework.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtc/type_ptr.hpp"
#include <cmath>

float xrot = 0.0f;
float yrot = 0.0f;
float zrot = 0.0f;

int main() {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "3D Plotter", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "GLEW Init Failed : %s" << std::endl;
    }

    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    initGL();

    std::vector<GLuint> shaders;
    shaders.push_back(LoadShaderGL(GL_VERTEX_SHADER, "vertex_shader.glsl"));
    shaders.push_back(LoadShaderGL(GL_FRAGMENT_SHADER, "fragment_shader.glsl"));

    GLuint program = CreateProgramGL(shaders);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    GLuint vColor = glGetAttribLocation(program, "vColor");
    GLuint uModelViewProjectionMatrix = glGetUniformLocation(program, "uModelViewProjectMatrix");

    // Enable front-face culling so only back faces are visible
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    // ---- CUBE (wireframe-style with uniform gray color) ----
    // 24 vertices: 4 per face, each with the same color per face
    GLfloat cube_vertices[] = {
        // Front face (z = -1) — dark gray
        -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        // Back face (z = +1)
        -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,
        // Bottom face (y = -1)
        -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
        // Top face (y = +1)
        -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,
        // Left face (x = -1)
        -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
        // Right face (x = +1)
         1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f,
    };

    // All vertices get the same muted gray color
    GLfloat cube_colors[24 * 3];
    for (int i = 0; i < 24; i++) {
        cube_colors[i * 3 + 0] = 0.25f;
        cube_colors[i * 3 + 1] = 0.25f;
        cube_colors[i * 3 + 2] = 0.30f;
    }

    GLushort cube_indices[] = {
        // Front face
         0,  2,  1,  2,  0,  3,
        // Back face
         4,  5,  6,  6,  7,  4,
        // Bottom face
         8,  9, 10, 10, 11,  8,
        // Top face
        12, 14, 13, 14, 12, 15,
        // Left face
        16, 18, 17, 18, 16, 19,
        // Right face
        20, 21, 22, 22, 23, 20,
    };

    GLuint cube_vao, cube_vbo_pos, cube_vbo_col, cube_ebo;
    glGenVertexArrays(1, &cube_vao);
    glBindVertexArray(cube_vao);

    glGenBuffers(1, &cube_vbo_pos);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_pos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenBuffers(1, &cube_vbo_col);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_col);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenBuffers(1, &cube_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    // ---- AXES (X=red, Y=green, Z=blue) ----
    GLfloat axes_vertices[] = {
        // X axis
        -1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
        // Y axis
         0.0f,-1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
        // Z axis
         0.0f, 0.0f,-1.0f,   0.0f, 0.0f, 1.0f,
    };

    GLfloat axes_colors[] = {
        // X axis — red
        1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
        // Y axis — green
        0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
        // Z axis — blue
        0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
    };

    GLuint axes_vao, axes_vbo_pos, axes_vbo_col;
    glGenVertexArrays(1, &axes_vao);
    glBindVertexArray(axes_vao);

    glGenBuffers(1, &axes_vbo_pos);
    glBindBuffer(GL_ARRAY_BUFFER, axes_vbo_pos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axes_vertices), axes_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenBuffers(1, &axes_vbo_col);
    glBindBuffer(GL_ARRAY_BUFFER, axes_vbo_col);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axes_colors), axes_colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);

    // ---- SURFACE PLOT: z = sin(x * pi) * cos(y * pi) ----
    const int GRID_N = 50;
    const int SURF_VERTS = GRID_N * GRID_N;
    const int SURF_TRIS = (GRID_N - 1) * (GRID_N - 1) * 6;

    std::vector<GLfloat> surf_vertices(SURF_VERTS * 3);
    std::vector<GLfloat> surf_colors(SURF_VERTS * 3);
    std::vector<GLushort> surf_indices(SURF_TRIS);

    for (int iy = 0; iy < GRID_N; iy++) {
        for (int ix = 0; ix < GRID_N; ix++) {
            int idx = iy * GRID_N + ix;
            float x = -1.0f + 2.0f * ix / (GRID_N - 1);
            float y = -1.0f + 2.0f * iy / (GRID_N - 1);
            float z = sinf(x * M_PI) * cosf(y * M_PI);

            surf_vertices[idx * 3 + 0] = x;
            surf_vertices[idx * 3 + 1] = z;  // z value mapped to Y axis (height)
            surf_vertices[idx * 3 + 2] = y;  // y input mapped to Z axis (depth)

            // Color by height: blue (low) -> green (mid) -> yellow (high)
            float t = (z + 1.0f) * 0.5f; // normalize to [0, 1]
            surf_colors[idx * 3 + 0] = t;              // R
            surf_colors[idx * 3 + 1] = 0.4f + 0.6f * (1.0f - fabsf(2.0f * t - 1.0f)); // G
            surf_colors[idx * 3 + 2] = 1.0f - t;       // B
        }
    }

    int ti = 0;
    for (int iy = 0; iy < GRID_N - 1; iy++) {
        for (int ix = 0; ix < GRID_N - 1; ix++) {
            GLushort tl = iy * GRID_N + ix;
            GLushort tr = tl + 1;
            GLushort bl = tl + GRID_N;
            GLushort br = bl + 1;
            // Two triangles per quad
            surf_indices[ti++] = tl;
            surf_indices[ti++] = bl;
            surf_indices[ti++] = tr;
            surf_indices[ti++] = tr;
            surf_indices[ti++] = bl;
            surf_indices[ti++] = br;
        }
    }

    GLuint surf_vao, surf_vbo_pos, surf_vbo_col, surf_ebo;
    glGenVertexArrays(1, &surf_vao);
    glBindVertexArray(surf_vao);

    glGenBuffers(1, &surf_vbo_pos);
    glBindBuffer(GL_ARRAY_BUFFER, surf_vbo_pos);
    glBufferData(GL_ARRAY_BUFFER, surf_vertices.size() * sizeof(GLfloat), surf_vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenBuffers(1, &surf_vbo_col);
    glBindBuffer(GL_ARRAY_BUFFER, surf_vbo_col);
    glBufferData(GL_ARRAY_BUFFER, surf_colors.size() * sizeof(GLfloat), surf_colors.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenBuffers(1, &surf_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surf_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, surf_indices.size() * sizeof(GLushort), surf_indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 c_ortho_matrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -10.0f, 10.0f);
        glm::mat4 c_view_matrix = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 5.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glm::mat4 c_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(xrot), glm::vec3(1.0f, 0.0f, 0.0f));
        c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(yrot), glm::vec3(0.0f, 1.0f, 0.0f));
        c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(zrot), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 mvp = c_ortho_matrix * c_view_matrix * c_rotation_matrix;

        glUseProgram(program);
        glUniformMatrix4fv(uModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(mvp));

        // Draw cube
        glBindVertexArray(cube_vao);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
        glBindVertexArray(0);

        // Draw axes (disable face culling for lines)
        glDisable(GL_CULL_FACE);

        glBindVertexArray(axes_vao);
        glLineWidth(2.0f);
        glDrawArrays(GL_LINES, 0, 6);
        glBindVertexArray(0);

        // Draw surface
        glBindVertexArray(surf_vao);
        glDrawElements(GL_TRIANGLES, SURF_TRIS, GL_UNSIGNED_SHORT, (void*)0);
        glBindVertexArray(0);

        // Re-enable face culling for cube next frame
        glEnable(GL_CULL_FACE);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &cube_vbo_pos);
    glDeleteBuffers(1, &cube_vbo_col);
    glDeleteBuffers(1, &cube_ebo);
    glDeleteVertexArrays(1, &cube_vao);
    glDeleteBuffers(1, &axes_vbo_pos);
    glDeleteBuffers(1, &axes_vbo_col);
    glDeleteVertexArrays(1, &axes_vao);
    glDeleteBuffers(1, &surf_vbo_pos);
    glDeleteBuffers(1, &surf_vbo_col);
    glDeleteBuffers(1, &surf_ebo);
    glDeleteVertexArrays(1, &surf_vao);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}