#pragma once
#include <service/boss_zay.hpp>
#include <service/boss_zaywidget.hpp>
#include <service/boss_whisper.hpp>
#include "model/zaymodel.hpp"

class MessageCache
{
public:
    String mType;
    String mJson;
    Map<String> mSaveValues;
    String mErrorCode;
};
typedef Map<MessageCache> MessageCaches;

class zaytoyData : public ZayObject
{
public:
    zaytoyData();
    ~zaytoyData();

// 윈도우
public:
    void RenderWindowSystem(ZayPanel& panel);
    bool IsFullScreen();
    void SetFullScreen();
    void SetNormalWindow();
    void SetCursor(CursorRole role);
    bool CheckDOM(uint64 msec);
    void ReloadDOM();
    void InitWidget(ZayWidget& widget, chars name);
    void UpdateWhisper(rect128 window = Platform::GetWindowRect());

public:
    uint64 mUpdateMsec {0};
    uint64 mLastModifyMsec {0};
    size64 mWindowSize {0, 0};
    bool mIsFullScreen {false};
    rect128 mSavedNormalRect {0, 0, 0, 0};
    CursorRole mNowCursor {CR_Arrow};
    ZayWidget* mWidgetMain {nullptr};
    Whisper mWhisper;

// 제이모델
public:
    void ValidZayModelRoot();
    void UpdateZayModelOrderDOM();
    void UpdateZayModelMatrix(ZayModel& box);
    void AnimateZayModelMatrix(ZayModel& box);
    MessageCache& NewMessageCache(String& msgid, chars type, chars mission = nullptr);
    sint32 MessageCacheCount() const;
    String MessageCacheInfo(sint32 i, String& type, String& error) const;
    void MessageCacheFlush(MessageCache& msg, const Context& json) const;
    void SendJson(const String& json) const;
    void Send_CreateObject(chars objectid, chars filepath);
    void Send_SetChildStatus(chars objectid, const Strings& children, chars status);
    void Send_SetChildMatrix(chars objectid, const Strings& children, const ZMMatrix& matrix);
    void Send_CreateToolChild(chars child, chars model);
    void Send_RemoveToolChild(chars child);
    bool RenderUC_ZayModelIconButton(ZayPanel& panel, chars type, chars title, chars model, chars atlas, sint32 elementid, String uiname);
    bool RenderUC_ZayModelBg(ZayPanel& panel, chars classid);
    bool RenderUC_ZayModelWire(ZayPanel& panel, sint32 x1, sint32 y1, sint32 x2, sint32 y2, float thick);
    bool RenderUC_ZayModelSpin(ZayPanel& panel, chars classid, chars group, chars key);

public:
    Tree<ZayModel> mZayModels; // [ZMClassID]
    Strings mZayModelOrder;
    MessageCaches mZayModelMsgCaches;
};
