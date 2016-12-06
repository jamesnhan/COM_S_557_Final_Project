#include "Camera.h"

Camera::Camera(glm::highp_vec3 Position, glm::highp_vec3 Up, GLfloat Yaw, GLfloat Pitch)
	: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	this->Position = Position;
	this->WorldUp = Up;
	this->Yaw = Yaw;
	this->Pitch = Pitch;
	this->UpdateCameraVectors();
}

Camera::Camera(GLfloat PosX, GLfloat PosY, GLfloat PosZ, GLfloat UpX, GLfloat UpY, GLfloat UpZ, GLfloat Yaw, GLfloat Pitch)
	: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	this->Position = glm::vec3(PosX, PosY, PosZ);
	this->WorldUp = glm::vec3(UpX, UpY, UpZ);
	this->Yaw = Yaw;
	this->Pitch = Pitch;
	this->UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
}

void Camera::ProcessKeyboard(CameraMovement Direction, GLfloat DeltaTime, GLfloat Height)
{
	GLfloat velocity = this->MovementSpeed * DeltaTime;
	glm::vec3 front = glm::normalize(glm::vec3(this->Front.x, 0.0f, this->Front.z));
	switch (Direction)
	{
		case FORWARD:
			this->Position += this->Front * velocity;
			break;
		case BACKWARD:
			this->Position -= this->Front * velocity;
			break;
		case LEFT:
			this->Position -= this->Right * velocity;
			break;
		case RIGHT:
			this->Position += this->Right * velocity;
			break;
		default:
			break;
	}
	this->Position.y = Height;
}

void Camera::ProcessMouseMovement(GLfloat XOffset, GLfloat YOffset, GLboolean ConstrainPitch)
{
	XOffset *= this->MouseSensitivity;
	YOffset *= this->MouseSensitivity;

	this->Yaw += XOffset;
	this->Pitch += YOffset;

	if (ConstrainPitch)
	{
		if (this->Pitch > 89.0f)
		{
			this->Pitch = 89.0f;
		}

		if (this->Pitch < -89.0f)
		{
			this->Pitch = -89.0f;
		}
	}

	this->UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(GLfloat YOffset)
{
	if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
	{
		this->Zoom -= YOffset;
	}

	if (this->Zoom <= 1.0f)
	{
		this->Zoom = 1.0f;
	}

	if (this->Zoom >= 45.0f)
	{
		this->Zoom = 45.0f;
	}
}

void Camera::UpdateCameraVectors()
{
	glm::vec3 front;

	front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	front.y = sin(glm::radians(this->Pitch));
	front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));

	this->Front = glm::normalize(front);

	this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
	this->Up = glm::normalize(glm::cross(this->Right, this->Front));
}

