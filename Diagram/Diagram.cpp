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
void DrawDiagram1(HDC& hdc, vector<Participant>& p,int x0, int x, int y);
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
	wndclass.lpszClassName = "Diagram";
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
		DrawDiagram1(hdc, people,0.05*x, x, y);
		DrawScale(hdc, x*0.05, y);
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
		"Diagram",
		"Diagram",
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
void DrawDiagram1(HDC& hdc, vector<Participant>& p, int x0, int x, int y)
{
	int columnWidth = (x - x0) / p.size();
	vector<int> height;

	vector<Participant>::iterator max = max_element(p.begin(), p.end(), [](Participant p1, Participant p2) {return p1.value < p2.value; });

	for_each(p.begin(), p.end(), [&p, y, &height, &max](Participant& p)
	{
		//height.push_back((double) p.value/ (*max).value * y);
		height.push_back((double)p.value / 100 * y);
	});


	for (int i = 0; i < p.size(); i++)
	{
		RECT column, text;

		column.left = x0 + i*columnWidth; column.top = y - height[i];
		column.right = x0 + (i + 1)*columnWidth; column.bottom = y;

		Participant pt = p[i];
		HBRUSH brush, old; brush = CreateSolidBrush(RGB(rand() % 255, rand() % 255, rand() % 255));
		old = (HBRUSH)SelectObject(hdc, brush);

		Rectangle(hdc, column.left, column.top, column.right, column.bottom);
		MoveToEx(hdc, column.left, column.top, NULL);
		LineTo(hdc, 0.05*x, column.top);
		SelectObject(hdc, brush);
		DeleteObject(brush);

		/*text.left = x0 + i*columnWidth; text.top = 0;
		text.right = x0 + (i + 1)*columnWidth; text.bottom = y - height[i];*/
		SetRect(&text, x0 + i*columnWidth, y, x0 + (i + 1)*columnWidth, 0);
		string out = pt.surname + " " + to_string(pt.value) + "%";

		LOGFONT turnedTextLF;
		int txtH = abs(column.top - column.bottom) / out.size();

		turnedTextLF.lfCharSet = DEFAULT_CHARSET;
		turnedTextLF.lfPitchAndFamily = DEFAULT_PITCH;
		strcpy(turnedTextLF.lfFaceName, "Arial");
		turnedTextLF.lfHeight = (txtH < columnWidth) ?txtH :columnWidth;
		turnedTextLF.lfWidth = turnedTextLF.lfHeight /2;
		turnedTextLF.lfEscapement = 900;
		turnedTextLF.lfItalic = FALSE;
		turnedTextLF.lfUnderline = FALSE;
		turnedTextLF.lfStrikeOut = FALSE;
		SetBkMode(hdc, TRANSPARENT);

		HFONT hTurnedText = CreateFontIndirect(&turnedTextLF);
		HFONT oldFont = (HFONT)SelectObject(hdc, hTurnedText);
		DrawText(hdc, out.data(), out.size(), &text, DT_SINGLELINE | DT_VCENTER  | DT_BOTTOM); // FIX 100% TEXT DIPLAYING!
		SelectObject(hdc, oldFont);
		DeleteObject(hTurnedText);
	}
}

void DrawScale(HDC& hdc, int x, int y)
{
	HPEN pen, old; pen = CreatePen(PS_DOT, 3, BLACK_PEN);
	old = (HPEN)SelectObject(hdc, pen);

	MoveToEx(hdc, x, 0, NULL);
	LineTo(hdc, x, y);

	double yScale = (double)y / 10;
	for (double i = y, j = 1; i >= 0; i -= yScale, j++)
	{
		RECT r;
		r.left = 0; r.top = i - yScale;
		r.right = x; r.bottom = y;
		string text = to_string((int)j * 10);

		DrawText(hdc, text.data(), text.size(), &r, NULL);
		MoveToEx(hdc, x, i, NULL);
		LineTo(hdc, x - x*0.1, i);
	}

	SelectObject(hdc, old);
	DeleteObject(pen);
}