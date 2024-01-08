#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
using namespace std;
class Shader
{
private:
	const char* vertexSource;
	const char* fragmentSource;
public:
	Shader(int, int);
	unsigned int ID;	//Shader Program ID
	void use();
};

