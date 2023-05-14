#pragma once
#include <service/boss_zay.hpp>
#include <huecoding.h>

class Cell
{
public:
    String mLetter;
    Color mColor;
    Color mBGColor;
};

class hueconsoleData : public ZayObject
{
public:
    hueconsoleData();
    ~hueconsoleData();

public:
    static void SetScreen(int w, int h, const char* bgcolor);
    static void GotoXY(int x, int y);
    static void SetColor(const char* color, const char* bgcolor);
    static void TextLabel(const char* text);
    static void ClickBox(int w, int h, ClickCB cb);
    static void TextBox(int w, int h, TextCB cb);
    static void Redraw(int count);

public:
    String mLastApp;
    sint32 mCellWidth;
    sint32 mCellHeight;
    Array<Cell> mCells;
    sint32 mLabelFocus;
    sint32 mBoxFocus;

public:
    static inline Map<AppCB>& _AllApps()
    {static Map<AppCB> _; return _;}
};
