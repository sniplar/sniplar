#include <nan.h>
#include <windows.h>

using namespace std;

const wchar_t selectionWindowClassName[] = L"selection_window";
HINSTANCE globalInstance;

RECT clientRect { 0, 0, 0, 0 };
POINTS selectionBegin { 0, 0 };
POINTS selectionEnd { 0, 0 };
RECT screenshotRect { 0, 0, 0,0 };

COLORREF bgColor = RGB(255, 255, 255);
COLORREF selColor = RGB(255, 0, 255);
COLORREF frameColor = RGB(255, 0, 0);

HBRUSH backgroundBrush;
HBRUSH selectionBrush;
HBRUSH frameBrush;

static void takeScreenshot(RECT bounds)
{
	int width = abs(bounds.right - bounds.left);
	int height = abs(bounds.bottom - bounds.top);

	// Copy screen to bitmap
	HDC     hScreen = GetDC(NULL);
	HDC     hDC = CreateCompatibleDC(hScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, width, height);
	HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
	BOOL    bRet = BitBlt(hDC, 0, 0, width, height, hScreen, bounds.left, bounds.top, SRCCOPY);

	// Save bitmap to clipboard
	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, hBitmap);
	CloseClipboard();

	// Clean up
	SelectObject(hDC, old_obj);
	DeleteDC(hDC);
	ReleaseDC(NULL, hScreen);
	DeleteObject(hBitmap);
}

static void selectionWndEraseBackground(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	HDC dc = (HDC)wParam;

	RECT background;
	GetClientRect(hwnd, &background);

	HBRUSH bgBrush = CreateSolidBrush(RGB(255, 255, 255));
	FillRect(dc, &background, backgroundBrush);

	RECT selection;
	selection.left   = min(selectionBegin.x, selectionEnd.x);
	selection.top    = min(selectionBegin.y, selectionEnd.y);
	selection.right  = max(selectionBegin.x, selectionEnd.x);
	selection.bottom = max(selectionBegin.y, selectionEnd.y);

	FillRect(dc, &selection, selectionBrush);

	selection.left -= 1;
	selection.top -= 1;
	selection.right += 1;
	selection.bottom += 1;

	FrameRect(dc, &selection, frameBrush);
}

static void selectionWndBeginRubberBand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	// Capture the cursor
	SetCapture(hwnd);

	// Clip the cursor to this window
	GetClientRect(hwnd, &clientRect);

	POINT topLeft { clientRect.left, clientRect.top };
	ClientToScreen(hwnd, &topLeft);

	POINT bottomRight{ clientRect.right, clientRect.bottom };
	ClientToScreen(hwnd, &bottomRight);

	SetRect(&clientRect, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
	ClipCursor(&clientRect);

	// Save start position
	selectionBegin = MAKEPOINTS(lParam);
}

static void selectionWndEndRubberBand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	// Save end position
	selectionEnd = MAKEPOINTS(lParam);

	// Release cursor capturing
	ReleaseCapture();

	// Release the clipped cursor
	ClipCursor(NULL);

	// Calculate screen rect
	POINT topLeft { min(selectionBegin.x, selectionEnd.x), min(selectionBegin.y, selectionEnd.y) };
	ClientToScreen(hwnd, &topLeft);

	POINT bottomRight { max(selectionBegin.x, selectionEnd.x), max(selectionBegin.y, selectionEnd.y) };
	ClientToScreen(hwnd, &bottomRight);

	SetRect(&screenshotRect, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);

	// Close the window, exit message loop
	CloseWindow(hwnd);
	PostQuitMessage(0);
}

static void selectionWndMouseMove(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	if (wParam & MK_LBUTTON) {
		selectionEnd = MAKEPOINTS(lParam);
		InvalidateRect(hwnd, NULL, TRUE);
	}
}

static LRESULT CALLBACK selectionWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_ERASEBKGND:
		selectionWndEraseBackground(hwnd, wParam, lParam);
		return 1;
	case WM_LBUTTONDOWN:
		selectionWndBeginRubberBand(hwnd, wParam, lParam);
		return 1;
	case WM_LBUTTONUP:
		selectionWndEndRubberBand(hwnd, wParam, lParam);
		return 1;
	case WM_MOUSEMOVE:
		selectionWndMouseMove(hwnd, wParam, lParam);
		return 1;
	case WM_CHAR:
		if (wParam == 0x1B /* Escape */) {
			PostQuitMessage(0);
			return 1;
		}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 1;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

static bool createWindowClass()
{
	WNDCLASS wc;
	wc.style = 0;
	wc.lpfnWndProc = selectionWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = globalInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = selectionWindowClassName;

	return !!RegisterClass(&wc);
}

static HWND createSelectionWindow()
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	HWND wnd = CreateWindowEx(WS_EX_TOPMOST|WS_EX_LAYERED, selectionWindowClassName, L"Selection Window", WS_POPUP, 0, 0, screenWidth, screenHeight, NULL, NULL, globalInstance, NULL);
	if (IsWindow(wnd)) {
		SetLayeredWindowAttributes(wnd, selColor, 180 /* alpha 0..255 */, LWA_COLORKEY | LWA_ALPHA);
	}

	return wnd;
}

NAN_METHOD(CaptureScreen) {
        globalInstance = GetModuleHandle(NULL);

	// Register window class
	if (!createWindowClass()) {
		return -1;
	}

	// Create brushes for drawing
	backgroundBrush = CreateSolidBrush(bgColor);
	selectionBrush = CreateSolidBrush(selColor);
	frameBrush = CreateSolidBrush(frameColor);

	// Create the transparent window
	HWND selectionWnd = createSelectionWindow();
	ShowWindow(selectionWnd, SW_SHOW);
	SetCapture(selectionWnd);

	// Set cross cursor
	SetCursor(LoadCursor(NULL, IDC_CROSS));

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Take screenshot
	if (!IsRectEmpty(&screenshotRect)) {
		takeScreenshot(screenshotRect);
	}

	// Delete brushes
	DeleteObject(backgroundBrush);
	DeleteObject(selectionBrush);
        DeleteObject(frameBrush);
}

NAN_MODULE_INIT(Initialize) {
        NAN_EXPORT(target, CaptureScreen);
}

NODE_MODULE(addon, Initialize)
