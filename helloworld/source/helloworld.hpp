#pragma once
#include <service/boss_zay.hpp>
#include <service/boss_zaywidget.hpp>

class helloworldData : public ZayObject
{
public:
    helloworldData();
    ~helloworldData();

public:
    // 업데이트요청
    uint64 mUpdateMsec;
    // 제이에디터 위젯
    ZayWidget mWidget;
};
