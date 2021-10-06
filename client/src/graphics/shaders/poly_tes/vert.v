#version 330 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Color;
layout (location = 2) in float Pid;
layout (location = 3) in vec3 Normal;

uniform mat4 matrix;
uniform sampler2D tex[32];

//out vec3 col;
//out float ProvId;
//out vec2 tc;
//out vec3 pos;
//out vec3 normal;

out VS_OUT
{
  vec2 tc;
  float provId;
  //vec3 normal;
} vs_out;

void main()
{
float mapWidth = 5632.0f, mapHeight = 2048.0f, scale = 4.0f;
vec2 Tc = vec2(Pos.x / (mapWidth * scale), Pos.y / (mapHeight * scale));
    vs_out.tc = Tc;
    vs_out.provId = Pid;
gl_Position = vec4(Pos, 1.0f);
//ProvId = Pid;
//col = Color;
////Col = vec4(1.0, 0.0, 0.0, 1.0);
//float z = texture(tex[3], Tc).r * 255.0f;//4.0 * 2.0 * 30.0 + 0.0f;
//tc = Tc;
//z = Pos.z + .0f;
////z = 80.0f;
//gl_Position = matrix * vec4(Pos.xy, z, 1.0f);
//vs_out.normal = normalize(Normal);
//    pos = Pos;
}
