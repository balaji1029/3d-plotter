#version 330 core

in vec3 in_pos;
in vec3 in_color;

uniform mat4 u_mvp;

out vec3 v_color;

void main() {
    v_color = in_color;
    gl_Position = u_mvp * vec4(in_pos, 1.0);
}
