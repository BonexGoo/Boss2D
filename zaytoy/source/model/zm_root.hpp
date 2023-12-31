#pragma once
#include <service/boss_zay.hpp>
#include "zaymodel.hpp"

class ABRoot : public ZayModel
{
public:
    ABRoot(ZMClassID id);
    ~ABRoot() override;

public:
    chars Type() const override;
};
