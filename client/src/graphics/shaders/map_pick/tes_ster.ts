#version 450 core 

layout (vertices = 4) out;

in VS_OUT
{
    vec2 tc;
} tcs_in[];

out TCS_OUT
{
    vec2 tc;
} tcs_out[];

uniform mat4 matrix;
uniform float level;
void main()
{
    if (gl_InvocationID == 0) {
        if (1 == 1) {
            gl_TessLevelInner[0] = level;
            gl_TessLevelOuter[0] = level;
            gl_TessLevelOuter[1] = level;
            gl_TessLevelOuter[2] = level;
        }
        else if (1 == 2) {
           gl_TessLevelInner[0] = 24.0;
           gl_TessLevelOuter[0] = 24.0;
           gl_TessLevelOuter[1] = 24.0;
           gl_TessLevelOuter[2] = 24.0;
        }
        else if (1 == 2) {
           gl_TessLevelInner[0] = 12.0;
           gl_TessLevelOuter[0] = 12.0;
           gl_TessLevelOuter[1] = 12.0;
           gl_TessLevelOuter[2] = 12.0;
        }
        else if (10 == 11) {
           gl_TessLevelInner[0] = 64.0;
           gl_TessLevelOuter[0] = 64.0;
           gl_TessLevelOuter[1] = 64.0;
           gl_TessLevelOuter[2] = 64.0;
        }

    }
    tcs_out[gl_InvocationID].tc = tcs_in[gl_InvocationID].tc;
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
