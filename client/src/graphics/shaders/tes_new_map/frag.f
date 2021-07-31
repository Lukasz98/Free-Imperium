#version 450 core

// in vec4 col;
// layout(binding=1) uniform sampler2D texID;
// layout(binding=3) uniform sampler2D grassTexID;
// layout(binding=4) uniform sampler2D stoneTexID;
// layout(binding=5) uniform sampler2D waterTexID;
// layout(binding=6) uniform sampler2D cBordersTexID;
// layout(binding=7) uniform sampler2D pBordersTexID;

// layout (binding=0) uniform2D texId;
uniform sampler2D tex[32];
uniform float borderTime;

out vec4 color;

in TES_OUT
{
    vec2 tc;
    float h;
    float x, y;
    float stone;
    vec2 normal;
    vec4 ccolor;
}
fs_in;

void main()
{
    // color = texture(tex[1], fs_in.tc);
    float off = 0.00008;
    vec4 c1 = texture(tex[1], fs_in.tc);
    vec4 c2 = texture(tex[1], vec2(fs_in.tc.x + off, fs_in.tc.y));
    vec4 c3 = texture(tex[1], vec2(fs_in.tc.x + off, fs_in.tc.y + off));
    vec4 c4 = texture(tex[1], vec2(fs_in.tc.x + off, fs_in.tc.y - off));
    vec4 c5 = texture(tex[1], vec2(fs_in.tc.x, fs_in.tc.y + off));
    vec4 c6 = texture(tex[1], vec2(fs_in.tc.x, fs_in.tc.y - off));

    if (c1 == c2 && c1 == c3 && c1 == c4 && c1 == c5 && c1 == c6) {
        color = c1;
        /*
        color = texture(tex[0], fs_in.tc * 1000);
        
        if (fs_in.normal.x > 0.1f) {
            color.x += 0.1f;
            color.y += 0.1f;
            color.z += 0.1f;
        }
        if (fs_in.normal.y > 0.1f)
            color.y += 0.1f;
*/
        color = mix(texture(tex[0], fs_in.tc * 1000), vec4(fs_in.normal, 1.0, 1.0), 0.5);
        color = mix(vec4(fs_in.normal, 1.0, 1.0), texture(tex[0], fs_in.tc * 1000), 0.8);
        //color = texture(tex[0], fs_in.tc * 1000) + vec4(fs_in.normal.x, fs_in.normal.y, 0.0, 0.0);
        // color = vec4(fs_in.normal.x, fs_in.normal.y, 0.0, 1.0);
    }
    else {
        float vv = 0.3;
        vec4 provCol = vec4(27.0 / 255.0, 36.0 / 255.0, 255.0 / 255.0, 1.0);
        if (c1 == provCol || c2 == provCol || c3 == provCol || c4 == provCol || c5 == provCol || c6 == provCol) {
            float sinBor = sin(borderTime);
            float d = sin(borderTime + fs_in.x / 64.0);
            float r = abs(d), g = abs(d), b = 0.0f;
            if (r < 0.5) r = 0.5;
            if (g < 0.5) g = 0.5;
            color = vec4(r, g, b, 1.0);
            color = mix(vec4(r, g, b, 1.0), vec4(0.9f, 0.8f, 0.0f, 1.0), d);
        }
        else
            color = vec4(vv, vv, vv, 1.0);
    }

    // color = fs_in.ccolor;
    // vec4 tex_color = texture(TEXTURE, );

    // color = vec4(fs_in.tc.x, fs_in.tc.y, 0.0,1.0);
    // color = vec4(1.0,0.0,0.0,1.0);
}
