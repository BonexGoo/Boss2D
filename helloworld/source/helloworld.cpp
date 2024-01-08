#include <boss.hpp>
#include "helloworld.hpp"

#include <resource.hpp>

ZAY_DECLARE_VIEW_CLASS("helloworldView", helloworldData)

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
    ZAY_RGB(panel, 255, 255, 255)
        panel.fill();

    ZAY_FONT(panel, 3.0)
    ZAY_RGB(panel, 0, 0, 0)
        panel.text("Hello World", UIFA_CenterMiddle);
}

helloworldData::helloworldData()
{
}

helloworldData::~helloworldData()
{
}
