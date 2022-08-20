// DX12.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "WinMain.h"

// 面倒だけと書かなければいけない関数
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// ウィンドウが破棄されたれ呼ばれる
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);	// OSに対して「もうこのアプリは終わる」と伝える
		return 0;
	}
	if (msg == UM_CURSORSTATUS)
	{
		BOOL showFlag = (BOOL)wparam;
		ShowCursor(showFlag);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);	// 既定の処理を行う
}

int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	
	// ウィンドウクラスの生成＆登録
	WNDCLASSEX w = {};
	w.cbSize = sizeof(WNDCLASSEX);				// サイズ
	w.style = CS_HREDRAW | CS_VREDRAW;
	w.lpfnWndProc = (WNDPROC)WindowProcedure;	// コールバック関数を指定
	w.lpszClassName = _T("DX12Game");			// アプリケーションクラス名(適当でよい)
	w.hInstance = GetModuleHandle(nullptr);		// ハンドルの取得
	w.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&w);// アプリケーションクラス(ウィンドウクラスの指定をOSに伝える)

	// 矩形情報
	RECT wrc = { 0, 0, (LONG)g_window_width, (LONG)g_window_height };	// ウィンドウサイズを決める
	//デスクトップの矩形情報
	RECT rcDeskTop = {};
	//デスクトップの矩形情報を取得
	::GetWindowRect(::GetDesktopWindow(),
		(LPRECT)&rcDeskTop);

	//ウィンドウスタイルを指定（標準）
	DWORD WindowStyle = WS_OVERLAPPEDWINDOW;
		

	//ウィンドウの描画座標
	int x = 0, y = 0;

	if (g_FullScreen)
	{
		wrc = rcDeskTop;
		//ウィンドウスタイルを指定(ポップアップ)
		WindowStyle = WS_POPUP | WS_VISIBLE;
		g_window_width = rcDeskTop.right;
		g_window_height = rcDeskTop.bottom;
	}
	else
	{
		//デスクトップウィンドウの中心に座標を決定
		x = (rcDeskTop.right - g_window_width) / 2;
		y = (rcDeskTop.bottom - g_window_height) / 2;
	}

	// 関数を使ってウィンドウのサイズを補正する
	AdjustWindowRect(&wrc, WindowStyle, false);

	// ウィンドウオブジェクトの生成
	g_hwnd = CreateWindow(
		w.lpszClassName,		// クラス名を指定
		_T("DX12GAME"),		// タイトルバーの文字
		WindowStyle,	// タイトルバーと境界線があるウィンドウ
		x,			// 表示x座標はOSにお任せ
		y,			// 表示y座標はOSにお任せ
		wrc.right - wrc.left,	// ウィンドウ幅
		wrc.bottom - wrc.top,	// ウィンドウ高さ
		nullptr,				// 親ウィンドウハンドル
		nullptr,				// メニューハンドル
		w.hInstance,			// 呼び出しアプリケーションハンドル
		nullptr);				// 追加パラメーター

	// ========== Initialize ==========
	{
		start = std::chrono::system_clock::now(); // 計測開始時間
		g_elapsedTime /= 1000;
		game.Initialize(w);
	}
	// ================================
	// ウィンドウ表示
	ShowWindow(g_hwnd, SW_SHOW);
	UpdateWindow(g_hwnd);

	// スレッドの生成
	std::thread thread(Update);

	MSG msg = {};
	
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// アプリケーションが終わるときにmessageがWM_QUITになる
		if (msg.message == WM_QUIT)
		{
			break;
		}

		if (game.GetGameFlag())
		{
			break;
		}

		Sleep(1);	// 1ミリ秒待機
	}
	// ========== Destroy ==========
	{
		_loopFlag = false;
		thread.join();	// スレッドの終了を待つ
		game.Shutdown();

		// もうクラスは使わないので登録解除する
		UnregisterClass(w.lpszClassName, w.hInstance);
	}
	// ==============================

	return (int)msg.wParam;
}

// アップデート
void Update()
{
	while (_loopFlag)
	{
		end = std::chrono::system_clock::now();  // 計測終了時間
		g_elapsedTime = (float)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); //処理に要した時間をミリ秒に変換
		start = std::chrono::system_clock::now(); // 計測開始時間
		g_elapsedTime /= 1000;
		game.RunLoop();
		Sleep(1);	// 1ミリ秒待機
	}
}