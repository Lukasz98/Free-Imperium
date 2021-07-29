#version 400 core

//in vec4 Position;
in vec4 Color;
in vec2 TexCoord;
in float TextureID;
in vec2 Normal;

uniform vec2 lightPos;
uniform sampler2D tex[32];

void main()
{
    //gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    vec4 c = texture(tex[int(TextureID + 0.5)], TexCoord);
    //vec4 c = texture(tex[0], TexCoord);
    //if (TextureID > 0.0f)
    //    c = mix(c, texture(tex[int(TextureID + 0.5f)], TexCoord), 1.0f);

    
    //Color.x += Normal.y;
    //Color.y += Normal.y;
    //Color.z += Normal.y;
    c = mix(Color, c, 0.6f);
    c.x += Normal.y;
    c.y += Normal.y;
    c.z += Normal.y;
    //c = mix(normal, c, 0.7f);
    //c = Color;
   //Color.x = color.x / 255.0f; 
    gl_FragColor = c;
    //gl_FragColor = Color;	

}
