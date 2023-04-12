
#include<windows.h>
HWND hWnd;
HGLRC hGLRC;
HDC hDC;

LRESULT CALLBACK WindowFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	float pos[4] = { 3,3,3,1 };
	float dir[3] = { -1,-1,-1 };
	PAINTSTRUCT ps;
	switch (msg)
	{
	case WM_CREATE:
		hDC = GetDC(hWnd); // получаем контекст устройства нашего окна
		//engine->SetWindowPixelFormat(hDC); // устанавливаем параметры контекста воспроизведения OpenGL
		hGLRC = wglCreateContext(hDC); // создаем контекст воспроизведения OpenGL
		wglMakeCurrent(hDC, hGLRC); // делаем его текущим
		//engine->Init();
		break;
	case WM_DESTROY:
		if (hGLRC)
		{ // удаляем созданный выше контекст воспроизведения OpenGL
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(hGLRC);
		} // освобождаем контекст устройства нашего окна
		ReleaseDC(hWnd, hDC);
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		BeginPaint(hWnd, &ps);
		//engine->Draw();
		EndPaint(hWnd, &ps);
		break;
	case WM_SIZE:
		//engine->Resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_ERASEBKGND:
		return 1;
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR str, int nWinMode)
{
	MSG msg;
	WNDCLASS wcl;
	wcl.hInstance = hThisInst;
	wcl.lpszClassName = (LPCWSTR)"OpenGLWinClass";
	wcl.lpfnWndProc = WindowFunc;
	wcl.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wcl.hIcon = NULL;
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcl.lpszMenuName = NULL;
	wcl.cbClsExtra = 0;
	wcl.cbWndExtra = 0;
	wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	RegisterClass(&wcl);
	hWnd = CreateWindow((LPCWSTR)"OpenGLWinClass", L"Win32 OpenGL Template",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		200,
		150,
		400,
		420,
		HWND_DESKTOP, NULL,
		hThisInst, NULL);
	ShowWindow(hWnd, nWinMode);
	UpdateWindow(hWnd);
	while (1)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			if (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
				return 0;
		//display(); // функция display вызывается в бесконечном цикле, причем, когда в очереди сообщений окна нет ничего.
	}
	return 0;
}