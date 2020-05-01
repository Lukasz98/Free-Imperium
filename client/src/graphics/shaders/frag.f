#version 450 core

//in vec2 texCoord;
//in vec4 col;
layout(binding=1) uniform sampler2D texID;
layout(binding=3) uniform sampler2D grassTexID;
layout(binding=4) uniform sampler2D stoneTexID;
layout(binding=5) uniform sampler2D waterTexID;
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
    
    //color = mix(color, texture(grassTexID, ttc), 1.95f);
    color = texture(grassTexID, ttc);


    color = mix(color, texture(stoneTexID, ttc), fs_in.h * 1.5);
    if (fs_in.h > 0.7)
      color += vec4(0.1, 0.1, 0.1, 0.0);
   
    
  }

     
}

