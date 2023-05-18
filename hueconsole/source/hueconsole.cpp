#include <boss.hpp>
#include "hueconsole.hpp"

#include <resource.hpp>
#include <service/boss_zaywidget.hpp>

ZAY_DECLARE_VIEW_CLASS("hueconsoleView", hueconsoleData)

static hueconsoleData* gSelf = nullptr;

ZAY_VIEW_API OnCommand(CommandType type, id_share in, id_cloned_share* out)
{
    if(type == CT_Tick)
    {
        if(Platform::Utility::CurrentTimeMsec() <= m->mUpdateMsec)
            m->invalidate(2);

        // 스크롤
        const sint32 ScrollMin = 0 * 1000;
        const sint32 ScrollMax = Math::Max(0, (m->mCells.Count() / m->mCellWidth) - m->mCellHeight) * 1000;
        if(m->mScrollLog < ScrollMin)
        {
            m->mScrollLog = ((m->mScrollLog + 1) * 8 + ScrollMin * 2) / 10;
            m->invalidate(2);
        }
        else if(ScrollMax < m->mScrollLog)
        {
            m->mScrollLog = ((m->mScrollLog - 1) * 8 + ScrollMax * 2) / 10;
            m->invalidate(2);
        }
        if(m->mScrollLog != m->mScrollPhy)
        {
            m->mScrollPhy += (m->mScrollLog < m->mScrollPhy)? -1 : 1;
            m->mScrollPhy = (m->mScrollPhy * 9 + m->mScrollLog * 1) / 10;
            m->invalidate(2);
        }
    }
}

ZAY_VIEW_API OnNotify(NotifyType type, chars topic, id_share in, id_cloned_share* out)
{
    if(type == NT_ZayWidget)
    {
        if(!String::Compare(topic, "EnterPressing"))
        {
            const String DOMName(in);
            const sint32 BoxIdx = Parser::GetInt(DOMName.Offset(4)); // dom_0
            const String Text = ZayWidgetDOM::GetComment(DOMName);
            auto& CurBox = gSelf->mBoxes[BoxIdx];
            if(CurBox.mScanCB)
                CurBox.mScanCB(Text, 1);
        }
    }
}

ZAY_VIEW_API OnGesture(GestureType type, sint32 x, sint32 y)
{
    if(0 < m->mLastApp.Length())
    {
        if(type == GT_WheelUp || type == GT_WheelDown)
        {
            m->mScrollLog = m->mScrollLog + ((type == GT_WheelUp)? -1000 : 1000);
            m->invalidate(2);
        }
    }
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
    else ZAY_MOVE(panel, 0, -panel.h() * m->mScrollPhy / m->mCellHeight / 1000)
    {
        // 셀
        const uint64 CurMsec = Platform::Utility::CurrentTimeMsec();
        for(sint32 i = 0, iend = m->mCells.Count(); i < iend; ++i)
        {
            const auto& CurCell = m->mCells[i];
            const bool ExtendFont = (1 < CurCell.mLetter.Length());
            const sint32 X = i % m->mCellWidth;
            const sint32 Y = i / m->mCellWidth;
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
                const float Opacity = (500 - Math::Max(0, CurCell.mWrittenMsec - CurMsec)) / 500.0f;
                ZAY_COLOR(panel, CurCell.mColor)
                ZAY_RGBA(panel, 128, 128, 128, 128 * Opacity)
                    panel.text(panel.w() / 2 - 1, panel.h() / 2 - 1, CurCell.mLetter);
            }
            if(ExtendFont) i++;
        }

        // 박스
        for(sint32 i = 0, iend = m->mBoxes.Count(); i < iend; ++i)
        {
            auto& CurBox = m->mBoxes[i];
            const String UIName = String::Format("ui_%d", i);
            ZAY_LTRB(panel,
                panel.w() * CurBox.mLeft / m->mCellWidth,
                panel.h() * CurBox.mTop / m->mCellHeight,
                panel.w() * CurBox.mRight / m->mCellWidth,
                panel.h() * CurBox.mBottom / m->mCellHeight)
            {
                const bool Focused = ((panel.state(UIName) & (PS_Focused | PS_Dropping)) == PS_Focused);
                ZAY_COLOR(panel, CurBox.mColor)
                for(sint32 j = 0, jend = (Focused)? 4 : 2; j < jend; ++j)
                    ZAY_INNER(panel, -j)
                    ZAY_RGBA(panel, 128, 128, 128, 128 - (128 / jend) * j)
                        panel.rect(1);

                // 스캔에디터
                if(CurBox.mScanCB)
                {
                    const String DOMName = String::Format("dom_%d", i);
                    ZAY_COLOR(panel, CurBox.mColor)
                    ZAY_LTRB_SCISSOR(panel, 0, 0, panel.w(), panel.h())
                    ZAY_LTRB(panel, 0, 0, panel.w() - 4, panel.h())
                    if(ZayControl::RenderEditBox(panel, UIName, DOMName, 1, true, false))
                        panel.repaint();
                }
                // 클릭박스
                else if(CurBox.mClickCB)
                {
                    ZAY_INNER_UI(panel, 0, UIName,
                        ZAY_GESTURE_T(t, i)
                    {
                        if(t == GT_InReleased)
                        {
                            auto& CurBox = m->mBoxes[i];
                            if(CurBox.mClickCB)
                                CurBox.mClickCB();
                        }
                    });
                }
            }
        }
    }
}

hueconsoleData::hueconsoleData()
{
    gSelf = this;
    mLastColor = Color::Black;
    mLastBGColor = Color::White;
    mClearBGColor = Color::White;
    mUpdateMsec = 0;
    mScrollLog = 0;
    mScrollPhy = 0;
    ClearScreen(80, 25, Color::White);
}

hueconsoleData::~hueconsoleData()
{
    gSelf = nullptr;
}

void hueconsoleData::ValidCells(sint32 count)
{
    for(sint32 i = mCells.Count(); i < count; ++i)
    {
        auto& NewCell = mCells.AtAdding();
        NewCell.mColor = Color::Black;
        NewCell.mBGColor = mClearBGColor;
        NewCell.mWrittenMsec = 0;
    }
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
            NewCell.mWrittenMsec = 0;
        }
        gSelf->mCellFocus = 0;
        gSelf->mLastBGColor = bgcolor;
        gSelf->mClearBGColor = bgcolor;
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
        gSelf->ValidCells(gSelf->mCellFocus);
        const uint64 UpdateMsec = Platform::Utility::CurrentTimeMsec() + 500;
        sint32 LetterIndex = gSelf->mCellFocus;
        for(sint32 i = 0, iend = boss_strlen(text); i < iend;)
        {
            const sint32 LetterCount = String::GetLengthOfFirstLetter(((chars) text) + i);
            const String NewLetter = text.Middle(i, LetterCount);
            i += LetterCount;

            if(!NewLetter.Compare("\n"))
            {
                LetterIndex -= (LetterIndex % gSelf->mCellWidth); // 현재줄의 맨앞으로 이동
                LetterIndex += gSelf->mCellWidth; // 다음줄로 이동
            }
            else
            {
                gSelf->ValidCells(++LetterIndex);
                auto& CurCell = gSelf->mCells.At(LetterIndex - 1);
                CurCell.mLetter = NewLetter;
                CurCell.mColor = gSelf->mLastColor;
                CurCell.mBGColor = gSelf->mLastBGColor;
                CurCell.mWrittenMsec = UpdateMsec;
                if(1 < LetterCount) // 한글처럼 2칸을 쓰는 폰트에 대한 처리
                {
                    gSelf->ValidCells(++LetterIndex);
                    auto& NextCell = gSelf->mCells.At(LetterIndex - 1);
                    NextCell.mLetter.Empty();
                    NextCell.mColor = gSelf->mLastColor;
                    NextCell.mBGColor = gSelf->mLastBGColor;
                    NextCell.mWrittenMsec = UpdateMsec;
                }
            }
        }
        gSelf->mCellFocus = LetterIndex;
        gSelf->mUpdateMsec = UpdateMsec;
        const sint32 YSize = (gSelf->mCellFocus + gSelf->mCellWidth - 1) / gSelf->mCellWidth;
        gSelf->ValidCells(gSelf->mCellWidth * YSize);
    }
}

void hueconsoleData::TextScan(sint32 w, ScanCB cb)
{
    if(gSelf)
    {
        sint32 X = gSelf->mCellFocus % gSelf->mCellWidth;
        sint32 Y = gSelf->mCellFocus / gSelf->mCellWidth;
        sint32 Width = w;
        if(gSelf->mCellWidth < X + Width)
        {
            X = 0;
            Y++;
            Width = Math::Min(Width, gSelf->mCellWidth);
        }
        gSelf->mCellFocus = (Y + 1) * gSelf->mCellWidth;

        auto& NewBox = gSelf->mBoxes.AtAdding();
        NewBox.mColor = gSelf->mLastColor;
        NewBox.mLeft = X;
        NewBox.mTop = Y;
        NewBox.mRight = X + Width;
        NewBox.mBottom = Y + 1;
        NewBox.mScanCB = cb;

        const sint32 BoxIdx = gSelf->mBoxes.Count() - 1;
        const String DOMName = String::Format("dom_%d", BoxIdx);
        ZayWidgetDOM::SetComment(DOMName, "");
        ZayWidgetDOM::SetVariableFilter(DOMName,
            [BoxIdx](const String& formula, const SolverValue& value, float reliable)->bool
            {
                if(formula[0] == '?')
                {
                    auto& CurBox = gSelf->mBoxes[BoxIdx];
                    if(CurBox.mScanCB)
                        CurBox.mScanCB(((chars) formula) + 1, 0);
                }
                return true;
            });
    }
}

void hueconsoleData::ClickBox(sint32 w, sint32 h, ClickCB cb)
{
    if(gSelf)
    {
        sint32 X = gSelf->mCellFocus % gSelf->mCellWidth;
        sint32 Y = gSelf->mCellFocus / gSelf->mCellWidth;
        sint32 Width = w;
        if(gSelf->mCellWidth < X + Width)
        {
            X = 0;
            Y++;
            Width = Math::Min(Width, gSelf->mCellWidth);
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
