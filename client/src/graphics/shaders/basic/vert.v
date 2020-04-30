#version 330 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec4 Color;

uniform mat4 matrix;
uniform mat4 ml;

out vec2 texCoord;
out vec4 col;

void main()
{
    col = Color;
    texCoord = TexCoord;
    gl_Position = matrix * vec4(Pos, 1.0f);
}
