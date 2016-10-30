//---------------------------------------
//| Student ID : 1412592					|
//| Full Name : Võ Hiếu Trung				|
//| Email : vhieutrung@outlook.com		|
//-----------------------------------------
//Edit on
//
//Environment: 	Windows 10
//Tool : Visual Studio 2015
//*/
//


#include "stdafx.h"
#include "My Explorer.h"
#include <commctrl.h>
#include <windowsX.h>
#include <windows.h>
#include <shellapi.h>
#include <UIRibbon.h>

#include "RibbonFramework.h"
#include "RibbonIDs.h"
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "ComCtl32.lib")
//#include <Objbase.h>
//#pragma comment(lib, "Ole32.lib")

#define MAX_LOADSTRING 100
#define CX 24
#define CY 24
#define NUM_IMAGES 3

// Global Variables:
const int BUFFERSIZE = 256;
bool OPT_SHOWHIDDEN = FALSE;
bool OPT_SHOWSYSTEM = FALSE;
int g_RibbonHeigh = 147;
int g_nDiskList;
int g_nFolder;
int g_nOpticalDisc;
int g_nUSBDisc;
int g_nComputer;
int g_nFile;
const int pathBufferLength = 2048;
HINSTANCE g_hInst;                                // current instance
WCHAR g_szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR g_szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND g_hwndMain;
HWND g_hwndTV;
HWND g_hwndLV;
HTREEITEM g_thisPC;
HIMAGELIST  g_himl;

//Forward declarations of functions included in this code module :
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify);
void OnSize(HWND hwnd, UINT state, int cx, int cy);
void OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo);
LRESULT OnNotify(HWND hwnd, int idFrom, NMHDR *pnm);

BOOL ImageLists_Initialize(HIMAGELIST & himl);

//
HWND TreeView_Create(HWND hwndParent);
int TreeView_Initialize(HWND hWnd);
void TreeView_GetPath(HTREEITEM hTreeItem, LPWSTR path, int bufferSize);
int TreeView_LoadingChild(HTREEITEM &selectedItem, int indent = 0, BOOL showHidden = OPT_SHOWHIDDEN, BOOL showSystem = OPT_SHOWSYSTEM);


//
HWND ListView_Create(HWND hwndParent);
void ListView_Initialize(HWND hWnd);
int ListView_LoadingChild(HWND hWnd, LPWSTR path, BOOL showHidden, BOOL showSystem);

void Convert(__int64 nSize, TCHAR *buffer);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		return FALSE;
	}

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, g_szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MYEXPLORER, g_szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}


	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYEXPLORER));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	CoUninitialize();
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

	wcex.style = 0; // Don't use CS_HREDRAW or CS_VREDRAW with a Ribbon
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_MYEXPLORER));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0/*MAKEINTRESOURCEW(IDC_MYEXPLORER)*/;
	wcex.lpszClassName = g_szWindowClass;
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
	g_hInst = hInstance; // Store instance handle in our global variable

	g_hwndMain = CreateWindowW(g_szWindowClass, g_szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	//load window rect
	RECT rect;
	WCHAR curPath[BUFFERSIZE + 1],
		buffer[BUFFERSIZE + 1],
		default[BUFFERSIZE + 1];
	WCHAR * tmp;
	RECT actualDesktop;
	GetWindowRect(GetDesktopWindow(), &actualDesktop);

	GetCurrentDirectory(BUFFERSIZE, curPath);
	wcscat_s(curPath, BUFFERSIZE, L"\\conf.ini");

	swprintf_s(default, L"%d", 0);
	GetPrivateProfileString(L"winRect", L"top", default, buffer, BUFFERSIZE, curPath);
	tmp = buffer + wcslen(buffer);
	rect.top = wcstol(buffer, &tmp, 10);

	swprintf_s(default, L"%d", 600);
	GetPrivateProfileString(L"winRect", L"bottom", default, buffer, BUFFERSIZE, curPath);
	tmp = buffer + wcslen(buffer);
	rect.bottom = wcstol(buffer, &tmp, 10);

	swprintf_s(default, L"%d", 0);
	GetPrivateProfileString(L"winRect", L"left", default, buffer, BUFFERSIZE, curPath);
	tmp = buffer + wcslen(buffer);
	rect.left = wcstol(buffer, &tmp, 10);
	swprintf_s(default, L"%d", 800);
	GetPrivateProfileString(L"winRect", L"right", default, buffer, BUFFERSIZE, curPath);
	tmp = buffer + wcslen(buffer);
	rect.right = wcstol(buffer, &tmp, 10);

	int heigh, width;
	heigh = rect.bottom - rect.top;
	if (heigh < 210)
		heigh = 600;
	width = rect.right - rect.left;
	if (width < 300)
		width = 800;
	if (rect.top > actualDesktop.bottom || rect.top < 0)
		rect.top = 0;
	if (rect.left > actualDesktop.right || rect.left < 0)
		rect.left = 0;

	SetWindowPos(g_hwndMain, 0, rect.left, rect.top, width, heigh, 0);

	if (!g_hwndMain)
	{
		return FALSE;
	}

	ShowWindow(g_hwndMain, nCmdShow);
	UpdateWindow(g_hwndMain);

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
	switch (message)//WM_GETMINMAXINFO
	{
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_SIZE, OnSize);
		HANDLE_MSG(hWnd, WM_GETMINMAXINFO, OnGetMinMaxInfo);
		HANDLE_MSG(hWnd, WM_NOTIFY, OnNotify);

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
	{
		RECT rect;
		GetWindowRect(hWnd, &rect);
		WCHAR curPath[BUFFERSIZE + 1],
			buffer[BUFFERSIZE + 1];

		GetCurrentDirectory(BUFFERSIZE, curPath);
		wcscat_s(curPath, BUFFERSIZE, L"\\conf.ini");
		swprintf_s(buffer, L"%d", rect.top);
		WritePrivateProfileString(L"winRect", L"top", buffer, curPath);
		swprintf_s(buffer, L"%d", rect.bottom);
		WritePrivateProfileString(L"winRect", L"bottom", buffer, curPath);
		swprintf_s(buffer, L"%d", rect.left);
		WritePrivateProfileString(L"winRect", L"left", buffer, curPath);
		swprintf_s(buffer, L"%d", rect.right);
		WritePrivateProfileString(L"winRect", L"right", buffer, curPath);


		DestroyFramework();
		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{

	//// Get system font
	//LOGFONT lf;
	//GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	//HFONT hFont = CreateFont(lf.lfHeight, lf.lfWidth,
	//	lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
	//	lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
	//	lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
	//	lf.lfPitchAndFamily, lf.lfFaceName);


	bool initSuccess = InitializeFramework(hWnd);
	if (!initSuccess)
	{
		return -1;
	}


	// Initialize the image list
	if (!ImageLists_Initialize(g_himl))
	{
		return FALSE;
	}

	// Create tree view
	g_hwndTV = TreeView_Create(hWnd);
	TreeView_SetImageList(g_hwndTV, g_himl, TVSIL_NORMAL);
	TreeView_Initialize(hWnd);


	// Create list view
	g_hwndLV = ListView_Create(hWnd);
	ListView_SetImageList(g_hwndLV, g_himl, TVSIL_NORMAL);
	ListView_SetImageList(g_hwndLV, g_himl, LVSIL_SMALL);
	ListView_Initialize(g_hwndLV);


	CheckMenuItem(GetMenu(hWnd), ID_VIEW_HIDDENITEMS, OPT_SHOWHIDDEN ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(GetMenu(hWnd), ID_VIEW_SYSTEMITEMS, OPT_SHOWSYSTEM ? MF_CHECKED : MF_UNCHECKED);
	return true;
}


void OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	int width = (cx > 300) ? (cx / 3) : 100;
	if (width > 300)
		width = 300;
	int heigh = cy > 150 ? cy : 150;
	SetWindowPos(g_hwndTV, 0, 0, 0, width, heigh - g_RibbonHeigh, SWP_NOMOVE);

	SetWindowPos(g_hwndLV, 0, width - 1, g_RibbonHeigh, cx - width + 1, heigh - g_RibbonHeigh, 0);
}


void OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo)
{
	lpMinMaxInfo->ptMinTrackSize.x = 300;
	lpMinMaxInfo->ptMinTrackSize.y = 210;

}


LRESULT OnNotify(HWND hwnd, int idFrom, NMHDR *pnm)
{
	LPNMTREEVIEW lpnmTree = (LPNMTREEVIEW)pnm;
	switch (pnm->code)
	{
	case TVN_ITEMEXPANDING:
	{

		if (lpnmTree->action == TVE_EXPAND)//only load child on expanding
			TreeView_LoadingChild(lpnmTree->itemNew.hItem, 0);
		break;
	}
	case TVN_SELCHANGED:
	{
		WCHAR path[pathBufferLength];
		TreeView_GetPath(lpnmTree->itemNew.hItem, path, pathBufferLength);
		ListView_DeleteAllItems(g_hwndLV);

		ListView_LoadingChild(g_hwndLV, path, OPT_SHOWHIDDEN, OPT_SHOWSYSTEM);

		break;
	}

	case NM_DBLCLK:
	{
		if (pnm->hwndFrom == g_hwndLV)
		{
			//g_ListView->LoadCurSel();
			//ListView_Get
			LRESULT index = SendMessage(g_hwndLV, LVM_GETNEXTITEM, (WPARAM)-1, (LPARAM)LVNI_SELECTED);
			HTREEITEM selectedItem = TreeView_GetSelection(g_hwndTV);
			HTREEITEM hItemChild = TreeView_GetChild(g_hwndTV, selectedItem); 
			for (int i = 0; i < index; ++i)
			{
				hItemChild = TreeView_GetNextSibling(g_hwndTV, hItemChild);
			}

			if (!hItemChild)// double click on file not folder
				return 0;

			TreeView_Select(g_hwndTV, hItemChild, TVGN_CARET);
			TreeView_Expand(g_hwndTV, hItemChild, TVGN_CARET);
		}
		break;
	}
	}
	return 0;
}


BOOL ImageLists_Initialize(HIMAGELIST & himl)
{
	HICON hicon;     // handle to icon 

					 // Create the image list. 
	if ((himl = ImageList_Create(CX,
		CY,
		ILC_COLOR32 | ILC_HIGHQUALITYSCALE,
		NUM_IMAGES, 0)) == NULL)
		return FALSE;

	// Add the folder icon. 
	hicon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_FOLDER));
	g_nFolder = ImageList_AddIcon(himl, hicon);
	DeleteObject(hicon);

	// Add the optical disc icon. 
	hicon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_OPTICALDISC));
	g_nOpticalDisc = ImageList_AddIcon(himl, hicon);
	DeleteObject(hicon);

	// Add the USB disc icon. 
	hicon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_USBDISK));
	g_nUSBDisc = ImageList_AddIcon(himl, hicon);
	DeleteObject(hicon);

	// Add the computer icon. 
	hicon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_COMPUTER));
	g_nComputer = ImageList_AddIcon(himl, hicon);
	DeleteObject(hicon);

	// Add the file icon. 
	hicon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_FILE));
	g_nFile = ImageList_AddIcon(himl, hicon);
	DeleteObject(hicon);

	// Add the drive icons. 
	hicon = LoadIcon(g_hInst, MAKEINTRESOURCE(140));
	g_nDiskList = ImageList_AddIcon(himl, hicon);
	DeleteObject(hicon);
	for (int i = 1; i < 27; ++i)
	{
		hicon = LoadIcon(g_hInst, MAKEINTRESOURCE(140 + i));
		ImageList_AddIcon(himl, hicon);
		DeleteObject(hicon);
	}

	// Fail if not all of the images were added. 
	if (ImageList_GetImageCount(himl) < 32)
		return FALSE;


	return TRUE;
}


//
//for tree view
//
HWND TreeView_Create(HWND hwndParent)
{
	HWND hwndTV;    // handle to tree-view control 

	// Get the dimensions of the parent window's client area, and create 
	// the tree-view control. 
	RECT rect;
	GetWindowRect(hwndParent, &rect);
	int parentWidth = rect.right - rect.left;
	int width = (parentWidth > 300) ? (parentWidth / 3) : 100;
	if (width > 300)
		width = 300;

	int parentHeigh = rect.bottom - rect.top;
	int heigh = parentHeigh > 150 ? parentHeigh : 150;

	hwndTV = CreateWindowEx(0,
		WC_TREEVIEW,
		TEXT("Tree View"),
		WS_VISIBLE | WS_CHILD | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | WS_BORDER | TVS_DISABLEDRAGDROP | TVS_SINGLEEXPAND | TVS_TRACKSELECT,
		0,
		g_RibbonHeigh,
		width,
		heigh - g_RibbonHeigh,
		hwndParent,
		(HMENU)IDC_TREEVIEW,
		g_hInst,
		NULL);

	return hwndTV;
}

int TreeView_Initialize(HWND hWnd)
{
	int i = 0;
	int index = 0;
	WCHAR logicalDriveStr[105];
	TVINSERTSTRUCT insrtItem;
	//get drive info
	const int bufferLength = 253;
	WCHAR bufferStr[bufferLength + 1];
	WCHAR driveName[bufferLength + 4];

	insrtItem.hParent = 0;
	insrtItem.hInsertAfter = TVI_SORT;
	insrtItem.item.pszText = _wcsdup(L"This PC");
	insrtItem.item.iImage = g_nComputer;
	insrtItem.item.iSelectedImage = g_nComputer;
	insrtItem.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	insrtItem.item.cChildren = I_CHILDRENCALLBACK;
	g_thisPC = TreeView_InsertItem(g_hwndTV, &insrtItem);


	GetLogicalDriveStrings(104, logicalDriveStr);

	while (logicalDriveStr[i] != '\0')
	{
		//get drive index from zero
		index = logicalDriveStr[i] - 'A';

		GetVolumeInformation(logicalDriveStr + i, bufferStr, bufferLength, 0, 0, 0, 0, 0);

		//get drive type
		swprintf_s(driveName, bufferLength + 3, L"(%ls ", logicalDriveStr + i);// affter this you have somthing like "(X:\ "
		driveName[3] = ')';// then you have "(X:) "

		// Get drive type
		switch (GetDriveType(logicalDriveStr + i))
		{
		case DRIVE_REMOVABLE:
		{
			if (bufferStr[0] == '\0')
				wcscat_s(driveName, bufferLength, L"Removable Disk");
			else
				wcscat_s(driveName, bufferLength, bufferStr);

			insrtItem.item.iImage = g_nUSBDisc;
			insrtItem.item.iSelectedImage = g_nUSBDisc;

			break;
		}
		case DRIVE_FIXED:
		{
			if (bufferStr[0] == '\0')
				wcscat_s(driveName, bufferLength, L"Local Disk");
			else
				wcscat_s(driveName, bufferLength, bufferStr);

			// load corresponding icon basing on lable
			insrtItem.item.iImage = g_nDiskList + logicalDriveStr[i] - 'A';
			insrtItem.item.iSelectedImage = g_nDiskList + logicalDriveStr[i] - 'A';

			break;
		}
		case DRIVE_CDROM:
		{
			if (bufferStr[0] == '\0')
				wcscat_s(driveName, bufferLength, L"Optical Drive");
			else
				wcscat_s(driveName, bufferLength, bufferStr);

			insrtItem.item.iImage = g_nOpticalDisc;
			insrtItem.item.iSelectedImage = g_nOpticalDisc;

			break;
		}
		default:
			if (bufferStr[0] == '\0')
				wcscat_s(driveName, bufferLength, L"Drive");
			else
				wcscat_s(driveName, bufferLength, bufferStr);

			insrtItem.item.iImage = g_nDiskList + 26;
			insrtItem.item.iSelectedImage = g_nDiskList + 26;
		}

		insrtItem.hParent = g_thisPC;
		insrtItem.hInsertAfter = TVI_SORT;
		insrtItem.item.pszText = (driveName);
		insrtItem.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
		insrtItem.item.cChildren = I_CHILDRENCALLBACK;

		HTREEITEM hItem = TreeView_InsertItem(g_hwndTV, &insrtItem);
		TreeView_LoadingChild(hItem);

		i += 4;
	}
	return 0;
}

void TreeView_GetPath(HTREEITEM hTreeItem, LPWSTR path, int bufferSize)
{
	if (hTreeItem == NULL)
		return;
	if (hTreeItem == g_thisPC)// in case hTreeItem is handle to This PC item
	{
		path[0] = '\0';
		return;
	}

	// in case hTreeItem is handle to logical drive
	HTREEITEM hIParent = TreeView_GetParent(g_hwndTV, hTreeItem);
	if (hIParent == g_thisPC)
	{
		swprintf_s(path, bufferSize, L"%lc:\\", ((LPTVITEM)hTreeItem)->pszText[1]);
		return;
	}

	TreeView_GetPath(hIParent, path, bufferSize);
	//if (wcslen(path) != 3)
	swprintf_s(path, bufferSize, L"%ls\\%ls", path, ((LPTVITEM)hTreeItem)->pszText);// path will contain double back-slash if parent is logical drive but it's ok
//else
//	swprintf_s(path, bufferSize, L"%ls%ls", path, ((LPTVITEM)hTreeItem)->pszText);
}

int TreeView_LoadingChild(HTREEITEM &selectedItem, int indent, BOOL showHidden, BOOL showSystem)
{
	// prevent loading more than 2 level of child
	if (indent == 2)
		return -1;

	// need to load nothing
	if (selectedItem == g_thisPC || selectedItem == NULL)
		return -2;
	WCHAR path[pathBufferLength];
	TreeView_GetPath(selectedItem, path, pathBufferLength);

	// processing the back-slash
	if (TreeView_GetParent(g_hwndTV, selectedItem) == g_thisPC)
	{
		path[3] = '*';
		path[4] = '\0';
	}
	else
		wcscat_s(path, pathBufferLength, L"\\*");

	//inserted item
	HTREEITEM hItemChild;
	TV_INSERTSTRUCT tvInsert;
	tvInsert.hParent = selectedItem;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvInsert.item.iImage = g_nFolder;
	tvInsert.item.iSelectedImage = g_nFolder;

	//searching
	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFileW(path, &fd);
	BOOL bFound = 1;
	BOOL isExist;
	if (hFile == INVALID_HANDLE_VALUE)
		bFound = FALSE;
	int childNum = 0;
	while (bFound)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& (showHidden ? TRUE : ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN))//option for hidden folders
			&& (showSystem ? TRUE : ((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM))//option for system folders
			&& (wcscmp(fd.cFileName, L".") != 0) && (wcscmp(fd.cFileName, L"..") != 0))
		{
			hItemChild = TreeView_GetChild(g_hwndTV, selectedItem);
			isExist = FALSE;
			while (hItemChild)
			{
				if (wcscmp(((LPTVITEM)hItemChild)->pszText, fd.cFileName) == 0)
				{
					isExist = TRUE;
					break;
				}
				hItemChild = TreeView_GetNextSibling(g_hwndTV, hItemChild);
			}
			// need to insert this child
			if (!isExist)
			{
				tvInsert.item.pszText = fd.cFileName;
				hItemChild = TreeView_InsertItem(g_hwndTV, &tvInsert);
				++childNum;

				if (indent == 1 && childNum >= 100)// loading some children (if exist) on 2nd level is enough, amount bases on the performance
					return childNum;
			}
			TreeView_LoadingChild(hItemChild, indent + 1);//load next level of child

		}

		bFound = FindNextFileW(hFile, &fd);
	}
	return childNum;
}

//
//for list view
//
HWND ListView_Create(HWND hwndParent)
{
	HWND hwndLV;    // handle to list-view control 
	RECT rect;
	GetWindowRect(hwndParent, &rect);
	int parentWidth = rect.right - rect.left;
	int width = (parentWidth > 300) ? (parentWidth / 3) : 100;
	if (width > 300)
		width = 300;

	int parentHeigh = rect.bottom - rect.top;
	int heigh = parentHeigh > 150 ? parentHeigh : 150;

	hwndLV = CreateWindowEx(0,
		WC_LISTVIEW,
		TEXT("List View"),
		WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | WS_VSCROLL | LVS_SINGLESEL,
		width - 1,//overlap to tree view 
		g_RibbonHeigh,
		parentWidth - width + 1,
		heigh - g_RibbonHeigh,
		hwndParent,
		(HMENU)IDC_LISTVIEW,
		g_hInst,
		NULL);



	return hwndLV;


}

void ListView_Initialize(HWND hWnd)
{
	LVCOLUMN lvCol;
	RECT rect;
	GetWindowRect(hWnd, &rect);
	int width = rect.right - rect.left;

	// 1st column
	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = width / 3;
	lvCol.pszText = L"Name";
	ListView_InsertColumn(g_hwndLV, 0, &lvCol);

	// 2nd column
	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol.fmt = LVCFMT_RIGHT;
	lvCol.cx = width / 6;
	lvCol.pszText = L"Type";
	ListView_InsertColumn(g_hwndLV, 1, &lvCol);

	// 3rd column
	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol.fmt = LVCFMT_RIGHT;
	lvCol.cx = width / 6;
	lvCol.pszText = _T("Size");
	ListView_InsertColumn(g_hwndLV, 2, &lvCol);

}

int ListView_LoadingChild(HWND hWnd, LPWSTR path, BOOL showHidden, BOOL showSystem)
{
	int childNum = 0;
	if (path[0] == '\0')// content loading for This PC
	{
		RECT rect;
		GetWindowRect(hWnd, &rect);
		int width = rect.right - rect.left;
		LVCOLUMN lvCol;

		ListView_GetColumn(g_hwndLV, 1, &lvCol);
		lvCol.mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;
		lvCol.fmt = LVCFMT_RIGHT;
		lvCol.cx = ListView_GetColumnWidth(g_hwndLV, 1);
		lvCol.pszText = _T("Total Size");
		ListView_SetColumn(g_hwndLV, 1, &lvCol);

		lvCol.mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;
		lvCol.fmt = LVCFMT_RIGHT;
		lvCol.cx = ListView_GetColumnWidth(g_hwndLV, 2);
		lvCol.pszText = _T("Free Space");
		ListView_SetColumn(g_hwndLV, 2, &lvCol);


		int i = 0;
		int index = 0;

		WCHAR logicalDriveStr[105];
		LVITEM insrtItem;
		insrtItem.mask = LVIF_TEXT | LVIF_IMAGE;
		insrtItem.iItem = 0;
		//insrtItem.pszText
		insrtItem.cchTextMax = 256;
		//iImage

		//get drive info
		const int bufferLength = 253;
		WCHAR bufferStr[bufferLength + 1];
		WCHAR driveName[bufferLength + 4];

		GetLogicalDriveStrings(104, logicalDriveStr);

		while (logicalDriveStr[i] != '\0')
		{
			//get drive index from zero
			index = logicalDriveStr[i] - 'A';

			GetVolumeInformation(logicalDriveStr + i, bufferStr, bufferLength, 0, 0, 0, 0, 0);

			//get drive type
			swprintf_s(driveName, bufferLength + 3, L"(%ls ", logicalDriveStr + i);
			driveName[3] = ')';
			switch (GetDriveType(logicalDriveStr + i))
			{
			case DRIVE_REMOVABLE:
			{
				if (bufferStr[0] == '\0')
					wcscat_s(driveName, bufferLength, L"Removable Disk");
				else
					wcscat_s(driveName, bufferLength, bufferStr);

				// 1st column
				insrtItem.mask = LVIF_TEXT | LVIF_IMAGE;
				insrtItem.iItem = i / 4;
				insrtItem.pszText = driveName;
				insrtItem.cchTextMax = 256;
				insrtItem.iImage = g_nUSBDisc;
				insrtItem.pszText = driveName;
				insrtItem.iSubItem = 0;
				ListView_InsertItem(g_hwndLV, &insrtItem);

				TCHAR buffer[11];

				// 2nd column
				insrtItem.mask = LVIF_TEXT;
				insrtItem.iSubItem = 1;
				__int64 nSize;
				SHGetDiskFreeSpaceEx(logicalDriveStr + i, NULL, (PULARGE_INTEGER)&nSize, NULL);
				Convert(nSize, buffer);
				insrtItem.pszText = buffer;
				ListView_SetItem(g_hwndLV, &insrtItem);

				// 3rd column
				insrtItem.mask = LVIF_TEXT;
				insrtItem.iSubItem = 2;
				__int64 nFreeSpace;
				GetDiskFreeSpaceEx(logicalDriveStr + i, NULL, NULL, (PULARGE_INTEGER)&nFreeSpace);
				Convert(nFreeSpace, buffer);
				insrtItem.pszText = buffer;
				ListView_SetItem(g_hwndLV, &insrtItem);


				break;
			}
			case DRIVE_FIXED:
			{
				if (bufferStr[0] == '\0')
					wcscat_s(driveName, bufferLength, L"Local Disk");
				else
					wcscat_s(driveName, bufferLength, bufferStr);

				insrtItem.iImage = g_nDiskList + logicalDriveStr[i] - 'A';

				// 1st column
				insrtItem.mask = LVIF_TEXT | LVIF_IMAGE;
				insrtItem.iItem = i / 4;
				insrtItem.pszText = driveName;
				insrtItem.cchTextMax = 256;
				insrtItem.iImage = g_nDiskList + logicalDriveStr[i] - 'A';
				insrtItem.pszText = driveName;
				insrtItem.iSubItem = 0;
				ListView_InsertItem(g_hwndLV, &insrtItem);

				// 2nd column
				insrtItem.mask = LVIF_TEXT;
				insrtItem.iSubItem = 1;
				__int64 nSize;
				SHGetDiskFreeSpaceEx(logicalDriveStr + i, NULL, (PULARGE_INTEGER)&nSize, NULL);
				TCHAR buffer[11];
				Convert(nSize, buffer);
				insrtItem.pszText = buffer;
				ListView_SetItem(g_hwndLV, &insrtItem);

				// 3rd column
				insrtItem.mask = LVIF_TEXT;
				insrtItem.iSubItem = 2;
				__int64 nFreeSpace;
				GetDiskFreeSpaceEx(logicalDriveStr + i, NULL, NULL, (PULARGE_INTEGER)&nFreeSpace);
				nSize = nSize - nSize;
				Convert(nFreeSpace, buffer);
				insrtItem.pszText = buffer;
				ListView_SetItem(g_hwndLV, &insrtItem);
				break;
			}
			case DRIVE_CDROM:
			{
				if (bufferStr[0] == '\0')
					wcscat_s(driveName, bufferLength, L"Optical Drive");
				else
					wcscat_s(driveName, bufferLength, bufferStr);

				insrtItem.iImage = g_nOpticalDisc;

				break;
			}
			default:
				if (bufferStr[0] == '\0')
					wcscat_s(driveName, bufferLength, L"Drive");
				else
					wcscat_s(driveName, bufferLength, bufferStr);

				insrtItem.iImage = g_nDiskList + 26;
			}

			i += 4;
			++insrtItem.iItem;
		}
	}
	else// loading files and folders in almost cases
	{
		LVCOLUMN lvCol;

		// 2nd column
		lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvCol.fmt = LVCFMT_RIGHT;
		lvCol.cx = ListView_GetColumnWidth(g_hwndLV, 1);
		lvCol.pszText = L"Type";
		ListView_SetColumn(g_hwndLV, 1, &lvCol);

		// 3rd column
		lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvCol.fmt = LVCFMT_RIGHT;
		lvCol.cx = ListView_GetColumnWidth(g_hwndLV, 2);
		lvCol.pszText = _T("Size");
		ListView_SetColumn(g_hwndLV, 2, &lvCol);

		//cearch for folder & file
		WIN32_FIND_DATA fd;
		HANDLE hFile;
		BOOL bFound = true;
		LV_ITEM lv;

		int nItemCount = 0;

		if (wcslen(path) == 3) //process the backslash
		{
			path[3] = '*';
			path[4] = '\0';
		}
		else
			wcscat_s(path, pathBufferLength, L"\\*");


		//1st time search to get folder info
		hFile = FindFirstFileW(path, &fd);
		bFound = TRUE;

		if (hFile == INVALID_HANDLE_VALUE)
			bFound = FALSE;

		while (bFound)
		{
			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				&& (showHidden ? TRUE : ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN))
				&& (showSystem ? TRUE : ((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM))
				&& (wcscmp(fd.cFileName, L".") != 0) && (wcscmp(fd.cFileName, L"..") != 0))
			{

				lv.iGroup = 0;
				lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
				lv.iItem = nItemCount;
				lv.iSubItem = 0;
				lv.pszText = fd.cFileName;
				lv.iImage = g_nFolder;
				ListView_InsertItem(g_hwndLV, &lv);

				// Type
				ListView_SetItemText(g_hwndLV, nItemCount, 1, L"Folder");


				++nItemCount;
			}

			bFound = FindNextFileW(hFile, &fd);
		}


		//2nd time search to get file info
		hFile = FindFirstFileW(path, &fd);
		bFound = TRUE;

		if (hFile == INVALID_HANDLE_VALUE)
			bFound = FALSE;

		while (bFound)
		{
			if (((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) &&
				((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM) &&
				((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN))
			{

				lv.iGroup = 1;

				//1st column
				lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
				lv.iItem = nItemCount;
				lv.iSubItem = 0;
				lv.pszText = fd.cFileName;
				lv.iImage = g_nFile;
				ListView_InsertItem(g_hwndLV, &lv);

				//file type
				size_t length = wcslen(fd.cFileName);
				size_t dotIndex = length - 1;
				while (true)
				{
					if (dotIndex < 0)// have no extension
					{
						ListView_SetItemText(g_hwndLV, nItemCount, 1, L"Unknown File");
						break;
					}
					else if (fd.cFileName[dotIndex] == '.')
					{
						TCHAR extension[20];
						wcscpy_s(extension, 20, fd.cFileName + dotIndex + 1);
						wcscat_s(extension, 20, L" File");
						ListView_SetItemText(g_hwndLV, nItemCount, 1, extension);
						break;
					}
					--dotIndex;
				}


				//Size
				DWORD dwSize = fd.nFileSizeLow;
				const int bufferLength = 253;
				WCHAR bufferStr[bufferLength + 1];
				Convert(fd.nFileSizeLow, bufferStr);
				ListView_SetItemText(g_hwndLV, nItemCount, 2, bufferStr);

				++nItemCount;
			}

			bFound = FindNextFileW(hFile, &fd);
		}

	}
	return 0;
}


void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case ID_VIEW_HIDDENITEMS:
	{
		OPT_SHOWHIDDEN = !OPT_SHOWHIDDEN;
		CheckMenuItem(GetMenu(hWnd), ID_VIEW_HIDDENITEMS, OPT_SHOWHIDDEN ? MF_CHECKED : MF_UNCHECKED);
		TreeView_DeleteAllItems(g_hwndTV);
		TreeView_Initialize(g_hwndTV);
		ListView_DeleteAllItems(g_hwndLV);
		break;
	}
	case ID_VIEW_SYSTEMITEMS:
	{
		OPT_SHOWSYSTEM = !OPT_SHOWSYSTEM;
		CheckMenuItem(GetMenu(hWnd), ID_VIEW_SYSTEMITEMS, OPT_SHOWSYSTEM ? MF_CHECKED : MF_UNCHECKED);
		TreeView_DeleteAllItems(g_hwndTV);
		TreeView_Initialize(g_hwndTV);
		ListView_DeleteAllItems(g_hwndLV);
		break;
	}
	case IDM_ABOUT:
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		break;
	case IDM_EXIT:
		DestroyWindow(hWnd);
		break;
	}
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

//copied content from demo code
void Convert(__int64 nSize, TCHAR *buffer)
{
	int nType = 0; //Bytes

	while (nSize >= 1048576)
	{
		nSize /= 1024;
		++nType;
	}

	__int64 nRight;

	if (nSize >= 1024)
	{
		//Lấy một chữ số sau thập phân của nSize chứa trong nRight
		nRight = nSize % 1024;

		while (nRight > 99)
			nRight /= 10;

		nSize /= 1024;
		++nType;
	}
	else
		nRight = 0;
	_itow_s(int(nSize), buffer, 11, 10);

	if (nRight != 0 && nType > 1)
	{
		wcscat_s(buffer, 11, L".");
		TCHAR *right = new TCHAR[3];
		_itow_s(int(nRight), right, 3, 10);
		wcscat_s(buffer, 11, right);
	}

	switch (nType)
	{
	case 0://Bytes
		wcscat_s(buffer, 11, L" bytes");
		break;
	case 1:
		wcscat_s(buffer, 11, L" KB");
		break;
	case 2:
		wcscat_s(buffer, 11, L" MB");
		break;
	case 3:
		wcscat_s(buffer, 11, L" GB");
		break;
	case 4:
		wcscat_s(buffer, 11, L" TB");
		break;
	}
}











//
//
//
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
#include "stdafx.h"
#include <UIRibbon.h>

#include "CommandHandler.h"
#include "RibbonIDs.h"
#include "RibbonFramework.h"

// Static method to create an instance of the object.
HRESULT CCommandHandler::CreateInstance(IUICommandHandler **ppCommandHandler)
{
	if (!ppCommandHandler)
	{
		return E_POINTER;
	}

	*ppCommandHandler = NULL;

	HRESULT hr = S_OK;

	CCommandHandler* pCommandHandler = new CCommandHandler();

	if (pCommandHandler != NULL)
	{
		*ppCommandHandler = static_cast<IUICommandHandler *>(pCommandHandler);
	}
	else
	{
		hr = E_OUTOFMEMORY;
	}

	return hr;
}

// IUnknown method implementations.
STDMETHODIMP_(ULONG) CCommandHandler::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CCommandHandler::Release()
{
	LONG cRef = InterlockedDecrement(&m_cRef);
	if (cRef == 0)
	{
		delete this;
	}

	return cRef;
}

STDMETHODIMP CCommandHandler::QueryInterface(REFIID iid, void** ppv)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppv = static_cast<IUnknown*>(this);
	}
	else if (iid == __uuidof(IUICommandHandler))
	{
		*ppv = static_cast<IUICommandHandler*>(this);
	}
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

//
//  FUNCTION: UpdateProperty()
//
//  PURPOSE: Called by the Ribbon framework when a command property (PKEY) needs to be updated.
//
//  COMMENTS:
//
//    This function is used to provide new command property values, such as labels, icons, or
//    tooltip information, when requested by the Ribbon framework.  
//    
//    In this SimpleRibbon sample, the method is not implemented.  
//
STDMETHODIMP CCommandHandler::UpdateProperty(
	UINT nCmdID,
	REFPROPERTYKEY key,
	const PROPVARIANT* ppropvarCurrentValue,
	PROPVARIANT* ppropvarNewValue)
{
	UNREFERENCED_PARAMETER(nCmdID);
	UNREFERENCED_PARAMETER(key);
	UNREFERENCED_PARAMETER(ppropvarCurrentValue);
	UNREFERENCED_PARAMETER(ppropvarNewValue);

	return E_NOTIMPL;
}

//
//  FUNCTION: Execute()
//
//  PURPOSE: Called by the Ribbon framework when a command is executed by the user.  For example, when
//           a button is pressed.
//
STDMETHODIMP CCommandHandler::Execute(
	UINT nCmdID,
	UI_EXECUTIONVERB verb,
	const PROPERTYKEY* key,
	const PROPVARIANT* ppropvarValue,
	IUISimplePropertySet* pCommandExecutionProperties)
{
	//UNREFERENCED_PARAMETER(pCommandExecutionProperties);
	//UNREFERENCED_PARAMETER(ppropvarValue);
	//UNREFERENCED_PARAMETER(key);
	//UNREFERENCED_PARAMETER(verb);
	//UNREFERENCED_PARAMETER(nCmdID);

	switch (nCmdID)
	{
	case ID_CMD_HIDDEN:
		OPT_SHOWHIDDEN = !OPT_SHOWHIDDEN;
		TreeView_DeleteAllItems(g_hwndTV);
		TreeView_Initialize(g_hwndTV);
		ListView_DeleteAllItems(g_hwndLV);
		break;
	case ID_CMD_SYSTEM:
		OPT_SHOWSYSTEM = !OPT_SHOWSYSTEM;
		TreeView_DeleteAllItems(g_hwndTV);
		TreeView_Initialize(g_hwndTV);
		ListView_DeleteAllItems(g_hwndLV);
		break;
	case ID_CMD_EXIT:
		DestroyWindow(g_hwndMain);
		break;
	case ID_CMD_ABOUT:
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), g_hwndMain, About);
		//UI_PKEY_Minimized
		break;
		//case IDR_CMD_MINIMIZE:
		//{
		//	if (g_RibbonHeigh)
		//		g_RibbonHeigh = 0;
		//	else
		//		g_RibbonHeigh = 147;
		//	break;
		//}
	}

	return S_OK;
}

//
//
//
//
