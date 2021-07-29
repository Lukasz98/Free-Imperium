#version 450 core

//in vec4 col;
//layout(binding=1) uniform sampler2D texID;
//layout(binding=3) uniform sampler2D grassTexID;
//layout(binding=4) uniform sampler2D stoneTexID;
//layout(binding=5) uniform sampler2D waterTexID;
//layout(binding=6) uniform sampler2D cBordersTexID;
//layout(binding=7) uniform sampler2D pBordersTexID;

//layout (binding=0) uniform2D texId;
uniform sampler2D tex[32];

out vec4 color;

in TES_OUT
{
  vec2 tc;
  float h;
  float x,y;
  float stone;
  vec2 normal;
} fs_in;


void main()
{
color = texture(tex[1], fs_in.tc);
//color = vec4(fs_in.tc.x, fs_in.tc.y, 0.0,1.0);
//color = vec4(1.0,0.0,0.0,1.0);
}
