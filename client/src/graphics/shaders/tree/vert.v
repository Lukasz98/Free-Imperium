#version 330 core
layout(location = 0) in vec3 mPos;
layout(location = 1) in vec4 color;
layout(location = 2) in mat4 ml;

uniform mat4 matrix;
out vec4 fColor;
//uniform sampler2D tex[32];

void main()
{
    fColor = color;
    
  //  vec2 tc = vec2(mPos.x / 5632.0 * 4.0, mPos.y / 2048 * 4.0);

    //float z = (texture(tex[3], tc).r) * 4.0 * 2.0 * 30.0;
    
    
    gl_Position = matrix * ml * vec4(mPos, 1.0);
    //gl_Position.z += 130.0;
}
