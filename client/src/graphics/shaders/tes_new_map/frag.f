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
uniform vec3 provColor;

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

vec4 getTerrainColor()
{
    vec4 ccolor = texture(tex[0], fs_in.tc * 1000);
    vec4 mountain = vec4(105.0 / 255.0, 24.0 / 255.0, 4.0 / 255.0, 1.0);
    vec4 highland = vec4(176.0 / 255.0, 129.0 / 255.0, 21.0 / 255.0, 1.0);
    vec4 steppe = vec4(147.0 / 255.0, 200.0 / 255.0, 83.0 / 255.0, 1.0);
    vec4 desert = vec4(242.0 / 255.0, 242.0 / 255.0, 111.0 / 255.0, 1.0);
    vec4 sandRock = vec4(232.0 / 255.0, 172.0 / 255.0, 102.0 / 255.0, 1.0);
    vec4 cc1 = texture(tex[5], fs_in.tc);
    if (cc1 == sandRock) {
        /*
        float off = 0.0008;
        vec4 cc2 = texture(tex[5], vec2(fs_in.tc.x + off, fs_in.tc.y));
        vec4 cc3 = texture(tex[5], vec2(fs_in.tc.x + off, fs_in.tc.y + off));
        vec4 cc4 = texture(tex[5], vec2(fs_in.tc.x + off, fs_in.tc.y - off));
        vec4 cc5 = texture(tex[5], vec2(fs_in.tc.x, fs_in.tc.y + off));
        vec4 cc6 = texture(tex[5], vec2(fs_in.tc.x, fs_in.tc.y - off));
        // color = mix(texture(tex[5], fs_in.tc), color, 0.5);
        float www = 0.2;
        if (cc1 != cc2 || cc1 != cc3 || cc1 != cc4 || cc1 != cc5 || cc1 != cc6) {
            www = 0.6;
        }
        */
        float www = fs_in.h / 120.0f;
        ccolor = mix(texture(tex[6], fs_in.tc * 1000), ccolor, www);
        // color = texture(tex[6], fs_in.tc * 1000);
    }
    else if (cc1 == mountain) {
        // if (fs_in.h > 80.0f) {
        // vec4 pick = vec4(0.2f, 0.2f, 0.2f, 1.0f);
        vec4 pick = vec4(0.33f, 0.33f, 0.33f, 1.0f);
        ccolor = mix(ccolor, pick, fs_in.h / 120.0f);
        //}
    }
    else if (cc1 == highland) {
        vec4 pick = vec4(0.33f, 0.33f, 0.33f, 1.0f);
        ccolor = mix(ccolor, pick, fs_in.h / 220.0f);
    }
    else if (cc1 == steppe) {
        vec4 pick = vec4(191.0 / 255.0, 239.0 / 255.0, 146.0 / 255.0, 1.0f);
        ccolor = mix(ccolor, pick, fs_in.h / 220.0f);
    }
    else if (cc1 == desert) {
        vec4 pick = vec4(209.0 / 255.0, 219.0 / 255.0, 160.0 / 255.0, 1.0f);
        ccolor = mix(ccolor, pick, 1.0);                              // fs_in.h / 20.0f);
        ccolor = mix(ccolor, texture(tex[6], fs_in.tc * 1000), 0.7);  // fs_in.h / 20.0f);
    }
    return ccolor;
}

void main()
{
bool test = true;
    if (!test) {
        float shortOff = 0.00007, shortOffY = 0.000135;
        vec4 c1 = texture(tex[5], fs_in.tc);
        vec4 c2 = texture(tex[5], vec2(fs_in.tc.x + shortOff, fs_in.tc.y));
        vec4 c3 = texture(tex[5], vec2(fs_in.tc.x + shortOff, fs_in.tc.y + shortOffY));
        vec4 c4 = texture(tex[5], vec2(fs_in.tc.x + shortOff, fs_in.tc.y - shortOffY));
        vec4 c5 = texture(tex[5], vec2(fs_in.tc.x, fs_in.tc.y + shortOffY));
        vec4 c6 = texture(tex[5], vec2(fs_in.tc.x, fs_in.tc.y - shortOffY));
        vec4 c7 = texture(tex[5], vec2(fs_in.tc.x - shortOff, fs_in.tc.y + shortOffY));
        vec4 c8 = texture(tex[5], vec2(fs_in.tc.x - shortOff, fs_in.tc.y));
        vec4 c9 = texture(tex[5], vec2(fs_in.tc.x - shortOff, fs_in.tc.y - shortOffY));
        vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
        // if (c1 == black) {
        if (c1 == black && c2 == black && c3 == black && c3 == black && c4 == black && c5 == black &&
            c6 == black && c7 == black && c8 == black && c9 == black)
        {
            vec4 borderColor = vec4(0.3, 0.3, 0.3, 1.0);
            // if (c1.xyz == provColor || c2.xyz == provColor || c3.xyz == provColor || c4.xyz == provColor ||
            //    c5.xyz == provColor || c6.xyz == provColor)
            if (texture(tex[1], fs_in.tc).xyz == provColor) {
                float sinBor = sin(borderTime);
                float d = sin(borderTime + fs_in.x / 64.0);
                float r = abs(d), g = abs(d), b = 0.0f;
                if (r < 0.5)
                    r = 0.5;
                if (g < 0.5)
                    g = 0.5;
                borderColor = vec4(r, g, b, 1.0);
                borderColor = mix(vec4(r, g, b, 1.0), vec4(0.9f, 0.8f, 0.0f, 1.0), d);
            }
            float longOff = 0.00008, longOffY = 0.0002;
            vec4 cc1 = texture(tex[5], fs_in.tc);
            vec4 cc2 = texture(tex[5], vec2(fs_in.tc.x + longOff, fs_in.tc.y));
            vec4 cc3 = texture(tex[5], vec2(fs_in.tc.x + longOff, fs_in.tc.y + longOffY));
            vec4 cc4 = texture(tex[5], vec2(fs_in.tc.x + longOff, fs_in.tc.y - longOffY));
            vec4 cc5 = texture(tex[5], vec2(fs_in.tc.x, fs_in.tc.y + longOffY));
            vec4 cc6 = texture(tex[5], vec2(fs_in.tc.x, fs_in.tc.y - longOffY));
            vec4 cc7 = texture(tex[5], vec2(fs_in.tc.x - longOff, fs_in.tc.y));
            vec4 cc8 = texture(tex[5], vec2(fs_in.tc.x - longOff, fs_in.tc.y + longOffY));
            vec4 cc9 = texture(tex[5], vec2(fs_in.tc.x - longOff, fs_in.tc.y - longOffY));
            // if (cc1 != black || cc2 != black || cc3 != black || cc4 != black || cc5 != black || cc6 != black ||
            //    cc7 != black || cc8 != black || cc9 != black)
            if ((cc5 != black || cc6 != black) || (cc2 != black || cc7 != black)) {
                color = getTerrainColor();
                color = mix(color, borderColor, 0.3);
                // color.x = 1.0;
                // color = borderColor;
            }
            else {
                color = getTerrainColor();
                color = mix(color, borderColor, 0.7);
                color = borderColor;
            }
        }
        else {
            color = getTerrainColor();
        }
    }
    else {
        // color = texture(tex[1], fs_in.tc);
        float off = 0.00008;
        vec4 c1 = texture(tex[1], fs_in.tc);
        vec4 c2 = texture(tex[1], vec2(fs_in.tc.x + off, fs_in.tc.y));
        vec4 c3 = texture(tex[1], vec2(fs_in.tc.x + off, fs_in.tc.y + off));
        vec4 c4 = texture(tex[1], vec2(fs_in.tc.x + off, fs_in.tc.y - off));
        vec4 c5 = texture(tex[1], vec2(fs_in.tc.x, fs_in.tc.y + off));
        vec4 c6 = texture(tex[1], vec2(fs_in.tc.x, fs_in.tc.y - off));

        if (c1 == c2 && c1 == c3 && c1 == c4 && c1 == c5 && c1 == c6) {
            // color = mix(texture(tex[0], fs_in.tc * 1000), vec4(fs_in.normal, 1.0, 1.0), 0.8);
            // color = mix(vec4(fs_in.normal, 1.0, 1.0), texture(tex[0], fs_in.tc * 1000), 0.8);
            color = texture(tex[0], fs_in.tc * 1000);
            color = mix(color, texture(tex[0], fs_in.tc * 1000), 0.7);

            vec4 mountain = vec4(105.0 / 255.0, 24.0 / 255.0, 4.0 / 255.0, 1.0);
            vec4 highland = vec4(176.0 / 255.0, 129.0 / 255.0, 21.0 / 255.0, 1.0);
            vec4 steppe = vec4(147.0 / 255.0, 200.0 / 255.0, 83.0 / 255.0, 1.0);
            vec4 desert = vec4(242.0 / 255.0, 242.0 / 255.0, 111.0 / 255.0, 1.0);
            vec4 sandRock = vec4(232.0 / 255.0, 172.0 / 255.0, 102.0 / 255.0, 1.0);
            vec4 cc1 = texture(tex[5], fs_in.tc);
            if (cc1 == sandRock) {
                off = 0.0008;
                vec4 cc2 = texture(tex[5], vec2(fs_in.tc.x + off, fs_in.tc.y));
                vec4 cc3 = texture(tex[5], vec2(fs_in.tc.x + off, fs_in.tc.y + off));
                vec4 cc4 = texture(tex[5], vec2(fs_in.tc.x + off, fs_in.tc.y - off));
                vec4 cc5 = texture(tex[5], vec2(fs_in.tc.x, fs_in.tc.y + off));
                vec4 cc6 = texture(tex[5], vec2(fs_in.tc.x, fs_in.tc.y - off));
                // color = mix(texture(tex[5], fs_in.tc), color, 0.5);
                float www = 0.2;
                if (cc1 != cc2 || cc1 != cc3 || cc1 != cc4 || cc1 != cc5 || cc1 != cc6) {
                    www = 0.6;
                }
                www = fs_in.h / 120.0f;
                color = mix(texture(tex[6], fs_in.tc * 1000), color, www);
                // color = texture(tex[6], fs_in.tc * 1000);
            }
            else if (cc1 == mountain) {
                vec4 pick = vec4(0.33f, 0.33f, 0.33f, 1.0f);
                color = mix(color, pick, fs_in.h / 220.0f);
            }
            else if (cc1 == highland) {
                vec4 pick = vec4(0.33f, 0.33f, 0.33f, 1.0f);
                color = mix(color, pick, fs_in.h / 220.0f);
            }
            else if (cc1 == steppe) {
                vec4 pick = vec4(191.0 / 255.0, 239.0 / 255.0, 146.0 / 255.0, 1.0f);
                color = mix(color, pick, fs_in.h / 220.0f);
            }
            else if (cc1 == desert) {
                vec4 pick = vec4(209.0 / 255.0, 219.0 / 255.0, 160.0 / 255.0, 1.0f);
                color = mix(color, pick, 1.0);                              // fs_in.h / 20.0f);
                //color = mix(color, texture(tex[6], fs_in.tc * 1000), 0.7);  // fs_in.h / 20.0f);
            }
            
            if (texture(tex[8], fs_in.tc) != vec4(1.0, 1.0, 1.0, 1.0))
                color = mix(color, texture(tex[8], fs_in.tc), 0.6);  // fs_in.h / 20.0f);
            // color = texture(tex[0], fs_in.tc * 1000) + vec4(fs_in.normal.x, fs_in.normal.y, 0.0, 0.0);
            // color = vec4(fs_in.normal.x, fs_in.normal.y, 0.0, 1.0);
       /* 
            if (fs_in.normal.x > 0.0f || true) {
                color.x -= fs_in.normal.x * 0.1f;
                color.y -= fs_in.normal.x * 0.1f;
                color.z -= fs_in.normal.x * 0.1f;
            }
            if (fs_in.normal.x > fs_in.normal.y) {
                color.x = fs_in.normal.x;
                color.y = fs_in.normal.x;
                color.z = fs_in.normal.x;
            }
            else{
                color.x = fs_in.normal.y;
                color.y = fs_in.normal.y;
                color.z = fs_in.normal.y;
            }
*/
        }
        else {
        /*
            float vv = 0.3;
            // vec4 provColoror = vec4(27.0 / 255.0, 36.0 / 255.0, 255.0 / 255.0, 1.0);
            if (c1.xyz == provColor || c2.xyz == provColor || c3.xyz == provColor || c4.xyz == provColor ||
                c5.xyz == provColor || c6.xyz == provColor)
            {
                float sinBor = sin(borderTime);
                float d = sin(borderTime + fs_in.x / 64.0);
                float r = abs(d), g = abs(d), b = 0.0f;
                if (r < 0.5)
                    r = 0.5;
                if (g < 0.5)
                    g = 0.5;
                color = vec4(r, g, b, 1.0);
                color = mix(vec4(r, g, b, 1.0), vec4(0.9f, 0.8f, 0.0f, 1.0), d);
            }
            else
                color = vec4(vv, vv, vv, 1.0);
        */
        }

        // color = fs_in.ccolor;
        // vec4 tex_color = texture(TEXTURE, );

        // color = vec4(fs_in.tc.x, fs_in.tc.y, 0.0,1.0);
        // color = vec4(1.0,0.0,0.0,1.0);
    }
}
