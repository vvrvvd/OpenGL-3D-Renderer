#version 330 core
struct Material{
	vec3 color;
};

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 pos;
out vec3 aColor;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform Material mat;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0) ;
	aColor = mat.color;
	pos = aPos;
	
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(1.0f, 1.0f, 1.0f);
    aColor = ambient * mat.color;
}