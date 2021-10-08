#version 330 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec4 Color;

uniform mat4 matrix;

out vec4 col;

void main()
{
    col = Color;
    gl_Position = matrix * vec4(Pos, 1.0f);
}
