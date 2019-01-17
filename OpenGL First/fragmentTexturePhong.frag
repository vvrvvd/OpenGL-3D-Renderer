#version 330 core
in vec3 Pos;
in vec3 Color;
in vec3 Normal;
in vec3 lightPosFrag;
in vec3 lightColorFrag;

out vec4 FragColor;

void main()
{
	//Ambient
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColorFrag;

	//Diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPosFrag - Pos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColorFrag;

    vec3 ColorWithLight = (diffuse + ambient) * Color;
	FragColor = vec4(ColorWithLight.x, ColorWithLight.y, ColorWithLight.z, 1.0);
}