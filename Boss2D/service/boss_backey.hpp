#pragma once
#include <service/boss_zay.hpp>

namespace BOSS
{
    /// @brief 가상키보드
    class Backey
    {
    public:
        typedef void (*ButtonCB)(ZayPanel& panel, sint32 id, PanelState ps);
        typedef void (*SelectorCB)(ZayPanel& panel, chars letter, bool selected);

    public:
        Backey(ButtonCB button = nullptr, SelectorCB selector = nullptr);
        ~Backey();

    public:
        void Render(ZayPanel& panel, h_view view = h_view::null());

    private:
        static void DefaultButton(ZayPanel& panel, sint32 id, PanelState ps);
        static void DefaultSelector(ZayPanel& panel, chars letter, bool selected);
        void BeginSelector(sint32 id);
        void MoveSelector(float add_x, float add_y);
        void EndSelector(h_view view);
        sint32 CalcLetterBy(sint32 id, float x, float y);

    private:
        ButtonCB mButton;
        SelectorCB mSelector;
        String mWords;
        sint32s mLetters;
        sint32 mSelectedId;
        Point mSelectedPos;
        sint32 mSelectedLetter;
    };
}
