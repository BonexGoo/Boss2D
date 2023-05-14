#include <boss.hpp>
#include "huecoding.h"

#include <resource.hpp>
#include <hueconsole.hpp>
#include <stdarg.h>

int _declareapp(const char* app, AppCB cb)
{
    hueconsoleData::_AllApps()(app) = cb;
    return 0;
}

void setscreen(int w, int h, const char* bgcolor)
{
    hueconsoleData::SetScreen(w, h, bgcolor);
}

void gotoxy(int x, int y)
{
    hueconsoleData::GotoXY(x, y);
}

void setcolor(const char* color, const char* bgcolor)
{
    hueconsoleData::SetColor(color, bgcolor);
}

void print(const char* format, ...)
{
    va_list Args;
    va_start(Args, format);
    const sint32 Size = boss_vsnprintf(nullptr, 0, format, Args);
    va_end(Args);

    chararray Collector;
    if(0 <= Size)
    {
        Collector.AtWherever(Size) = '\0';
        va_start(Args, format);
        boss_vsnprintf(Collector.AtDumping(0, Size + 1), Size + 1, format, Args);
        va_end(Args);
        hueconsoleData::TextLabel(&Collector[0]);
    }
    else
    {
        BOSS_ASSERT("vsnprintf에서 text의 길이를 추산하지 못함", false);
        hueconsoleData::TextLabel("<error>");
    }
}

void clickbox(int w, int h, ClickCB cb)
{
    hueconsoleData::ClickBox(w, h, cb);
}

void textbox(int w, int h, TextCB cb)
{
    hueconsoleData::TextBox(w, h, cb);
}

void redraw(int count)
{
    hueconsoleData::Redraw(count);
}
