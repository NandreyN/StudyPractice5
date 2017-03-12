#undef UNICODE
#include <windows.h>
#include  <math.h>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <ctime>
#include <numeric>

using namespace std;

struct Participant
{
	Participant() { value = -1; };
	string surname;
	int value;
};
istream& operator >> (istream& i, Participant& p);


BOOL InitApplication(HINSTANCE hinstance);
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
void ReadFromFile(vector<Participant>& p);
void DrawChart(HDC& hdc, vector<Participant>& p, int x, int y);


int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prevHinstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	srand(time(NULL));
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
	wndclass.lpszClassName = "Pie";
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
	static vector<Participant> people;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		ReadFromFile(people);
		break;
	case WM_SIZE:
		x = LOWORD(lparam);
		y = HIWORD(lparam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		SetBkMode(hdc, TRANSPARENT);
		DrawChart(hdc, people, x, y);
		SetBkMode(hdc, NULL);
		EndPaint(hwnd, &ps);
		break;
	case WM_CLOSE:
		//ReleaseDC(hwnd, hdc);
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


BOOL InitInstance(HINSTANCE hinstance, int nCmdShow)
{
	HWND hwnd;
	hwnd = CreateWindow(
		"Pie",
		"Pie",
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

istream& operator >> (istream& i, Participant& p)
{
	string a; getline(i, a);

	istringstream iss(a);
	iss >> a; p.surname = a;
	iss >> a; p.value = atoi(a.data());

	return i;
}

void ReadFromFile(vector<Participant>& p)
{
	ifstream in("in.txt");
	copy(istream_iterator<Participant>(in), istream_iterator<Participant>(), back_inserter(p));
	in.close();
}

void DrawChart(HDC& hdc, vector<Participant>& p, int x, int y)
{
	SetMapMode(hdc, MM_ANISOTROPIC);
	SetWindowExtEx(hdc, x, y, NULL);
	SetViewportExtEx(hdc, x, -y, NULL);
	SetViewportOrgEx(hdc, x / 2, y / 2, NULL);

	int R = (x <= y) ? x / 2 : y / 2;

	int lastX = R, lastY = 0;
	double prevAngle = 0.0;

	for_each(p.begin(), p.end(), [&lastX, &lastY, &hdc, &R, &prevAngle](Participant par)
	{
		HBRUSH brush, oldBrush;
		brush = CreateSolidBrush(RGB(rand() % 255, rand() % 255, rand() % 255));
		oldBrush = (HBRUSH)SelectObject(hdc, brush);

		double angle = 6.28 / (100.0f / par.value) + prevAngle;
		int newX = R*cos(angle), newY = R*sin(angle);
		Pie(hdc, -R, R, R, -R, lastX, lastY, newX, newY);

		double midAngle = 6.28 / (100.0f / par.value * 2) + prevAngle;
		double threeAngle = 6.28 / (100.0f / par.value * 3) + prevAngle;
		int middleX = R*cos(midAngle), middleY = R*sin(midAngle);

		// Text
		int LTX = (R / 5)*cos(angle), LTY = (R / 5)*sin(angle);
		int RBX = (R * 0.9)*cos(threeAngle), RBY = (R * 0.9)*sin(threeAngle);
		int LBX = (R / 5)*cos(prevAngle), LBY = (R / 5)*sin(prevAngle);
		//threeAngle = 6.28 / (100.0f / par.value * 1.5) + prevAngle;
		int RTX = (R * 0.9)*cos(threeAngle), RTY = (R * 0.9)*sin(threeAngle);
		/*POINT p1, p2, p3, p4; p1.x = LTX; p1.y = LTY;
		p2.x = RBX; p2.y = RBY;
		p3.x = LBX; p3.y = LBY;
		p4.x = RTX; p4.y = RTY;*/

		/*Ellipse(hdc, middleX - 5, middleY + 5, middleX + 5, middleY - 5);
		Ellipse(hdc, LTX - 5, LTY + 5, LTX + 5, LTY - 5);
		Ellipse(hdc, RBX - 5, RBY + 5, RBX + 5, RBY - 5);
		Ellipse(hdc, LBX - 2, LBY + 2, LBX + 2, LBY - 2);
		Ellipse(hdc, RTX - 5, RTY + 5, RTX + 5, RTY - 5);*/
		/*POINT ar[] = { p1,p4,p2,p3 };
		Polygon(hdc,ar , 4);*/
		int x0, y0; x0 = (R / 4)*cos(threeAngle); y0 = (R/4)*sin(threeAngle);
		LOGFONT lf; 
		lf.lfEscapement = -(double)threeAngle / 3.14 * 1800;
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfPitchAndFamily = DEFAULT_PITCH;
		strcpy(lf.lfFaceName, "Arial");
		lf.lfHeight = 30;
		lf.lfWidth = lf.lfHeight *0.4;
		lf.lfItalic = FALSE;
		lf.lfUnderline = FALSE;
		lf.lfStrikeOut = FALSE;
		SelectObject(hdc, CreateFontIndirect(&lf));
		

		RECT r;
		SetRect(&r, LTX, LTY, RBX, RBY);
		//DrawText(hdc, par.surname.data(), par.surname.size(), &r, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		TextOut(hdc, x0, y0, par.surname.data(), par.surname.size());

		lastX = newX; lastY = newY;
		prevAngle = angle;

		SelectObject(hdc, oldBrush);
		DeleteObject(brush);
	});
}