#version 450 core

// in vec4 col;
// layout(binding=1) uniform sampler2D texID;
// layout(binding=3) uniform sampler2D grassTexID;
// layout(binding=4) uniform sampler2D stoneTexID;
// layout(binding=5) uniform sampler2D waterTexID;
// layout(binding=6) uniform sampler2D cBordersTexID;
// layout(binding=7) uniform sampler2D pBordersTexID;

// layout (binding=0) uniform2D texId;
uniform sampler2D tex[32];
uniform float borderTime;

out vec4 color;

in TES_OUT
{
    vec2 tc;
    float h;
    float x, y;
    float stone;
    vec2 normal;
    vec4 ccolor;
}
fs_in;

void main()
{
    color = texture(tex[1], fs_in.tc);
}
