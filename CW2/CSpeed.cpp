#include "CSpeed.h"


void CSpeed::ShowWarning()
{

}

void CSpeed::Init(Camera* c, Model* m, Shader* s, CWindow* w)
{
	camera = c;
	plane = m;
	shader = s;
	pWnd = w;
	ForwardSpeed = &(camera->moveZ);
	UpSpeed = &(camera->moveY);
}

void CSpeed::Forward()
{

	Speed += AddSpeed;
	if (Speed > SpeedMax)
		Speed = SpeedMax;
	if (Speed > 0.5)
		AddSpeed = 0.01;
	if (Speed > 1)
		AddSpeed = 0.03;
	if (Speed > 2.5)
		AddSpeed = 0.05;
	if (Speed > 5)
		AddSpeed = 0.1;

}

void CSpeed::SlowDown()
{
	Speed -= AddSpeed;
	if (Speed < SpeedMin)
		Speed = SpeedMin;
	if (Speed > 0.5)
		AddSpeed = 0.03;
	if (Speed > 1)
		AddSpeed = 0.05;
	if (Speed > 2.5)
		AddSpeed = 0.1;

}

void CSpeed::Update()
{
	//Speed less than 2 when head down
	if (Speed < 2 && !camera->isOnGround())
		angle = min(-(2 - Speed) * 90 / 2, angle);


	if (angle > MaxAngle)
		angle = MaxAngle;



	//Speed of rise/fall
	if (Speed > 2)
		*UpSpeed = Speed * sin(glm::radians(angle));
	else
		*UpSpeed = (4 - Speed) * sin(glm::radians(angle));
	*ForwardSpeed = Speed * cos(glm::radians(angle)) * cos(glm::radians(lrangle));

	//gravity
	*UpSpeed -= 2 * fabs(sin(glm::radians(angle))) * max(tempangle / 45,1.0f);

	camera->Position.x -= Speed * sin(glm::radians(lrangle));

	camera->updateCameraPos();
	if (camera->isOnGround())
	{
		if (fWarning)
		{
			pWnd->Cause = Cause;
			pWnd->fDestroy = true;
			pWnd->hRestart = NULL;
		}
		angle = 0;
		//lrangle = 0;
	}
	if (nowangle > angle)
		nowangle -= 0.5;
	if (nowangle < angle)
		nowangle += 0.5;

	if (tempangle < 0)
		tempangle += 0.5;
	if (tempangle > 0)
		tempangle -= 0.5;

	//This is where the roll Angle goes beyond 180 degrees and then it goes the other way

	if (tempangle > 0)
		tempangle = ((int)tempangle % 360) > 180 ? (int(tempangle) % 360) - 360 : ((int)tempangle % 360) + (int(10 * tempangle) % 10) / 10.0;
	else
		tempangle = (abs(int(tempangle)) % 360) > 180 ? -(abs(int(tempangle)) % 360) + 360 : (int(tempangle) % 360) + (int(10 * tempangle) % 10) / 10.0;

	ShowWarning();
}

void CSpeed::Up()
{
	if (Speed < 2)
		return;
	angle += 0.5;
}

void CSpeed::Down()
{
	if (angle <= -MaxAngle)
		return;
	angle -= 0.5;
}

void CSpeed::Draw()
{
	//Rotary plotting aircraft
	glm::mat4 modelMat;
	modelMat = glm::translate(modelMat, glm::vec3(camera->Position.x - sin(glm::radians(lrangle)) * 15,
		camera->Position.y - 5.5, camera->Position.z + cos(glm::radians(lrangle)) * 15));
	modelMat = glm::rotate(modelMat, glm::radians(lrangle), glm::vec3(0, -1, 0));
	modelMat = glm::rotate(modelMat, glm::radians(tempangle), glm::vec3(0, 0, 1));

	modelMat = glm::rotate(modelMat, glm::radians(nowangle), glm::vec3(-1, 0, 0));
	//modelMat = glm::translate(danweiMat, glm::vec3(50, 2.5, 50));
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMat"), 1, GL_FALSE, glm::value_ptr(modelMat));
	plane->Draw(*shader);
}

void CSpeed::AddManySpeed()
{
	Speed += 2;
}

void CSpeed::TurnLeft()
{
	if (camera->isOnGround())
		return;

	lrangle -= 2;
	//Let the camera Angle move and refresh as well
	camera->Yaw += glm::radians(2.0);
	camera->ProcessMouseMovement(0, 0);
	tempangle -= 1;
}

void CSpeed::TurnRight()
{
	if (camera->isOnGround())
		return;

	lrangle += 2;
	camera->Yaw -= glm::radians(2.0);
	camera->ProcessMouseMovement(0, 0);
	tempangle += 1;
}

void CSpeed::restart()
{
	angle = 0;
	lrangle = 0;
	nowangle = 0;
	MaxAngle = 30;
	tempangle = 0;
	fWarning = false;
	AddSpeed = 0.01;
	Speed = 0;
}
