#version 330 core

in vec2 texCoord;
in vec4 col;
in vec3 normal;
uniform sampler2D tex[32];

out vec4 color;



void main()
{
    color = texture(tex[10], texCoord);
    //color.r =1.0f;
    //color.g =.5f;
    //color.r =0.5f;
}
