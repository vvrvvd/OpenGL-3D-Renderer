#include "main.h"

int main()
{
	initGLFW();
	initWindow();
	initGLAD();
	configOpenGL();
	initImGUI();

	loadShaders();

	initCameraFrustumBuffers();

	while (!glfwWindowShouldClose(window))
	{
		updateTime();
		coreLoop();
	}

	ImGui_ImplGlfw_Shutdown();

	dispose();

	glfwTerminate();
	return 0;
}

void initGLFW()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void initWindow()
{
	window = glfwCreateWindow(WIDTH, HEIGHT, "Learn OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create a window";
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetScrollCallback(window, scrollCallback);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
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

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if(camera!=NULL)
		camera->ProcessMouseScroll(yoffset);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	WIDTH = width;
	HEIGHT = height;
	glViewport(0, 0, width, height);
}

void initGLAD()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Couldn't load GLAD";
	}
}

void configOpenGL()
{
	glViewport(0, 0, WIDTH, HEIGHT);

	//Z-BUFFER
	glEnable(GL_DEPTH_TEST);
}

void initImGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); 
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(NULL);
}

void drawUI()
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

		if (ImGui::BeginMenu("Light"))
		{
			if (scene != NULL)
			{
				if (ImGui::Button("Phong"))
					sceneShader = phongShader;

				ImGui::SameLine();

				if (ImGui::Button("Gouraud"))
					sceneShader = gouraudShader;

				if (ImGui::InputFloat3("Position", lightPos))
				{
					scene->LightPos = glm::vec3(lightPos[0], lightPos[1], lightPos[2]);
					light->position = scene->LightPos;
				}

				if (ImGui::ColorPicker3("Color", lightColor))
				{
					scene->LightColor = glm::vec3(lightColor[0], lightColor[1], lightColor[2]);
					light->color = scene->LightColor;
				}
				
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Material"))
		{
			if (scene != NULL)
			{
				ImGui::SameLine();

				if (ImGui::InputFloat("Ambient", &DefaultMaterial.ambient));

				if (ImGui::InputFloat("Specular", &DefaultMaterial.specular));

				if (ImGui::ColorPicker3("Color", DefaultColor))
				{
					DefaultMaterial.color.r = DefaultColor[0];
					DefaultMaterial.color.g = DefaultColor[1];
					DefaultMaterial.color.b = DefaultColor[2];
				}

			}
			ImGui::EndMenu();
		}
	ImGui::EndMainMenuBar();


	if (openSceneFileDialog)
		drawFileChooser();
	
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void drawFileChooser()
{
	std::string extensions = "";
	std::string title = "Choose File";

	if (openSceneFileDialog)
	{
		title = "Choose scene file";
		extensions = "\0";
	}

	if (ImGuiFileDialog::Instance()->FileDialog(title.c_str(), extensions.c_str(), ".", ""))
	{
		if (ImGuiFileDialog::Instance()->IsOk == true)
		{
			filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
			path = ImGuiFileDialog::Instance()->GetCurrentPath();
			fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
			cameraPath = path + "//" + fileName.substr(0, fileName.length() - 3).append("cam");
			filter = ImGuiFileDialog::Instance()->GetCurrentFilter();

			if (openSceneFileDialog)
			{
				loadScene();
				cameraCenter[0] = tppCamera->CameraCenter.x;
				cameraCenter[1] = tppCamera->CameraCenter.y;
				cameraCenter[2] = tppCamera->CameraCenter.z;
				cameraPosition[0] = tppCamera->Position.x;
				cameraPosition[1] = tppCamera->Position.y;
				cameraPosition[2] = tppCamera->Position.z;
				lightPos[0] = scene->LightPos[0];
				lightPos[1] = scene->LightPos[1];
				lightPos[2] = scene->LightPos[2];
				lightColor[0] = scene->LightColor[0];
				lightColor[1] = scene->LightColor[1];
				lightColor[2] = scene->LightColor[2];
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

void loadShaders()
{
	phongShader = new Shader("Shaders/vertexTexturePhong.vert", "Shaders/fragmentTexturePhong.frag");
	gouraudShader = new Shader("Shaders/vertexTextureGouraud.vert", "Shaders/fragmentTextureGouraud.frag");
	sceneShader = gouraudShader;
	frustumShader = new Shader("Shaders/frustum.vert","Shaders/frustum.frag");
}

void loadScene()
{
	if (scene != NULL)
		delete scene;

	if (tppCamera != NULL)
		delete tppCamera;
	
	if (light != NULL)
		delete light;

	scene = new Scene(filePathName.c_str());
	tppCamera = new TPPcamera(cameraPath.c_str());
	light = new Light(scene->LightPos, scene->LightColor, LIGHT_SCALE, "Shaders/light.vert", "Shaders/light.frag");
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

	if (gouraudShader != NULL)
		delete gouraudShader;

	if (phongShader != NULL)
		delete phongShader;

	if(frustumShader != NULL)
		delete frustumShader;

	if (light != NULL)
		delete light;
}

void initCameraFrustumBuffers()
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

void updateFrustumPoints()
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
	glBufferSubData(GL_ARRAY_BUFFER, 0, 24 * sizeof(float), frustumVertices);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void updateTime()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void coreLoop()
{
	processInput(window);

	//rendering
	glClearColor(BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, BACKGROUND_COLOR.a);

	//Z-BUFFER
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (scene != NULL)
	{
		updateFrustumPoints();

		glm::mat4 frustum_model = glm::mat4();

		//left top
		glViewport(0, HEIGHT*0.5, WIDTH*0.5, HEIGHT*0.5);
		drawPerspectiveView();

		//left bottom
		glViewport(0, 0, WIDTH*0.5, HEIGHT*0.5);
		drawOrtho(Scene::TOP);
		drawFrustum(frustum_model, scene->GetOrthoView(Scene::TOP), scene->GetOrthoProjection(RATIO, Scene::TOP));

		//right bottom
		glViewport(WIDTH*0.5, 0, WIDTH*0.5, HEIGHT*0.5);
		drawOrtho(Scene::FRONT);
		drawFrustum(frustum_model, scene->GetOrthoView(Scene::FRONT), scene->GetOrthoProjection(RATIO, Scene::FRONT));

		//right top
		glViewport(WIDTH*0.5, HEIGHT*0.5, WIDTH*0.5, HEIGHT*0.5);
		drawOrtho(Scene::RIGHT);
		drawFrustum(frustum_model, scene->GetOrthoView(Scene::RIGHT), scene->GetOrthoProjection(RATIO, Scene::RIGHT));
	}

	glViewport(0, 0, WIDTH, HEIGHT); //restore default
	drawUI();

	//check and call events and swap buffers
	glfwPollEvents();
	glfwSwapBuffers(window);
}

void processInput(GLFWwindow* window)
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
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (lastRmbState == GLFW_RELEASE && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		firstMouse = true;
		glfwSetCursorPosCallback(window, mouseCallback);
		lastRmbState = GLFW_PRESS;
	}
	else if(lastRmbState == GLFW_PRESS && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursorPosCallback(window, NULL);
		lastRmbState = GLFW_RELEASE;
	}

	if (tppCamera != NULL)
	{
		cameraPosition[0] = tppCamera->Position.x;
		cameraPosition[1] = tppCamera->Position.y;
		cameraPosition[2] = tppCamera->Position.z;
	}
	
}

void drawFrustum(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
	frustumShader->use();
	frustumShader->setMat4("model", model);
	frustumShader->setMat4("view", view);
	frustumShader->setMat4("projection", projection);
	glBindVertexArray(cameraVAO);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void drawPerspectiveView()
{
	glm::mat4 model = glm::mat4();
	glm::mat4 view = camera->GetViewMatrix();
	glm::mat4 projection = PerspectiveMatrix(glm::radians(camera->Zoom), (float)WIDTH / (float)HEIGHT, NEAR_PLANE, FAR_PLANE);
	sceneShader->use();
	sceneShader->setMat4("model", model);
	sceneShader->setMat4("view", view);
	sceneShader->setMat4("projection", projection);
	sceneShader->setVec3("viewPos", tppCamera->Position);
	scene->Draw(sceneShader);
	light->Draw(view, projection);
}

void drawOrtho(Scene::Side side)
{
	glm::mat4 model = glm::mat4();
	glm::mat4 view = scene->GetOrthoView(side);
	glm::mat4 projection = scene->GetOrthoProjection(RATIO, side);
	sceneShader->use();
	sceneShader->setMat4("model", model);
	sceneShader->setMat4("view", view);
	sceneShader->setMat4("projection", projection);
	sceneShader->setVec3("viewPos", tppCamera->Position);
	scene->Draw(sceneShader);
	light->Draw(view, projection);
}

