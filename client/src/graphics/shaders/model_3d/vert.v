#version 330 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

uniform mat4 matrix;
uniform mat4 ml;

out vec2 texCoord;
out vec4 col;
out vec3 normal;

void main()
{
    texCoord = TexCoord;
    normal = Normal;
    gl_Position = matrix * ml * vec4(Pos, 1.0f);
    col = vec4(0.0, 0.0, 0.0, 1.0);
}