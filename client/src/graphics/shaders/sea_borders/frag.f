#version 450 core

in vec4 col;

//layout (binding=0) uniform2D texId;

out vec4 color;


void main()
{

//gl_FragColor = color;
//color = col;
//color = vec4(0.25,0.25,0.25,1.0);
color = vec4(0.5,0.5,0.5,1.0);

//color = texture(tex[1], fs_in.tc);
//color = vec4(fs_in.tc.x, fs_in.tc.y, 0.0,1.0);
}
