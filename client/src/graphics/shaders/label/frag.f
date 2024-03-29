#version 330 core

in vec2 texCoord;
in float tid;
in vec4 col;

out vec4 color;

uniform sampler2D tex[32];

void main()
{
    if (tid == 0.0f)
        color = texture(tex[0], texCoord);
    else if (tid == 1.0f)
        color = texture(tex[1], texCoord);
    else if (tid == 2.0f)
        color = texture(tex[2], texCoord);
    else if (tid == 3.0f)
        color = texture(tex[3], texCoord);
    else if (tid == 4.0f)
        color = texture(tex[4], texCoord);
    else if (tid == 5.0f)
        color = texture(tex[5], texCoord);
    if (color.w > 0.0f) {
        color.x = col.x;
        color.y = col.y;
        color.z = col.z;
    }
    if (tid > 20.0f)
        color = col;
}

