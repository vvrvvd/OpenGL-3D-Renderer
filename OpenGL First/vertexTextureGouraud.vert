#version 330 core
struct Material{
	vec3 color;
};

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Pos;
out vec3 Color;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform Material mat;

uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
	Pos = aPos;
	Normal =  aNormal;

	//Ambient
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

	//Diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - Pos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

    Color = (diffuse + ambient) * mat.color;
}