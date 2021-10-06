#version 330 core

layout (location = 0) in vec3 Pos;
//layout (location = 1) in vec4 Color;
//layout (location = 2) in float textureID;
layout (location = 1) in vec2 TexCoord;
//layout (location = 4) in vec2 normal;

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
