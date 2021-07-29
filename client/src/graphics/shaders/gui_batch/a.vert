#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in float textureID;

uniform mat4 pr_matrix;
uniform mat4 vw_matrix = mat4(1.0);
uniform mat4 ml_matrix = mat4(1.0);


out vec4 Position;
out vec4 Color;
out vec2 TexCoord;
out float
 TextureID;

void main()
{
    if (textureID >= 0) {
        Position = pr_matrix * vec4(position, 1.0f);
        gl_Position = Position;
	} else { // if is negative, it should not be visible
        gl_Position = vec4(-1000.0f, 0.0f, 0.0f, 1.0f);
    }

    //Position = ml_matrix * vec4(position, 1.0f);
	//if(texCoord.x == 1.0)
	//{
	//	Color = vec4(0.5, 0.5, 0.0, 1.0);
	//}
	//else
	//{
	//	Color = vec4(0.0, 0.0, 1.0, 1.0);
	//}

    //Color = vec4(1.0,0.0,0.0,1.0);
    Color = color;
	TexCoord = texCoord;
	TextureID = textureID;
	//gl_Position = vec4(position, 1.0f);
	

    //gl_Position = ml_matrix * vec4(position, 1.0f);
	//gl_Position = pr_matrix * vw_matrix * ml_matrix * vec4(position, 1.0f);// 
}

//Color = color;
//Texcoord = texcoord;
//gl_Position = vec4(position, 1.0, 1.0);
//gl_Position = ftransform();
//in vec2 texcoord;
//in vec4 color;
//in vec2 position;

//out vec4 Color;
//out vec2 Texcoord;
