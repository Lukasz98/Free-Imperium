#version 450 core

//layout (binding=0) uniform2D texId;
uniform sampler2D tex[32];

out vec4 color;
in vec2 tc;

void main()
{

//gl_FragColor = color;
//color = col;
//color = vec4(0.25,0.25,0.25,1.0);
color = texture(tex[1], tc);
//color = texture(tex[1], fs_in.tc);
//color = vec4(fs_in.tc.x, fs_in.tc.y, 0.0,1.0);
}
