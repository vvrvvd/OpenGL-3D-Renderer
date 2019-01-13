#include <iostream>
#include <sstream>
#include "string_utils.h"
#include "matrix_utils.h"

const unsigned int VERTEX_SIZE = 3;
const unsigned int INDEX_SIZE = 3;
const float ORTHO_OFFSET = 0.5f;
const glm::vec3 DEFAULT_COLOR = glm::vec3(1.0f, 0.0f, 0.0f);

class Scene
{

public:

	enum Side {
		LEFT,
		RIGHT,
		TOP,
		BOTTOM,
		FRONT,
		BACK
	};

private:
	const std::string VERTICES_HEADER = "points_count";
	const std::string INDICES_HEADER = "triangles_count";
	const std::string PARTS_HEADER = "parts_count";
	const std::string MATERIALS_HEADER = "materials_count";
	const std::string MATERIAL_NAME_HEADER = "mat_name";
	const std::string MATERIAL_COLOR_HEADER = "rgb";

	const unsigned int X = 0;
	const unsigned int Y = 1;
	const unsigned int Z = 2;

	const unsigned int LOOK_FOR_VERTICES = 0;
	const unsigned int ASSIGN_VERTICES_ARRAY = 1;
	const unsigned int LOAD_VERTICES_ARRAY = 2;
	const unsigned int ASSIGN_INDICES_ARRAY = 3;
	const unsigned int LOAD_INDICES_ARRAY = 4;
	const unsigned int ASSIGN_PARTS_ARRAY = 5;
	const unsigned int LOAD_TRIANGLES_PARTS_ARRAY = 6;
	const unsigned int ASSIGN_MATERIALS_ARRAY = 7;
	const unsigned int LOAD_MATERIALS_ARRAY = 8;
	const unsigned int LOAD_PARTS_ARRAY = 9; 

	float* vertices;
	unsigned int vertices_count;

	unsigned int* indices;
	unsigned int indices_count;
	
	unsigned int* triangles_parts;
	unsigned int triangles_count;

	unsigned int* parts;
	unsigned int parts_count;

	glm::vec3** materials_colors;
	unsigned int materials_count;

	std::vector<std::string> materials_names;

	float minCoords[VERTEX_SIZE];
	float maxCoords[VERTEX_SIZE];
	float cameraCenter[VERTEX_SIZE];

	std::stringstream sceneDataStream;
	unsigned int VBO, VAO, EBO;

public:

	Scene(const char* scenePath)
	{
		load_file(scenePath);
		parse_data();
		find_clipping_coords();
		init_opengl_buffors();
	}

	void Draw(Shader* shader)
	{
		glBindVertexArray(VAO);
		if (materials_count > 0)
		{
			for (unsigned int i = 0; i < indices_count / INDEX_SIZE; ++i)
			{
				shader->setVec3("color", *(materials_colors[parts[triangles_parts[i]]]));
				glDrawElements(GL_TRIANGLES, INDEX_SIZE, GL_UNSIGNED_INT, (void*)(i*INDEX_SIZE*sizeof(unsigned int)));
			}
		}
		else
		{
			shader->setVec3("color", DEFAULT_COLOR);
			glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, (void*)0);
		}
		glBindVertexArray(0);
	}

	float* GetMinCoords()
	{
		return minCoords;
	}

	float* GetMaxCoords()
	{
		return maxCoords;
	}

	float* GetCenter()
	{
		return cameraCenter;
	}

	glm::mat4 GetOrtho(float ratio, Side side)
	{
		glm::mat2 ortho;
		float ratioWidth = ratio;
		float ratioHeight = 1.0f / ratio;

		if (side == LEFT || side == RIGHT)
		{
			if ((glm::abs(minCoords[0]) + glm::abs(maxCoords[0])) >= (glm::abs(minCoords[1]) + glm::abs(maxCoords[1])) &&
				(glm::abs(minCoords[0]) + glm::abs(maxCoords[0]))*ratioHeight > (glm::abs(minCoords[1]) + glm::abs(maxCoords[1])))
			{
				float minW = minCoords[0];
				float maxW = maxCoords[0];
				float minH = cameraCenter[1] + minW * ratioHeight;
				float maxH = cameraCenter[1] + maxW * ratioHeight;
				float offsetH = ((minH + maxH) - (minCoords[1] + maxCoords[1]))/2.0f;
				minH -= offsetH;
				maxH -= offsetH;
				return OrthographicMatrix(minW - ORTHO_OFFSET, maxW + ORTHO_OFFSET, minH - ORTHO_OFFSET*ratioHeight, maxH + ORTHO_OFFSET * ratioHeight, minCoords[2] - 100.0f, maxCoords[2] + 100.0f);
			}
			else
			{
				float minH = minCoords[1];
				float maxH = maxCoords[1];
				float minW = cameraCenter[0] + minH * ratioWidth;
				float maxW = cameraCenter[0] + maxH * ratioWidth;
				float offsetW = ((minW + maxW) - (minCoords[0] + maxCoords[0])) / 2.0f;
				minW -= offsetW;
				maxW -= offsetW;
				return OrthographicMatrix(minW - ORTHO_OFFSET * ratioWidth, maxW + ORTHO_OFFSET * ratioWidth, minH - ORTHO_OFFSET, maxH + ORTHO_OFFSET, minCoords[2] - 100.0f, maxCoords[2] + 100.0f);
			}
		}
		else if (side == TOP || side == BOTTOM)
		{
			if ((glm::abs(minCoords[0]) + glm::abs(maxCoords[0])) <= (glm::abs(minCoords[2]) + glm::abs(maxCoords[2])) &&
				(glm::abs(minCoords[2]) + glm::abs(maxCoords[2]))*ratioHeight > (glm::abs(minCoords[0]) + glm::abs(maxCoords[0])))
			{
				float minW = minCoords[2];
				float maxW = maxCoords[2];
				float minH = cameraCenter[0] + minW * ratioHeight;
				float maxH = cameraCenter[0] + maxW * ratioHeight;
				float offsetH = ((minH + maxH) - (minCoords[0] + maxCoords[0])) / 2.0f;
				minH -= offsetH;
				maxH -= offsetH;
				return OrthographicMatrix(minW - ORTHO_OFFSET, maxW + ORTHO_OFFSET, minH - ORTHO_OFFSET * ratioHeight, maxH + ORTHO_OFFSET * ratioHeight, minCoords[1] - 100.0f, maxCoords[1] + 100.0f);
			}
			else
			{
				float minH = minCoords[0];
				float maxH = maxCoords[0];
				float minW = cameraCenter[2] + minH * ratioWidth;
				float maxW = cameraCenter[2] + maxH * ratioWidth;
				float offsetW = ((minW + maxW) - (minCoords[2] + maxCoords[2])) / 2.0f;
				minW -= offsetW;
				maxW -= offsetW;
				return OrthographicMatrix(minW - ORTHO_OFFSET * ratioWidth, maxW + ORTHO_OFFSET * ratioWidth, minH - ORTHO_OFFSET, maxH + ORTHO_OFFSET, minCoords[1] - 100.0f, maxCoords[1] + 100.0f);
			}
		}
		else if (side == FRONT || side == BACK)
		{
			if ((glm::abs(minCoords[1]) + (glm::abs(maxCoords[1])) >= (glm::abs(minCoords[2]) + glm::abs(maxCoords[2]))) &&
				(glm::abs(minCoords[1]) + glm::abs(maxCoords[1]))*ratioWidth > (glm::abs(minCoords[2]) + glm::abs(maxCoords[2])))
			{
				float minH = minCoords[1];
				float maxH = maxCoords[1];
				float minW = cameraCenter[2] + minH * ratioWidth;
				float maxW = cameraCenter[2] + maxH * ratioWidth;
				float offsetW = ((minW + maxW) - (minCoords[2] + maxCoords[2])) / 2.0f;
				minW -= offsetW;
				maxW -= offsetW;
				return OrthographicMatrix(minW - ORTHO_OFFSET, maxW + ORTHO_OFFSET, minH - ORTHO_OFFSET * ratioHeight, maxH + ORTHO_OFFSET * ratioHeight, minCoords[0] - 100.0f, maxCoords[0] + 100.0f);
			}
			else
			{
				float minW = minCoords[2];
				float maxW = maxCoords[2];
				float minH = cameraCenter[1] + minW * ratioHeight;
				float maxH = cameraCenter[1] + maxW * ratioHeight;
				float offsetH = ((minH + maxH) - (minCoords[1] + maxCoords[1])) / 2.0f;
				minH -= offsetH;
				maxH -= offsetH;
				return OrthographicMatrix(minW - ORTHO_OFFSET * ratioWidth, maxW + ORTHO_OFFSET * ratioWidth, minH - ORTHO_OFFSET, maxH + ORTHO_OFFSET, minCoords[0] - 100.0f, maxCoords[0] + 100.0f);
			}
		}

		return glm::mat4(ortho);
	}

	glm::mat4 GetOrthoView(Side side)
	{
		switch (side) {
			case LEFT:
				return LookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
			case RIGHT:
				return LookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
			case TOP:
				return LookAt(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
			case BOTTOM:
				return LookAt(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
			case FRONT:
				return LookAt(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
			case BACK:
				return LookAt(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
			}

		return glm::mat4();
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

		if (triangles_parts != NULL)
		{
			delete triangles_parts;
			triangles_parts = NULL;
		}

		if (parts != NULL)
		{
			delete parts;
			parts = NULL;
		}

		if (materials_colors != NULL)
		{
			for (int i = 0; i < materials_count; ++i)
				delete[] materials_colors[i];

			delete[] materials_colors;
			materials_colors = NULL;
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
						if (words[i] == (VERTICES_HEADER))
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
						if (words[i] == (INDICES_HEADER))
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
						triangles_count = std::stoul(words[i]);
						indices_count = triangles_count * INDEX_SIZE;
						indices = new unsigned int[indices_count];
						loadingState = LOAD_INDICES_ARRAY;
						index = 0;
					}
					else if (loadingState == LOAD_INDICES_ARRAY)
					{
						if (words[i] == (PARTS_HEADER))
						{
							loadingState = ASSIGN_PARTS_ARRAY;
						}
						else if (index < indices_count)
						{
							float element = std::stof(words[i]);
							indices[index] = element;
							index++;
						}
					}
					else if (loadingState == ASSIGN_PARTS_ARRAY)
					{
						parts_count = std::stoul(words[i]);
						parts = new unsigned int[parts_count];
						triangles_parts = new unsigned int[triangles_count];
						loadingState = LOAD_TRIANGLES_PARTS_ARRAY;
						index = 0;
					}
					else if (loadingState == LOAD_TRIANGLES_PARTS_ARRAY)
					{
						if (words[i] == (MATERIALS_HEADER))
						{
							loadingState = ASSIGN_MATERIALS_ARRAY;
							index = 0;
						}
						else if (index < triangles_count)
						{
							float element = std::stoul(words[i]);
							triangles_parts[index] = element;
							index++;
						}
					}
					else if (loadingState == ASSIGN_MATERIALS_ARRAY)
					{
						materials_count = std::stoul(words[i]);
						materials_colors = new glm::vec3*[materials_count];
						loadingState = LOAD_MATERIALS_ARRAY;
					}
					else if (loadingState == LOAD_MATERIALS_ARRAY)
					{
						if (words[i] == MATERIAL_NAME_HEADER)
						{
							materials_names.push_back(words[i + 1]);
						}
						else if (words[i] == MATERIAL_COLOR_HEADER)
						{
							materials_colors[index] = new glm::vec3();
							materials_colors[index]->x = std::stof(words[i + 1]);
							materials_colors[index]->y = std::stof(words[i + 2]);
							materials_colors[index]->z = std::stof(words[i + 3]);
							index++;
							i += 3;
						}
						if (index >= materials_count && words.size() == 2 && words[0] == "0")
						{
							loadingState = LOAD_PARTS_ARRAY;
							index = 0;
							i--;
						}
					}
					else if (loadingState == LOAD_PARTS_ARRAY)
					{
						if (index < parts_count && words.size() == 2)
						{
							unsigned int name_ind = -1;
							for (unsigned int k = 0; k < materials_names.size(); k++)
							{
								if (materials_names[k] == words[i + 1])
								{
									name_ind = k;
									k = materials_names.size() + 1;
								}
							}
							parts[index] = name_ind;
							index++;
							i += 1;
						}
					}
				}
			}
		}
	}

	void find_clipping_coords()
	{
		minCoords[0] = find_min_vertex_coord(X);
		minCoords[1] = find_min_vertex_coord(Y);
		minCoords[2] = find_min_vertex_coord(Z);

		maxCoords[0] = find_max_vertex_coord(X);
		maxCoords[1] = find_max_vertex_coord(Y);
		maxCoords[2] = find_max_vertex_coord(Z);

		cameraCenter[0] = (minCoords[0] + maxCoords[0]) / 2.0f;
		cameraCenter[1] = (minCoords[1] + maxCoords[2]) / 2.0f;
		cameraCenter[2] = (minCoords[2] + maxCoords[2]) / 2.0f;
	}

	float find_min_vertex_coord(unsigned int axis)
	{
		float min = vertices[axis];
		for (int i = axis + VERTEX_SIZE; i < vertices_count; i += VERTEX_SIZE)
		{
			if (vertices[i] < min)
				min = vertices[i];
		}

		return min;
	}

	float find_max_vertex_coord(unsigned int axis)
	{
		float max = vertices[axis];
		for (int i = axis + VERTEX_SIZE; i < vertices_count; i += VERTEX_SIZE)
		{
			if (vertices[i] > max)
				max = vertices[i];
		}

		return max;
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