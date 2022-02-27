#version 330 core

layout (location = 0) in vec3 Pos;

uniform mat4 matrix;


void main()
{
    gl_Position = matrix * vec4(Pos.x, Pos.y, Pos.z + 0.0f, 1.0f);
    //gl_Position = vec4(Pos.x, Pos.y, Pos.z + 10.0f, 1.0f);
    //gl_Position = vec4(-100.0, 0.0, 0.0, 1.0);
    //gl_Position = vec4(Pos.x, Pos.y, Pos.z + 20.0f, 1.0f);
}
