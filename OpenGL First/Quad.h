#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Quad
{
public:
	float Width;
	float Height;

private:
	float vertices[12];
	unsigned int indices[6];

	unsigned int VBO, VAO, EBO;
public:

	Quad()
	{

	}

	Quad(float width, float height)
	{
		this->Width = width;
		this->Height = height;
		vertices[0] = -width/2.0f;
		vertices[1] = -height/2.0f;
		vertices[2] = 0.0f;

		vertices[3] = -width / 2.0f;
		vertices[4] = height / 2.0f;
		vertices[5] = 0.0f;

		vertices[6] = width / 2.0f;
		vertices[7] = height / 2.0f;
		vertices[8] = 0.0f;

		vertices[9] = width / 2.0f;
		vertices[10] = -height / 2.0f;
		vertices[11] = 0.0f;

		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 2;
		indices[4] = 3;
		indices[5] = 0;

		init_opengl_buffors();
	}

	void Draw()
	{
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

private:
	void init_opengl_buffors()
	{
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glGenVertexArrays(1, &VAO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

};