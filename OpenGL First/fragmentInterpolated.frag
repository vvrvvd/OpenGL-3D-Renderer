#version 330 core
in vec3 vertexColor;
in vec4 vertexPosition;

out vec4 FragColor;

void main()
{
	FragColor = vec4(vertexPosition);
}