#include <nan.h>
#include <windows.h>

using namespace std;

void screenshot(POINT a, POINT b)
{
    POINT start, end;

    start.x = a.x < b.x ? a.x : b.x;
    start.y = a.y < b.y ? a.y : b.y;
    end.x = a.x < b.x ? b.x : a.x;
    end.y = a.y < b.y ? b.y : a.y;

    // copy screen to bitmap
    HDC     hScreen = GetDC(NULL);
    HDC     hDC     = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, abs(end.x-start.x), abs(end.y-start.y));
    HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
    BOOL    bRet    = BitBlt(hDC, 0, 0, abs(end.x-start.x), abs(end.y-start.y), hScreen, start.x, start.y, SRCCOPY);

    // save bitmap to clipboard
    OpenClipboard(NULL);
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, hBitmap);
    CloseClipboard();

    // clean up
    SelectObject(hDC, old_obj);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);
}

NAN_METHOD(CaptureScreen) {
//  auto message = Nan::New<v8::String>("I'm a Node Hero!").ToLocalChecked();
//  info.GetReturnValue().Set(message);
  POINT b, p;
  p.x = 0;
  p.y = 0;

  //b = p;
  if (!GetCursorPos(&p)) {
      p.x = 0;
      p.y = 0;
  }

  b.x = p.x + 500;
  b.y = p.y + 500;
  Sleep(10);
  screenshot(p, b);
}

NAN_METHOD(WhoAmI) {
  auto message = Nan::New<v8::String>("I'm a Node Hero!").ToLocalChecked();
  info.GetReturnValue().Set(message);
}

NAN_MODULE_INIT(Initialize) {
  NAN_EXPORT(target, WhoAmI);
  NAN_EXPORT(target, CaptureScreen);
}

NODE_MODULE(addon, Initialize)
