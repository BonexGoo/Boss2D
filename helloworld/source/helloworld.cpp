#include <boss.hpp>
#include "helloworld.hpp"

#include <resource.hpp>

ZAY_DECLARE_VIEW_CLASS("helloworldView", helloworldData)

ZAY_VIEW_API OnCommand(CommandType type, chars topic, id_share in, id_cloned_share* out)
{
    if(type == CT_Tick)
    {
        // 제이위젯에 틱전달
        if(m->mWidget.TickOnce())
            m->invalidate();
    }
}

ZAY_VIEW_API OnNotify(NotifyType type, chars topic, id_share in, id_cloned_share* out)
{
}

ZAY_VIEW_API OnGesture(GestureType type, sint32 x, sint32 y)
{
}

ZAY_VIEW_API OnRender(ZayPanel& panel)
{
    // 제이에디터식 랜더링
    m->mWidget.Render(panel);

    // 소스코드식 랜더링
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
    // 제이위젯 리소스로드
    // 앱실행시 제이에디터를 켜면 실시간연동 GUI작업이 가능
    // 제이에디터에서 제이박스식으로 GUI를 작업하며 F5를 누르세요
    mWidget.Init("helloworld", nullptr, [](chars name)->const Image* {return &((const Image&) R(name));});
    mWidget.Reload("widget/helloworld.json");

    // 제이위젯에 문서를 전달
    // 제이에디터에서도 표현됩니다
    ZayWidgetDOM::Add("message", "\"Hello World!\"");
    ZayWidgetDOM::AddFlush();
}

helloworldData::~helloworldData()
{
}
