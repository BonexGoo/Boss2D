# Boss2D

### Hello World

![boss512](https://user-images.githubusercontent.com/1957880/83536663-7b10d200-a52e-11ea-9a93-883f38f782f0.png)

## Introduction

Boss2D is a GUI framework for app development created using QT.

## How to start

```tsx
#pragma once
#include <service/boss_zay.hpp>

class helloworldData : public ZayObject
{
public:
    helloworldData();
    ~helloworldData();
};
```

```tsx
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
```

## How to run

- Install QT 5.15.2 : https://www.qt.io/download-qt-installer-oss
- DoubleClick : helloworld/project/helloworld.pro

### Features

- helloworld : basic project
- zaycode : code review software
- zaypro : ui editor software
