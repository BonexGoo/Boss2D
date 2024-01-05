#pragma once
#include <service/boss_zay.hpp>
#include <service/boss_whisper.hpp>

#include "zayscene.hpp"

class zay3dData : public ZayObject
{
public:
    zay3dData();
    ~zay3dData();

public:
    void RenderWindowSystem(ZayPanel& panel);
    bool IsFullScreen();
    void SetFullScreen();
    void SetNormalWindow();
    void SetCursor(CursorRole role);

public:
    uint64 mUpdateMsec {0};
    uint64 mLastModifyMsec {0};
    size64 mWindowSize {0, 0};
    bool mIsFullScreen {false};
    rect128 mSavedNormalRect {0, 0, 0, 0};
    CursorRole mNowCursor {CR_Arrow};
    Whisper mWhisper;
    ZayScene mScene;
};
