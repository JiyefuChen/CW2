#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Windows.h>

class Camera
{
public:
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldup);
	Camera(glm::vec3 position, float pitch, float yaw, glm::vec3 worldup);

	glm::vec3 Position;
	glm::vec3 Forward;
	glm::vec3 Right;
	glm::vec3 Up;
	glm::vec3 WorldUp;
	float Pitch;
	float Yaw;
	float Sense = 0.001;
	float moveZ = 0;
	float moveX = 0;
	float moveY = 0;

	glm::mat4 getViewMatrix();
	void ProcessMouseMovement(float deltaX, float deltaY);
	void updateCameraPos();
	void SetxLimit(int, int);
	void SetyLimit(int, int);
	void SetzLimit(int, int);
	bool isOnGround();
private:
	void updateCameraVectors();
	bool fxLimit = false;
	bool fyLimit = false;
	bool fzLimit = false;
	int xMin;
	int xMax;
	int yMin;
	int yMax;
	int zMin;
	int zMax;
};

