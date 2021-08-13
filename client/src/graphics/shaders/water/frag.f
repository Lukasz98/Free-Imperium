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

out vec4 color;

in TES_OUT
{
    vec2 tc;
    float h;
    float x, y;
    vec4 ccolor;
}
fs_in;
uniform float waterTime;
void main()
{
    color = fs_in.ccolor; 
    color = texture(tex[4], fs_in.tc * 100.0);
    //color = mix(color, vec4(0.2, 0.2, 1.0, 1.0), 0.5 - sin(waterTime) * 0.2);
    //color.w = 0.5;
    //color = vec4(0.0, 0.0, 1.0, 1.0);
}
