#include "Shader.h"

Shader::Shader(int vertexID,int fragmentID)
{

	int success;
	char infoLog[512];

	vertexSource = (const char*)LockResource(LoadResource(NULL, FindResource(NULL, MAKEINTRESOURCE(vertexID), TEXT("txt"))));
	fragmentSource = (const char*)LockResource(LoadResource(NULL, FindResource(NULL, MAKEINTRESOURCE(fragmentID), TEXT("txt"))));

	unsigned int vertex, fragment;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexSource, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		MessageBoxA(NULL, infoLog, "vertex error!", MB_OK | MB_ICONERROR);
		exit(-1);
	};



	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentSource, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		MessageBoxA(NULL, infoLog, "fragment error!", MB_OK | MB_ICONERROR);
		exit(-1);
	};

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		MessageBoxA(NULL, infoLog, "link error!", MB_OK | MB_ICONERROR);
		exit(-1);
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	
}

void Shader::use()
{
	glUseProgram(ID);
}