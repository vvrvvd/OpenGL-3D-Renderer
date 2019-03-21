#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "Config/CameraConfig.h"

#include "Utils/Shader.h"
#include "Utils/MatrixUtils.h"

#include "Scene/Scene.h"
#include "Scene/Light.h"
#include "Scene/TPPcamera.h"
#include "Scene/FPScamera.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/ImGuiFileDialog.h"

unsigned int WIDTH = 800;
unsigned int HEIGHT = 600;

TPPcamera* tppCamera;
FPScamera* fpsCamera;
Camera* camera;

//Constants
const float RATIO = (float)WIDTH / (float)HEIGHT;

int main();

//Initialize
void init_glfw();
void init_window();
void init_glad();
void config_openGL();
void init_imgui();
void draw_UI();
void draw_file_chooser();
void load_shaders();
void load_scene();
void dispose();
void init_camera_frustum_buffers();
void update_frustum_points();

//Core loop
void core_loop();
void update_time();
void process_input(GLFWwindow* window);
void draw_perspective_view();
void draw_ortho(Scene::Side side);
void draw_frustum(glm::mat4 model, glm::mat4 view, glm::mat4 projection);

//Callbacks and listeners
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//Mouse callback variables
bool firstMouse = true;
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
unsigned int last_rmb_state = GLFW_RELEASE;

//Time variables
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//Dynamic objects
Shader* sceneShader;
Shader* phongShader;
Shader* gouraudShader;
Shader* frustumShader;
Scene* scene;
Light* light;

//Static objects
GLFWwindow* window;
unsigned int cameraVAO;
unsigned int cameraVBO;
unsigned int cameraEBO;
float frustumVertices[24];

//Camera parameters
float cameraCenter[3];
float cameraPosition[3];
std::string filePathName = "";
std::string path = "";
std::string fileName = "";
std::string cameraPath = "";
std::string filter = "";
bool openSceneFileDialog = false;

//Light parameters
float lightPos[3];
float lightColor[3];
