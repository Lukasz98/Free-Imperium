#version 330 core
layout(location = 0) in vec3 mPos;
layout(location = 1) in vec4 color;
layout(location = 2) in mat4 ml;

uniform mat4 matrix;
out vec4 fColor;

void main()
{
    fColor = color;
    
    gl_Position = matrix * ml * vec4(mPos, 1.0);

}
