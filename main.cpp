#include "gl_framework.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtc/type_ptr.hpp"
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

float xrot = 0.0f;
float yrot = 0.0f;
float zrot = 0.0f;

struct PlotData {
    std::vector<GLfloat> verts;
    std::vector<GLfloat> colors;
    std::vector<GLuint>  indices;
    GLenum primitive = GL_TRIANGLES;
    int    count = 0;
    bool   use_indices = true;
    const char* name = "";
};

// Viridis colormap — polynomial approximation (Matrosov 2018).
// Perceptually uniform, monotonic in luminance, colorblind-friendly.
static void rainbow(float t, float* rgb) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    const float c0[3] = { 0.2777273272f,  0.0054073445f,  0.3340998053f};
    const float c1[3] = { 0.1050930431f,  1.4046135299f,  1.3845901626f};
    const float c2[3] = {-0.3308618287f,  0.2148475595f,  0.0950951630f};
    const float c3[3] = {-4.6342304990f, -5.7991009734f, -19.3324409563f};
    const float c4[3] = { 6.2282699363f, 14.1799333668f,  56.6905526007f};
    const float c5[3] = { 4.7763849977f,-13.7451453777f, -65.3530326334f};
    const float c6[3] = {-5.4354558559f,  4.6458526122f,  26.3124352496f};
    for (int i = 0; i < 3; i++) {
        float v = c0[i] + t*(c1[i] + t*(c2[i] + t*(c3[i] + t*(c4[i] + t*(c5[i] + t*c6[i])))));
        if (v < 0.0f) v = 0.0f;
        if (v > 1.0f) v = 1.0f;
        rgb[i] = v;
    }
}

// Normalize points to fit the cube, then write vertices + height-based rainbow colors.
static void fitAndColor(PlotData& d, std::vector<glm::vec3>& pts, bool auto_fit) {
    glm::vec3 mn(1e9f), mx(-1e9f);
    for (auto& p : pts) { mn = glm::min(mn, p); mx = glm::max(mx, p); }
    glm::vec3 c = (mn + mx) * 0.5f;
    glm::vec3 ext = (mx - mn);
    float s = auto_fit ? 1.8f / std::max({ext.x, ext.y, ext.z, 1e-6f}) : 1.0f;

    float ymin = 1e9f, ymax = -1e9f;
    d.verts.resize(pts.size() * 3);
    for (size_t i = 0; i < pts.size(); i++) {
        glm::vec3 p = auto_fit ? (pts[i] - c) * s : pts[i];
        pts[i] = p;
        d.verts[i*3+0] = p.x;
        d.verts[i*3+1] = p.y;
        d.verts[i*3+2] = p.z;
        if (p.y < ymin) ymin = p.y;
        if (p.y > ymax) ymax = p.y;
    }
    d.colors.resize(pts.size() * 3);
    float yspan = (ymax - ymin) > 1e-6f ? (ymax - ymin) : 1.0f;
    for (size_t i = 0; i < pts.size(); i++) {
        float t = (pts[i].y - ymin) / yspan;
        rainbow(t, &d.colors[i*3]);
    }
}

static void meshGrid(PlotData& d, std::vector<glm::vec3>& pts, int NU, int NV,
                     bool close_u, bool close_v, bool auto_fit) {
    fitAndColor(d, pts, auto_fit);
    int uStep = close_u ? NU : (NU - 1);
    int vStep = close_v ? NV : (NV - 1);
    d.indices.reserve((size_t)uStep * vStep * 6);
    for (int iv = 0; iv < vStep; iv++) {
        for (int iu = 0; iu < uStep; iu++) {
            int iu1 = (iu + 1) % NU;
            int iv1 = (iv + 1) % NV;
            GLuint a = iv  * NU + iu;
            GLuint b = iv  * NU + iu1;
            GLuint cc = iv1 * NU + iu;
            GLuint dd = iv1 * NU + iu1;
            d.indices.push_back(a);  d.indices.push_back(cc); d.indices.push_back(b);
            d.indices.push_back(b);  d.indices.push_back(cc); d.indices.push_back(dd);
        }
    }
    d.primitive = GL_TRIANGLES;
    d.count = (int)d.indices.size();
    d.use_indices = true;
}

static void meshCurve(PlotData& d, std::vector<glm::vec3>& pts, bool auto_fit) {
    fitAndColor(d, pts, auto_fit);
    // Overwrite colors with a parameter-based rainbow (more useful for curves)
    for (size_t i = 0; i < pts.size(); i++) {
        float t = (float)i / (pts.size() - 1);
        rainbow(t, &d.colors[i*3]);
    }
    d.primitive = GL_LINE_STRIP;
    d.count = (int)pts.size();
    d.use_indices = false;
}

static PlotData buildPlot(int idx) {
    PlotData d;
    std::vector<glm::vec3> pts;

    switch (idx) {
        case 0: { // Sombrero
            d.name = "1: Sombrero  z = sin(r)/r";
            const int NU = 120, NV = 120;
            const float DOMAIN = 8.0f;
            pts.reserve(NU * NV);
            for (int iv = 0; iv < NV; iv++)
                for (int iu = 0; iu < NU; iu++) {
                    float u = -1.0f + 2.0f * iu / (NU - 1);
                    float v = -1.0f + 2.0f * iv / (NV - 1);
                    float rx = u * DOMAIN, ry = v * DOMAIN;
                    float r = sqrtf(rx*rx + ry*ry);
                    float z = (r < 1e-4f) ? 1.0f : sinf(r) / r;
                    pts.emplace_back(u, z * 0.9f, v);
                }
            meshGrid(d, pts, NU, NV, false, false, false);
            break;
        }
        case 1: { // Egg crate
            d.name = "2: Egg crate  z = sin(x)cos(y)";
            const int NU = 80, NV = 80;
            pts.reserve(NU * NV);
            for (int iv = 0; iv < NV; iv++)
                for (int iu = 0; iu < NU; iu++) {
                    float u = -1.0f + 2.0f * iu / (NU - 1);
                    float v = -1.0f + 2.0f * iv / (NV - 1);
                    float z = sinf(u * 4.0f) * cosf(v * 4.0f);
                    pts.emplace_back(u, z * 0.7f, v);
                }
            meshGrid(d, pts, NU, NV, false, false, false);
            break;
        }
        case 2: { // Monkey saddle
            d.name = "3: Monkey saddle  z = x^3 - 3xy^2";
            const int NU = 80, NV = 80;
            pts.reserve(NU * NV);
            for (int iv = 0; iv < NV; iv++)
                for (int iu = 0; iu < NU; iu++) {
                    float u = -1.0f + 2.0f * iu / (NU - 1);
                    float v = -1.0f + 2.0f * iv / (NV - 1);
                    float z = u*u*u - 3.0f*u*v*v;
                    pts.emplace_back(u, z * 0.5f, v);
                }
            meshGrid(d, pts, NU, NV, false, false, false);
            break;
        }
        case 3: { // Enneper
            d.name = "4: Enneper surface";
            const int NU = 80, NV = 80;
            pts.reserve(NU * NV);
            for (int iv = 0; iv < NV; iv++)
                for (int iu = 0; iu < NU; iu++) {
                    float u = -1.2f + 2.4f * iu / (NU - 1);
                    float v = -1.2f + 2.4f * iv / (NV - 1);
                    float x = u - u*u*u/3.0f + u*v*v;
                    float y = v - v*v*v/3.0f + v*u*u;
                    float z = u*u - v*v;
                    pts.emplace_back(x, z, y);
                }
            meshGrid(d, pts, NU, NV, false, false, true);
            break;
        }
        case 4: { // Klein bottle (figure-8 immersion)
            d.name = "5: Klein bottle";
            const int NU = 100, NV = 40;
            pts.reserve(NU * NV);
            const float R = 2.0f, rr = 1.0f;
            for (int iv = 0; iv < NV; iv++)
                for (int iu = 0; iu < NU; iu++) {
                    float u = 2.0f * (float)M_PI * iu / NU;
                    float v = 2.0f * (float)M_PI * iv / NV;
                    float base = R + rr*cosf(u/2.0f)*sinf(v) - rr*sinf(u/2.0f)*sinf(2.0f*v);
                    float x = base * cosf(u);
                    float y = base * sinf(u);
                    float z = rr*sinf(u/2.0f)*sinf(v) + rr*cosf(u/2.0f)*sinf(2.0f*v);
                    pts.emplace_back(x, z, y);
                }
            meshGrid(d, pts, NU, NV, true, true, true);
            break;
        }
        case 5: { // Möbius strip
            d.name = "6: Mobius strip";
            const int NU = 180, NV = 10;
            pts.reserve(NU * NV);
            for (int iv = 0; iv < NV; iv++)
                for (int iu = 0; iu < NU; iu++) {
                    float u = 2.0f * (float)M_PI * iu / NU;
                    float v = -0.4f + 0.8f * iv / (NV - 1);
                    float x = (1.0f + v * cosf(u/2.0f)) * cosf(u);
                    float y = (1.0f + v * cosf(u/2.0f)) * sinf(u);
                    float z = v * sinf(u/2.0f);
                    pts.emplace_back(x, z, y);
                }
            meshGrid(d, pts, NU, NV, true, false, true);
            break;
        }
        case 6: { // Dini's surface
            d.name = "7: Dini's surface";
            const int NU = 120, NV = 60;
            pts.reserve(NU * NV);
            const float a = 1.0f, b = 0.2f;
            for (int iv = 0; iv < NV; iv++)
                for (int iu = 0; iu < NU; iu++) {
                    float u = 4.0f * (float)M_PI * iu / (NU - 1);
                    float v = 0.1f + 1.9f * iv / (NV - 1);
                    float x = a * cosf(u) * sinf(v);
                    float y = a * sinf(u) * sinf(v);
                    float z = a * (cosf(v) + logf(tanf(v/2.0f))) + b * u;
                    pts.emplace_back(x, z, y);
                }
            meshGrid(d, pts, NU, NV, false, false, true);
            break;
        }
        case 7: { // Torus
            d.name = "8: Torus";
            const int NU = 80, NV = 40;
            pts.reserve(NU * NV);
            const float R = 0.6f, rr = 0.25f;
            for (int iv = 0; iv < NV; iv++)
                for (int iu = 0; iu < NU; iu++) {
                    float u = 2.0f * (float)M_PI * iu / NU;
                    float v = 2.0f * (float)M_PI * iv / NV;
                    float x = (R + rr * cosf(v)) * cosf(u);
                    float y = (R + rr * cosf(v)) * sinf(u);
                    float z = rr * sinf(v);
                    pts.emplace_back(x, z, y);
                }
            meshGrid(d, pts, NU, NV, true, true, false);
            break;
        }
        case 8: { // Flower / rose curve extruded
            d.name = "9: Flower  r=cos(k theta), z=sin(k theta)";
            const int NU = 240, NV = 8;
            pts.reserve(NU * NV);
            const int k = 5;
            for (int iv = 0; iv < NV; iv++)
                for (int iu = 0; iu < NU; iu++) {
                    float theta = 2.0f * (float)M_PI * iu / NU;
                    float h = -0.3f + 0.6f * iv / (NV - 1);
                    float r = fabsf(cosf(k * theta));
                    float x = r * cosf(theta);
                    float y = r * sinf(theta);
                    float z = h + 0.25f * sinf(k * theta);
                    pts.emplace_back(x, z, y);
                }
            meshGrid(d, pts, NU, NV, true, false, true);
            break;
        }
        case 9: { // Trefoil knot
            d.name = "0: Trefoil knot";
            const int N = 800;
            pts.reserve(N);
            for (int i = 0; i < N; i++) {
                float t = 2.0f * (float)M_PI * i / (N - 1);
                float x = sinf(t) + 2.0f * sinf(2.0f*t);
                float y = cosf(t) - 2.0f * cosf(2.0f*t);
                float z = -sinf(3.0f*t);
                pts.emplace_back(x, z, y);
            }
            meshCurve(d, pts, true);
            break;
        }
        case 10: { // Torus knot (3,7)
            d.name = "-: Torus knot (3,7)";
            const int N = 1500;
            const int p = 3, q = 7;
            const float R = 1.0f, rr = 0.3f;
            pts.reserve(N);
            for (int i = 0; i < N; i++) {
                float t = 2.0f * (float)M_PI * i / (N - 1);
                float x = (R + rr * cosf(q*t)) * cosf(p*t);
                float y = (R + rr * cosf(q*t)) * sinf(p*t);
                float z = rr * sinf(q*t);
                pts.emplace_back(x, z, y);
            }
            meshCurve(d, pts, true);
            break;
        }
        case 11: { // Lorenz attractor
            d.name = "=: Lorenz attractor";
            const int N = 12000;
            const float dt = 0.005f;
            const float sigma = 10.0f, rho = 28.0f, beta = 8.0f/3.0f;
            pts.reserve(N);
            float x = 0.1f, y = 0.0f, z = 0.0f;
            for (int i = 0; i < N; i++) {
                float dx = sigma * (y - x);
                float dy = x * (rho - z) - y;
                float dz = x * y - beta * z;
                x += dx * dt;
                y += dy * dt;
                z += dz * dt;
                pts.emplace_back(x, z, y);
            }
            meshCurve(d, pts, true);
            break;
        }
        case 12: { // Two-source interference: 3 sin(r1) + 2 sin(r2)
            d.name = "Q: Interference  3 sin(r) + 2 sin(r from (5,5))";
            const int NU = 160, NV = 160;
            // Wave sources sit at half-coord (0,0) and (5,5) → midpoint (2.5, 2.5).
            // With hx = 0.4*X, that's X = 6.25. Center the sampling there so the
            // visual pattern (not just the origin) sits at the cube's center.
            const float CENTER = 6.25f;
            const float DOM_MIN = CENTER - 37.5f, DOM_MAX = CENTER + 37.5f;
            pts.reserve(NU * NV);
            for (int iv = 0; iv < NV; iv++)
                for (int iu = 0; iu < NU; iu++) {
                    float u = -1.0f + 2.0f * iu / (NU - 1);   // display x in [-1, 1]
                    float v = -1.0f + 2.0f * iv / (NV - 1);   // display z in [-1, 1]
                    // Map to problem-space (x, y)
                    float X = DOM_MIN + (DOM_MAX - DOM_MIN) * iu / (NU - 1);
                    float Y = DOM_MIN + (DOM_MAX - DOM_MIN) * iv / (NV - 1);
                    float hx = X * 0.4f, hy = Y * 0.4f;   // apply x -> x/2, y -> y/2
                    float r1 = sqrtf(hx*hx + hy*hy);
                    float r2 = sqrtf((hx - 5.0f)*(hx - 5.0f) + (hy - 5.0f)*(hy - 5.0f));
                    float z = 3.0f * sinf(r1) + 2.0f * sinf(r2);
                    pts.emplace_back(u, z * 0.05f, v);  // scale z to fit cube (|z| <= 5)
                }
            meshGrid(d, pts, NU, NV, false, false, false);
            break;
        }
    }
    return d;
}

constexpr int PLOT_COUNT = 13;

static void savePPM(const char* path, int w, int h, const unsigned char* pixels) {
    FILE* f = fopen(path, "wb");
    if (!f) { perror(path); return; }
    fprintf(f, "P6\n%d %d\n255\n", w, h);
    // OpenGL origin is bottom-left, PPM is top-left — flip rows
    for (int y = h - 1; y >= 0; y--) fwrite(pixels + (size_t)y * w * 3, 1, (size_t)w * 3, f);
    fclose(f);
}

int main(int argc, char** argv) {
    // Capture mode: --capture <N> <plot_idx>  (defaults 90 frames, plot 12)
    int   cap_frames = 0;
    int   cap_plot   = 12;
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--capture") {
            cap_frames = (i + 1 < argc) ? atoi(argv[i+1]) : 90;
            if (i + 2 < argc) cap_plot = atoi(argv[i+2]);
            if (cap_frames <= 0) cap_frames = 90;
        }
    }

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "3D Plotter", NULL, NULL);
    if (!window) { glfwTerminate(); exit(EXIT_FAILURE); }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) std::cerr << "GLEW Init Failed" << std::endl;

    std::cout << "Vendor: "        << glGetString(GL_VENDOR)   << std::endl;
    std::cout << "Renderer: "      << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: "       << glGetString(GL_VERSION)  << std::endl;
    std::cout << "GLSL Version: "  << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "\nSwitch plots: keys 1-9, 0, -, =   Cycle: N / P   Quit: ESC\n" << std::endl;

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    initGL();

    std::vector<GLuint> shaders;
    shaders.push_back(LoadShaderGL(GL_VERTEX_SHADER,   "vertex_shader.glsl"));
    shaders.push_back(LoadShaderGL(GL_FRAGMENT_SHADER, "fragment_shader.glsl"));
    GLuint program = CreateProgramGL(shaders);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    GLuint vColor    = glGetAttribLocation(program, "vColor");
    GLuint uModelViewProjectionMatrix = glGetUniformLocation(program, "uModelViewProjectMatrix");

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    // ---- CUBE ----
    GLfloat cube_vertices[] = {
        -1,-1,-1,  1,-1,-1,  1, 1,-1, -1, 1,-1,
        -1,-1, 1,  1,-1, 1,  1, 1, 1, -1, 1, 1,
        -1,-1,-1,  1,-1,-1,  1,-1, 1, -1,-1, 1,
        -1, 1,-1,  1, 1,-1,  1, 1, 1, -1, 1, 1,
        -1,-1,-1, -1, 1,-1, -1, 1, 1, -1,-1, 1,
         1,-1,-1,  1, 1,-1,  1, 1, 1,  1,-1, 1,
    };
    GLfloat cube_colors[24 * 3];
    for (int i = 0; i < 24; i++) {
        cube_colors[i*3+0] = 0.25f; cube_colors[i*3+1] = 0.25f; cube_colors[i*3+2] = 0.30f;
    }
    GLushort cube_indices[] = {
         0, 2, 1,  2, 0, 3,
         4, 5, 6,  6, 7, 4,
         8, 9,10, 10,11, 8,
        12,14,13, 14,12,15,
        16,18,17, 18,16,19,
        20,21,22, 22,23,20,
    };

    GLuint cube_vao, cube_vbo_pos, cube_vbo_col, cube_ebo;
    glGenVertexArrays(1, &cube_vao);
    glBindVertexArray(cube_vao);
    glGenBuffers(1, &cube_vbo_pos);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_pos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glGenBuffers(1, &cube_vbo_col);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_col);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glGenBuffers(1, &cube_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    glBindVertexArray(0);

    // ---- AXES ----
    GLfloat axes_vertices[] = {
        -1, 0, 0,  1, 0, 0,
         0,-1, 0,  0, 1, 0,
         0, 0,-1,  0, 0, 1,
    };
    GLfloat axes_colors[] = {
        1,0,0, 1,0,0,
        0,1,0, 0,1,0,
        0,0,1, 0,0,1,
    };
    GLuint axes_vao, axes_vbo_pos, axes_vbo_col;
    glGenVertexArrays(1, &axes_vao);
    glBindVertexArray(axes_vao);
    glGenBuffers(1, &axes_vbo_pos);
    glBindBuffer(GL_ARRAY_BUFFER, axes_vbo_pos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axes_vertices), axes_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glGenBuffers(1, &axes_vbo_col);
    glBindBuffer(GL_ARRAY_BUFFER, axes_vbo_col);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axes_colors), axes_colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindVertexArray(0);

    // ---- PLOT (dynamic) ----
    GLuint plot_vao, plot_vbo_pos, plot_vbo_col, plot_ebo;
    glGenVertexArrays(1, &plot_vao);
    glBindVertexArray(plot_vao);
    glGenBuffers(1, &plot_vbo_pos);
    glBindBuffer(GL_ARRAY_BUFFER, plot_vbo_pos);
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glGenBuffers(1, &plot_vbo_col);
    glBindBuffer(GL_ARRAY_BUFFER, plot_vbo_col);
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glGenBuffers(1, &plot_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plot_ebo);
    glBindVertexArray(0);

    int current_plot = 0;
    bool plot_dirty = true;
    PlotData plot;

    int cap_idx = 0;
    std::vector<unsigned char> cap_buf;
    if (cap_frames > 0) {
        current_plot = cap_plot;
        cap_buf.resize(800 * 800 * 3);
        std::printf("[capture] %d frames of plot %d -> /tmp/plot_frame_XXX.ppm\n",
                    cap_frames, cap_plot);
    }

    // Edge-detected key map: GLFW key -> plot index
    struct KeyMap { int key; int plot; };
    KeyMap keymap[] = {
        {GLFW_KEY_1, 0}, {GLFW_KEY_2, 1}, {GLFW_KEY_3, 2},  {GLFW_KEY_4, 3},
        {GLFW_KEY_5, 4}, {GLFW_KEY_6, 5}, {GLFW_KEY_7, 6},  {GLFW_KEY_8, 7},
        {GLFW_KEY_9, 8}, {GLFW_KEY_0, 9}, {GLFW_KEY_MINUS,10}, {GLFW_KEY_EQUAL,11},
        {GLFW_KEY_Q, 12},
    };
    int prev_state[16] = {0};
    int prev_n = GLFW_RELEASE, prev_p = GLFW_RELEASE;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (cap_frames > 0) {
            // Capture mode: drive rotation deterministically, skip interactive input
            xrot = 25.0f;
            yrot = 360.0f * cap_idx / cap_frames;
            zrot = 0.0f;
        } else {
            // Direct plot selection
            for (int i = 0; i < (int)(sizeof(keymap)/sizeof(keymap[0])); i++) {
                int s = glfwGetKey(window, keymap[i].key);
                if (s == GLFW_PRESS && prev_state[i] == GLFW_RELEASE) {
                    current_plot = keymap[i].plot;
                    plot_dirty = true;
                }
                prev_state[i] = s;
            }
            // Cycle next / previous
            int sn = glfwGetKey(window, GLFW_KEY_N);
            if (sn == GLFW_PRESS && prev_n == GLFW_RELEASE) {
                current_plot = (current_plot + 1) % PLOT_COUNT;
                plot_dirty = true;
            }
            prev_n = sn;
            int sp = glfwGetKey(window, GLFW_KEY_P);
            if (sp == GLFW_PRESS && prev_p == GLFW_RELEASE) {
                current_plot = (current_plot + PLOT_COUNT - 1) % PLOT_COUNT;
                plot_dirty = true;
            }
            prev_p = sp;
        }

        if (plot_dirty) {
            plot = buildPlot(current_plot);
            std::printf("[plot] %s\n", plot.name);
            glBindVertexArray(plot_vao);
            glBindBuffer(GL_ARRAY_BUFFER, plot_vbo_pos);
            glBufferData(GL_ARRAY_BUFFER, plot.verts.size() * sizeof(GLfloat),
                         plot.verts.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(vPosition);
            glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glBindBuffer(GL_ARRAY_BUFFER, plot_vbo_col);
            glBufferData(GL_ARRAY_BUFFER, plot.colors.size() * sizeof(GLfloat),
                         plot.colors.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(vColor);
            glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
            if (plot.use_indices) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plot_ebo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, plot.indices.size() * sizeof(GLuint),
                             plot.indices.data(), GL_STATIC_DRAW);
            }
            glBindVertexArray(0);
            plot_dirty = false;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -10.0f, 10.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0,0,5), glm::vec3(0,0,0), glm::vec3(0,1,0));
        glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(xrot), glm::vec3(1,0,0));
        rot = glm::rotate(rot, glm::radians(yrot), glm::vec3(0,1,0));
        rot = glm::rotate(rot, glm::radians(zrot), glm::vec3(0,0,1));
        glm::mat4 mvp = proj * view * rot;

        glUseProgram(program);
        glUniformMatrix4fv(uModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(mvp));

        // Cube (front-face culled)
        glEnable(GL_CULL_FACE);
        glBindVertexArray(cube_vao);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);

        // Axes + plot — no culling
        glDisable(GL_CULL_FACE);
        glLineWidth(2.0f);

        glBindVertexArray(axes_vao);
        glDrawArrays(GL_LINES, 0, 6);
        glBindVertexArray(0);

        glBindVertexArray(plot_vao);
        if (plot.use_indices) {
            glDrawElements(plot.primitive, plot.count, GL_UNSIGNED_INT, 0);
        } else {
            glDrawArrays(plot.primitive, 0, plot.count);
        }
        glBindVertexArray(0);

        if (cap_frames > 0) {
            glFinish();
            glReadPixels(0, 0, 800, 800, GL_RGB, GL_UNSIGNED_BYTE, cap_buf.data());
            char path[64];
            snprintf(path, sizeof(path), "/tmp/plot_frame_%03d.ppm", cap_idx);
            savePPM(path, 800, 800, cap_buf.data());
            cap_idx++;
            if (cap_idx >= cap_frames) glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        glfwSwapBuffers(window);
    }

    glDeleteBuffers(1, &cube_vbo_pos);
    glDeleteBuffers(1, &cube_vbo_col);
    glDeleteBuffers(1, &cube_ebo);
    glDeleteVertexArrays(1, &cube_vao);
    glDeleteBuffers(1, &axes_vbo_pos);
    glDeleteBuffers(1, &axes_vbo_col);
    glDeleteVertexArrays(1, &axes_vao);
    glDeleteBuffers(1, &plot_vbo_pos);
    glDeleteBuffers(1, &plot_vbo_col);
    glDeleteBuffers(1, &plot_ebo);
    glDeleteVertexArrays(1, &plot_vao);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
