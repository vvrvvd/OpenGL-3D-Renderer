#version 330 core
in vec3 Pos;
in vec3 Color;
in vec3 Normal;

out vec4 FragColor;

void main()
{
	FragColor = vec4(Color.x, Color.y, Color.z, 1.0);
}