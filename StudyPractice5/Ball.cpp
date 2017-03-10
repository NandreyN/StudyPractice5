#undef UNICODE
#include <windows.h>
#include  <math.h>
#include <cmath>
#include <string>
#define BALL_TIMER 0

using namespace std;

struct Circle
{
	Circle() { R = -1; };
	int R;
	POINT center;
};

BOOL InitApplication(HINSTANCE hinstance);
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
void DeleteCircle(HDC& hdc, Circle& circle);
void DrawCircle(HDC& hdc, Circle& circle);
bool HandleTimer(HDC& hdc, int x, int y, Circle& circle, bool isDirect);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prevHinstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	if (!InitApplication(hinstance))
	{
		MessageBox(NULL, "Unable to Init App", "Error", MB_OK);
		return FALSE;
	}

	if (!InitInstance(hinstance, nCmdShow))
	{
		MessageBox(NULL, "Unable to Init Instance", "Error", MB_OK);
		return FALSE;
	}

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

BOOL InitApplication(HINSTANCE hinstance)
{
	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hinstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_CROSS);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = "Ball";
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wndclass))
	{
		MessageBox(NULL, "Cannot register class", "Error", MB_OK);
		return FALSE;
	}
	return TRUE;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	static int x, y;
	static HDC hdc;
	static Circle circle;
	static bool isDirect;

	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hwnd);
		SetTimer(hwnd, BALL_TIMER, 30, (TIMERPROC)NULL);
		circle.center.x = circle.center.y = 0;
		circle.R = 5;
		isDirect = true;
		break;
	case WM_SIZE:
		x = LOWORD(lparam);
		y = HIWORD(lparam);
		break;
	case WM_TIMER:
		switch (wparam)
		{
		case BALL_TIMER:
			isDirect = HandleTimer(hdc, x - 5, y - 5, circle, isDirect);
			break;
		default:return FALSE;
		}
		break;
	case WM_CLOSE:
		ReleaseDC(hwnd, hdc);
		KillTimer(hwnd, BALL_TIMER);
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
	return FALSE;
}

void DeleteCircle(HDC& hdc, Circle& circle)
{
	SetROP2(hdc, R2_WHITE);
	DrawCircle(hdc, circle);
	SetROP2(hdc, R2_BLACK);
}

void DrawCircle(HDC& hdc, Circle& circle)
{
	int x = circle.center.x, y = circle.center.y, r = circle.R;
	Ellipse(hdc, x - r, y - r, x + r, y + r);
}

bool HandleTimer(HDC& hdc, int x, int y, Circle& circle, bool isDirect)
{
	DeleteCircle(hdc, circle);
	int perX = x / 10, perY = y / 2; // ���� ������� � ��������

	circle.center.x += (isDirect) ? 5 : -5;

	double val = sin((double)circle.center.x / perX)  * perY;
	circle.center.y = y / 2 + val;
	DrawCircle(hdc, circle);
	
	if ((x - 2 <= circle.center.x && circle.center.x <= x + 3) ||( circle.center.x >= -3 && circle.center.x <= 2))
		return !isDirect;
	return isDirect;
}

BOOL InitInstance(HINSTANCE hinstance, int nCmdShow)
{
	HWND hwnd;
	hwnd = CreateWindow(
		"Ball",
		"Ball",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		NULL,
		NULL,
		hinstance,
		NULL);


	if (!hwnd)
		return FALSE;
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	return TRUE;
}
