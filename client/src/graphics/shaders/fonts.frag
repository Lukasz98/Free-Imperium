#version 330 core

in vec2 texCoord;
in vec4 col;

uniform sampler2D texID;

out vec4 color;



void main()
{
    //color = texture(texID, texCoord);
    //if (color == vec4(0.0, 0.0, 0.0, 1.0))
      color = col;
      
}

