#version 450 core

in vec3 col;

//layout (binding=0) uniform2D texId;
uniform sampler2D tex[32];
uniform vec3 provColor;
uniform vec3 eyeLight;
uniform float provId;
uniform float mapType;
uniform float provCount;

out vec4 color;
in float ProvId;
in vec2 tc;
in vec3 pos;
in vec3 normal;

void main()
{
//color = vec4(col, 1.0f);
int mapTypei = int(mapType);
//float provCount = 3773.0f;
color = texture(tex[mapTypei], vec2((0.1 + ProvId) / provCount, .5f));
//color = texture(tex[mapTypei], vec2(ProvId, 0.0f));
//color = texture(tex[20], vec2(ProvId / 3773.0f, 0.0f));
//color = texture(tex[20], tc);
//color.r = ProvId / 3773.0f;
//color = texture(tex[10], vec2(ProvId, 0.0f));
//color = texture(tex[1], tc * 1000.0f);
//color = texture(tex[1], tc);
//color = mix(color, texture(tex[0],  tc* 1000.0), 0.4);

if (ProvId == provId)
color = mix(color, vec4(1.0, 1.0, 1.0, 1.0), 0.5);

vec3 normall = vec3(0.0f, 0.0f, 1.0f);
normall = normal;

vec3 lightPos = eyeLight;
lightPos.z  -= 200.0f;
if (lightPos.z < 400.0f)
    lightPos.z = 400.0f;
vec3 lightDir = normalize(lightPos - pos);
float diffuse = max(dot(normall, lightDir), 0.0f);

float dd = .8f;
vec4 lightColor = vec4(dd, dd, dd, 1.0f);

float ambient = 0.5f;

float specularLight = 0.3f;
vec3 reflectionDirection = reflect(-lightDir, normall);
float specAmount = pow(max(dot(lightDir, reflectionDirection), 0.0f), 4);
float specular = specAmount * specularLight;

color =  color * lightColor * (diffuse + ambient + specular);
//color.w = 1.0f;
}
