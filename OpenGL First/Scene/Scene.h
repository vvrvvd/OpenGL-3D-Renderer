#include <iostream>
#include <sstream>

#include "utils/StringUtils.h"
#include "utils/MatrixUtils.h"

const unsigned int VERTEX_SIZE = 3;
const unsigned int INDEX_SIZE = 3;
const float ORTHO_OFFSET = 0.5f;

Material DefaultMaterial;
float* DefaultColor;

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
	
	glm::vec3 LightPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 LightColor = glm::vec3(1.0f, 1.0f, 1.0f);

private:
	const unsigned int X = 0;
	const unsigned int Y = 1;
	const unsigned int Z = 2;

	float* vertices;
	unsigned int vertices_count;

	unsigned int* indices;
	unsigned int indices_count;

	unsigned int* triangles_parts;
	unsigned int triangles_count;

	unsigned int* parts;
	unsigned int parts_count;

	Material* materials;
	unsigned int materials_count;
	unsigned int** parts_indices;
	unsigned int* triangles_parts_count;
	std::vector<std::string> materials_names;

	float* normals;
	unsigned int normals_count;

	float minCoords[VERTEX_SIZE];
	float maxCoords[VERTEX_SIZE];
	float cameraCenter[VERTEX_SIZE];

	std::stringstream sceneDataStream;
	unsigned int VBO, mainVAO, normalsBuffer;
	unsigned int* EBO;

	//Obj file data
	std::vector<glm::vec3> obj_vertices;
	std::vector<glm::vec3> obj_normals;
	std::vector<unsigned int> obj_normals_indices;
	std::vector<unsigned int> obj_elements;

public:

	Scene(const char* scenePath)
	{
		parseFromFile(scenePath);
		findClippingCoords();
		initOpenglBuffors();

		DefaultColor = new float[3]{ DefaultMaterial.color.r, DefaultMaterial.color.g, DefaultMaterial.color.b };
	}

	void Draw(Shader* shader)
	{
		glBindVertexArray(mainVAO);
		shader->setVec3("lightPos", LightPos);
		shader->setVec3("lightColor", LightColor);
		if (parts_count > 0)
		{
			for (unsigned int i = 0; i < parts_count; ++i)
			{
				shader->setMaterial(materials[parts[i]]);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
				glDrawElements(GL_TRIANGLES, triangles_parts_count[i] * INDEX_SIZE, GL_UNSIGNED_INT, (void*)0);			}
		}
		else
		{
			shader->setMaterial(DefaultMaterial);
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

	glm::mat4 GetOrthoProjection(float ratio, Side side)
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
				float offsetH = ((minH + maxH) - (minCoords[1] + maxCoords[1])) / 2.0f;
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

		if (triangles_parts_count != NULL)
		{
			delete triangles_parts_count;
			triangles_parts_count = NULL;
		}

		if (parts != NULL)
		{
			delete parts;
			parts = NULL;
		}

		if (materials != NULL)
		{
			delete[] materials;
			materials = NULL;
		}

		if (parts_count != NULL)
		{
			for (unsigned int i = 0; i < parts_count; i++)
				delete parts_indices[i];

			delete parts_indices;
			parts_indices = NULL;
		}

		if (normals != NULL)
		{
			delete normals;
		}

		glDeleteVertexArrays(1, &mainVAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &normalsBuffer);

		if (EBO != NULL)
		{
			glDeleteBuffers(parts_count, EBO);
			delete EBO;
			EBO = NULL;
		}

		if (DefaultColor != NULL)
		{
			delete DefaultColor;
			DefaultColor = NULL;
		}

	}

private:
	
	//Finding clipping coords for scene
	void findClippingCoords()
	{
		minCoords[0] = findMinVertexCoord(X);
		minCoords[1] = findMinVertexCoord(Y);
		minCoords[2] = findMinVertexCoord(Z);

		maxCoords[0] = findMaxVertexCoord(X);
		maxCoords[1] = findMaxVertexCoord(Y);
		maxCoords[2] = findMaxVertexCoord(Z);

		cameraCenter[0] = (minCoords[0] + maxCoords[0]) / 2.0f;
		cameraCenter[1] = (minCoords[1] + maxCoords[2]) / 2.0f;
		cameraCenter[2] = (minCoords[2] + maxCoords[2]) / 2.0f;
	}

	float findMinVertexCoord(unsigned int axis)
	{
		float min = vertices[axis];
		for (int i = axis + VERTEX_SIZE; i < vertices_count; i += VERTEX_SIZE)
		{
			if (vertices[i] < min)
				min = vertices[i];
		}

		return min;
	}

	float findMaxVertexCoord(unsigned int axis)
	{
		float max = vertices[axis];
		for (int i = axis + VERTEX_SIZE; i < vertices_count; i += VERTEX_SIZE)
		{
			if (vertices[i] > max)
				max = vertices[i];
		}

		return max;
	}

	//Initialize opengl buffors and create parts and materials if available
	void initOpenglBuffors()
	{

		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &mainVAO);

		glBindVertexArray(mainVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices_count * sizeof(float), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &normalsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
		glBufferData(GL_ARRAY_BUFFER, normals_count * sizeof(float), normals, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);

		if (parts_count > 0)
		{
			createMaterialsAndPartsIndices();

			EBO = new unsigned int[parts_count];

			glGenBuffers(parts_count, EBO);
			for (unsigned int i = 0; i < parts_count; ++i)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles_parts_count[i] * INDEX_SIZE * sizeof(unsigned int), parts_indices[i], GL_STATIC_DRAW);
			}
		}
		else
		{
			EBO = new unsigned int;
			glGenBuffers(1, &EBO[0]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	//Initialize materials and parts
	void createMaterialsAndPartsIndices()
	{
		unsigned int* temp_indices = new unsigned int[parts_count];
		triangles_parts_count = new unsigned int[parts_count];
		parts_indices = new unsigned int*[parts_count];

		for (unsigned int i = 0; i < parts_count; ++i)
		{
			temp_indices[i] = 0;
			triangles_parts_count[i] = 0;
		}

		for (unsigned int i = 0; i < triangles_count; ++i)
			triangles_parts_count[triangles_parts[i]] += 1;

		for (unsigned int i = 0; i < parts_count; ++i)
			parts_indices[i] = new unsigned int[triangles_parts_count[i] * INDEX_SIZE];


		for (unsigned int i = 0; i < triangles_count; i++)
		{
			unsigned int part_index = triangles_parts[i];
			parts_indices[part_index][temp_indices[part_index]] = indices[i * 3];
			parts_indices[part_index][temp_indices[part_index] + 1] = indices[(i * 3) + 1];
			parts_indices[part_index][temp_indices[part_index] + 2] = indices[(i * 3) + 2];
			temp_indices[part_index] += 3;
		}

		delete temp_indices;
	}

	//Loading scene from file (.brp or .obj)
	//--------------------------------------------------------------------------------------------------

	//Load scene from file
	void parseFromFile(std::string scenePathString)
	{
		if (scenePathString.substr(scenePathString.length() - 3, 3) == "obj")
		{
			loadObj(scenePathString.c_str(), obj_vertices, obj_normals, obj_elements, obj_normals_indices);
			vertices_count = obj_vertices.size() * VERTEX_SIZE;
			vertices = new float[vertices_count];
			int count = 0;
			for (unsigned int i = 0; i < obj_vertices.size(); i++)
			{
				vertices[i * 3] = obj_vertices[i].x;
				vertices[i * 3 + 1] = obj_vertices[i].y;
				vertices[i * 3 + 2] = obj_vertices[i].z;
				if (vertices[i * 3] == 0 || vertices[i * 3 + 1] == 0 || vertices[i * 3 + 2] == 0)
					count++;
			}

			indices_count = obj_elements.size();
			indices = new unsigned int[indices_count];
			for (unsigned int i = 0; i < obj_elements.size(); i++)
			{
				indices[i] = obj_elements[i] - 1;
			}

			std::vector<float> populated_vertices;
			std::vector<float> populated_normals;
			for (unsigned int i = 0; i < indices_count; i++)
			{
				populated_vertices.push_back(vertices[indices[i] * 3]);
				populated_vertices.push_back(vertices[indices[i] * 3 + 1]);
				populated_vertices.push_back(vertices[indices[i] * 3 + 2]);
				populated_normals.push_back(obj_normals[obj_normals_indices[i] - 1].x);
				populated_normals.push_back(obj_normals[obj_normals_indices[i] - 1].y);
				populated_normals.push_back(obj_normals[obj_normals_indices[i] - 1].z);
				indices[i] = i;
			}

			delete vertices;
			vertices_count = populated_vertices.size();
			vertices = new float[vertices_count];

			delete normals;
			normals_count = populated_normals.size();
			normals = new float[normals_count];

			for (unsigned int i = 0; i < vertices_count; i++)
			{
				vertices[i] = populated_vertices[i];
			}

			for (unsigned int i = 0; i < normals_count; i++)
			{
				normals[i] = populated_normals[i];
			}
		}
		else
		{
			loadFile(scenePathString.c_str());
			parseData();
		}
	}

	//Loading .obj model from file
	void loadObj(const char* filename, std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &normals, std::vector<unsigned int> &elements, std::vector<unsigned int> &normals_indices)
	{
		std::ifstream in(filename, std::ios::in);
		if (!in)
		{
			std::cerr << "Cannot open " << filename << std::endl;
		}

		std::string line;
		while (getline(in, line))
		{
			if (line.substr(0, 2) == "v ")
			{
				std::vector<std::string> lines_v = Split(line.substr(2), " ");
				glm::vec3 v;
				v.x = stof(lines_v[0]);
				v.y = stof(lines_v[1]);;
				v.z = stof(lines_v[2]);;
				vertices.push_back(v);
			}
			else if (line.substr(0, 3) == "vn ")
			{
				std::vector<std::string> lines_v = Split(line.substr(3), " ");
				glm::vec3 v;
				v.x = stof(lines_v[0]);
				v.y = stof(lines_v[1]);;
				v.z = stof(lines_v[2]);;
				normals.push_back(v);
			}
			else if (line.substr(0, 2) == "f ")
			{
				std::vector<std::string> lines_f = Split(line.substr(2), " ");
				for (unsigned int i = 0; i < 3; i++)
				{
					std::vector<std::string> lines_e = Split(lines_f[i], "/");
					elements.push_back(std::stoi(lines_e[0]));
					normals_indices.push_back(std::stoi(lines_e[2]));
				}
				if (lines_f.size() == 4)
				{
					for (unsigned int i = 0; i < 4; i++)
					{
						if (i != 1)
						{
							std::vector<std::string> lines_e = Split(lines_f[i], "/");
							elements.push_back(std::stoi(lines_e[0]));
							normals_indices.push_back(std::stoi(lines_e[2]));
						}
					}
				}

			}
			else
			{
				/* ignoring this line */
			}

		}
	}

	//Load file stream
	void loadFile(const char* scenePath)
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

	//Parsing constant variables
	const std::string VERTICES_HEADER = "points_count";
	const std::string INDICES_HEADER = "triangles_count";
	const std::string PARTS_HEADER = "parts_count";
	const std::string MATERIALS_HEADER = "materials_count";
	const std::string MATERIAL_NAME_HEADER = "mat_name";
	const std::string MATERIAL_COLOR_HEADER = "rgb";
	const std::string MESHES_INIT_HEADER = "node_count";
	const std::string MESH_NAME_HEADER = "hier_name";
	const std::string MESH_TRIANGLE_COUNT_HEADER = "triangle_count";

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
	const unsigned int ASSIGN_MESH_ARRAY = 10;
	const unsigned int LOAD_MESH_ARRAY = 11;
	const unsigned int ASSIGN_MESH_NAME = 12;
	const unsigned int ASSIGN_MESH_TRIANGLES = 13;
	const unsigned int LOAD_MESH_TRIANGLES = 14;

	//Parse loaded file stream
	void parseData()
	{
		unsigned int loadingState = LOOK_FOR_VERTICES;
		unsigned int index = 0;
		std::string line;
		while (std::getline(sceneDataStream, line)) {
			std::vector<std::string> words = Split(line, " ");
			for (unsigned int i = 0; i < words.size(); i++)
			{
				if (words[i].substr(0, 2) != "//")
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
						else if (index < vertices_count)
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
						materials = new Material[materials_count];
						loadingState = LOAD_MATERIALS_ARRAY;
					}
					else if (loadingState == LOAD_MATERIALS_ARRAY)
					{
						if (words[i] == MATERIAL_NAME_HEADER)
						{
							materials[index].name = words[i + 1];
							materials_names.push_back(words[i + 1]);
						}
						else if (words[i] == MATERIAL_COLOR_HEADER)
						{
							materials[index].color = glm::vec3(std::stof(words[i + 1]), std::stof(words[i + 2]), std::stof(words[i + 3]));
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

	//Optional operation on scene when object loaded
	//-----------------------------------------------------------------------------------

	//Generating separable triangles with normals (giving mesh without normals smoothing)
	void populateIndices()
	{
		std::cout << "Before " << vertices_count << " ";
		std::vector<float> populated_vertices;
		for (unsigned int i = 0; i < indices_count; i++)
		{
			populated_vertices.push_back(vertices[indices[i] * 3]);
			populated_vertices.push_back(vertices[indices[i] * 3 + 1]);
			populated_vertices.push_back(vertices[indices[i] * 3 + 2]);
			indices[i] = i;
		}

		delete vertices;
		vertices_count = populated_vertices.size();
		std::cout << "after " << vertices_count << std::endl;
		vertices = new float[vertices_count];
		for (unsigned int i = 0; i < vertices_count; i++)
		{
			vertices[i] = populated_vertices[i];
		}
	}

	//Automatic calculating normals based on triangles
	void calculateNormalsWithoutMeshes()
	{
		glm::vec3* triangles_normals = new glm::vec3[triangles_count];

		glm::vec3 v[3];
		glm::vec3 edge1, edge2;
		glm::vec3 tempNormal;

		for (unsigned int i = 0; i < triangles_count; i++)
		{
			for (unsigned int k = 0; k < 3; k++)
			{
				v[k].x = vertices[indices[(i * 3) + k] * 3];
				v[k].y = vertices[indices[(i * 3) + k] * 3 + 1];
				v[k].z = vertices[indices[(i * 3) + k] * 3 + 2];
			}

			edge1 = v[1] - v[0];
			edge2 = v[2] - v[0];
			tempNormal = glm::normalize(glm::cross(edge1, edge2));
			triangles_normals[i] = tempNormal;
		}

		normals_count = vertices_count;
		normals = new float[normals_count];

		for (unsigned int i = 0; i < vertices_count / VERTEX_SIZE; i++)
		{
			tempNormal = glm::vec3(0);
			for (unsigned int k = 0; k < indices_count / INDEX_SIZE; k++)
			{
				glm::vec3 v1;
				glm::vec3 v2;
				bool is_adjacent = false;

				if (indices[3 * k] == i)
				{
					v1.x = vertices[indices[3 * k + 1] * 3];
					v1.y = vertices[indices[3 * k + 1] * 3 + 1];
					v1.z = vertices[indices[3 * k + 1] * 3 + 2];
					v2.x = vertices[indices[3 * k + 2] * 3];
					v2.y = vertices[indices[3 * k + 2] * 3 + 1];
					v2.z = vertices[indices[3 * k + 2] * 3 + 2];
					is_adjacent = true;
				}
				else if (indices[3 * k + 1] == i)
				{
					v1.x = vertices[indices[3 * k] * 3];
					v1.y = vertices[indices[3 * k] * 3 + 1];
					v1.z = vertices[indices[3 * k] * 3 + 2];
					v2.x = vertices[indices[3 * k + 2] * 3];
					v2.y = vertices[indices[3 * k + 2] * 3 + 1];
					v2.z = vertices[indices[3 * k + 2] * 3 + 2];
					is_adjacent = true;
				}
				else if (indices[3 * k + 2] == i)
				{
					v1.x = vertices[indices[3 * k] * 3];
					v1.y = vertices[indices[3 * k] * 3 + 1];
					v1.z = vertices[indices[3 * k] * 3 + 2];
					v2.x = vertices[indices[3 * k + 1] * 3];
					v2.y = vertices[indices[3 * k + 1] * 3 + 1];
					v2.z = vertices[indices[3 * k + 1] * 3 + 2];
					is_adjacent = true;
				}

				if (is_adjacent)
				{
					glm::vec3 v0;
					v0.x = vertices[i * 3];
					v0.y = vertices[i * 3 + 1];
					v0.z = vertices[i * 3 + 2];

					glm::vec3 e1 = v1 - v0;
					glm::vec3 e2 = v2 - v0;
					float weight = 1.0f;

					if (glm::dot(e1, e2) >= 0)
						weight = (glm::asin(glm::length(glm::cross(e1, e2)) / (glm::length(e1) * glm::length(e2))));
					else
						weight = (glm::pi<float>() - glm::asin(glm::length(glm::cross(e1, e2)) / (glm::length(e1) * glm::length(e2))));

					tempNormal += weight * triangles_normals[k];
				}
			}
			tempNormal = glm::normalize(tempNormal);
			normals[i*VERTEX_SIZE] = tempNormal.x;
			normals[i*VERTEX_SIZE + 1] = tempNormal.y;
			normals[i*VERTEX_SIZE + 2] = tempNormal.z;
		}
	}

	//-----------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------

};