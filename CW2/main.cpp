#include <iostream>
using namespace std;
#include <Windows.h>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "resource.h"
//CWindow is responsible for all the display of text, buttons, etc
#include "CWindow.h"
//CSpeed manages all aspects of the aircraft's properties
#include "CSpeed.h"
//The contents of the following header files are basically copy and paste
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Model.h"



struct Map
{
	//position
	int x;
	int y;
	int id;
	//Angle of rotation
	int angle;
	//Scale of scaling
	float k;
};
Map*** map;

//Load images from memory
enum { type_jpg, type_png, type_bmp };
void Load_From_ID(int id, int* pwidth, int* pheight, int* pnrChannel, unsigned char*& p, int type = type_jpg)
{
	HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(id), type == type_jpg ? TEXT("jpg") : (type == type_png ? TEXT("png") : RT_BITMAP));
	void* pBuffer = LockResource(LoadResource(NULL, hRes));
	int size = SizeofResource(NULL, hRes);
	p = stbi_load_from_memory((unsigned char*)pBuffer, size, pwidth, pheight, pnrChannel, 0);
}
//Load the sky box, also from memory
unsigned int loadCubemap(vector<int> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	stbi_set_flip_vertically_on_load(false);
	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data;
		Load_From_ID(faces[i], &width, &height, &nrChannels, data);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			MessageBoxA(0, "Resource loading error", 0, 0);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
unsigned int LoadImageToGPU(int id, int plus)
{
	unsigned int TexBuffer;
	glGenTextures(1, &TexBuffer);
	glActiveTexture(GL_TEXTURE0 + plus);
	glBindTexture(GL_TEXTURE_2D, TexBuffer);


	stbi_set_flip_vertically_on_load(true);

	int width, height, nrChannel;
	unsigned char* data;
	Load_From_ID(id, &width, &height, &nrChannel, data);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	return TexBuffer;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

//The map size shown actually shows a ShowRange * 2 square in both length and width
int ShowRange = 250;
//True map size
int mapsize = 100000;
//It's divided into blocks
int BlockNum = mapsize / ShowRange;
int HousePerBlock = ShowRange / 25;
bool firstMouse = true;
CWindow Wnd;
Camera camera(glm::vec3(mapsize / 2, 9, mapsize / 10), glm::radians(-15.0), glm::radians(0.0), glm::vec3(0, 1, 0));
CSpeed Plane;

float vertices[] = {

	-ShowRange,  0.5f, -ShowRange,  0.0f, ShowRange,
	 ShowRange,  0.5f, -ShowRange,  ShowRange, ShowRange,
	 ShowRange,  0.5f,  ShowRange,  ShowRange, 0.0f,
	 ShowRange,  0.5f,  ShowRange,  ShowRange, 0.0f,
	-ShowRange,  0.5f,  ShowRange,  0.0f, 0.0f,
	-ShowRange,  0.5f, -ShowRange,  0.0f, ShowRange
};

//Compute fps
int CalcFps()
{
	static clock_t Time;
	static int fps;
	clock_t temp;
	temp = Time;
	Time = clock();
	if (Wnd.stop)
		return fps;
	if (Time == 0)
		return -1;
	if (Time == temp)
		return 1000;
	fps = CLOCKS_PER_SEC / (Time - temp);
	return fps;

}
//Initialize map
void InitMap(vector<Model> models)
{
	//Allocation space
	map = new Map * *[BlockNum];
	for (int i = 0; i < BlockNum; i++)
	{
		map[i] = new Map * [BlockNum];
		for (int j = 0; j < BlockNum; j++)
		{
			map[i][j] = new Map[HousePerBlock];
		}
	}
	int num = models.size();

	//Partition block initialization information
	for (int i = 0; i < BlockNum; i++)
	{
		for (int j = 0; j < BlockNum; j++)
		{
			for (int r = 0; r < HousePerBlock; r++)
			{
				map[i][j][r] = { rand() % ShowRange + i * ShowRange,rand() % ShowRange + j * ShowRange,
					rand() % num,rand() % 360, float(rand() % 400 + 800) / 1000.0f };
			}
		}
	}
}
//Mouse control
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	static float lastX, lastY;
	if (Wnd.stop)
		return;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float deltaX, deltaY;
	deltaX = xpos - lastX;
	deltaY = ypos - lastY;



	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(deltaX, deltaY);
}
void processInput(GLFWwindow* window)
{
	if (Wnd.stop)
		return;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstMouse = true;
		Wnd.Stop();

	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Plane.Forward();
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Plane.SlowDown();
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		Plane.Up();
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		Plane.Down();
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Plane.TurnLeft();
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Plane.TurnRight();
	}
	Plane.Update();
}


int WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
	srand(time(0));
	//Prevents DPI amplification from causing problems in the CWindow
	SetProcessDPIAware();
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);


#pragma region Open a Window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(cx, cy, "CW2", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glewExperimental = true;

	glewInit();

	glViewport(0, 0, cx, cy);
	glEnable(GL_DEPTH_TEST);
#pragma endregion

	//Load Shader from memory
	Shader MainShader(IDR_VERTEX, IDR_FRAGMENT);


#pragma region Init and Load Models to VAO, VBO

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	vector<int> faces
	{
		IDR_RIGHT,
		IDR_LEFT,
		IDR_TOP,
		IDR_bottom,
		IDR_FRONT,
		IDR_BACK
	};
	unsigned int cubemapTexture = loadCubemap(faces);
	Shader SkyShader(IDR_SKY_VERTEX, IDR_SKY_FRAGMENT);
#pragma endregion

	Model house("house\\house.obj");
	Model house2("house2\\house.obj");
	Model plane("plane\\plane.obj");

	Plane.Init(&camera, &plane, &MainShader, &Wnd);

	vector<Model> houses;
	houses.push_back(house);
	houses.push_back(house2);

	InitMap(houses);


	//Load map material
	int t1 = LoadImageToGPU(IDR_JPG1, 1);

	MainShader.use();

	glm::mat4 viewMat;
	glm::mat4 projMat;
	projMat = glm::perspective(glm::radians(60.0), (double)cx / cy, 0.5, 2048.0);

	//Settings
	camera.SetxLimit(0, mapsize);
	camera.SetyLimit(1 + 8, INT_MAX);
	camera.SetzLimit(0, mapsize);

	//Get the window handle created by OpenGL
	HWND hwnd = GetActiveWindow();
	TCHAR Buffer[128];
	ShowWindow(hwnd, SW_MAXIMIZE);
	Wnd.SetWindow(hwnd);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		glClear(GL_DEPTH_BUFFER_BIT);

		//plot map
		viewMat = camera.getViewMatrix();
		MainShader.use();
		glUniformMatrix4fv(glGetUniformLocation(MainShader.ID, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(glGetUniformLocation(MainShader.ID, "projMat"), 1, GL_FALSE, glm::value_ptr(projMat));

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, t1);
		glBindVertexArray(VAO);

		glm::mat4 modelMat;
		//It's a weird feeling without blurring the location
		modelMat = glm::translate(modelMat, glm::vec3((int)camera.Position.x / 10 * 10, 0, (int)camera.Position.z / 10 * 10));
		glUniformMatrix4fv(glGetUniformLocation(MainShader.ID, "modelMat"), 1, GL_FALSE, glm::value_ptr(modelMat));
		glUniform1i(glGetUniformLocation(MainShader.ID, "pic"), 1);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glUniform1i(glGetUniformLocation(MainShader.ID, "pic"), 0);

		//Plot aircraft
		Plane.Draw();

//Draw sky box
#pragma region skybox
		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		SkyShader.use();
		glUniform1i(glGetUniformLocation(SkyShader.ID, "skybox"), 0);
		viewMat = glm::mat4(glm::mat3(camera.getViewMatrix())); // remove translation from the view matrix
		glUniformMatrix4fv(glGetUniformLocation(SkyShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(glGetUniformLocation(SkyShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projMat));
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default
#pragma endregion


		glfwSwapBuffers(window);
		glfwPollEvents();

//Draw text
#pragma region text

		Wnd.clear();

		wsprintf(Buffer, TEXT("Jiyefu Chen"), CalcFps());
		Wnd.Textout(Buffer, 1300, 800, 40);

#pragma endregion 

//restart
#pragma region restart
		if (Wnd.fDestroy)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			Wnd.Destroy();
		}
		if (Wnd.restart)
		{
			Wnd.fDestroy = false;
			Wnd.stop = false;
			Wnd.restart = false;

			camera = Camera(glm::vec3(mapsize / 2, 9, mapsize / 10), glm::radians(-15.0), glm::radians(0.0), glm::vec3(0, 1, 0));
			camera.SetxLimit(0, mapsize);
			camera.SetyLimit(1 + 8, 2048 + 8);
			camera.SetzLimit(0, mapsize);

			Plane.restart();
			Plane.Init(&camera, &plane, &MainShader, &Wnd);

			firstMouse = true;
		}
#pragma endregion

		//Pause slightly depending on fluency
		Sleep(Wnd.SpeedLevel * 10);
	}
	glfwTerminate();

}

