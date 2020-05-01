#version 450 core

//in vec2 texCoord;
//in vec4 col;
layout(binding=1) uniform sampler2D texID;
layout(binding=3) uniform sampler2D grassTexID;
layout(binding=4) uniform sampler2D stoneTexID;
layout(binding=5) uniform sampler2D waterTexID;
layout(binding=6) uniform sampler2D cBordersTexID;
layout(binding=7) uniform sampler2D pBordersTexID;

//layout (binding=0) uniform2D texId;
//uniform sampler2D texID;

out vec4 color;

in TES_OUT
{
  vec2 tc;
  //vec2 grass_tc;
  float h;
  float x,y;
} fs_in;


void main()
{
  color = texture(texID, fs_in.tc);
  
  if (color.z == 1.0) {
    vec2 ttc = vec2(fs_in.x / 32.0, fs_in.y / 32.0); 
    
    color = texture(waterTexID, ttc);
  }
  else {
    vec2 ttc = vec2(fs_in.x / 16.0, fs_in.y / 16.0); 
    
    //color = mix(color, texture(grassTexID, ttc), 0.9f);
    color = texture(grassTexID, ttc);

    float h = fs_in.h;
    if (h > 0.5)
      h *= 1.2;

    color = mix(color, texture(stoneTexID, ttc), h);
    //if (fs_in.h > 0.7)
    //  color += vec4(0.1, 0.1, 0.1, 0.0);
   
vec4 cBorderColor = texture(cBordersTexID, fs_in.tc);    
//color.xyzw += borderColor.xyzw;
color = mix(color, cBorderColor, cBorderColor.w);

if (cBorderColor.w == 0.0) {
  vec4 pBorderColor = texture(pBordersTexID, fs_in.tc);
  color = mix(color, pBorderColor, pBorderColor.w);
}

  //if (int(fs_in.x) % 3 == 0)
//    color = vec4(0.0,1.0,0.0,1.0);

//color = mix(color, texture(cBordersTexID, fs_in.tc), 0.1f);
  }
     
}

