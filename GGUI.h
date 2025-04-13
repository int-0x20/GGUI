#pragma once
#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")

bool GUI_CreateWindow(const char* title, int width, int height);
bool GUI_BeginFrame();
void GUI_EndFrame();

bool GUI_Button(float x, float y, float w, float h, const char* label);

extern int GUI_MouseX, GUI_MouseY;
extern bool GUI_MousePressed;

struct GUI_Rect {
    float x, y, width, height;
    D2D1_COLOR_F color;
};

struct GUI_Circle {
    float x, y, radius;
    D2D1_COLOR_F color;
};

void GUI_Init(HWND hwnd);
void GUI_BeginDraw();
void GUI_EndDraw();
void GUI_Cleanup();

void GUI_DrawRect(const GUI_Rect& rect);
void GUI_DrawCircle(const GUI_Circle& circle);