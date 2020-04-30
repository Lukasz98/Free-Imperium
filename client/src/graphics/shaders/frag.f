#version 450 core

//in vec2 texCoord;
//in vec4 col;
layout(binding=1) uniform sampler2D texID;
layout(binding=3) uniform sampler2D grassTexID;
layout(binding=4) uniform sampler2D stoneTexID;
//layout (binding=0) uniform2D texId;
//uniform sampler2D texID;

out vec4 color;

in TES_OUT
{
  vec2 tc;
  vec2 grass_tc;
  float h;
  float x,y;
} fs_in;


void main()
{
  color = texture(texID, fs_in.tc);
  
  vec2 ttc = vec2(fs_in.x / 64.0, fs_in.y / 64.0); 
  
  if (color.z != 1.0) {

   //color = vec4(0.0,0.0,0.0,1.0);
    //color.x = fs_in.grass_tc.x;
    //color.y = fs_in.grass_tc.y;//za_ktorym_x;//tc.x;
    color = mix(color, texture(grassTexID, ttc), 0.9f);
    //color = mix(color, texture(grassTexID, fs_in.grass_tc), 0.9f);
    
  }

  if (fs_in.h > 0.4) {
    //color.x += fs_in.h / 2;
    //color.y += fs_in.h/2;
    //color.z += fs_in.h/2;
 
    if (fs_in.h < 0.7)
      color = mix(color, texture(stoneTexID, ttc), fs_in.h);
      //color = mix(color, texture(stoneTexID, fs_in.grass_tc), fs_in.h);
    else
      color += vec4(10.0, 10.0, 10.0, 0.0);
  }

   //color = texture(texID, texCoord);
    //if (color == vec4(0.0, 0.0, 0.0, 1.0))
      //color = col;
      
}

