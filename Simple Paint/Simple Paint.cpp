// Simple Paint.cpp : Defines the entry CPoint for the application.
//
#include "stdafx.h"
#include <windowsx.h>
#include <commdlg.h>
#include "Simple Paint.h"
#include "Shape.h"
#include "Line.h"
#include "Rectangle.h"
#include "Ellipse.h"
#include <vector>
#include <fstream>
using namespace std;

#define MAX_LOADSTRING 256


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szBuffer[MAX_LOADSTRING + 20];
WCHAR szDefaultFileNameBuffer[MAX_LOADSTRING + 20];
WCHAR szFileNameBuffer[MAX_LOADSTRING + 1];
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
bool g_isDrawing = false;
bool g_isMoving = false;
HWND g_hWnd;
vector<CShape*> g_shapes;
CPoint g_leftTop, g_rightBottom/*, g_currentMouse*/;
CShape* g_currentShape;
OPENFILENAMEW g_ofn;
BYTE g_DrawCheckingBackup;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
int ExportData(LPWCH fileName);
int ImportData(LPWCH fileName, bool removeOldShapes = false);
void ClearShape();
void MoveShapes(int distanceX, int distanceY);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_SIMPLEPAINT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SIMPLEPAINT));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SIMPLEPAINT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SIMPLEPAINT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	g_hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!g_hWnd)
	{
		return FALSE;
	}

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		ZeroMemory(&g_ofn, sizeof(g_ofn));
		g_ofn.lStructSize = sizeof(g_ofn);
		g_ofn.hwndOwner = hWnd;
		g_ofn.lpstrFile = szFileNameBuffer;
		g_ofn.nMaxFile = MAX_LOADSTRING;
		g_ofn.lpstrFilter = L"Simple Paint Data (*.spd)\0*.spd\0";
		g_ofn.nFilterIndex = 1;
		GetCurrentDirectory(MAX_LOADSTRING, szFileNameBuffer);
		wsprintf(szDefaultFileNameBuffer, L"%ls\\data.spd", szFileNameBuffer);
		g_ofn.lpstrInitialDir = szFileNameBuffer;
		g_ofn.lpstrInitialDir = NULL;

		g_ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		ImportData(szDefaultFileNameBuffer);

		break;

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_TOOLS_BUFFERING:
		{
			if (GetMenuState(GetMenu(g_hWnd), ID_TOOLS_BUFFERING, 0) == MFS_UNCHECKED)
				CheckMenuItem(GetMenu(g_hWnd), ID_TOOLS_BUFFERING, MFS_CHECKED);
			else
				CheckMenuItem(GetMenu(g_hWnd), ID_TOOLS_BUFFERING, MFS_UNCHECKED);
			//InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
		case ID_DRAW_LINE:
		{
			if (GetMenuState(GetMenu(g_hWnd), ID_TOOLS_MOVE, 0) == MFS_CHECKED)
			{
				g_DrawCheckingBackup = 0;
				break;
			}
			if (GetMenuState(GetMenu(g_hWnd), ID_DRAW_LINE, 0) == MFS_UNCHECKED)
			{
				CheckMenuItem(GetMenu(g_hWnd), ID_DRAW_LINE, MFS_CHECKED);
				CheckMenuItem(GetMenu(g_hWnd), ID_DRAW_ELLIPSE, MFS_UNCHECKED);
				CheckMenuItem(GetMenu(g_hWnd), ID_DRAW_RECTANGLE, MFS_UNCHECKED);
			}
			break;
		}
		case ID_DRAW_RECTANGLE:
		{
			if (GetMenuState(GetMenu(g_hWnd), ID_TOOLS_MOVE, 0) == MFS_CHECKED)
			{
				g_DrawCheckingBackup = 1;
				break;
			}
			if (GetMenuState(GetMenu(g_hWnd), ID_DRAW_RECTANGLE, 0) == MFS_UNCHECKED)
			{
				CheckMenuItem(GetMenu(g_hWnd), ID_DRAW_RECTANGLE, MFS_CHECKED);
				CheckMenuItem(GetMenu(g_hWnd), ID_DRAW_LINE, MFS_UNCHECKED);
				CheckMenuItem(GetMenu(g_hWnd), ID_DRAW_ELLIPSE, MFS_UNCHECKED);
			}
			break;
		}
		case ID_DRAW_ELLIPSE:
		{
			if (GetMenuState(GetMenu(g_hWnd), ID_TOOLS_MOVE, 0) == MFS_CHECKED)
			{
				g_DrawCheckingBackup = 2;
				break;
			}
			if (GetMenuState(GetMenu(g_hWnd), ID_DRAW_ELLIPSE, 0) == MFS_UNCHECKED)
			{
				CheckMenuItem(GetMenu(g_hWnd), ID_DRAW_ELLIPSE, MFS_CHECKED);
				CheckMenuItem(GetMenu(g_hWnd), ID_DRAW_LINE, MFS_UNCHECKED);
				CheckMenuItem(GetMenu(g_hWnd), ID_DRAW_RECTANGLE, MFS_UNCHECKED);
			}
			break;
		}
		case ID_FILE_OPEN:
		{
			if (GetOpenFileName(&g_ofn) == false)
			{
				//MessageBox(hWnd, L"You haven't choosen any file!", L"Cannot open", MB_OK);
				break;
			}
			else
			{
				//MessageBox(hWnd, g_ofn.lpstrFile, L"Cannot open", MB_OK);
				ImportData(g_ofn.lpstrFile, true);
				InvalidateRect(hWnd, NULL, TRUE);
			}
			break;
		}
		case ID_FILE_SAVE:
		{
			if (GetSaveFileName(&g_ofn) == false)
			{
				//MessageBox(hWnd, L"You haven't choosen any file!", L"Cannot open", MB_OK);
				break;
			}
			else
			{
				//MessageBox(hWnd, g_ofn.lpstrFile, L"Cannot open", MB_OK);
				ExportData(g_ofn.lpstrFile);
			}
			break;
		}
		case ID_FILE_IMPORT:
		{
			if (GetOpenFileName(&g_ofn) == false)
			{
				//MessageBox(hWnd, L"You haven't choosen any file!", L"Cannot open", MB_OK);
				break;
			}
			ImportData(g_ofn.lpstrFile, false);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
		case ID_TOOLS_CLEAR:
		{

			for (unsigned int i = 0; i < g_shapes.size(); ++i)
			{
				if (g_shapes[i])
					delete g_shapes[i];
			}
			g_shapes.clear();
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
		case ID_TOOLS_MOVE:
		{
			if (GetMenuState(GetMenu(g_hWnd), ID_TOOLS_MOVE, 0) == MFS_UNCHECKED)
			{
				CheckMenuItem(GetMenu(g_hWnd), ID_TOOLS_MOVE, MFS_CHECKED);
				if (GetMenuState(GetMenu(g_hWnd), ID_DRAW_LINE, 0) == MFS_CHECKED)
				{
					CheckMenuItem(GetMenu(g_hWnd), ID_DRAW_LINE, MFS_UNCHECKED);
					g_DrawCheckingBackup = 0;
				}
				else if (GetMenuState(GetMenu(g_hWnd), ID_DRAW_RECTANGLE, 0) == MFS_CHECKED)
				{
					CheckMenuItem(GetMenu(g_hWnd), ID_DRAW_RECTANGLE, MFS_UNCHECKED);
					g_DrawCheckingBackup = 1;
				}
				else if (GetMenuState(GetMenu(g_hWnd), ID_DRAW_ELLIPSE, 0) == MFS_CHECKED)
				{
					CheckMenuItem(GetMenu(g_hWnd), ID_DRAW_ELLIPSE, MFS_UNCHECKED);
					g_DrawCheckingBackup = 2;
				}
			}
			else
			{
				CheckMenuItem(GetMenu(g_hWnd), ID_TOOLS_MOVE, MFS_UNCHECKED);
				switch (g_DrawCheckingBackup)
				{
				case 0:
					CheckMenuItem(GetMenu(g_hWnd), ID_DRAW_LINE, MFS_CHECKED);
					break;
				case 1:
					CheckMenuItem(GetMenu(g_hWnd), ID_DRAW_RECTANGLE, MFS_CHECKED);
					break;
				case 2:
					CheckMenuItem(GetMenu(g_hWnd), ID_DRAW_ELLIPSE, MFS_CHECKED);
					break;
				}

			}
			break;
		}
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{

		if (!g_isDrawing && GetMenuState(GetMenu(g_hWnd), ID_TOOLS_MOVE, 0) == MFS_UNCHECKED)
		{
			g_isDrawing = true;
			//g_rightBottom.m_X = g_leftTop.m_X = GET_X_LPARAM(lParam);
			//g_rightBottom.m_Y = g_leftTop.m_Y = GET_Y_LPARAM(lParam);

			if (GetMenuState(GetMenu(g_hWnd), ID_DRAW_LINE, 0) == MFS_CHECKED)
			{
				g_currentShape = new CLine(g_leftTop, g_rightBottom);
			}
			else if (GetMenuState(GetMenu(g_hWnd), ID_DRAW_RECTANGLE, 0) == MFS_CHECKED)
			{
				g_currentShape = new CRectangle(g_leftTop, g_rightBottom);
			}
			else if (GetMenuState(GetMenu(g_hWnd), ID_DRAW_ELLIPSE, 0) == MFS_CHECKED)
			{
				g_currentShape = new CEllipse(g_leftTop, g_rightBottom);
			}
			else
				break;
			//InvalidateRect(hWnd, NULL, TRUE);
		}
		else if (GetMenuState(GetMenu(g_hWnd), ID_TOOLS_MOVE, 0) == MFS_CHECKED)
		{
			g_isMoving = true;
			//g_rightBottom.m_X = g_leftTop.m_X = GET_X_LPARAM(lParam);
			//g_rightBottom.m_Y = g_leftTop.m_Y = GET_Y_LPARAM(lParam);
		}
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (g_isDrawing)
		{
			g_rightBottom.m_X = GET_X_LPARAM(lParam);
			g_rightBottom.m_Y = GET_Y_LPARAM(lParam);
			g_currentShape->SetData(g_leftTop, g_rightBottom);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		else if (g_isMoving)
		{
			g_rightBottom.m_X = GET_X_LPARAM(lParam);
			g_rightBottom.m_Y = GET_Y_LPARAM(lParam);
			int distanceX = g_rightBottom.m_X - g_leftTop.m_X;
			int distanceY = g_rightBottom.m_Y - g_leftTop.m_Y;

			g_leftTop = g_rightBottom;
			MoveShapes(distanceX, distanceY);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		else
		{
			g_leftTop.m_X = GET_X_LPARAM(lParam);
			g_leftTop.m_Y = GET_Y_LPARAM(lParam);
		}
		wsprintf(szBuffer, L"%ls (%d, %d)", szTitle, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		SetWindowText(hWnd, szBuffer);

		break;
	}
	case WM_LBUTTONUP:
	{
		if (g_isDrawing)
		{
			g_rightBottom.m_X = GET_X_LPARAM(lParam);
			g_rightBottom.m_Y = GET_Y_LPARAM(lParam);
			g_currentShape->SetData(g_leftTop, g_rightBottom);
			if ((g_leftTop.m_X != g_rightBottom.m_X || g_leftTop.m_Y != g_rightBottom.m_Y))
				g_shapes.push_back(g_currentShape);

			g_isDrawing = FALSE;
		}
		else if (g_isMoving)
		{
			g_isMoving = false;
		}


		//InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_SIZE:
	{

		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...

		CPaint::SetHDC(&hdc);
		for (unsigned int i = 0; i < g_shapes.size(); ++i)
		{
			g_shapes[i]->Paint();
		}
		if (g_isDrawing)
		{
			g_currentShape->Paint();
		}

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		ExportData(szDefaultFileNameBuffer);
		for (unsigned int i = 0; i < g_shapes.size(); ++i)
		{
			if (g_shapes[i])
				delete g_shapes[i];
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

int ImportData(LPWCH fileName, bool removeOldShapes)
{
	ifstream dataFile(fileName, ios::in | ios::binary);

	if (!dataFile)
		return -1;
	int shapeNum;
	dataFile.read((char*)(&shapeNum), sizeof(shapeNum));
	if (shapeNum < 0)
		return -2;

	BYTE shapeType;
	CPoint leftTop;
	CPoint rightBottom;
	CShape * pShape;
	if (removeOldShapes)
	{
		ClearShape();
	}
	for (int i = 0; i < shapeNum; ++i)
	{
		dataFile.read((char*)(&shapeType), sizeof(shapeType));
		if (dataFile.good() == false)
		{
			ClearShape();
			return -3;
		}
		dataFile.read((char*)(&(leftTop.m_X)), sizeof(leftTop.m_X));
		if (dataFile.good() == false)
		{
			ClearShape();
			return -3;
		}
		dataFile.read((char*)(&(leftTop.m_Y)), sizeof(leftTop.m_Y));
		if (dataFile.good() == false)
		{
			ClearShape();
			return -3;
		}
		dataFile.read((char*)(&(rightBottom.m_X)), sizeof(rightBottom.m_X));
		if (dataFile.good() == false)
		{
			ClearShape();
			return -3;
		}
		dataFile.read((char*)(&(rightBottom.m_Y)), sizeof(rightBottom.m_Y));
		if (dataFile.good() == false)
		{
			ClearShape();
			return -3;
		}
		switch (shapeType)
		{
		case LINESHAPECODE:
			pShape = new CLine(leftTop, rightBottom);
			break;
		case ELLIPSESHAPECODE:
			pShape = new CEllipse(leftTop, rightBottom);
			break;
		case RECTANGLESHAPECODE:
			pShape = new CRectangle(leftTop, rightBottom);
			break;
		}
		g_shapes.push_back(pShape);
	}
	return 0;
}
void ClearShape()
{
	for (size_t i = 0; i < g_shapes.size(); ++i)
	{
		if (g_shapes[i] != nullptr)
		{
			delete g_shapes[i];
			g_shapes[i] = nullptr;
		}
	}
	g_shapes.clear();
}
int ExportData(LPWCH fileName)
{
	ofstream dataFile(fileName, ios::out | ios::binary);

	if (!dataFile)
		return -1;
	size_t shapeNum = g_shapes.size();
	dataFile.write((char*)(&shapeNum), sizeof(shapeNum));

	BYTE shapeType;
	CPoint leftTop;
	CPoint rightBottom;
	for (unsigned int i = 0; i < shapeNum; ++i)
	{
		shapeType = g_shapes[i]->GetType();
		dataFile.write((char*)(&shapeType), sizeof(shapeType));
		leftTop = g_shapes[i]->GetLeftTop();
		dataFile.write((char*)(&(leftTop.m_X)), sizeof(leftTop.m_X));
		dataFile.write((char*)(&(leftTop.m_Y)), sizeof(leftTop.m_Y));

		rightBottom = g_shapes[i]->GetRightBottom();
		dataFile.write((char*)(&(rightBottom.m_X)), sizeof(rightBottom.m_X));
		dataFile.write((char*)(&(rightBottom.m_Y)), sizeof(rightBottom.m_Y));
	}
	return 0;
}

void MoveShapes(int distanceX, int distanceY)
{
	for (unsigned int i = 0; i < g_shapes.size(); ++i)
	{
		g_shapes[i]->Move(distanceX, distanceY);
	}
}