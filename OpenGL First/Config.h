#ifndef  CONFIG
#define CONFIG

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "shader.h"
#include "image_loader.h"
#include "Quad.h"
#include "Scene.h"
#include "matrix_utils.h"
#include "TPPcamera.h"
#include "FPScamera.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog.h"

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 20.0f;
const float FRUSTUM_SIZE = 2.5f;
const glm::vec4 BACKGROUND_COLOR = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f);

TPPcamera* tppCamera;
FPScamera* fpsCamera;
Camera* camera;

#endif // ! CONFIG
