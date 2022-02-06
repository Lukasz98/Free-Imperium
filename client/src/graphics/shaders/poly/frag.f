#version 450 core

in vec3 col;

// layout (binding=0) uniform2D texId;
uniform sampler2D tex[32];
// uniform vec3 provColor;
uniform vec3 eyeLight;
uniform float provId;
uniform float mapType;
uniform float provCount;

out vec4 color;
in float ProvId;
in vec2 tc;
in vec3 pos;
in vec3 normal;
uniform float time;

void main()
{
    // color = vec4(col, 1.0f);
    int mapTypei = int(mapType);
    // float provCount = 3773.0f;
    if (mapTypei == 20) {  // only for detecting clicks on prov
        color = texture(tex[mapTypei], vec2((0.5 + ProvId) / provCount, .5f));
    }
    else {
        color = texture(tex[mapTypei], vec2((ProvId + .5) / provCount, .5f));
        if (ProvId == provId && mapTypei != 22)
            color = mix(color, vec4(1.0, 1.0, 1.0, 1.0), 0.5);

        vec4 markedCtr = texture(tex[5], vec2((ProvId + .5) / provCount, .5f));
        if (markedCtr.w == 1.0) {
            color = mix(color, markedCtr, abs(sin(time * 2.0f)));
        }

        vec4 occupiedCol = texture(tex[3], vec2((ProvId + .5) / provCount, .5f));
        vec4 occupyingCol = texture(tex[4], vec2((ProvId + .5) / provCount, .5f));
        if (occupiedCol.x > 0.0f || occupiedCol.y > 0.0f || occupiedCol.z > 0.0f) {
            if ((int(pos.x) + int(pos.y)) % 10 <= 1) {
                color = occupiedCol;
            }
        }
        else if (occupyingCol.x > 0.0f || occupyingCol.y > 0.0f || occupyingCol.z > 0.0f) {
            if ((int(pos.x) + int(pos.y)) % 10 <= 1) {
                color = mix(color, occupyingCol,
                            abs(sin(time + pos.x *

                                               0.5f)));
            }
        }
        vec3 normall = vec3(0.0f, 0.0f, 1.0f);
        normall = normal;

        vec3 lightPos = eyeLight;
        lightPos.z -= 200.0f;
        if (lightPos.z < 400.0f)
            lightPos.z = 400.0f;
        vec3 lightDir = normalize(lightPos - pos);
        // float diffuse = max(dot(normall, lightDir), 0.0f);
        float diffuse = (dot(normall, lightDir));

        float dd = .8f;
        vec4 lightColor = vec4(dd, dd, dd, 1.0f);
        // color = color * lightColor * (diffuse);
        // color = vec4(normall, 1.0f);

        float ambient = 0.5f;

        float specularLight = 0.3f;
        vec3 reflectionDirection = reflect(-lightDir, normall);
        float specAmount = pow(max(dot(lightDir, reflectionDirection), 0.0f), 4);
        float specular = specAmount * specularLight;

        color = color * lightColor * (diffuse + ambient + specular);
        // color.w = 1.0f;
    }
}
