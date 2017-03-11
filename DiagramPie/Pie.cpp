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
void DrawDiagram1(HDC& hdc, vector<Participant>& p, int x0, int x, int y);
void DrawScale(HDC& hdc, int x, int y);
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
		//hdc = GetDC(hwnd);
		ReadFromFile(people);
		break;
	case WM_SIZE:
		x = LOWORD(lparam);
		y = HIWORD(lparam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		
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


