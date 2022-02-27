#version 450 core

in vec4 pos;
in float outer;
uniform float time;

out vec4 outColor;



void main()
{
    vec4 yellow = vec4(252.0 / 255.0, 248.0 / 255.0, 30.0 / 255.0, 1.0);
    vec4 red = vec4(193.0 / 255.0, 29.0 / 255.0, 3.0 / 255.0, 1.0);
    //outColor = mix(yellow, red, abs(sin((pos.x+pos.y) * 0.1f + time*5.0)) );// + sin(pos.x));
    if (outer > 0.0f) {
    outColor = mix(yellow, red, abs(sin((time*2.0))) );// + sin(pos.x));
    }
    else {
    outColor = mix(yellow, red, abs(sin(time*2.0+0.1)) );// + sin(pos.x));
    }

    //outColor = vec4(sin(time) * pos.x/255.0, 1.0, 0.0, 1.0);
}
