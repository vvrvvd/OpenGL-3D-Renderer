#version 330 core
in vec3 aColor;
in vec3 pos;

out vec4 FragColor;

void main()
{
	FragColor = vec4(pos.x, pos.y, pos.z, 1.0);
}