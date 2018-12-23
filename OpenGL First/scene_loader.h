#include <iostream>
#include <sstream>
#include "string_utils.h"

const unsigned int VERTEX_SIZE = 3;
const unsigned int INDEX_SIZE = 3;

class Scene
{

private:
	const std::string VERTICES_HEADER = "points_count";
	const std::string INDICES_HEADER = "triangles_count";

	const unsigned int LOOK_FOR_VERTICES = 0;
	const unsigned int ASSIGN_VERTICES_ARRAY = 1;
	const unsigned int LOAD_VERTICES_ARRAY = 2;
	const unsigned int ASSIGN_INDICES_ARRAY = 3;
	const unsigned int LOAD_INDICES_ARRAY = 4;

	float* vertices;
	unsigned int vertices_count;

	unsigned int* indices;
	unsigned int indices_count;

	std::stringstream sceneDataStream;
	unsigned int VBO, VAO, EBO;

public:

	Scene(const char* scenePath)
	{
		load_file(scenePath);
		parse_data();
		init_opengl_buffors();
	}

	void Draw()
	{
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	~Scene()
	{
		Dispose();
	}

	void Dispose()
	{
		if (vertices != NULL)
		{
			delete vertices;
			vertices = NULL;
		}

		if (indices != NULL)
		{
			delete indices;
			indices = NULL;
		}

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

private:

	void load_file(const char* scenePath)
	{
		std::ifstream sceneFile;

		sceneFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			sceneFile.open(scenePath);
			sceneDataStream << sceneFile.rdbuf();
			sceneFile.close();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SCENE::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
	}

	void parse_data()
	{
		unsigned int loadingState = LOOK_FOR_VERTICES;
		unsigned int index = 0;
		
		std::string line;
		while (std::getline(sceneDataStream, line)) {
			std::vector<std::string> words = split(line, " ");
			for (unsigned int i = 0; i < words.size(); i++)
			{
				if (words[i].substr(0, 2) !=  "//")
				{
					if (loadingState == LOOK_FOR_VERTICES)
					{
						if (words[i]._Equal(VERTICES_HEADER))
						{
							loadingState = 1;
						}
					}
					else if (loadingState == ASSIGN_VERTICES_ARRAY)
					{
						vertices_count = std::stof(words[i]) * VERTEX_SIZE;
						vertices = new float[vertices_count];
						loadingState = LOAD_VERTICES_ARRAY;
						index = 0;
					}
					else if (loadingState == LOAD_VERTICES_ARRAY)
					{
						if (words[i]._Equal(INDICES_HEADER))
						{
							loadingState = ASSIGN_INDICES_ARRAY;
						}
						else if(index < vertices_count)
						{
							float element = std::stof(words[i]);
							vertices[index] = element;
							index++;
						}
					}
					else if (loadingState == ASSIGN_INDICES_ARRAY)
					{
						indices_count = std::stoul(words[i]) * INDEX_SIZE;
						indices = new unsigned int[indices_count];
						loadingState = LOAD_INDICES_ARRAY;
						index = 0;
					}
					else if (loadingState == LOAD_INDICES_ARRAY)
					{
						if (index < indices_count)
						{
							float element = std::stof(words[i]);
							indices[index] = element;
							index++;
						}
					}
				}
			}
		}
	}

	void init_opengl_buffors()
	{
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glGenVertexArrays(1, &VAO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices_count * sizeof(float), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};