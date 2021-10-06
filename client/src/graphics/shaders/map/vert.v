#version 330 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec4 Color;

uniform mat4 matrix;
uniform mat4 ml;

out VS_OUT
{
  vec2 tc;
} vs_out;


void main()
{
    vs_out.tc = TexCoord;
    gl_Position = vec4(Pos, 1.0f);
}