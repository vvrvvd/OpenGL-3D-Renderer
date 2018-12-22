#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "shader.h"

void init_glfw();
void process_input(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void initialize_triangle();
void initialize_rectangle();

float* get_triangle_color();

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;
const float COLOR1[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
const float COLOR2[4] = { 0.0f, 1.0f, 0.0f, 1.0f };

unsigned int VBOs[2];
unsigned int VAOs[2];
unsigned int EBO;

int main()
{
	init_glfw();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Learn OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create a window";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Couldn't load GLAD";
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);

	initialize_triangle();
	//initialize_rectangle();

	//Wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	Shader shaderInterpolated = Shader("vertexInterpolated.vert", "fragmentInterpolated.frag");
	Shader shaderUniform = Shader("vertexUniform.vert", "fragmentUniform.frag");

	while (!glfwWindowShouldClose(window))
	{
		//input
		process_input(window);

		//rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderInterpolated.use();
		float time = glfwGetTime();
		float deltaX = time / 10.0f;
		shaderInterpolated.setFloat("xOffset", deltaX);
		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		float* triangleColor = get_triangle_color();
		shaderUniform.use();
		shaderUniform.setFloatVec4("ourColor", triangleColor);
		delete triangleColor;
		glBindVertexArray(VAOs[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//check and call events and swap buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(2, VAOs);
	glDeleteBuffers(2, VBOs);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}

void init_glfw()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void initialize_triangle()
{
	float vertices1[] = {
	-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
	-0.25f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	 0.0f,  -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
	};

	float vertices2[] = {
	 0.25f, 0.25f, 0.0f,
	 0.5f, 0.75f, 0.0f,
	 0.75f,  0.25f, 0.0f,
	};

	glGenBuffers(2, VBOs);
	glGenVertexArrays(2, VAOs);

	glBindVertexArray(VAOs[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(VAOs[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void initialize_rectangle()
{
	float vertices[] = {
	 0.5f,  0.5f, 0.0f,  // top right
	 0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f, -0.5f, 0.0f,  // bottom left
	-0.5f,  0.5f, 0.0f   // top left 
	};

	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	glGenBuffers(1, VBOs);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, VAOs);

	glBindVertexArray(VAOs[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

float* get_triangle_color()
{
	float time = glfwGetTime();
	float alpha = (sin(time) / 2.0f) + 0.5f;
	float beta = 1.0f - alpha;
	float* interpolatedColor = new float[4];
	interpolatedColor[0] = alpha * COLOR1[0] + beta * COLOR2[0];
	interpolatedColor[1] = alpha * COLOR1[1] + beta * COLOR2[1];
	interpolatedColor[2] = alpha * COLOR1[2] + beta * COLOR2[2];
	interpolatedColor[3] = alpha * COLOR1[3] + beta * COLOR2[3];

	return interpolatedColor;
}