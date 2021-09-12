#version 330 core

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

in vec4 Col[2];
uniform sampler2D tex[32];

out vec4 col;

void main()
{
    col = Col[0];
    float z;
    vec2 tc;
    float scale = 4.0f;
    float mapWidth = 5632.0f * scale, mapHeight = 2048.0f * scale;
    if (
    //(gl_in[0].gl_Position.x == gl_in[1].gl_Position.x) ||
    //(gl_in[0].gl_Position.y == gl_in[1].gl_Position.y)) {
    (abs(gl_in[0].gl_Position.x - gl_in[1].gl_Position.x) == 0.0f)) {
        float w = 0.5f;
        gl_Position = gl_in[0].gl_Position + vec4(-w, 0.0, 0.0, 0.0);
        EmitVertex();
        gl_Position = gl_in[0].gl_Position + vec4(w, 0.0, 0.0, 0.0);
        EmitVertex();

        gl_Position = gl_in[1].gl_Position + vec4(-w, 0.0, 0.0, 0.0);
        EmitVertex();
        gl_Position = gl_in[1].gl_Position + vec4(w, 0.0, 0.0, 0.0);
        EmitVertex();
    }
    else if ((abs(gl_in[0].gl_Position.y - gl_in[1].gl_Position.y) == 0.0f)) {
        float w = .5f;
        gl_Position = gl_in[0].gl_Position + vec4(0.0, -w, 0.0, 0.0);
        EmitVertex();
        gl_Position = gl_in[0].gl_Position + vec4(0.0, w, 0.0, 0.0);
        EmitVertex();

        gl_Position = gl_in[1].gl_Position + vec4(0.0, -w, 0.0, 0.0);
        EmitVertex();
        gl_Position = gl_in[1].gl_Position + vec4(0.0, w, 0.0, 0.0);
        EmitVertex();

    }
    else {
        float WIDTH = .5f;
        float width;

        float a = (gl_in[1].gl_Position.y - gl_in[0].gl_Position.y) / (gl_in[1].gl_Position.x - gl_in[0].gl_Position.x);
        float b = gl_in[1].gl_Position.y - gl_in[1].gl_Position.x * a;
        
        float a1 = -1.0f / a;
        width = WIDTH * a;
        float b1 = gl_in[1].gl_Position.y - (a1 * gl_in[1].gl_Position.x);
        float x1 = gl_in[1].gl_Position.x + width;
        float y1 = a1 * x1 + b1;
        gl_Position = vec4(x1, y1, gl_in[1].gl_Position.z - 0.0f, gl_in[1].gl_Position.w);
        EmitVertex();
        x1 = gl_in[1].gl_Position.x - width;
        y1 = a1 * x1 + b1;
        gl_Position = vec4(x1, y1, gl_in[1].gl_Position.zw);
        EmitVertex();
        

        a = (gl_in[0].gl_Position.y - gl_in[1].gl_Position.y) / (gl_in[0].gl_Position.x - gl_in[1].gl_Position.x);
        b = gl_in[0].gl_Position.y - gl_in[0].gl_Position.x * a;
        width = WIDTH * a;
        a1 = -1.0f / a;
        b1 = gl_in[0].gl_Position.y - (a1 * gl_in[0].gl_Position.x);
        x1 = gl_in[0].gl_Position.x + width;
        y1 = a1 * x1 + b1;
        gl_Position = vec4(x1, y1, gl_in[0].gl_Position.zw);
        EmitVertex();
        x1 = gl_in[0].gl_Position.x - width;
        y1 = a1 * x1 + b1;
        gl_Position = vec4(x1, y1, gl_in[0].gl_Position.zw);
        EmitVertex();
    }

    EndPrimitive();
}
