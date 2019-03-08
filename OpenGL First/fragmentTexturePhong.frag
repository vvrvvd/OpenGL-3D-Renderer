#version 330 core
struct Material{
	vec3 color;
	float ambient;
	float specular;
};

in vec3 Pos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform Material mat;

void main()
{
	// ambient
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

    vec3 result = (ambient + diffuse + specular) * mat.color;
    FragColor = vec4(result, 1.0);
}