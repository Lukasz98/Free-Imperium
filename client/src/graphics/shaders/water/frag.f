#version 450 core

uniform sampler2D tex[32];
uniform vec3 eyeLight;

out vec4 color;

in TES_OUT
{
    vec2 tc;
    float h;
    vec3 pos;
    vec4 ccolor;
}
fs_in;
uniform float waterTime;
void main()
{
    color = vec4(145.0 / 255.0, 174.0 / 255.0, 183.0 / 255.0, abs(sin(waterTime)) + 0.2);  // 1.0);

    vec3 normal = vec3(0.0f, 0.0f, 1.0f);

    vec3 lightPos = eyeLight;
    lightPos.z -= 200.0f;
    if (lightPos.z < 400.0f)
        lightPos.z = 400.0f;
    vec3 lightDir = normalize(lightPos - fs_in.pos);
    float diffuse = max(dot(normal, lightDir), 0.0f);

    float dd = .8f;
    vec4 lightColor = vec4(dd, dd, dd, 1.0f);

    float ambient = 0.4f;

    float specularLight = 0.5f;
    vec3 reflectionDirection = reflect(-lightDir, normal);
    float specAmount = pow(max(dot(lightDir, reflectionDirection), 0.0f), 4);
    float specular = specAmount * specularLight;

    color = color * lightColor * (diffuse + ambient + specular);
    color.w = 1.0f;
}
