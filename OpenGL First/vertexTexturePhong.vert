#version 330 core
struct Material{
	vec3 color;
};

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Pos;
out vec3 Color;
out vec3 Normal;
out vec3 lightPosFrag;
out vec3 lightColorFrag;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform Material mat;

uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{
	Pos = vec3(model * vec4(aPos, 1));
    gl_Position = projection * view * vec4(Pos, 1);
	Normal = transpose(inverse(mat3(model))) * aNormal;
	Color = mat.color;
	lightPosFrag = lightPos;
	lightColorFrag = lightColor;
}