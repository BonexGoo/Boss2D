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

class Box
{
public:
    Color mColor;
    sint32 mLeft {0};
    sint32 mTop {0};
    sint32 mRight {0};
    sint32 mBottom {0};
    ScanCB mScanCB {nullptr};
    ClickCB mClickCB {nullptr};
};

class hueconsoleData : public ZayObject
{
public:
    hueconsoleData();
    ~hueconsoleData();

public:
    static void ClearScreen(sint32 w, sint32 h, Color bgcolor);
    static void GotoXY(sint32 x, sint32 y);
    static void SetColor(Color color);
    static void SetBGColor(Color bgcolor);
    static void TextPrint(String text);
    static void TextScan(sint32 w, ScanCB cb);
    static void ClickBox(sint32 w, sint32 h, ClickCB cb);
    static void Repaint();

public:
    String mLastApp;
    sint32 mCellWidth;
    sint32 mCellHeight;
    Array<Cell> mCells;
    sint32 mCellFocus;
    Color mLastColor;
    Color mLastBGColor;
    Array<Box> mBoxes;

public:
    static inline Map<AppCB>& _AllApps()
    {static Map<AppCB> _; return _;}
};
