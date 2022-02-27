#version 450 core

layout(points) in;
layout(triangle_strip, max_vertices = 32) out;

out vec4 pos;
out float outer;

void main()
{
    const float PI = 3.1415926;

    float r1 = 50.0f;
    float r2 = 40.0f;
    float xshift = 10.0f, yshift = 5.0f;
    for (int i = 0; i <= 10; i++) {
        // Angle between each side in radians
        float ang = PI * 2.0 / 10.0 * i;

        // Offset from center of point (0.3 to accomodate for aspect ratio)
        vec4 offset = vec4(cos(ang) * 0.3 * r1 - xshift, -sin(ang) * 0.4 * r1 + yshift, 0.0, 0.0);
        gl_Position = gl_in[0].gl_Position + offset;
        pos = gl_Position; 
        outer = 1.0f;
        EmitVertex();

        offset = vec4(cos(ang) * 0.3 * r2 - xshift, -sin(ang) * 0.4 * r2 + yshift, 0.0, 0.0);
        gl_Position = gl_in[0].gl_Position + offset;
        pos = gl_Position;
        outer = -1.0f;
        EmitVertex();
    }
    EndPrimitive();
}
