#version 330 core

layout (location = 0) in vec3 Pos;
//layout (location = 1) in vec4 Color;
layout (location = 1) in vec2 Tc;

uniform mat4 matrix;
uniform sampler2D tex[32];

out vec4 Col;


void main()
{
//col = Color;
Col = vec4(1.0, 0.0, 0.0, 1.0);
float z = texture(tex[3], Tc).r * 4.0 * 2.0 * 30.0 + 1.0f;
    gl_Position = matrix * vec4(Pos.xy, z * 1.0f - 0.0f, 1.0f);
    //gl_Position = matrix * vec4(Pos.xy, Pos.z - 5.0f, 1.0f);
}
