#version 330 core

//in vec2 texCoord;
//in vec4 col;

//uniform sampler2D texID;
uniform vec4 col;

out vec4 color;



void main()
{
    color = col;
    
    //color = texture(texID, texCoord);
    //color = vec4(1.0, 1.0, 0.0, 1.0);
    //  color = col;
      
}

