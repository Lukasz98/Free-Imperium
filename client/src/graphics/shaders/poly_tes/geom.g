#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in TES_OUT
{
    float provId;
    // vec2 tc;
    vec3 pos;
    vec3 normal;
}
tes_in[];


out GEO_OUT
{
    float provId;
    // vec2 tc;
    vec3 pos;
    vec3 normal;
}
geo_out;

//in vec4 Col[2];
//uniform sampler2D tex[32];

//out vec4 col;

void main()
{
    geo_out.provId = tes_in[0].provId;

//    vec3 w = tes_in[0].pos - tes_in[1].pos;
//    vec3 v = tes_in[2].pos - tes_in[1].pos;
//geo_out.normal.x = (v.y * w.z) - (v.z * w.y);
//geo_out.normal.y = (v.z * w.x) - (v.x * w.z);
//geo_out.normal.z = (v.x * w.y) - (v.y * w.x);
geo_out.pos = tes_in[0].pos;
//geo_out.pos = geo_out.normal;
//geo_out.normal = normalize(geo_out.normal);
geo_out.normal = tes_in[0].normal;
geo_out.pos = tes_in[0].pos;
geo_out.pos = gl_in[0].gl_Position.xyz;

gl_Position = gl_in[0].gl_Position;
EmitVertex();

geo_out.normal = tes_in[1].normal;
geo_out.pos = tes_in[1].pos;
geo_out.pos = gl_in[1].gl_Position.xyz;
gl_Position = gl_in[1].gl_Position;
EmitVertex();

geo_out.normal = tes_in[2].normal;
geo_out.pos = tes_in[2].pos;
geo_out.pos = gl_in[2].gl_Position.xyz;
gl_Position = gl_in[2].gl_Position;
EmitVertex();


    EndPrimitive();
}
