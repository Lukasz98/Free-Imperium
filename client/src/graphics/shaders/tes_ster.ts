#version 450 core 

layout (vertices = 3) out;

in VS_OUT
{
    vec2 tc;
} tcs_in[];

out TCS_OUT
{
    vec2 tc;
} tcs_out[];

uniform mat4 matrix;

void main()
{
    if (gl_InvocationID == 0) {
        if (1 == 10) {
 gl_TessLevelInner[0] = 6015.0;
        gl_TessLevelOuter[0] = 6015.0;
        gl_TessLevelOuter[1] = 6015.0;
        gl_TessLevelOuter[2] = 6015.0;
        
        }
        else if (10 == 1) {
        gl_TessLevelInner[0] = 215.0;
        gl_TessLevelOuter[0] = 215.0;
        gl_TessLevelOuter[1] = 215.0;
        gl_TessLevelOuter[2] = 215.0;
        }
        else if (10 == 1) {
        gl_TessLevelInner[0] = 64.0;
        gl_TessLevelOuter[0] = 64.0;
        gl_TessLevelOuter[1] = 64.0;
        gl_TessLevelOuter[2] = 64.0;
        }
         else if (1 == 1) {
        gl_TessLevelInner[0] = 24.0;
        gl_TessLevelOuter[0] = 24.0;
        gl_TessLevelOuter[1] = 24.0;
        gl_TessLevelOuter[2] = 24.0;
        }
        
else {
        gl_TessLevelInner[0] = 5.0;
        gl_TessLevelOuter[0] = 5.0;
        gl_TessLevelOuter[1] = 5.0;
        gl_TessLevelOuter[2] = 5.0;
    }
}
    tcs_out[gl_InvocationID].tc = tcs_in[gl_InvocationID].tc;
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}