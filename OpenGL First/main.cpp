#include "main.h"

int main()
{
	init_glfw();
	init_window();
	init_glad();
	config_openGL();
	init_imgui();

	load_shaders();

	init_camera_frustum_buffers();

	while (!glfwWindowShouldClose(window))
	{
		update_time();
		core_loop();
	}

	ImGui_ImplGlfw_Shutdown();

	dispose();

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

	camera->ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if(camera!=NULL)
		camera->ProcessMouseScroll(yoffset);
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

void init_imgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(NULL);
}

void draw_UI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::BeginMainMenuBar();
		if(ImGui::MenuItem("Load"))
		{
			openSceneFileDialog = true;
		}
		
		if (ImGui::BeginMenu("Camera"))
		{
			if (scene!=NULL)
			{

				if (ImGui::InputFloat3("Center", cameraCenter))
					tppCamera->SetCenter(glm::vec3(cameraCenter[0], cameraCenter[1], cameraCenter[2]));
				
				if (ImGui::InputFloat3("Position", cameraPosition))
				{
					tppCamera->SetPosition(glm::vec3(cameraPosition[0], cameraPosition[1], cameraPosition[2]));
				}

				ImGui::Text("FOV");
				ImGui::SliderFloat("", &camera->Zoom, 1.0f, 89.0f);
				if (ImGui::Button("Save camera"))
				{
					tppCamera->SaveToFile();
				}
			}
			ImGui::EndMenu();
		}
	ImGui::EndMainMenuBar();


	if (openSceneFileDialog)
		draw_file_chooser();
	
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void draw_file_chooser()
{
	std::string extensions = "";
	std::string title = "Choose File";

	if (openSceneFileDialog)
	{
		title = "Choose scene file";
		extensions = ".brp\0\0";
	}

	if (ImGuiFileDialog::Instance()->FileDialog(title.c_str(), extensions.c_str(), ".", ""))
	{
		if (ImGuiFileDialog::Instance()->IsOk == true)
		{
			filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
			path = ImGuiFileDialog::Instance()->GetCurrentPath();
			fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
			cameraPath = fileName.substr(0, fileName.length() - 3).append("cam");
			filter = ImGuiFileDialog::Instance()->GetCurrentFilter();

			if (openSceneFileDialog)
			{
				load_scene();
				cameraCenter[0] = tppCamera->CameraCenter.x;
				cameraCenter[1] = tppCamera->CameraCenter.y;
				cameraCenter[2] = tppCamera->CameraCenter.z;
				cameraPosition[0] = tppCamera->Position.x;
				cameraPosition[1] = tppCamera->Position.y;
				cameraPosition[2] = tppCamera->Position.z;
			}
		}
		else
		{
			filePathName = "";
			path = "";
			fileName = "";
			cameraPath = "";
			filter = "";
		}
	
		openSceneFileDialog = false;
	}
}

void load_shaders()
{
	ourShader = new Shader("vertexTexture.vert", "fragmentTexture.frag");
	frustumShader = new Shader("vertexTexture.vert","frustum.frag");
}

void load_scene()
{
	if (scene != NULL)
		delete scene;

	if (tppCamera != NULL)
		delete tppCamera;
	
	scene = new Scene(fileName.c_str());
	tppCamera = new TPPcamera(cameraPath.c_str());
	camera = tppCamera;
}

void dispose()
{
	if(scene!=NULL)
		delete scene;

	if (tppCamera != NULL)
		delete tppCamera;

	if (fpsCamera != NULL)
		delete fpsCamera;

	if(ourShader != NULL)
		delete ourShader;

	if(frustumShader != NULL)
		delete frustumShader;
}

void init_camera_frustum_buffers()
{
	unsigned int frustumIndices[]=
	{ 
	0, 1,
	1, 2,
	2, 3,
	3, 0,
	4, 5,
	5, 6,
	6, 7,
	7, 4,
	0, 4,
	1, 5,
	2, 6,
	3, 7
	};

	glGenBuffers(1, &cameraVBO);
	glGenBuffers(1, &cameraEBO);
	glGenVertexArrays(1, &cameraVAO);
	glBindVertexArray(cameraVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cameraVBO);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), frustumVertices, GL_STREAM_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cameraEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(unsigned int), frustumIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_frustum_points()
{
	glm::mat4 model = glm::mat4();
	glm::mat4 view = camera->GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)WIDTH / (float)HEIGHT, NEAR_PLANE, FRUSTUM_FAR_PLANE);

	glm::vec4 screenVertices[8];
	screenVertices[0] = glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
	screenVertices[1] = glm::vec4(1.0f, 1.0f, -1.0f, 1.0f);
	screenVertices[2] = glm::vec4(1.0f, -1.0f, -1.0f, 1.0f);
	screenVertices[3] = glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f);
	screenVertices[4] = glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);
	screenVertices[5] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	screenVertices[6] = glm::vec4(1.0f, -1.0f, 1.0f, 1.0f);
	screenVertices[7] = glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f);	
	glm::mat4 invMatrix = glm::inverse(projection * view * model);

	for (unsigned int i = 0; i < 8; i++)
	{
		screenVertices[i] = (invMatrix * screenVertices[i]);
		screenVertices[i] /= screenVertices[i].w;
		for (unsigned int j = i * 3; j < i * 3 + 3; j++)
		{
			frustumVertices[j] = screenVertices[i][j % 3];
		}
	}

	glBindVertexArray(cameraVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cameraVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 24* sizeof(float), frustumVertices);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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

	if (scene != NULL)
	{
		update_frustum_points();

		glm::mat4 frustum_model = glm::mat4();

		//left top
		glViewport(0, HEIGHT*0.5, WIDTH*0.5, HEIGHT*0.5);
		draw_perspective_view();

		//left bottom
		glViewport(0, 0, WIDTH*0.5, HEIGHT*0.5);
		draw_ortho(Scene::TOP);
		draw_frustum(frustum_model, scene->GetOrthoView(Scene::TOP), scene->GetOrtho(RATIO, Scene::TOP));

		//right bottom
		glViewport(WIDTH*0.5, 0, WIDTH*0.5, HEIGHT*0.5);
		draw_ortho(Scene::FRONT);
		draw_frustum(frustum_model, scene->GetOrthoView(Scene::FRONT), scene->GetOrtho(RATIO, Scene::FRONT));

		//right top
		glViewport(WIDTH*0.5, HEIGHT*0.5, WIDTH*0.5, HEIGHT*0.5);
		draw_ortho(Scene::RIGHT);
		draw_frustum(frustum_model, scene->GetOrthoView(Scene::RIGHT), scene->GetOrtho(RATIO, Scene::RIGHT));
	}

	glViewport(0, 0, WIDTH, HEIGHT); //restore default
	draw_UI();
	//check and call events and swap buffers
	glfwPollEvents();
	glfwSwapBuffers(window);
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->ProcessKeyboard(RIGHT, deltaTime);

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

	if (tppCamera != NULL)
	{
		cameraPosition[0] = tppCamera->Position.x;
		cameraPosition[1] = tppCamera->Position.y;
		cameraPosition[2] = tppCamera->Position.z;
	}
	
}

void draw_frustum(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
	frustumShader->use();
	frustumShader->setMat4("model", model);
	frustumShader->setMat4("view", view);
	frustumShader->setMat4("projection", projection);
	glBindVertexArray(cameraVAO);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void draw_perspective_view()
{
	glm::mat4 model = glm::mat4();
	glm::mat4 view = camera->GetViewMatrix();
	glm::mat4 projection = PerspectiveMatrix(glm::radians(camera->Zoom), (float)WIDTH / (float)HEIGHT, NEAR_PLANE, FAR_PLANE);
	ourShader->use();
	ourShader->setMat4("model", model);
	ourShader->setMat4("view", view);
	ourShader->setMat4("projection", projection);
	scene->Draw();
}

void draw_ortho(Scene::Side side)
{
	glm::mat4 model = glm::mat4();
	glm::mat4 view = scene->GetOrthoView(side);
	glm::mat4 projection = scene->GetOrtho(RATIO, side);
	ourShader->use();
	ourShader->setMat4("model", model);
	ourShader->setMat4("view", view);
	ourShader->setMat4("projection", projection);
	scene->Draw();
}

