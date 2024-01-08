#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldup)
{
	Position = position;
	WorldUp = worldup;
	Forward = glm::normalize(target - position);
	Right = glm::normalize(glm::cross(Forward, WorldUp));
	Up = glm::normalize(glm::cross(Forward, Right));
}

Camera::Camera(glm::vec3 position, float pitch, float yaw, glm::vec3 worldup)
{
	Position = position;
	WorldUp = worldup;
	Pitch = pitch;
	Yaw = yaw;
	Forward.x = glm::cos(Pitch) * glm::sin(Yaw);
	Forward.y = glm::sin(Pitch);
	Forward.z = glm::cos(Pitch) * glm::cos(Yaw);
	Right = glm::normalize(glm::cross(Forward, WorldUp));
	Up = glm::normalize(glm::cross(Forward, Right));
}


glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(Position, Position + Forward, WorldUp);
}

void Camera::updateCameraVectors()
{
	// calculate the new Front vector
	Forward.x = glm::cos(Pitch) * glm::sin(Yaw);
	Forward.y = glm::sin(Pitch);
	Forward.z = glm::cos(Pitch) * glm::cos(Yaw);
	Right = glm::normalize(glm::cross(Forward, WorldUp));
	Up = glm::normalize(glm::cross(Right, Forward));
}

void Camera::ProcessMouseMovement(float deltaX, float deltaY)
{
	Pitch -= deltaY * Sense;
	Yaw -= deltaX * Sense;

	if (Pitch > 1)
		Pitch = 1;
	if (Pitch < -1)
		Pitch = -1;
	updateCameraVectors();
}
void Camera::updateCameraPos()
{
	//float tempy = Position.y;
	//float tempx = Position.x;
	Position.z += moveZ;
	//Position.x = tempx;
	//Position += Right * moveX;
	//Position.y = tempy;
	Position.y += moveY;
	if (fxLimit)
	{
		Position.x = max(xMin, Position.x);
		Position.x = min(xMax, Position.x);
	}
	if (fyLimit)
	{
		Position.y = max(yMin, Position.y);
		Position.y = min(yMax, Position.y);
	}
	if (fzLimit)
	{
		Position.z = max(zMin, Position.z);
		Position.z = min(zMax, Position.z);
	}
}
void Camera::SetxLimit(int Min, int Max)
{
	xMax = Max;
	xMin = Min;
	fxLimit = true;
}
void Camera::SetyLimit(int Min, int Max)
{
	yMax = Max;
	yMin = Min;
	fyLimit = true;
}
void Camera::SetzLimit(int Min, int Max)
{
	zMax = Max;
	zMin = Min;
	fzLimit = true;
}

bool Camera::isOnGround()
{
	return Position.y == yMin;
}
