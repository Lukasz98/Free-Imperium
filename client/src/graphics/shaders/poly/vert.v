#version 330 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Color;
layout (location = 2) in float Tc;

uniform mat4 matrix;
uniform sampler2D tex[32];

out vec3 col;
out float ProvId;

void main()
{
ProvId = Tc;
col = Color;
//Col = vec4(1.0, 0.0, 0.0, 1.0);
    float mapWidth = 5632.0f, mapHeight = 2048.0f, scale = 4.0f;
vec2 Tc = vec2(Pos.x / (mapWidth * scale), Pos.y / (mapHeight * scale));
float z = texture(tex[3], Tc).r * 4.0 * 2.0 * 30.0 + 0.0f;
    gl_Position = matrix * vec4(Pos.xy, z, 1.0f);
    //gl_Position = matrix * vec4(Pos.xy, Pos.z - 5.0f, 1.0f);
}
