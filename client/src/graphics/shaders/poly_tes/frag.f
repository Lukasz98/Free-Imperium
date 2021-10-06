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
// in float ProvId;
// in vec2 tc;
// in vec3 pos;
// in vec3 normal;

in GEO_OUT
{
    float provId;
    // vec2 tc;
    vec3 pos;
    vec3 normal;
}
tes_in;

void main()
{
    // color = vec4(col, 1.0f);
    int mapTypei = int(mapType);
    // float provCount = 3773.0f;
    if (mapTypei == 20) {  // only for detecting clicks on prov
        color = texture(tex[mapTypei], vec2((0.1 + tes_in.provId) / provCount, .5f));
    }
    else {
        color = texture(tex[mapTypei], vec2((0.1 + tes_in.provId) / provCount, .5f));

        //color = texture(tex[mapTypei], vec2((0.1 + tes_in.provId) / provCount, .8f));
        if (tes_in.provId == provId)
            color = mix(color, vec4(1.0, 1.0, 1.0, 1.0), 0.5);

        vec3 normall = vec3(0.0f, 0.0f, 1.0f);
        normall = tes_in.normal;
//normall.x = 1.0f - normall.x;
//normall.y = 1.0f - normall.y;
//normall.z = 1.0f - normall.z;
//normall.z *=  -1.0f;
        vec3 lightPos = eyeLight;
        lightPos.z -= 200.0f;
        if (lightPos.z < 400.0f)
            lightPos.z = 400.0f;
        vec3 lightDir = normalize(lightPos - tes_in.pos);
       float diffuse = max(dot(normall, lightDir), 0.0f);
        //float diffuse = (dot(normall, lightDir));

        float dd = .8f;
        vec4 lightColor = vec4(dd, dd, dd, 1.0f);
        //color = color * lightColor * (diffuse);
        //normall.z = 0.0f;
        //color = vec4(normall, 1.0f);

        float ambient = 0.5f;
        //color = color * lightColor * (diffuse + ambient);

        float specularLight = 0.3f;
        vec3 reflectionDirection = reflect(-lightDir, normall);
        float specAmount = pow(max(dot(lightDir, reflectionDirection), 0.0f), 4);
        float specular = specAmount * specularLight;
        color = color * lightColor * (diffuse + ambient + specular);
        //color = vec4(normalize(lightPos), 1.0f);
        color.w = 1.0f;
    }
}
