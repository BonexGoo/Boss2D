#pragma once
#include <service/boss_zay.hpp>

class helloworldData : public ZayObject
{
public:
    helloworldData();
    ~helloworldData();

public:
    String mMessage;
};
