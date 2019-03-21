#pragma once

float lightModelVertices[] = {
	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	0.5f, -0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	0.5f,  0.5f,  0.5f,
	0.5f,  0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f,  0.5f,
	0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	0.5f,  0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
};

class Light
{
	public:

		Shader* lightShader;

		float scale;
		glm::vec3 color;
		glm::vec3 position;

		unsigned int lightVBO;
		unsigned int lightVAO;

		Light(glm::vec3 pos, glm::vec3 col,float scaleMod, std::string vertShaderPath, std::string fragShaderPath)
		{
			position = pos;
			color = col;
			scale = scaleMod;
			lightShader = new Shader(vertShaderPath.c_str(), fragShaderPath.c_str());

			glGenVertexArrays(1, &lightVAO);
			glGenBuffers(1, &lightVBO);

			glBindVertexArray(lightVAO);

			glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lightModelVertices), lightModelVertices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

		}


		// Draws light model on screen
		void Draw(const glm::mat4 &view, const glm::mat4 &projection)
		{
			glm::mat4 model;
			model = glm::translate(model, position);
			model = glm::scale(model, glm::vec3(scale));

			lightShader->use();
			lightShader->setVec3("color", color);
			lightShader->setMat4("model", model);
			lightShader->setMat4("view", view);
			lightShader->setMat4("projection", projection);

			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}

		~Light()
		{
			Dispose();
		}

		// Cleans gpu memory deleting buffers, arrays and shaders
		void Dispose()
		{
			if (lightShader != NULL)
			{
				delete lightShader;
				lightShader = NULL;
			}

			glDeleteBuffers(1, &lightVBO);
			glDeleteVertexArrays(1, &lightVAO);
		}
};