#version 330 core

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

in vec4 Col[2];

out vec4 col;

void main()
{
    col = Col[0];
    if (
    //(gl_in[0].gl_Position.x == gl_in[1].gl_Position.x) ||
    //(gl_in[0].gl_Position.y == gl_in[1].gl_Position.y)) {
    (abs(gl_in[0].gl_Position.x - gl_in[1].gl_Position.x) == 0.0f)) {
        gl_Position = gl_in[0].gl_Position + vec4(-1.1, 0.0, 0.0, 0.0);
        EmitVertex();
        gl_Position = gl_in[0].gl_Position + vec4(1.1, 0.0, 0.0, 0.0);
        EmitVertex();

        gl_Position = gl_in[1].gl_Position + vec4(-1.1, 0.0, 0.0, 0.0);
        EmitVertex();
        gl_Position = gl_in[1].gl_Position + vec4(1.1, 0.0, 0.0, 0.0);
        EmitVertex();
    }
    else if ((abs(gl_in[0].gl_Position.y - gl_in[1].gl_Position.y) == 0.0f)) {
        gl_Position = gl_in[0].gl_Position + vec4(0.0, -1.1, 0.0, 0.0);
        EmitVertex();
        gl_Position = gl_in[0].gl_Position + vec4(0.0, 1.1, 0.0, 0.0);
        EmitVertex();

        gl_Position = gl_in[1].gl_Position + vec4(0.0, -1.1, 0.0, 0.0);
        EmitVertex();
        gl_Position = gl_in[1].gl_Position + vec4(0.0, 1.1, 0.0, 0.0);
        EmitVertex();

    }
    else {
        float a = (gl_in[1].gl_Position.y - gl_in[0].gl_Position.y) / (gl_in[1].gl_Position.x - gl_in[0].gl_Position.x);
        float b = gl_in[1].gl_Position.y - gl_in[1].gl_Position.x * a;
        
        float a1 = -1.0f / a;
        float b1 = gl_in[1].gl_Position.y - (a1 * gl_in[1].gl_Position.x);
        float x1 = gl_in[1].gl_Position.x + 1.0f;
        float y1 = a1 * x1 + b1;
        gl_Position = vec4(x1, y1, gl_in[1].gl_Position.zw);
        EmitVertex();
        x1 = gl_in[1].gl_Position.x - 1.0f;
        gl_Position = vec4(x1, y1, gl_in[1].gl_Position.zw);
        EmitVertex();
        

        a = (gl_in[0].gl_Position.y - gl_in[1].gl_Position.y) / (gl_in[0].gl_Position.x - gl_in[1].gl_Position.x);
        b = gl_in[0].gl_Position.y - gl_in[0].gl_Position.x * a;
        a1 = -1.0f / a;
        b1 = gl_in[0].gl_Position.y - (a1 * gl_in[0].gl_Position.x);
        x1 = gl_in[0].gl_Position.x + 1.0f;
        y1 = a1 * x1 + b1;
        gl_Position = vec4(x1, y1, gl_in[0].gl_Position.zw);
        EmitVertex();
        x1 = gl_in[0].gl_Position.x - 1.0f;
        gl_Position = vec4(x1, y1, gl_in[0].gl_Position.zw);
        EmitVertex();
    }

    EndPrimitive();
}
