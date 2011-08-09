// KinectSign.cpp : アプリケーションのエントリ ポイントを定義します。
//

// 下で指定された定義の前に対象プラットフォームを指定しなければならない場合、以下の定義を変更してください。
// 異なるプラットフォームに対応する値に関する最新情報については、MSDN を参照してください。
#ifndef WINVER				// Windows XP 以降のバージョンに固有の機能の使用を許可します。
#define WINVER 0x0501		// これを Windows の他のバージョン向けに適切な値に変更してください。
#endif

#ifndef _WIN32_WINNT		// Windows XP 以降のバージョンに固有の機能の使用を許可します。                   
#define _WIN32_WINNT 0x0501	// これを Windows の他のバージョン向けに適切な値に変更してください。
#endif						

#ifndef _WIN32_WINDOWS		// Windows 98 以降のバージョンに固有の機能の使用を許可します。
#define _WIN32_WINDOWS 0x0410 // これを Windows Me またはそれ以降のバージョン向けに適切な値に変更してください。
#endif

#ifndef _WIN32_IE			// IE 6.0 以降のバージョンに固有の機能の使用を許可します。
#define _WIN32_IE 0x0600	// これを IE. の他のバージョン向けに適切な値に変更してください。
#endif

#include "stdafx.h"
#include "KinectSign.h"
#include "I4C3DSocketCommunication.h"
#include <map>

#define MAX_LOADSTRING		100
#define PARENT_WINDOW		_T("PartTransparent")
#define CHILD_WINDOW		_T("PartTransparentChild")
#define TRANSPARENT_COLOR	RGB(255, 255, 255)	// 白
#define WINDOW_SIZE			150

static const char* KINECT_ZOOMIN	= "kinect zoomin";
static const char* KINECT_ZOOMOUT	= "kinect zoomout";
static const char* KINECT_UP		= "kinect up";
static const char* KINECT_DOWN		= "kinect down";
static const char* KINECT_LEFT		= "kinect left";
static const char* KINECT_RIGHT		= "kinect right";
static const char* KINECT_STOP		= "kinect stop";

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名

typedef enum {
	DOLLY_ZOOMIN,
	DOLLY_ZOOMOUT,
	TUMBLE_UP,
	TUMBLE_DOWN,
	TUMBLE_LEFT,
	TUMBLE_RIGHT,
	COMMAND_STOP,
} I4C3D_COMMAND;

static std::map<I4C3D_COMMAND, const wchar_t*> g_ImageMap;
static I4C3D_COMMAND g_i4c3dCommand					= COMMAND_STOP;
static I4C3DSocketCommunication* g_i4c3dSocket		= NULL;


// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(WNDPROC lpWndProc, LPTSTR szWindowClassName, HBRUSH hbrush);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void				InitializeImageMap(void);

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

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_KINECTSIGN, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(WndProc, PARENT_WINDOW, (HBRUSH)GetStockObject(WHITE_BRUSH));

	// アプリケーションの初期化を実行します:
	if (!InitInstance (hInstance, nCmdShow))
	{
		WSACleanup();
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KINECTSIGN));

	// メイン メッセージ ループ:
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

void InitializeImageMap(void)
{
	g_ImageMap.insert( std::map<I4C3D_COMMAND, const wchar_t*>::value_type(DOLLY_ZOOMIN, _T("Image/zoomin.png")) );
	g_ImageMap.insert( std::map<I4C3D_COMMAND, const wchar_t*>::value_type(DOLLY_ZOOMOUT, _T("Image/zoomout.png")) );
	g_ImageMap.insert( std::map<I4C3D_COMMAND, const wchar_t*>::value_type(TUMBLE_UP, _T("Image/up.png")) );
	g_ImageMap.insert( std::map<I4C3D_COMMAND, const wchar_t*>::value_type(TUMBLE_DOWN, _T("Image/down.png")) );
	g_ImageMap.insert( std::map<I4C3D_COMMAND, const wchar_t*>::value_type(TUMBLE_LEFT, _T("Image/left.png")) );
	g_ImageMap.insert( std::map<I4C3D_COMMAND, const wchar_t*>::value_type(TUMBLE_RIGHT, _T("Image/right.png")) );
	g_ImageMap.insert( std::map<I4C3D_COMMAND, const wchar_t*>::value_type(COMMAND_STOP, _T("Image/stop.png")) );
}

//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
//  コメント:
//
//    この関数および使い方は、'RegisterClassEx' 関数が追加された
//    Windows 95 より前の Win32 システムと互換させる場合にのみ必要です。
//    アプリケーションが、関連付けられた
//    正しい形式の小さいアイコンを取得できるようにするには、
//    この関数を呼び出してください。
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
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

   hWnd = CreateWindowEx(
	   WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
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

   RECT desktop;
   SystemParametersInfo(SPI_GETWORKAREA, 0, &desktop, 0);

   MoveWindow(hWnd, 0, desktop.bottom-WINDOW_SIZE, WINDOW_SIZE, WINDOW_SIZE, TRUE);
   SetLayeredWindowAttributes(hWnd, 0, 196, LWA_ALPHA);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:  メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 中止メッセージを表示して戻る
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
		// 子ウィンドウ作成
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
		MoveWindow(hWndChild, 0, 0, WINDOW_SIZE, WINDOW_SIZE, TRUE);
		SetLayeredWindowAttributes(hWndChild, TRANSPARENT_COLOR, 0, LWA_COLORKEY);
		ShowWindow(hWndChild, SW_SHOW);
		UpdateWindow(hWndChild);

		// 表示するイメージをマップに格納
		InitializeImageMap();

		// KinectSignを受信するためのソケット生成
		g_i4c3dSocket = new I4C3DSocketCommunication(hWnd);
		break;

	case WM_MOVE:
		// 子ウィンドウを制御
		if (hWndChild != NULL) {
			RECT rect;
			GetWindowRect(hWnd, &rect);
			rect.left	+= 1;
			rect.top	+= 1;
			rect.right	-= 1;
			rect.bottom	-= 1;
			MoveWindow(hWndChild, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
		}
		break;

	case WM_LBUTTONDOWN:
		// ウィンドウを移動できるようにするトリック
		SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		break;

	case WM_RBUTTONDOWN:
		// 右クリックで終了
		DestroyWindow(hWnd);
		break;

	case WSOCK_SELECT:
		{
			char buf[32] = {0};
			switch (WSAGETSELECTEVENT(lParam)) {
			case FD_READ:
				if (recv(g_i4c3dSocket->m_socketHandler, buf, sizeof(buf), 0) <= 0) {
					g_i4c3dCommand = COMMAND_STOP;

				} else {
					if (!_stricmp(buf, KINECT_ZOOMIN))
					{
						g_i4c3dCommand = DOLLY_ZOOMIN;
					}
					else if (!_stricmp(buf, KINECT_ZOOMOUT))
					{
						g_i4c3dCommand = DOLLY_ZOOMOUT;
					}
					else if (!_stricmp(buf, KINECT_UP))
					{
						g_i4c3dCommand = TUMBLE_UP;
					}
					else if (!_stricmp(buf, KINECT_DOWN))
					{
						g_i4c3dCommand = TUMBLE_DOWN;
					}
					else if (!_stricmp(buf, KINECT_LEFT))
					{
						g_i4c3dCommand = TUMBLE_LEFT;
					}
					else if (!_stricmp(buf, KINECT_RIGHT))
					{
						g_i4c3dCommand = TUMBLE_RIGHT;
					}
					else if (!_stricmp(buf, KINECT_STOP))
					{
						g_i4c3dCommand = COMMAND_STOP;
					}
				}
				InvalidateRect(hWndChild, NULL, TRUE);
				break;
			}
		}
		break;

	//case WM_COMMAND:
	//	wmId    = LOWORD(wParam);
	//	wmEvent = HIWORD(wParam);
	//	// 選択されたメニューの解析:
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
	//	// TODO: 描画コードをここに追加してください...
	//	EndPaint(hWnd, &ps);
	//	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		if (g_i4c3dSocket != NULL) {
			delete g_i4c3dSocket;
			g_i4c3dSocket = NULL;
		}
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
			Gdiplus::Image *img = new Gdiplus::Image(g_ImageMap[g_i4c3dCommand]);
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

// バージョン情報ボックスのメッセージ ハンドラーです。
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
