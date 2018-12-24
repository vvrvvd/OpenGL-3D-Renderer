#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "shader.h"
#include "Camera.h"
#include "TPPcamera.h"
#include "FPScamera.h"
#include "image_loader.h"
#include "scene_loader.h"


//Constants
const const char* SCENE_PATH = "scene3.txt";
const const char* CAM_PATH = "cam.txt";
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;
const float RATIO = (float)WIDTH / (float)HEIGHT;

const glm::vec4 BACKGROUND_COLOR = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f);

int main();

//Initialize
void init_glfw();
void init_window();
void init_glad();
void config_openGL();
void load_shaders();
void dispose_shaders();
void load_scene();
void dispose_scene();

//Core loop
void core_loop();
void update_time();
void process_input(GLFWwindow* window);

//Callbacks and listeners
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//Mouse callback variables
bool firstMouse = true;
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;

//Time variables
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//Dynamic objects
Shader* ourShader;
Scene* scene;

//Static objects
GLFWwindow* window;
TPPcamera tppCamera = TPPcamera(CAM_PATH);
FPScamera fpsCamera = FPScamera(glm::vec3(0.0f, 1.0f, 0.0f));
Camera& camera = tppCamera;
