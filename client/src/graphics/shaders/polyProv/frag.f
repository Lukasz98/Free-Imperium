#version 450 core

in vec3 col;

//layout (binding=0) uniform2D texId;
uniform sampler2D tex[32];
uniform vec3 provColor;
uniform float provId;
out vec4 color;
in float ProvId;
in vec2 tc;

void main()
{
//gl_FragColor = color;
color = vec4(col, 1.0f);
//color = mix(color, texture(tex[0],  tc* 1000.0), 0.4);
//if (provColor == vec3(col.xyz))

if (ProvId == provId)
color = mix(color, vec4(1.0, 1.0, 1.0, 1.0), 0.1);

//color = texture(tex[1], fs_in.tc);
//color = vec4(fs_in.tc.x, fs_in.tc.y, 0.0,1.0);
//color = vec4(1.0,0.0,0.0,1.0);
}
