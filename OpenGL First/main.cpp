#include "main.h"

int main()
{
	init_glfw();
	init_window();
	init_glad();
	config_openGL();

	load_scene();
	load_shaders();

	while (!glfwWindowShouldClose(window))
	{
		update_time();
		core_loop();
	}

	dispose_shaders();
	dispose_scene();

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

void init_window()
{
	window = glfwCreateWindow(WIDTH, HEIGHT, "Learn OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create a window";
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void init_glad()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Couldn't load GLAD";
	}
}

void config_openGL()
{
	glViewport(0, 0, WIDTH, HEIGHT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void load_shaders()
{
	ourShader = new Shader("vertexTexture.vert", "fragmentTexture.frag");
}

void dispose_shaders()
{
	delete ourShader;
}

void load_scene()
{
	scene = new Scene(SCENE_PATH);
}

void dispose_scene()
{
	delete scene;
}

void update_time()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void core_loop()
{
	process_input(window);

	//rendering
	glClearColor(BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, BACKGROUND_COLOR.a);
	glClear(GL_COLOR_BUFFER_BIT);

	ourShader->use();

	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 model;

	//left bottom
	view = glm::mat4();
	view = scene->GetOrthoView(Scene::TOP);
	projection = scene->GetOrtho(RATIO, Scene::TOP);
	ourShader->setMat4("model", model);
	ourShader->setMat4("view", view);
	ourShader->setMat4("projection", projection);
	glViewport(0, 0, WIDTH*0.5, HEIGHT*0.5);
	scene->Draw();


	//right bottom
	view = scene->GetOrthoView(Scene::FRONT);
	projection = scene->GetOrtho(RATIO, Scene::FRONT);
	ourShader->setMat4("model", model);
	ourShader->setMat4("view", view);
	ourShader->setMat4("projection", projection);
	glViewport(WIDTH*0.5, 0, WIDTH*0.5, HEIGHT*0.5);
	scene->Draw();      

	//left top
	view = camera.GetViewMatrix();
	ourShader->setMat4("view", view);
	projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	model = glm::mat4();
	ourShader->setMat4("projection", projection);
	ourShader->setMat4("model", model);
	glViewport(0, HEIGHT*0.5, WIDTH*0.5, HEIGHT*0.5);
	scene->Draw();

	//right top
	view = scene->GetOrthoView(Scene::RIGHT);
	projection = scene->GetOrtho(RATIO, Scene::RIGHT);
	ourShader->setMat4("model", model);
	ourShader->setMat4("view", view);
	ourShader->setMat4("projection", projection);
	
	glViewport(WIDTH*0.5, HEIGHT*0.5, WIDTH*0.5, HEIGHT*0.5);
	scene->Draw();

	glViewport(0, 0, WIDTH, HEIGHT); //restore default

	//check and call events and swap buffers
	glfwPollEvents();
	glfwSwapBuffers(window);
}

unsigned int last_rmb_state = GLFW_RELEASE;

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (last_rmb_state == GLFW_RELEASE && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		firstMouse = true;
		glfwSetCursorPosCallback(window, mouse_callback);
		last_rmb_state = GLFW_PRESS;
	}
	else if(last_rmb_state == GLFW_PRESS && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursorPosCallback(window, NULL);
		last_rmb_state = GLFW_RELEASE;
	}
}



