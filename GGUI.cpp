#include "GGUI.h"
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <windowsx.h>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")

HWND hwnd = nullptr;
ID2D1Factory* d2dFactory = nullptr;
ID2D1HwndRenderTarget* renderTarget = nullptr;
ID2D1SolidColorBrush* brush = nullptr;
IDWriteFactory* dwriteFactory = nullptr;
IDWriteTextFormat* textFormat = nullptr;

int GUI_MouseX = 0, GUI_MouseY = 0;
bool GUI_MousePressed = false;

LRESULT CALLBACK GUI_WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:
            if (renderTarget) {
                RECT rc;
                GetClientRect(hwnd, &rc);
                D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
                renderTarget->Resize(size);
            }
            return 0;
        case WM_MOUSEMOVE:
            GUI_MouseX = GET_X_LPARAM(lParam);
            GUI_MouseY = GET_Y_LPARAM(lParam);
            return 0;
        case WM_LBUTTONDOWN:
            GUI_MousePressed = true;
            return 0;
        case WM_LBUTTONUP:
            GUI_MousePressed = false;
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool GUI_CreateWindow(const char* title, int width, int height) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = GUI_WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "GGUIClass";
    RegisterClass(&wc);

    hwnd = CreateWindowEx(0, "GGUIClass", title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                          width, height, NULL, NULL, wc.hInstance, NULL);
    if (!hwnd) return false;

    ShowWindow(hwnd, SW_SHOW);

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);

    RECT rc;
    GetClientRect(hwnd, &rc);
    D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
    D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(
        hwnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top));
    d2dFactory->CreateHwndRenderTarget(rtProps, hwndProps, &renderTarget);

    renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &brush);

    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&dwriteFactory);
    dwriteFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                                    DWRITE_FONT_STRETCH_NORMAL, 18.0f, L"en-us", &textFormat);

    return true;
}

bool GUI_BeginFrame() {
    MSG msg = {};
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT)
            return false;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    renderTarget->BeginDraw();
    renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
    return true;
}

void GUI_EndFrame() {
    renderTarget->EndDraw();
}

bool GUI_Button(float xRatio, float yRatio, float wRatio, float hRatio, const char* label) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    int w = rc.right;
    int h = rc.bottom;

    float x = xRatio * w;
    float y = yRatio * h;
    float bw = wRatio * w;
    float bh = hRatio * h;

    D2D1_RECT_F rect = D2D1::RectF(x, y, x + bw, y + bh);
    D2D1_COLOR_F color = D2D1::ColorF(0.6f, 0.2f, 1.0f);
    brush->SetColor(color);
    renderTarget->FillRectangle(rect, brush);

    brush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));

    std::wstring wlabel(label, label + strlen(label));
    renderTarget->DrawText(wlabel.c_str(), wlabel.size(), textFormat, rect, brush);

    bool clicked = GUI_MousePressed &&
                   GUI_MouseX >= x && GUI_MouseX <= x + bw &&
                   GUI_MouseY >= y && GUI_MouseY <= y + bh;
    return clicked;
}

void GUI_DrawRect(const GUI_Rect& rect) {
    if (!renderTarget) return;

    ID2D1SolidColorBrush* brush = nullptr;
    renderTarget->CreateSolidColorBrush(rect.color, &brush);

    D2D1_RECT_F r = D2D1::RectF(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
    renderTarget->FillRectangle(&r, brush);

    brush->Release();
}

void GUI_DrawCircle(const GUI_Circle& circle) {
    if (!renderTarget) return;

    ID2D1SolidColorBrush* brush = nullptr;
    renderTarget->CreateSolidColorBrush(circle.color, &brush);

    D2D1_ELLIPSE e = D2D1::Ellipse(D2D1::Point2F(circle.x, circle.y), circle.radius, circle.radius);
    renderTarget->FillEllipse(&e, brush);

    brush->Release();
}
