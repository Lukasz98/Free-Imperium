#version 330 core

in vec2 texCoord;
in vec4 col;
in vec3 normal;
uniform sampler2D texID;

out vec4 color;



void main()
{
//    color = col + vec4(normal, 0.0);
    color = texture(texID, texCoord);
//    if (color == vec4(0.0, 0.0, 0.0, 1.0))
 //     color = col;
      
}

