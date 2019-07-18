#include <boss.hpp>
#include "helloworld.hpp"

#include <resource.hpp>

ZAY_DECLARE_VIEW_CLASS("helloworldView", helloworldData)

ZAY_VIEW_API OnCommand(CommandType type, chars topic, id_share in, id_cloned_share* out)
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
    ZAY_RGB(panel, 255, 255, 255)
        panel.fill();

    ZAY_RGBA(panel, 160, 128, 128, 16)
    {
        panel.icon(R("mark_img"), UIA_CenterMiddle);

        ZAY_RGBA(panel, 64, 64, 64, 255)
        ZAY_FONT(panel, 1.5, "arial black")
        ZAY_LTRB(panel, 0, 0, panel.w(), panel.h() / 3)
            panel.text(m->mMessage, UIFA_CenterMiddle, UIFE_Right);
    }

    ZAY_LTRB(panel, 0, panel.h() * 2 / 3, panel.w(), panel.h())
    ZAY_INNER_UI(panel, 30, "exit",
        ZAY_GESTURE_T(type)
        {
            if(type == GT_InReleased)
                Platform::Utility::ExitProgram();
        })
    {
        bool IsFocused = !!(panel.state("exit") & PS_Focused);
        bool IsPressed = !!(panel.state("exit") & PS_Pressed);

        ZAY_RGBA_IF(panel, 220, 220, 128, 128, IsFocused)
        {
            if(panel.ninepatch(R("button_img")) == haschild_ok)
            {
                ZAY_CHILD_SCISSOR(panel)
                ZAY_FONT(panel, 3.0)
                ZAY_RGBA(panel, 0, 0, 0, 64)
                ZAY_RGBA_IF(panel, 255, 0, 0, 64, IsPressed)
                    panel.text("Press to exit!", UIFA_CenterMiddle);
            }
        }
    }
}

helloworldData::helloworldData()
{
    mMessage = "Hello World!";
}

helloworldData::~helloworldData()
{
}
