#pragma once
#include <boss.h>

#ifdef __cplusplus
    extern "C" {
#endif

    // 비공개부
    typedef void (*AppCB)();
    typedef void (*ClickCB)();
    typedef void (*TextCB)(const char* text);
    int _declareapp(const char* app, AppCB cb);

    // 공개부
    void setscreen(int w, int h, const char* bgcolor);
    void gotoxy(int x, int y);
    void setcolor(const char* color, const char* bgcolor);
    void print(const char* format, ...);
    void clickbox(int w, int h, ClickCB cb);
    void textbox(int w, int h, TextCB cb);
    void redraw(int count);

#ifdef __cplusplus
    }
#endif

// 앱등록
#define HUE_DECLARE_APP(NAME, FUNC) \
    static void FUNC(); \
    static int _##FUNC = _declareapp(NAME, FUNC);
