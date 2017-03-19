#undef UNICODE
#define IDR_MENU1 101
#define ID_FILE_ADD 40001
#define ID_FILE_CLEAR 40002
#define ID_FILE_NEW 40004
#define ID_FILE_SAVE 40003
#define ID_FILE_EXIT 40006
#define ID_HELP 40007
#define ID_SETTINGS_DRAWTEXT 40008
#define ID_SETTINGS_AUTOSAVE 40009
#define IDD_DIALOG1 103
#define IDD_ADDDIALOG 105
#define SAVETIMER 0
#define IDAPPLY 1004
#define ENLARGECOLLECTION (WM_USER + 1)

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
ostream& operator << (ostream& o, Participant& p);

BOOL InitApplication(HINSTANCE hinstance);
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
void ReadFromFile(vector<Participant>& p);
void DrawDiagram1(HDC& hdc, vector<Participant>& p, int x0, int x, int y, bool showText);
void ClearFile();
void SaveToFile(vector<Participant>& p);
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

BOOL CALLBACK ExitHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		break;

	case WM_COMMAND:
	{
		switch (LOWORD(wparam))
		{
		case IDOK:
			SendMessage(hwnd = GetParent(hwnd), WM_CLOSE, NULL, NULL); // Telling main window to shut down
			return TRUE;
		case IDCANCEL:
			EndDialog(hwnd, wparam);
			return TRUE;
		}
		break;
	}
	}
	return FALSE;
}

BOOL CALLBACK AddHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static int cellHeight, cellNameWidth, cellValueWidth, count;
	static int x, y;
	static RECT area;
	static vector<HWND> names, values;

	switch (msg)
	{
	case WM_INITDIALOG:
		count = 4; // Number of 
		
		HDC hdc; hdc = NULL;

		// Get text metrics to set Edit Box height 
		TEXTMETRIC metric;
		hdc = GetDC(GetParent(hwnd));
		GetTextMetrics(hdc, &metric);
		ReleaseDC(GetParent(hwnd), hdc);

		cellHeight = metric.tmHeight * 2;
		cellNameWidth = 2 * x / 3;
		cellValueWidth = metric.tmMaxCharWidth * 2;

		for (int i = 0; i < count; i++)
		{
			// Creating edit field for name and value
			names.push_back(CreateWindowEx(WS_EX_CLIENTEDGE, "edit", "", WS_CHILD | WS_VISIBLE | ES_LEFT, 20, 20 + i * 40,
				cellNameWidth, cellHeight, hwnd, NULL, GetModuleHandle(NULL), NULL));
			values.push_back(CreateWindowEx(WS_EX_CLIENTEDGE, "edit", "", WS_CHILD | WS_VISIBLE | ES_RIGHT, x - cellValueWidth - 20, 20 + i * 40,
				cellValueWidth, cellHeight, hwnd, NULL, GetModuleHandle(NULL), NULL));
			SendMessage(names[i], EM_SETPASSWORDCHAR, 0, NULL); // Telling to disable Password symbols
		}
		break;
	case WM_COMMAND:
		 vector<Participant> toShare;

		switch (LOWORD(wparam))
		{
		case IDAPPLY:
			// Here: to Get text from text boxes
			for (int i = 0; i < names.size(); i++)
			{
				char name[40], value[4];
				GetWindowText(names[i], name, 40); // GEtting text from "name" edit box
				GetWindowText(values[i], value, 4); // From value edit box
				int a = atoi(value);

				/*
				DO SOME CHECKS HERE
				*/

				if (a)
				{ 
					// Pushing new element to vector
					Participant p;
					p.surname = string(name);
					p.value = a;
					toShare.push_back(p);
				}
			}
			EndDialog(hwnd, 0); // Closing dialog
			toShare.push_back(Participant()); //
			SendMessage(GetParent(hwnd), ENLARGECOLLECTION, NULL, reinterpret_cast<LPARAM>(toShare.data())); // Sending message in order to
			// refresh data vector in WndProc

			return TRUE;
		case IDCANCEL:
			EndDialog(hwnd, wparam);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	static int x, y;
	static HDC hdc;
	static HMENU menu;
	static vector<Participant> people;
	PAINTSTRUCT ps;
	static bool drawText;
	vector<Participant> *vect;

	switch (message)
	{
	case WM_CREATE:
		ReadFromFile(people);
		menu = GetMenu(hwnd); // Loading menu, assigned in res 
		DWORD word; word = GetMenuState(menu, ID_SETTINGS_DRAWTEXT, MF_BYCOMMAND);
		drawText = (word & MF_CHECKED) ? true : false;

		break;
	case WM_COMMAND:
		/*Here is a section for 
		handling menu actions 
		*/

		switch (LOWORD(wparam))
		{
		case ID_FILE_ADD:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ADDDIALOG), hwnd, AddHandler);
			break;
		case ID_FILE_CLEAR:
			people.clear();
			InvalidateRect(hwnd, NULL, true);
			break;
		case ID_FILE_NEW:
			people.clear();
			ClearFile();
			InvalidateRect(hwnd, NULL, true);
			MessageBox(NULL, "INFO", "NEW", MB_OK);
			break;
		case ID_FILE_SAVE:
			SaveToFile(people);
			break;
		case ID_FILE_EXIT:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hwnd, ExitHandler);
			break;

		case ID_SETTINGS_DRAWTEXT:
		{
			DWORD state = GetMenuState(menu, ID_SETTINGS_DRAWTEXT, MF_BYCOMMAND);
			if (!(state & MF_CHECKED))
				CheckMenuItem(menu, ID_SETTINGS_DRAWTEXT, MF_BYCOMMAND | MF_CHECKED);
			else
				CheckMenuItem(menu, ID_SETTINGS_DRAWTEXT, MF_BYCOMMAND | MF_UNCHECKED);
			drawText = !drawText;
			InvalidateRect(hwnd, NULL, true);
			break;
		}
		case ID_SETTINGS_AUTOSAVE:
		{
			DWORD state = GetMenuState(menu, ID_SETTINGS_AUTOSAVE, MF_BYCOMMAND);
			if (!(state & MF_CHECKED))
			{
				SetTimer(hwnd, SAVETIMER, 5000, NULL);
				CheckMenuItem(menu, ID_SETTINGS_AUTOSAVE, MF_BYCOMMAND | MF_CHECKED);
			}
			else
			{
				KillTimer(hwnd, SAVETIMER);
				CheckMenuItem(menu, ID_SETTINGS_AUTOSAVE, MF_BYCOMMAND | MF_UNCHECKED);
			}

			break;
		}
		}
		break;
	case WM_SIZE:
		x = LOWORD(lparam);
		y = HIWORD(lparam);
		break;
	case ENLARGECOLLECTION:
		Participant *par;
		try
		{
			par = reinterpret_cast<Participant*>(lparam); // param param pam дмовм
			Participant* cpy = par; // Creating copy of a "head", for future checks

			int i = 0;
			while (par != nullptr && cpy->value != -1)
			{
				i++;
				cpy++;
			}
			if (i != 0)
			{
				people.insert(people.end(), par, par + i);
				InvalidateRect(hwnd, NULL, true);
				return FALSE;
			}
		}
		catch (...)
		{
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		DrawDiagram1(hdc, people, 0.05*x, x, y, drawText);
		DrawScale(hdc, x*0.05, y);

		EndPaint(hwnd, &ps);
		break;

	case WM_TIMER:
	{
		switch (wparam)
		{
		case SAVETIMER:
			SaveToFile(people);
			MessageBox(NULL, "", "", MB_OK);
			break;
		}
		break;
	}
	case WM_CLOSE:
		//ReleaseDC(hwnd, hdc);
		DestroyWindow(hwnd);
		KillTimer(hwnd, SAVETIMER);
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
		LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1)),
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

ostream& operator << (ostream& o, const Participant& p)
{
	o << p.surname << " " << p.value;
	return o;
}
void ReadFromFile(vector<Participant>& p)
{
	ifstream in("in.txt");
	copy(istream_iterator<Participant>(in), istream_iterator<Participant>(), back_inserter(p));
	in.close();
}
void SaveToFile(vector<Participant>& p)
{
	ofstream out("in.txt");
	copy(p.begin(), p.end(), ostream_iterator<Participant>(out, "\n"));
	out.close();
}
void ClearFile()
{
	ifstream in("in.txt");
	in.clear();
	in.close();
}
void DrawDiagram1(HDC& hdc, vector<Participant>& p, int x0, int x, int y, bool drawText)
{
	if (p.size() == 0) return;
	int columnWidth = (x - x0) / p.size();
	vector<int> height;

	vector<Participant>::iterator max = max_element(p.begin(), p.end(), [](Participant p1, Participant p2) {return p1.value < p2.value; });

	for_each(p.begin(), p.end(), [&p, y, &height, &max](Participant& par)
	{
		//height.push_back((double) p.value/ (*max).value * y);
		height.push_back((double)par.value / 100 * y);
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
		string out;
		out = (drawText) ? pt.surname + " " + to_string(pt.value) + "%" : " ";

		LOGFONT turnedTextLF;
		int txtH = abs(column.top - column.bottom) / out.size();

		turnedTextLF.lfCharSet = DEFAULT_CHARSET;
		turnedTextLF.lfPitchAndFamily = DEFAULT_PITCH;
		strcpy(turnedTextLF.lfFaceName, "Arial");
		turnedTextLF.lfHeight = (txtH < columnWidth) ? txtH : columnWidth;
		turnedTextLF.lfWidth = turnedTextLF.lfHeight / 2;
		turnedTextLF.lfEscapement = 900;
		turnedTextLF.lfItalic = FALSE;
		turnedTextLF.lfUnderline = FALSE;
		turnedTextLF.lfStrikeOut = FALSE;
		SetBkMode(hdc, TRANSPARENT);

		HFONT hTurnedText = CreateFontIndirect(&turnedTextLF);
		HFONT oldFont = (HFONT)SelectObject(hdc, hTurnedText);
		DrawText(hdc, out.data(), out.size(), &text, DT_SINGLELINE | DT_VCENTER | DT_BOTTOM); // FIX 100% TEXT DIPLAYING!
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