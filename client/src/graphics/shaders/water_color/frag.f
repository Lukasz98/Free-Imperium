#version 450 core

uniform sampler2D tex[32];

out vec4 color;
in vec2 tc;

void main()
{
    color = texture(tex[1], tc);
//color.r = tc.x;
color.w = 1.0f;
}
