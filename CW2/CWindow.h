#pragma once

#include <Windows.h>
#include <process.h>
#include <iostream>
#include <vector>
using namespace std;

//Output basic information about the string
struct TextInfo
{
	int size;
	int x;
	int y;
	COLORREF color;
	TCHAR* str;
	string type;
	bool xieti;
};

class CWindow
{
public:
	void SetWindow(HWND hwnd);
	//Here you can customize some information about the font
	void Textout(const TCHAR* str, int x, int y, int Length, COLORREF Color = RGB(255, 255, 255), string type = "ËÎÌו", bool xieti = true);
	void Stop();
	void Destroy();
	//Empty the container where the TextInfo is stored
	void clear();

	bool fDestroy = false;
	bool stop = false;
	bool restart = false;
	//Fluency 0 is high and 2 is low
	int SpeedLevel = 1;
	TCHAR* Cause;
	HWND hRestart = NULL;
private:
	//Window handle for this window and OpenGL
	HWND hwnd, hwndGL;
	//Button handle
	HWND hContinue, hExit, hHelp, hLevel;
	HDC hdc;
	HINSTANCE hInstance;
	//Window length and width
	int cxLength, cyLength;
	//Screen length and width
	int cxScreen, cyScreen;

	void InitWnd();
	void Update();

	//Window message response function
	friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	//Move this window to coincide with OpenGL's window
	friend void UpdateThread(LPVOID p);
};

