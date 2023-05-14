#include <boss.hpp>
#include "hueconsole.hpp"

#include <resource.hpp>

ZAY_DECLARE_VIEW_CLASS("hueconsoleView", hueconsoleData)

ZAY_VIEW_API OnCommand(CommandType type, id_share in, id_cloned_share* out)
{
}

ZAY_VIEW_API OnNotify(NotifyType type, chars topic, id_share in, id_cloned_share* out)
{
}

ZAY_VIEW_API OnGesture(GestureType type, sint32 x, sint32 y)
{
}

ZAY_VIEW_API OnRender(ZayPanel& panel)
{
    ZAY_RGB(panel, 192, 192, 192)
        panel.fill();

    if(m->mLastApp.Length() == 0)
    {
        auto& AllApps = hueconsoleData::_AllApps();
        for(sint32 i = 0, iend = AllApps.Count(); i < iend; ++i)
        {
            chararray CurApp;
            AllApps.AccessByOrder(i, &CurApp);
            ZAY_LTRB(panel, 0, panel.h() * i / iend, panel.w(), panel.h() * (i + 1) / iend)
            {
                const String UIName = String::Format("ui_app_%d", i);
                ZAY_INNER_UI_SCISSOR(panel, 10, UIName,
                    ZAY_GESTURE_T(t, CurApp)
                    {
                        if(t == GT_InReleased)
                        {
                            auto& AllApps = hueconsoleData::_AllApps();
                            if(auto OneApp = AllApps.Access(&CurApp[0]))
                            {
                                m->mLastApp = &CurApp[0];
                                (*OneApp)();
                            }
                        }
                    })
                {
                    ZAY_RGB(panel, 255, 255, 255)
                        panel.fill();
                    ZAY_RGB(panel, 0, 0, 0)
                        panel.text(&CurApp[0], UIFA_CenterMiddle, UIFE_Right);
                }
            }
        }
    }
    else
    {
        ZAY_LTRB(panel, 0, 1, panel.w(), panel.h())
        for(sint32 i = 0, iend = m->mCellWidth * m->mCellHeight; i < iend; ++i)
        {
            const sint32 X = i % m->mCellWidth;
            const sint32 Y = i / m->mCellWidth;
            if(i < m->mCells.Count())
            {
                const auto& CurCell = m->mCells[i];
                const bool ExtendFont = (1 < CurCell.mLetter.Length());
                const sint32 XEnd = (ExtendFont)? X + 2 : X + 1;
                ZAY_LTRB_SCISSOR(panel, panel.w() * X / m->mCellWidth, panel.h() * Y / m->mCellHeight,
                    panel.w() * XEnd / m->mCellWidth, panel.h() * (Y + 1) / m->mCellHeight)
                {
                    ZAY_COLOR(panel, CurCell.mBGColor)
                    {
                        ZAY_RGBA(panel, 128, 128, 128, 110)
                            panel.fill();
                        ZAY_LTRB(panel, 0, 0, panel.w() - 1, panel.h() - 1)
                            panel.fill();
                    }
                    ZAY_COLOR(panel, CurCell.mColor)
                        panel.text(panel.w() / 2 - 1, panel.h() / 2 - 1, CurCell.mLetter);
                }
                if(ExtendFont) i++;
            }
        }
    }
}

static hueconsoleData* gSelf = nullptr;
hueconsoleData::hueconsoleData()
{
    mCellWidth = 80;
    mCellHeight = 25;
    for(sint32 i = 0, iend = mCellWidth * mCellHeight; i < iend; ++i)
    {
        auto& NewCell = mCells.AtAdding();
        NewCell.mColor = Color::Black;
        NewCell.mBGColor = Color::White;
    }
    mLabelFocus = 0;
    mBoxFocus = 0;

    gSelf = this;
}

hueconsoleData::~hueconsoleData()
{
    gSelf = nullptr;
}

void hueconsoleData::SetScreen(int w, int h, const char* bgcolor)
{
    if(gSelf)
    {
    }
}

void hueconsoleData::GotoXY(int x, int y)
{
    if(gSelf)
    {
        gSelf->mLabelFocus = x + y * gSelf->mCellWidth;
        gSelf->mBoxFocus = x + y * gSelf->mCellWidth;
    }
}

void hueconsoleData::SetColor(const char* color, const char* bgcolor)
{
    if(gSelf)
    {
    }
}

void hueconsoleData::TextLabel(const char* text)
{
    if(gSelf)
    {
        sint32 LetterIndex = gSelf->mLabelFocus;
        for(sint32 i = 0, iend = boss_strlen(text); i < iend;)
        {
            auto& CurCell = gSelf->mCells.AtWherever(LetterIndex++);
            const sint32 LetterCount = String::GetLengthOfFirstLetter(&text[i]);
            CurCell.mLetter = String(&text[i], LetterCount);
            if(1 < LetterCount) // 한글처럼 2칸을 쓰는 폰트에 대한 처리
            {
                auto& NextCell = gSelf->mCells.AtWherever(LetterIndex++);
                NextCell.mLetter.Empty();
            }
            i += LetterCount;
        }
        gSelf->mLabelFocus = LetterIndex;
    }
}

void hueconsoleData::ClickBox(int w, int h, ClickCB cb)
{
    if(gSelf)
    {
    }
}

void hueconsoleData::TextBox(int w, int h, TextCB cb)
{
    if(gSelf)
    {
    }
}

void hueconsoleData::Redraw(int count)
{
    if(gSelf)
    {
    }
}
