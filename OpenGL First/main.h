#include "Config.h"


//Constants
const float RATIO = (float)WIDTH / (float)HEIGHT;

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
void init_camera_frustum_buffers();
void update_frustum_points(glm::mat4 model, glm::mat4 view, glm::mat4 projection);

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
Shader* ourShader;
Shader* frustumShader;
Scene* scene;

//Static objects
GLFWwindow* window;
unsigned int cameraVAO;
unsigned int cameraVBO;
unsigned int cameraEBO;
float frustumVertices[24];