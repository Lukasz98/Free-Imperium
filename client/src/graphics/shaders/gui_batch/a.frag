#version 400 core

in vec4 Position;
in vec4 Color;
in vec2 TexCoord;
in float TextureID;

uniform vec2 lightPos;
uniform sampler2D tex[32];

void main()
{
    if (int(TextureID + 0.5) == 443) {
        gl_FragColor = Color;
    }
    else if (int(TextureID + 0.5) == 31) {
        vec4 c = texture(tex[int(TextureID + 0.5)], TexCoord);
        gl_FragColor = c;
//        gl_FragColor = vec4(TexCoord.x, TexCoord.y , 0.0,1.0);

    }
    else {
        vec4 c = texture(tex[int(TextureID + 0.2)], TexCoord);
        //vec4 c = texture(tex[int(2.0 + 0.5)], TexCoord);
        
        c.x = Color.x;
        c.y = Color.y;
        c.z = Color.z;
        //c[0] = Color[0];
        //c[1] = Color[1];
        //c[2] = Color[2];
        
        //}
        //if (TextureID == 1.0 && c[3] < 1.0)
        //    c[3] = 1.0;
            //c = vec4(0.5, 0.5, 0.0, 1.0);
        gl_FragColor = c;
    }
	
    //gl_FragColor = vec4(0.0,1.0,0.0,1.0);
    //float intensity = 0.1 / length(Position.xy - lightPos);
	

}
