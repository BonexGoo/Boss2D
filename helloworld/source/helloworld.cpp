#include <boss.hpp>
#include "helloworld.hpp"

#include <resource.hpp>

ZAY_DECLARE_VIEW_CLASS("helloworldView", helloworldData)

ZAY_VIEW_API OnCommand(CommandType type, chars topic, id_share in, id_cloned_share* out)
{
    if(type == CT_Tick)
    {
        // 제이에디터에 틱전달
        if(m->mWidget.TickOnce())
            m->invalidate();
        if(Platform::Utility::CurrentTimeMsec() <= m->mUpdateMsec)
            m->invalidate(2);
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
            if(panel.ninepatch(R("button_img")) == sagolresult_included)
            {
                ZAY_SAGOL_SCISSOR(panel)
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
    mUpdateMsec = 0;

    // 제이위젯 리소스로드
    // 앱실행시 제이에디터를 켜면 실시간연동 GUI작업이 가능
    // 제이에디터에서 제이박스식으로 GUI를 작업하며 F5를 누르세요
    mWidget.Init("helloworld", nullptr, [](chars name)->const Image* {return &((const Image&) R(name));})
        .AddGlue("update", ZAY_DECLARE_GLUE(params, this) // 업데이트를 위한 글루함수를 추가
        {
            if(params.ParamCount() == 1)
            {
                auto Msec = sint32(params.Param(0).ToFloat() * 1000);
                mUpdateMsec = Platform::Utility::CurrentTimeMsec() + Msec;
            }
        });
    mWidget.Reload("widget/helloworld.json");

    // 제이위젯에 문서를 전달
    // 제이에디터에서도 표현됩니다
    ZayWidgetDOM::Add("message", "\"Hello World!\"");
    ZayWidgetDOM::AddFlush();
}

helloworldData::~helloworldData()
{
}
