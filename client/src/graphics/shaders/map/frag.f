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
  float h;
  float x,y;
  float stone;
  vec2 normal;
} fs_in;


void main()
{
  color = texture(texID, fs_in.tc);
  
  if (color.z == 1.0) {
    vec2 ttc = vec2(fs_in.x / 16.0, fs_in.y / 16.0); 
    color = texture(waterTexID, ttc);
  }
  else {
    vec2 ttc = vec2(fs_in.x / 16.0, fs_in.y / 16.0); 
    
    //color = mix(color, texture(grassTexID, ttc), 0.9f);
    color = texture(grassTexID, ttc);

    float h = fs_in.h;
    if (h > 0.5)
      h *= 1.2;

    if (fs_in.normal.y != 0.0) {
    color.x += fs_in.normal.y;
    color.y += fs_in.normal.y;
    color.z += fs_in.normal.y;
    
      //color = mix(color, texture(stoneTexID, ttc), 1.0);
    }
  /*
    if (fs_in.normal.x != 0.0) {
    color.x += fs_in.normal.x;
    color.y += fs_in.normal.x;
    color.z += fs_in.normal.x;
      //color = mix(color, texture(stoneTexID, ttc), 1.0);
    }
*/    //if (fs_in.stone > 0.5)
     // color = mix(color, texture(stoneTexID, ttc), 1.0);
    //if (fs_in.h > 0.5)
      //color = mix(color, texture(stoneTexID, ttc), h);
    ///if (fs_in.h > 0.6)
    //  color += vec4(0.1, 0.1, 0.1, 0.0);
   
    vec4 cBorderColor = texture(cBordersTexID, fs_in.tc);    
    color = mix(color, cBorderColor, cBorderColor.w);

    if (cBorderColor.w == 0.0) {
      vec4 pBorderColor = texture(pBordersTexID, fs_in.tc);
      color = mix(color, pBorderColor, pBorderColor.w);
    }
  }
     
}
