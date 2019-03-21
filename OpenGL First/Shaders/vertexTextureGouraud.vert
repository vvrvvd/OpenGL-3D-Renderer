#version 330 core
struct Material{
	vec3 color;
	float ambient;
	float specular;
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
uniform vec3 viewPos;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
	Pos = aPos;
	Normal =  aNormal;

	//Ambient
	float ambientStrength = mat.ambient;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - Pos);
    float diff = clamp(dot(norm, lightDir)/(length(norm)*length(lightDir)), 0.0, 1.0);
    vec3 diffuse = diff * lightColor;
    

	// specular
    float specularStrength = mat.specular;
    vec3 viewDir = normalize(viewPos - Pos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    Color = (ambient + diffuse + specular) * mat.color;
}