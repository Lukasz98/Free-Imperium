#version 450 core

in vec3 col;

//layout (binding=0) uniform2D texId;
uniform sampler2D tex[32];
uniform vec3 provColor;
uniform vec3 eyeLight;
uniform float provId;

out vec4 color;
in float ProvId;
in vec2 tc;
in vec3 pos;
in vec3 normal;

void main()
{
color = vec4(col, 1.0f);
color = mix(color, texture(tex[0],  tc* 1000.0), 0.4);

if (ProvId == provId)
color = mix(color, vec4(1.0, 1.0, 1.0, 1.0), 0.5);


vec3 lightPos = eyeLight;
lightPos.z  -= 200.0f;
if (lightPos.z < 400.0f)
    lightPos.z = 400.0f;
vec3 lightDir = normalize(lightPos - pos);
float diffuse = max(dot(normal, lightDir), 0.0f);

float dd = .8f;
vec4 lightColor = vec4(dd, dd, dd, 1.0f);

float ambient = 0.5f;

float specularLight = 0.3f;
vec3 reflectionDirection = reflect(-lightDir, normal);
float specAmount = pow(max(dot(lightDir, reflectionDirection), 0.0f), 4);
float specular = specAmount * specularLight;

color =  color * lightColor * (diffuse + ambient + specular);
color.w = 1.0f;
}
