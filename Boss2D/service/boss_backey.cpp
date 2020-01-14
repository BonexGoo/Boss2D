#include <boss.hpp>
#include "boss_backey.hpp"

namespace BOSS
{
    static chars gLetterSymbols[6] = {"ㄷ", "ㄱ", "ㅈ", "ㅇ", "ㅡ", "ㅣ"};
    static chars gSelectBoard[6][9] = {
        {"ㅃ", "", "",
         "ㄸ", "ㅂ", "",
         "ㄷ", "ㅌ", "ㅍ"},
        {"ㅋ", "", "",
         "ㄱ", "ㄴ", "",
         "ㄲ", "ㄹ", ""},
        {"", "", "",
         "ㅆ", "ㅅ", "",
         "ㅉ", "ㅈ", "ㅊ"},
        {"", "", "ⓔ",
         "", "ⓢ", "ㅁ",
         "←", "ㅎ", "ㅇ"},
        {"", "", "ㅗ",
         "", "ㅛ", "ㅡ",
         "", "ㅠ", "ㅜ"},
        {"", "", "",
         "", "ㅕ", "ㅑ",
         "ㅓ", "ㅣ", "ㅏ"}};
    static Point gSelectBoardPos[6] = {
        Point(0.0f, 1.0f), Point(0.0f, 0.5f), Point(0.5f, 1.0f),
        Point(1.0f, 1.0f), Point(1.0f, 0.5f), Point(0.5f, 1.0f)};

    Backey::Backey(ButtonCB button, SelectorCB selector)
    {
        mButton = (button)? button : DefaultButton;
        mSelector = (selector)? selector : DefaultSelector;
        SelectorCB mSelector;
        mSelectedId = -1;
        mSelectedLetter = -1;
    }

    Backey::~Backey()
    {
    }

    void Backey::Render(ZayPanel& panel, h_view view)
    {
        const sint32 MinSize = Math::Min(panel.w(), panel.h());
        const sint32 ButtonSize = MinSize / 6;

        // 'ㄷ'
        ZAY_XYWH_UI(panel, ButtonSize * 0, panel.h() - ButtonSize * 1, ButtonSize, ButtonSize, "backey:0",
            ZAY_GESTURE_VNTXY(v, n, t, x, y, this, view, ButtonSize)
            {
                if(t == GT_Pressed) BeginSelector(0);
                else if(t == GT_InDragging || t == GT_OutDragging)
                {
                    auto OldXY = v->oldxy(n);
                    MoveSelector((x - OldXY.x) / (float) ButtonSize, (y - OldXY.y) / (float) ButtonSize);
                    v->invalidate();
                }
                else if(t == GT_InReleased || t == GT_OutReleased)
                    EndSelector(view);
            })
            mButton(panel, 0, panel.state("backey:0"));

        // 'ㄱ'
        ZAY_XYWH_UI(panel, ButtonSize * 0, panel.h() - ButtonSize * 2, ButtonSize, ButtonSize, "backey:1",
            ZAY_GESTURE_VNTXY(v, n, t, x, y, this, view, ButtonSize)
            {
                if(t == GT_Pressed) BeginSelector(1);
                else if(t == GT_InDragging || t == GT_OutDragging)
                {
                    auto OldXY = v->oldxy(n);
                    MoveSelector((x - OldXY.x) / (float) ButtonSize, (y - OldXY.y) / (float) ButtonSize);
                    v->invalidate();
                }
                else if(t == GT_InReleased || t == GT_OutReleased)
                    EndSelector(view);
            })
            mButton(panel, 1, panel.state("backey:1"));

        // 'ㅈ'
        ZAY_XYWH_UI(panel, ButtonSize * 1, panel.h() - ButtonSize * 1, ButtonSize, ButtonSize, "backey:2",
            ZAY_GESTURE_VNTXY(v, n, t, x, y, this, view, ButtonSize)
            {
                if(t == GT_Pressed) BeginSelector(2);
                else if(t == GT_InDragging || t == GT_OutDragging)
                {
                    auto OldXY = v->oldxy(n);
                    MoveSelector((x - OldXY.x) / (float) ButtonSize, (y - OldXY.y) / (float) ButtonSize);
                    v->invalidate();
                }
                else if(t == GT_InReleased || t == GT_OutReleased)
                    EndSelector(view);
            })
            mButton(panel, 2, panel.state("backey:2"));

        // '·'
        ZAY_XYWH_UI(panel, panel.w() - ButtonSize * 1, panel.h() - ButtonSize * 1, ButtonSize, ButtonSize, "backey:3",
            ZAY_GESTURE_VNTXY(v, n, t, x, y, this, view, ButtonSize)
            {
                if(t == GT_Pressed) BeginSelector(3);
                else if(t == GT_InDragging || t == GT_OutDragging)
                {
                    auto OldXY = v->oldxy(n);
                    MoveSelector((x - OldXY.x) / (float) ButtonSize, (y - OldXY.y) / (float) ButtonSize);
                    v->invalidate();
                }
                else if(t == GT_InReleased || t == GT_OutReleased)
                    EndSelector(view);
            })
            mButton(panel, 3, panel.state("backey:3"));

        // '√'
        ZAY_XYWH_UI(panel, panel.w() - ButtonSize * 1, panel.h() - ButtonSize * 2, ButtonSize, ButtonSize, "backey:4",
            ZAY_GESTURE_VNTXY(v, n, t, x, y, this, view, ButtonSize)
            {
                if(t == GT_Pressed) BeginSelector(4);
                else if(t == GT_InDragging || t == GT_OutDragging)
                {
                    auto OldXY = v->oldxy(n);
                    MoveSelector((x - OldXY.x) / (float) ButtonSize, (y - OldXY.y) / (float) ButtonSize);
                    v->invalidate();
                }
                else if(t == GT_InReleased || t == GT_OutReleased)
                    EndSelector(view);
            })
            mButton(panel, 4, panel.state("backey:4"));

        // 'ㅂ'
        ZAY_XYWH_UI(panel, panel.w() - ButtonSize * 2, panel.h() - ButtonSize * 1, ButtonSize, ButtonSize, "backey:5",
            ZAY_GESTURE_VNTXY(v, n, t, x, y, this, view, ButtonSize)
            {
                if(t == GT_Pressed) BeginSelector(5);
                else if(t == GT_InDragging || t == GT_OutDragging)
                {
                    auto OldXY = v->oldxy(n);
                    MoveSelector((x - OldXY.x) / (float) ButtonSize, (y - OldXY.y) / (float) ButtonSize);
                    v->invalidate();
                }
                else if(t == GT_InReleased || t == GT_OutReleased)
                    EndSelector(view);
            })
            mButton(panel, 5, panel.state("backey:5"));

        // 셀렉터
        if(mSelectedId != -1)
        {
            ZAY_LTRB(panel, (panel.w() - ButtonSize * 2) / 2, panel.h() - ButtonSize * 2, (panel.w() + ButtonSize * 2) / 2, panel.h())
            ZAY_INNER(panel, ButtonSize / 2)
            {
                // 포커스
                ZAY_XYRR(panel, panel.w() * mSelectedPos.x, panel.h() * mSelectedPos.y, ButtonSize / 3, ButtonSize / 3)
                ZAY_RGBA(panel, 192, 192, 0, 160)
                    panel.circle();
                // 버튼
                for(sint32 y = 0; y < 3; ++y)
                for(sint32 x = 0; x < 3; ++x)
                {
                    const sint32 LetterId = x + y * 3;
                    chars CurLetter = gSelectBoard[mSelectedId][LetterId];
                    if(CurLetter[0] != '\0')
                    ZAY_XYRR(panel, panel.w() * x / 2, panel.h() * y / 2, ButtonSize / 4, ButtonSize / 4)
                        mSelector(panel, CurLetter, LetterId == mSelectedLetter);
                }
            }
        }
    }

    void Backey::DefaultButton(ZayPanel& panel, sint32 id, PanelState ps)
    {
        const bool Pressed = !!(ps & (PS_Pressed | PS_Dragging));
        ZAY_FONT(panel, panel.w() / 36)
        ZAY_INNER(panel, panel.w() / 12)
        {
            ZAY_RGB(panel, 0, 0, 0)
                panel.rect(2);
            ZAY_RGBA_IF(panel, 0, 0, 0, 160, !Pressed)
            ZAY_RGBA_IF(panel, 192, 192, 0, 160, Pressed)
                panel.fill();
            ZAY_RGB_IF(panel, 192, 192, 192, !Pressed)
            ZAY_RGB_IF(panel, 0, 0, 0, Pressed)
                panel.text(panel.w() / 2, panel.h() / 2, gLetterSymbols[id], UIFA_CenterMiddle);
        }
    }

    void Backey::DefaultSelector(ZayPanel& panel, chars letter, bool selected)
    {
        ZAY_FONT(panel, panel.w() / 32)
        ZAY_INNER(panel, panel.w() / 8)
        {
            ZAY_RGB(panel, 0, 0, 0)
                panel.rect(2);
            ZAY_RGBA_IF(panel, 0, 0, 0, 160, !selected)
            ZAY_RGBA_IF(panel, 192, 192, 0, 160, selected)
                panel.fill();
            ZAY_RGB_IF(panel, 192, 192, 192, !selected)
            ZAY_RGB_IF(panel, 0, 0, 0, selected)
                panel.text(panel.w() / 2, panel.h() / 2, letter, UIFA_CenterMiddle);
        }
    }

    void Backey::BeginSelector(sint32 id)
    {
        mSelectedId = id;
        mSelectedPos = gSelectBoardPos[id];
        mSelectedLetter = CalcLetterBy(mSelectedId, mSelectedPos.x, mSelectedPos.y);
    }

    void Backey::MoveSelector(float add_x, float add_y)
    {
        mSelectedPos += Point(add_x, add_y);
        mSelectedLetter = CalcLetterBy(mSelectedId, mSelectedPos.x, mSelectedPos.y);
    }

    void Backey::EndSelector(h_view view)
    {
        if(mSelectedLetter != -1 && view.get())
        {
            String KeyCode = gSelectBoard[mSelectedId][mSelectedLetter];
            WString KeyCodeW = WString::FromChars(KeyCode);
            Platform::SendNotify(view, "BackeyWideCode", KeyCodeW);
        }
        mSelectedId = -1;
        mSelectedLetter = -1;
    }

    sint32 Backey::CalcLetterBy(sint32 id, float x, float y)
    {
        const float LetterX = x * 2 + 0.5f;
        const float LetterY = y * 2 + 0.5f;
        if(0 <= LetterX && LetterX < 3 && 0 <= LetterY && LetterY < 3)
        {
            const sint32 LetterId = ((sint32) LetterX) + ((sint32) LetterY) * 3;
            if(gSelectBoard[id][LetterId][0] != '\0')
                return LetterId;
        }
        return -1;
    }
}
