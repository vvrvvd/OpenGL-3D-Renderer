#ifndef  CAMERA_CONFIG
#define CAMERA_CONFIG

// Default frustum and camera parameters
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 100.0f;
const float LIGHT_SCALE = 0.2f;
const float FRUSTUM_FAR_PLANE = 2.5f;
const glm::vec4 BACKGROUND_COLOR = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f);

// Default camera transform parameters
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

#endif