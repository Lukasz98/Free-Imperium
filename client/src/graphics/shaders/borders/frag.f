#version 450 core

in vec4 col;

//layout (binding=0) uniform2D texId;
uniform sampler2D tex[32];

out vec4 color;


void main()
{
//gl_FragColor = color;
color = col;
//color = texture(tex[1], fs_in.tc);
//color = vec4(fs_in.tc.x, fs_in.tc.y, 0.0,1.0);
//color = vec4(1.0,0.0,0.0,1.0);
}
