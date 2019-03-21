#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 LookAt(glm::vec3 pos, glm::vec3 center, glm::vec3 worldUp)
{
	glm::vec3 dir = glm::normalize(pos - center);                                                                                                                                                                                                                      
	glm::vec3 right = glm::normalize(glm::cross(worldUp, dir));
	glm::vec3 up = glm::normalize(glm::cross(dir, right));

	return glm::mat4(right[0], up[0], dir[0], 0, right[1], up[1], dir[1], 0, right[2], up[2], dir[2], 0, 0, 0, 0, 1)*glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -pos[0], -pos[1], -pos[2], 1);
}

glm::mat4 PerspectiveMatrix(float fov, float ratio, float nearPlane, float farPlane)
{
	float tan = glm::tan(fov / 2.0f);
	return glm::mat4(
		1.0f / (ratio*tan), 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f / tan, 0.0f, 0.0f,
		0.0f, 0.0f, -(farPlane + nearPlane) / (farPlane - nearPlane), -1.0f,
		0.0f, 0.0f, -(2 * farPlane*nearPlane) / (farPlane - nearPlane), 0.0f);
}

glm::mat4 OrthographicMatrix(float l, float r, float b, float t, float nearPlane, float farPlane)
{
	return glm::mat4(
		2.0f / (r - l), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (t - b), 0.0f, 0.0f,
		0.0f, 0.0f, -2.0f / (farPlane - nearPlane), 0.0f,
		- (r + l) / (r - l), -(t + b) / (t - b), -(farPlane + nearPlane) / (farPlane - nearPlane), 1.0f);
}
