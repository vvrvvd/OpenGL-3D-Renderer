#version 330 core
out vec4 FragColor;

in vec4 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	float pct = sqrt(pow((TexCoord.x-0.5)*2, 2) + pow((TexCoord.y-0.5)*2, 2));
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5) * (1-pct);
}