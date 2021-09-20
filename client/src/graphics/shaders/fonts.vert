#version 330 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec4 Color;
layout (location = 3) in float Tid;
layout (location = 4) in mat4 Ml;

uniform mat4 matrix;
//uniform mat4 ml;

out vec2 texCoord;
out vec4 col;
out float tid;

void main()
{
    tid = Tid;
    col = Color;
    texCoord = TexCoord;
    gl_Position = matrix  * Ml* vec4(Pos, 1.0f);
   // gl_Position = matrix  * Pos;
    //gl_Position = Pos;
}
