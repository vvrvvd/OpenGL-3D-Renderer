#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>
#include <sstream>
#include "string_utils.h"
#include "Camera.h"


//TPP camera class focused on image center point
class TPPcamera : public Camera
{
public:
	const char* path;
	glm::vec3 CameraCenter;
	float angleAroundPlayer;
	float Distance;

	TPPcamera(const char* cameraPath, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f))
	{
		path = cameraPath;
		MovementSpeed = SPEED;
		MouseSensitivity = SENSITIVITY;
		std::ifstream cameraFile;
		std::stringstream cameraDataStream;

		cameraFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			cameraFile.open(cameraPath);
			cameraDataStream << cameraFile.rdbuf();
			cameraFile.close();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::CAMERA::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}

		std::string line;
		std::getline(cameraDataStream, line);
		std::vector<std::string> pos = split(line, " ");

		glm::vec3 camPos = glm::vec3(stof(pos[0]), stof(pos[1]), stof(pos[2]));

		std::getline(cameraDataStream, line);
		pos = split(line, " ");
		glm::vec3 camCenter = glm::vec3(stof(pos[0]), stof(pos[1]), stof(pos[2]));

		std::getline(cameraDataStream, line);
		float fov = stof(line);


		Distance = glm::length(camCenter - camPos);
		CameraCenter = camCenter;
		Position = camPos;
		Zoom = fov;
		WorldUp = up;
		Front = glm::normalize(camCenter - camPos);
		calculateStartPitchAndAngle();
		updateCameraVectors();
	}

	// Constructor with vectors
	TPPcamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Camera(position, up, yaw, pitch)
	{
		updateCameraVectors();
	}

	// Constructor with scalar values
	TPPcamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Camera(posX, posY, posZ, upX, upY, upZ, yaw, pitch)
	{
		updateCameraVectors();
	}

	bool SaveToFile()
	{
		std::ofstream cameraFile;

		cameraFile.exceptions(std::ofstream::failbit | std::ofstream::badbit);

		try
		{
			cameraFile.open(path);
			cameraFile << Position.x << " " << Position.y << " " << Position.z << std::endl;
			cameraFile << CameraCenter.x << " " << CameraCenter.y << " " << CameraCenter.z << std::endl;
			cameraFile << Zoom << std::endl;
			cameraFile.close();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::CAMERA::FILE_NOT_SUCCESFULLY_SAVED" << std::endl;
			return false;
		}

		return true;
	}

	void SetCenter(glm::vec3 cameraCenter)
	{
		CameraCenter = cameraCenter;
		updateCameraVectors();
	}

	virtual void SetPosition(glm::vec3 pos)
	{
		Position = pos;
		Distance = glm::length(CameraCenter - Position);
		Front = glm::normalize(CameraCenter - Position);
		calculateStartPitchAndAngle();
		updateCameraVectors();
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		//return glm::lookAt(Position, CameraCenter, Up);
		return LookAt(Position, CameraCenter, Up);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Distance -= velocity;
		if (direction == BACKWARD)
			Distance += velocity;
		if (direction == LEFT)
			angleAroundPlayer += 30.0 * velocity;
		if (direction == RIGHT)
			angleAroundPlayer -= 30.0 * velocity;

		updateCameraVectors();
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;
		
		Pitch -= yoffset;
		angleAroundPlayer += xoffset;
		angleAroundPlayer = glm::mod(angleAroundPlayer, 360.0f);

		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		//// Update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		if (Zoom >= 1.0f && Zoom <= 89.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 89.0f)
			Zoom = 89.0f;
	}

private:

	void calculateStartPitchAndAngle()
	{
		Pitch = glm::degrees(asinf(-Front.y));
		Pitch = glm::mod(Pitch, 360.0f);
		float sinX = (-Position.x + CameraCenter.x) / (Distance*cos(glm::radians(Pitch)));
		float cosX = (Position.z - CameraCenter.z) / (Distance*cos(glm::radians(Pitch)));
		angleAroundPlayer = glm::degrees(atan2f(sinX, cosX));
		angleAroundPlayer = glm::mod(angleAroundPlayer, 360.0f);
	}

	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		float horDistance = Distance * cos(glm::radians(Pitch));
		float vertDistance = Distance * sin(glm::radians(Pitch));
		float offsetX = (horDistance * sin(glm::radians(angleAroundPlayer)));
		float offsetZ = (horDistance * cos(glm::radians(angleAroundPlayer)));
		Position.x = CameraCenter.x - offsetX;
		Position.z = CameraCenter.z + offsetZ;
		Position.y = CameraCenter.y + vertDistance;
		Front = glm::normalize(CameraCenter - Position);
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}

};


