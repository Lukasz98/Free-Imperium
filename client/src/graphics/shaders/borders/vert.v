#version 330 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec4 Color;
layout (location = 2) in vec2 Tc;

uniform mat4 matrix;
uniform sampler2D tex[32];

out vec4 col;


void main()
{
col = Color;
float z = texture(tex[3], Tc).r;// * 4.0f * 2.0f * 30.0f + 10.0f;
    gl_Position = matrix * vec4(Pos.xy, z * 255.0f - 0.0f, 1.0f);
}
