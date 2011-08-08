// KinectSign.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

// ���Ŏw�肳�ꂽ��`�̑O�ɑΏۃv���b�g�t�H�[�����w�肵�Ȃ���΂Ȃ�Ȃ��ꍇ�A�ȉ��̒�`��ύX���Ă��������B
// �قȂ�v���b�g�t�H�[���ɑΉ�����l�Ɋւ���ŐV���ɂ��ẮAMSDN ���Q�Ƃ��Ă��������B
#ifndef WINVER				// Windows XP �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B
#define WINVER 0x0501		// ����� Windows �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#ifndef _WIN32_WINNT		// Windows XP �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B                   
#define _WIN32_WINNT 0x0501	// ����� Windows �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif						

#ifndef _WIN32_WINDOWS		// Windows 98 �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B
#define _WIN32_WINDOWS 0x0410 // ����� Windows Me �܂��͂���ȍ~�̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#ifndef _WIN32_IE			// IE 6.0 �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B
#define _WIN32_IE 0x0600	// ����� IE. �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#include "stdafx.h"
#include "KinectSign.h"
#include "I4C3DSocketCommunication.h"
#include <map>

#define MAX_LOADSTRING		100
#define PARENT_WINDOW		_T("PartTransparent")
#define CHILD_WINDOW		_T("PartTransparentChild")
#define TRANSPARENT_COLOR	RGB(255, 255, 255)	// ��
//#define TRANSPARENT_COLOR	RGB(1, 2, 3)

// �O���[�o���ϐ�:
HINSTANCE hInst;								// ���݂̃C���^�[�t�F�C�X
TCHAR szTitle[MAX_LOADSTRING];					// �^�C�g�� �o�[�̃e�L�X�g
TCHAR szWindowClass[MAX_LOADSTRING];			// ���C�� �E�B���h�E �N���X��

typedef enum {
	DOLLY_ZOOMIN,
	DOLLY_ZOOMOUT,
	TUMBLE_UP,
	TUMBLE_DOWN,
	TUMBLE_LEFT,
	TUMBLE_RIGHT,
} I4C3D_COMMAND;

static std::map<I4C3D_COMMAND, TCHAR*> g_ImageMap;


// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
ATOM				MyRegisterClass(WNDPROC lpWndProc, LPTSTR szWindowClassName, HBRUSH hbrush);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	static WSAData wsaData;
	WORD wVersion;
	int nResult;

	wVersion = MAKEWORD(2,2);
	nResult = WSAStartup(wVersion, &wsaData);
	if (nResult != 0) {
		MessageBox(NULL, _T("[ERROR] Initialize Winsock."), szTitle, MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}
	if (wsaData.wVersion != wVersion) {
		MessageBox(NULL, _T("[ERROR] Winsock version error."), szTitle, MB_OK | MB_ICONERROR);
		WSACleanup();
		return EXIT_FAILURE;
	}

	MSG msg;
	HACCEL hAccelTable;

	// �O���[�o������������������Ă��܂��B
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_KINECTSIGN, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(WndProc, PARENT_WINDOW, (HBRUSH)GetStockObject(WHITE_BRUSH));

	// �A�v���P�[�V�����̏����������s���܂�:
	if (!InitInstance (hInstance, nCmdShow))
	{
		WSACleanup();
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KINECTSIGN));

	// ���C�� ���b�Z�[�W ���[�v:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	WSACleanup();
	Gdiplus::GdiplusShutdown(gdiplusToken);
	return (int) msg.wParam;
}

void InitializeImageMap()
{

}

//
//  �֐�: MyRegisterClass()
//
//  �ړI: �E�B���h�E �N���X��o�^���܂��B
//
//  �R�����g:
//
//    ���̊֐�����юg�����́A'RegisterClassEx' �֐����ǉ����ꂽ
//    Windows 95 ���O�� Win32 �V�X�e���ƌ݊�������ꍇ�ɂ̂ݕK�v�ł��B
//    �A�v���P�[�V�������A�֘A�t����ꂽ
//    �������`���̏������A�C�R�����擾�ł���悤�ɂ���ɂ́A
//    ���̊֐����Ăяo���Ă��������B
//
ATOM MyRegisterClass(WNDPROC lpWndProc, LPTSTR szWindowClassName, HBRUSH hbrush)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= 0;//CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= lpWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= (HINSTANCE)GetModuleHandle(NULL);
	wcex.hIcon			= NULL;//LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KINECTSIGN));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= hbrush;//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//MAKEINTRESOURCE(IDC_KINECTSIGN);
	wcex.lpszClassName	= szWindowClassName;//szWindowClass;
	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   �֐�: InitInstance(HINSTANCE, int)
//
//   �ړI: �C���X�^���X �n���h����ۑ����āA���C�� �E�B���h�E���쐬���܂��B
//
//   �R�����g:
//
//        ���̊֐��ŁA�O���[�o���ϐ��ŃC���X�^���X �n���h����ۑ����A
//        ���C�� �v���O���� �E�B���h�E���쐬����ѕ\�����܂��B
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂��B

   hWnd = CreateWindowEx(
	   WS_EX_LAYERED | WS_EX_TOOLWINDOW,
	   PARENT_WINDOW,
	   PARENT_WINDOW,
	   WS_POPUP | WS_VISIBLE,
	   0,
	   0,
	   0,
	   0,
	   NULL,
	   NULL,
	   hInst,
	   NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   MoveWindow(hWnd, 100, 100, 200, 200, TRUE);
   SetLayeredWindowAttributes(hWnd, 0, 196, LWA_ALPHA);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  �֐�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  �ړI:  ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
//
//  WM_COMMAND	- �A�v���P�[�V���� ���j���[�̏���
//  WM_PAINT	- ���C�� �E�B���h�E�̕`��
//  WM_DESTROY	- ���~���b�Z�[�W��\�����Ė߂�
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//int wmId, wmEvent;
	//PAINTSTRUCT ps;
	//HDC hdc;

	static HWND hWndChild = NULL;

	switch (message)
	{
	case WM_CREATE:
		// �q�E�B���h�E�쐬
		MyRegisterClass(ChildWndProc, CHILD_WINDOW, CreateSolidBrush(TRANSPARENT_COLOR));
		hWndChild = CreateWindowEx(
			WS_EX_LAYERED | WS_EX_TRANSPARENT,
			CHILD_WINDOW,
			CHILD_WINDOW,
			WS_POPUP | WS_VISIBLE,
			0,
			0,
			0,
			0,
			hWnd,
			NULL,
			(HINSTANCE) GetModuleHandle(NULL),
			NULL);
		//MoveWindow(hWndChild, 150, 150, 100, 100, TRUE);
		MoveWindow(hWndChild, 100, 100, 200, 200, TRUE);
		SetLayeredWindowAttributes(hWndChild, TRANSPARENT_COLOR, 0, LWA_COLORKEY);
		ShowWindow(hWndChild, SW_SHOW);
		UpdateWindow(hWndChild);

		
		break;

	case WM_MOVE:
		// �q�E�B���h�E�𐧌�
		if (hWndChild != NULL) {
			RECT rect;
			GetWindowRect(hWnd, &rect);
			rect.left	+= 50;
			rect.top	+= 50;
			rect.right	-= 50;
			rect.bottom	-= 50;
			MoveWindow(hWndChild, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
		}
		break;

	case WM_LBUTTONDOWN:
		// �E�B���h�E���ړ��ł���悤�ɂ���g���b�N
		SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		break;

	case WM_RBUTTONDOWN:
		// �E�N���b�N�ŏI��
		PostQuitMessage(0);
		break;

	case MY_KINECTNOTIFY:
		break;

	//case WM_COMMAND:
	//	wmId    = LOWORD(wParam);
	//	wmEvent = HIWORD(wParam);
	//	// �I�����ꂽ���j���[�̉��:
	//	switch (wmId)
	//	{
	//	case IDM_ABOUT:
	//		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
	//		break;
	//	case IDM_EXIT:
	//		DestroyWindow(hWnd);
	//		break;
	//	default:
	//		return DefWindowProc(hWnd, message, wParam, lParam);
	//	}
	//	break;
	//case WM_PAINT:
	//	hdc = BeginPaint(hWnd, &ps);
	//	// TODO: �`��R�[�h�������ɒǉ����Ă�������...
	//	EndPaint(hWnd, &ps);
	//	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		{
			RECT rect;
			GetClientRect(hWnd, &rect);

			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			FillRect(hdc, &rect, CreateSolidBrush(TRANSPARENT_COLOR));
			Gdiplus::Graphics g(hdc);
			Gdiplus::Image *img = new Gdiplus::Image(_T("image.png"));
			UINT width = img->GetWidth();
			g.DrawImage(img, rect.left, rect.top, rect.right, rect.bottom);
			EndPaint(hWnd, &ps);
			delete img;
		}
		break;

	case WM_WINDOWPOSCHANGED:
		{
			LPWINDOWPOS lpwinpos = reinterpret_cast<LPWINDOWPOS>(lParam);
			HWND hwnd = lpwinpos->hwnd;
			lpwinpos->hwnd = GetParent(hWnd);
			SendMessage(lpwinpos->hwnd, WM_WINDOWPOSCHANGED, NULL, (LPARAM)reinterpret_cast<LPWINDOWPOS>(lpwinpos));
			lpwinpos->hwnd = hwnd;
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �o�[�W�������{�b�N�X�̃��b�Z�[�W �n���h���[�ł��B
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
