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
        // 앱리스트
        auto& AllApps = hueconsoleData::_AllApps();
        ZAY_LTRB(panel, 10, 0, panel.w() - 10, panel.h() - 10)
        for(sint32 i = 0, iend = AllApps.Count(); i < iend; ++i)
        {
            chararray CurApp;
            AllApps.AccessByOrder(i, &CurApp);
            const String UIName = String::Format("ui_app_%d", i);
            ZAY_LTRB_UI_SCISSOR(panel, 0, panel.h() * i / iend + 10, panel.w(), panel.h() * (i + 1) / iend, UIName,
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
                {
                    ZAY_RGBA(panel, 128, 128, 128, 120)
                        panel.fill();
                    ZAY_LTRB(panel, 0, 0, panel.w() - 1, panel.h() - 1)
                        panel.fill();
                }
                ZAY_RGB(panel, 0, 0, 0)
                    panel.text(&CurApp[0], UIFA_CenterMiddle, UIFE_Right);
            }
        }
    }
    else
    {
        // 셀
        for(sint32 i = 0, iend = m->mCellWidth * m->mCellHeight; i < iend; ++i)
        {
            const sint32 X = i % m->mCellWidth;
            const sint32 Y = i / m->mCellWidth;
            if(i < m->mCells.Count())
            {
                const auto& CurCell = m->mCells[i];
                const bool ExtendFont = (1 < CurCell.mLetter.Length());
                const sint32 XEnd = (ExtendFont)? X + 2 : X + 1;
                ZAY_LTRB_SCISSOR(panel,
                    panel.w() * X / m->mCellWidth,
                    panel.h() * Y / m->mCellHeight,
                    panel.w() * XEnd / m->mCellWidth,
                    panel.h() * (Y + 1) / m->mCellHeight)
                {
                    ZAY_COLOR(panel, CurCell.mBGColor)
                    {
                        ZAY_RGBA(panel, 128, 128, 128, 120)
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

        // 박스
        for(sint32 i = 0, iend = m->mBoxes.Count(); i < iend; ++i)
        {
            auto& CurBox = m->mBoxes[i];
            const String UIName = String::Format("ui_%d", i);
            ZAY_LTRB_UI(panel,
                panel.w() * CurBox.mLeft / m->mCellWidth,
                panel.h() * CurBox.mTop / m->mCellHeight,
                panel.w() * CurBox.mRight / m->mCellWidth,
                panel.h() * CurBox.mBottom / m->mCellHeight, UIName,
                ZAY_GESTURE_T(t, i)
                {
                    if(t == GT_InReleased)
                    {
                        auto& CurBox = m->mBoxes[i];
                        if(CurBox.mClickCB)
                            CurBox.mClickCB();
                    }
                })
            {
                const bool Focused = ((panel.state(UIName) & (PS_Focused | PS_Dropping)) == PS_Focused);
                ZAY_COLOR(panel, CurBox.mColor)
                for(sint32 j = 0, jend = (Focused)? 4 : 2; j < jend; ++j)
                    ZAY_INNER(panel, -j)
                    ZAY_RGBA(panel, 128, 128, 128, 128 - (128 / jend) * j)
                        panel.rect(1);
            }
        }
    }
}

static hueconsoleData* gSelf = nullptr;
hueconsoleData::hueconsoleData()
{
    gSelf = this;
    mLastColor = Color::Black;
    mLastBGColor = Color::White;
    ClearScreen(80, 25, Color::White);
}

hueconsoleData::~hueconsoleData()
{
    gSelf = nullptr;
}

void hueconsoleData::ClearScreen(sint32 w, sint32 h, Color bgcolor)
{
    if(gSelf)
    {
        gSelf->mCellWidth = Math::Max(1, w);
        gSelf->mCellHeight = Math::Max(1, h);
        gSelf->mCells.Clear();
        for(sint32 i = 0, iend = gSelf->mCellWidth * gSelf->mCellHeight; i < iend; ++i)
        {
            auto& NewCell = gSelf->mCells.AtAdding();
            NewCell.mColor = Color::Black;
            NewCell.mBGColor = bgcolor;
        }
        gSelf->mCellFocus = 0;
        gSelf->mLastBGColor = bgcolor;
        gSelf->mBoxes.Clear();
    }
}

void hueconsoleData::GotoXY(sint32 x, sint32 y)
{
    if(gSelf)
        gSelf->mCellFocus = x + y * gSelf->mCellWidth;
}

void hueconsoleData::SetColor(Color color)
{
    if(gSelf)
        gSelf->mLastColor = color;
}

void hueconsoleData::SetBGColor(Color bgcolor)
{
    if(gSelf)
        gSelf->mLastBGColor = bgcolor;
}

void hueconsoleData::TextPrint(String text)
{
    if(gSelf)
    {
        sint32 LetterIndex = gSelf->mCellFocus;
        for(sint32 i = 0, iend = boss_strlen(text); i < iend;)
        {
            auto& CurCell = gSelf->mCells.AtWherever(LetterIndex++);
            const sint32 LetterCount = String::GetLengthOfFirstLetter(((chars) text) + i);
            CurCell.mLetter = text.Middle(i, LetterCount);
            CurCell.mColor = gSelf->mLastColor;
            CurCell.mBGColor = gSelf->mLastBGColor;
            if(1 < LetterCount) // 한글처럼 2칸을 쓰는 폰트에 대한 처리
            {
                auto& NextCell = gSelf->mCells.AtWherever(LetterIndex++);
                NextCell.mLetter.Empty();
                NextCell.mColor = gSelf->mLastColor;
                NextCell.mBGColor = gSelf->mLastBGColor;
            }
            i += LetterCount;
        }
        gSelf->mCellFocus = LetterIndex;
    }
}

void hueconsoleData::TextScan(sint32 w, ScanCB cb)
{
    if(gSelf)
    {
        sint32 X = gSelf->mCellFocus % m->mCellWidth;
        sint32 Y = gSelf->mCellFocus / m->mCellWidth;
        sint32 Width = w;
        if(m->mCellWidth < X + Width)
        {
            X = 0;
            Y++;
            Width = Math::Min(Width, m->mCellWidth);
        }

        auto& NewBox = gSelf->mBoxes.AtAdding();
        NewBox.mColor = gSelf->mLastColor;
        NewBox.mLeft = X;
        NewBox.mTop = Y;
        NewBox.mRight = X + Width;
        NewBox.mBottom = Y + 1;
        NewBox.mScanCB = cb;
    }
}

void hueconsoleData::ClickBox(sint32 w, sint32 h, ClickCB cb)
{
    if(gSelf)
    {
        sint32 X = gSelf->mCellFocus % m->mCellWidth;
        sint32 Y = gSelf->mCellFocus / m->mCellWidth;
        sint32 Width = w;
        if(m->mCellWidth < X + Width)
        {
            X = 0;
            Y++;
            Width = Math::Min(Width, m->mCellWidth);
        }

        auto& NewBox = gSelf->mBoxes.AtAdding();
        NewBox.mColor = gSelf->mLastColor;
        NewBox.mLeft = X;
        NewBox.mTop = Y;
        NewBox.mRight = X + Width;
        NewBox.mBottom = Y + h;
        NewBox.mClickCB = cb;
    }
}

void hueconsoleData::Repaint()
{
    if(gSelf)
    {
        if(auto OneApp = hueconsoleData::_AllApps().Access(gSelf->mLastApp))
        {
            (*OneApp)();
            gSelf->invalidate();
        }
    }
}
