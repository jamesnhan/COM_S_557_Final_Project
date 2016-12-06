#pragma once

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 1.0f;
const GLfloat SENSITIVITY = 0.25f;
const GLfloat ZOOM = 45.0f;

class Camera
{
public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Left;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	// NOTE: Change to quaternions to avoid gimble locks
	GLfloat Yaw;
	GLfloat Pitch;
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;
	GLfloat Zoom;

	Camera(glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat Yaw = YAW, GLfloat Pitch = PITCH);
	Camera(GLfloat PosX, GLfloat PosY, GLfloat PosZ, GLfloat UpX, GLfloat UpY, GLfloat UpZ, GLfloat Yaw, GLfloat Pitch);
	glm::mat4 GetViewMatrix() const;
	void ProcessKeyboard(CameraMovement Direction, GLfloat DeltaTime, GLfloat Height);
	void ProcessMouseMovement(GLfloat XOffset, GLfloat YOffset, GLboolean ConstrainPitch = true);
	void ProcessMouseScroll(GLfloat YOffset);

private:
	void UpdateCameraVectors();
};