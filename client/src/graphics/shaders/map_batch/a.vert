#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in float textureID;
layout (location = 3) in vec2 texCoord;
layout (location = 4) in vec2 normal;


uniform mat4 pr_matrix;
uniform mat4 vw_matrix = mat4(1.0);
uniform mat4 ml_matrix = mat4(1.0);


//out vec4 Position;
out vec4 Color;
out vec2 TexCoord;
out float TextureID;
out vec2 Normal;

void main()
{
    gl_Position = pr_matrix * vec4(position, 1.0f);
        //gl_Position = Position;
        //gl_Position = vec4(position, 1.0f);
        Color = color;
        TexCoord = texCoord;
        Normal = normal;
        //Normal = vec2(1.0f,1.0f);//normal;
        TextureID = textureID;
}
