#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixingAlpha;
void main()
{
	float pct = sqrt(pow((TexCoord.x-0.5)*2, 2) + pow((TexCoord.y-0.5)*2, 2));

	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixingAlpha)*vec4(1.0-1.0f*pct, 1.0-1.0f*pct, 1.0-1.0f*pct, 1.0f);
}