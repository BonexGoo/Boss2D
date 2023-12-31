#include <boss.hpp>
#include "zm_root.hpp"

ABRoot::ABRoot(ZMClassID id) : ZayModel(id)
{
    mText = "DECK";
}

ABRoot::~ABRoot()
{
}

chars ABRoot::Type() const
{
    return "root";
}
