#version 330 core
in vec3 Pos;
in vec3 Color;
in vec3 Normal;
in vec3 lightPosFrag;
in vec3 lightColorFrag;

out vec4 FragColor;
uniform vec3 viewPos;

void main()
{
	// ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColorFrag;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPosFrag - Pos);
    float diff = clamp(dot(norm, lightDir)/(length(norm)*length(lightDir)), 0.0, 1.0);
    vec3 diffuse = diff * lightColorFrag;
    

	// specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - Pos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColorFrag;

    vec3 result = (ambient + diffuse + specular) * Color;
    FragColor = vec4(result, 1.0);
}