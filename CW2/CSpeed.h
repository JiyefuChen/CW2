#pragma once
#include "Model.h"
#include "Camera.h"
#include "CWindow.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class CSpeed
{
private:
	Camera* camera;
	//Max Min speed, it shows 100 times that
	float SpeedMax = 10.24;
	int SpeedMin = 0;
	//Forward velocity on the z axis
	float* ForwardSpeed;
	//Forward velocity on the y axis
	float* UpSpeed;
	//The thrust of the plane is the speed shown on the screen
	float Speed;
	//Acceleration (actually, acceleration of acceleration)
	float AddSpeed = 0.01;
	Model* plane;
	Shader* shader;
	CWindow* pWnd;
	//Pitch Angle
	float angle = 0;
	//Yaw Angle
	float lrangle = 0;
	//The actual pitch Angle is for animation effect
	float nowangle = 0;
	//Upper pitch Angle
	float MaxAngle = 30;
	//Roll Angle
	float tempangle = 0;
	bool fWarning = false;
	TCHAR Cause[128];

	void ShowWarning();
public:
	void Init(Camera* c, Model* m, Shader* s, CWindow* w);
	void Forward();
	void SlowDown();
	void Update();
	void Up();
	void Down();
	void Draw();
	//This was used for initial testing and is no longer used
	void AddManySpeed();
	void TurnLeft();
	void TurnRight();
	float GetSpeed() { return Speed; }
	int GetPitch() { return nowangle * 10; }
	int GetYaw() { return lrangle; }
	int GetRoll() { return tempangle * 10; }
	void restart();
};

