#include "CWindow.h"
void CWindow::SetWindow(HWND hwnd)
{
	hwndGL = hwnd;
	InitWnd();
	hdc = GetDC(this->hwnd);
	cxScreen = GetSystemMetrics(SM_CXSCREEN);
	cyScreen = GetSystemMetrics(SM_CYSCREEN);
}
//Output text
void CWindow::Textout(const TCHAR* str, int x, int y, int Length, COLORREF Color, string type, bool xieti)
{

	TextInfo* temp = new TextInfo;
	temp->color = Color;
	temp->str = new TCHAR[lstrlen(str) + 1];
	lstrcpy(temp->str, str);
	temp->size = Length;
	temp->x = x;
	temp->y = y;
	temp->type = type;
	temp->xieti = xieti;
	SendMessage(hwnd, WM_USER + 1, (WPARAM)temp, NULL);

}

void CWindow::Stop()
{
	PostQuitMessage(0);
}

void CWindow::Destroy()
{
	
}
void CWindow::clear()
{
	SendMessage(hwnd, WM_USER + 2, NULL, NULL);
}
//Move
void CWindow::Update()
{
	RECT GlRect;
	GetWindowRect(hwndGL, &GlRect);
	cxLength = GlRect.right - GlRect.left;
	cyLength = GlRect.bottom - GlRect.top;
	if (cxLength < cxScreen / 2 || cyLength < cyScreen / 2)
	{
		MoveWindow(hwndGL, GlRect.left, GlRect.top, cxScreen / 2, cyScreen / 2, true);
		cxLength = cxScreen / 2;
		cyLength = cyScreen / 2;
	}

	cxLength -= 20;
	cyLength -= 50;

	MoveWindow(hwnd, GlRect.left + 10, GlRect.top + 40, cxLength, cyLength, true);
	MoveWindow(hContinue, cxLength / 3, cyLength / 5, cxLength / 3, 50, true);
	MoveWindow(hHelp, cxLength / 3, cyLength * 2 / 5, cxLength / 3, 50, true);
	MoveWindow(hLevel, cxLength / 3, cyLength * 3 / 5, cxLength / 3, 50, true);
	MoveWindow(hExit, cxLength / 3, cyLength * 4 / 5, cxLength / 3, 50, true);
	MoveWindow(hRestart, cxLength / 3, cyLength * 2 / 3, cxLength / 3, 50, true);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static CWindow* pWnd;
	static HWND hStatic, hReturn;
	static HINSTANCE hInstance;
	static int cxLength, cyLength;
	static HFONT hFont,hStaticFont;
	static TCHAR str;
	static vector<TextInfo> Texts;
	static bool noShowText = false;

	HDC hdc;
	PAINTSTRUCT ps;
	HBRUSH hBrush;

	switch (message)
	{
	case WM_CREATE:
		hStaticFont = CreateFontA(40,
			40, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
			0, 0, PROOF_QUALITY, 0, "Courier New");
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		SetLayeredWindowAttributes(hwnd, RGB(233, 133, 33), 0, LWA_COLORKEY);
		hStatic = CreateWindow(TEXT("static"), NULL, WS_CHILD | WS_VISIBLE | SS_CENTER, 0, 0, 0, 0, hwnd, NULL, hInstance, NULL);
		SetTimer(hwnd, 0, 50, NULL);
		return 0;
	case WM_TIMER:
		if (pWnd->stop || noShowText)
			return 0;
		InvalidateRect(hwnd, NULL, true);
		return 0;
	case WM_USER + 1:
		Texts.push_back(*(TextInfo*)wParam);
		return 0;
	case WM_USER + 2:
		Texts.clear();
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		SetBkMode(hdc, TRANSPARENT);

		for (int i = 0; i < Texts.size(); i++)
		{
			DeleteObject(hFont);
			hFont = CreateFontA(Texts[i].size, Texts[i].size, 0, 0, FW_THIN, Texts[i].xieti, false, false,
				CHINESEBIG5_CHARSET, OUT_CHARACTER_PRECIS,
				CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
				FF_MODERN, Texts[i].type.c_str());
			SelectObject(hdc, hFont);

			SetTextColor(hdc, Texts[i].color);
			TextOut(hdc, Texts[i].x, Texts[i].y, Texts[i].str, lstrlen(Texts[i].str));
		}

		EndPaint(hwnd, &ps);
		return 0;
	case WM_USER:
		pWnd = (CWindow*)lParam;
		return 0;
	case WM_CTLCOLORSTATIC:
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (LRESULT)CreateSolidBrush(RGB(187, 255, 255));
	case WM_LBUTTONDOWN:
		SetFocus(pWnd->hwndGL);
		return 0;
	case WM_COMMAND:
		switch (wParam)
		{
		case 1:
			//MessageBox(0, 0, 0, 0);
			pWnd->stop = false;
			DestroyWindow(pWnd->hContinue);
			DestroyWindow(pWnd->hExit);
			DestroyWindow(pWnd->hHelp);
			DestroyWindow(pWnd->hLevel);
			SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) ^ WS_EX_TRANSPARENT);
			break;
		case 2:
			exit(1);
			break;
		case 3:
			DestroyWindow(pWnd->hRestart);
			//pWnd->hRestart = NULL;
			pWnd->restart = true;
			InvalidateRect(hwnd, NULL, true);
			SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) ^ WS_EX_TRANSPARENT);
			break;
		case 4:
			
			
			break;
		case 5:
			ShowWindow(pWnd->hContinue, SW_SHOW);
			ShowWindow(pWnd->hExit, SW_SHOW);
			ShowWindow(pWnd->hLevel, SW_SHOW);
			ShowWindow(pWnd->hHelp, SW_SHOW);
			ShowWindow(hStatic, SW_HIDE);
			ShowWindow(hReturn, SW_HIDE);
			noShowText = false;
			InvalidateRect(hwnd, NULL, true);
			break;
		case 6:
			break;
		}
		SetFocus(pWnd->hwndGL);
		return 0;
	case WM_SIZE:
		cxLength = LOWORD(lParam);
		cyLength = HIWORD(lParam);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
void MsgThread(LPVOID)
{
	MSG msg;
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
void UpdateThread(LPVOID p)
{
	while (1)
	{

		((CWindow*)p)->Update();
		Sleep(20);
	}
}
void CWindow::InitWnd()
{
	hInstance = GetModuleHandle(NULL);
	WNDCLASS wndclass;
	TCHAR szAppName[] = TEXT("Window");

	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = CreateSolidBrush(RGB(233, 133, 33));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = NULL;
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wndclass);

	hwnd = CreateWindowEx(
		WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
		szAppName,
		NULL,
		WS_POPUP,
		0, 0, 0, 0,
		hwndGL,
		NULL,
		hInstance,
		NULL
	);
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);


	_beginthread(MsgThread, 0, NULL);
	_beginthread(UpdateThread, 0, (LPVOID)this);

	SendMessage(hwnd, WM_USER, 0, (LPARAM)this);
	SetFocus(hwndGL);
}
