#pragma once
#include <service/boss_zay.hpp>
#include <service/boss_zaywidget.hpp>

class helloworldWidgetData : public ZayObject
{
public:
    helloworldWidgetData();
    ~helloworldWidgetData();

public:
    // 업데이트요청
    uint64 mUpdateMsec;
    // 제이에디터 위젯
    ZayWidget mWidget;
};
