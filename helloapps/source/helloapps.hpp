#pragma once
#include <service/boss_zay.hpp>
#include <service/boss_zaywidget.hpp>

class ReservedValue
{
public:
    String mValue;
    uint64 mFlushMsec {0};
};
typedef Array<ReservedValue> ReservedValues;

class HtmlView
{
public:
    String mName;
    sint32 mLoading {-1};
    h_web mHtml;
    sint32 mWidth {0};
    sint32 mHeight {0};
    uint64 mLiveMsec {0};
};
typedef Map<HtmlView> HtmlViewes;

class helloappsData : public ZayObject
{
public:
    helloappsData();
    ~helloappsData();

public:
    void SetCursor(CursorRole role);
    sint32 MoveNcLeft(const rect128& rect, sint32 addx);
    sint32 MoveNcTop(const rect128& rect, sint32 addy);
    sint32 MoveNcRight(const rect128& rect, sint32 addx);
    sint32 MoveNcBottom(const rect128& rect, sint32 addy);
    void RenderWindowSystem(ZayPanel& panel);
    void RenderWindowOutline(ZayPanel& panel);
    bool RenderHtmlView(ZayPanel& panel, chars viewid, chars htmlname);
    bool RenderLogView(ZayPanel& panel, sint32 maxcount);
    bool RenderSlider(ZayPanel& panel, chars domname, sint32 maxvalue, bool flip);
    bool RenderLoader(ZayPanel& panel, float percent, sint32 gap);
    bool IsFullScreen();
    void SetFullScreen();
    void SetNormalWindow();
    bool CheckDOM(uint64 msec);
    void ReloadDOM();
    void ReloadDOMCore(const Context& data);
    void SetTitle(chars name);
    sint32 FindWidget(chars name) const;
    void EnterWidget(sint32 index);
    void InitWidget(ZayWidget& widget, chars name);
    void ChangeWidget(sint32 index);
    void ExitWidget();
    void CallScript(chars filename, const Strings& args) const;
    void ResetDom(sint32 excluded_peerid);
    void UpdateDom(chars key, chars value, sint32 delay_msec, sint32 excluded_peerid);
    void UpdateDomCore(chars key, chars value, sint32 excluded_peerid);
    void UpdateDomFlush(chars key, chars value);
    void RestoreDom(chars key, sint32 excluded_peerid);
    void RestoreDomFlush(chars key);
    void CancelReserve(sint32 excluded_peerid);
    void CancelReserveFlush();
    void PlaySound(chars filename, chars speaker, sint32 delay_msec, sint32 excluded_peerid);
    void PlaySoundCore(chars filename, chars speaker, sint32 excluded_peerid);
    void PlaySoundFlush(chars filename, chars speaker);
    void StopSound(chars speaker, sint32 excluded_peerid);
    void StopSoundFlush(chars speaker);
    void SendSerial(chars device, chars packet, sint32 delay_msec, sint32 excluded_peerid);
    void SendSerialCore(chars device, chars packet, sint32 excluded_peerid);
    void SendSerialFlush(chars device, chars packet);
    void RecvSerial(chars device, chars packet, sint32 excluded_peerid);
    void RecvSerialFlush(chars device, chars packet);
    void SendView(chars viewid, chars command, sint32 excluded_peerid);
    void SendViewFlush(chars viewid, chars command);
    void CallGate(chars gatename, sint32 excluded_peerid);
    void CallGateFlush(chars gatename);
    void ExitAll();

public: // 서버
    void TryServerRecvOnce();
    void OnServer_ResetDom(sint32 peerid);
    void OnServer_UpdateDom(const Context& json, sint32 peerid);
    void OnServer_RestoreDom(const Context& json, sint32 peerid);
    void OnServer_CancelReserve(const Context& json, sint32 peerid);
    void OnServer_PlaySound(const Context& json, sint32 peerid);
    void OnServer_StopSound(const Context& json, sint32 peerid);
    void OnServer_SendSerial(const Context& json, sint32 peerid);
    void OnServer_RecvSerial(const Context& json, sint32 peerid);
    void OnServer_SendView(const Context& json, sint32 peerid);
    void OnServer_CallGate(const Context& json, sint32 peerid);
    void OnServer_ExitAll(const Context& json);
    void ServerSend(const Context& json, sint32 excluded_peerid);

public: // 클라이언트
    void TryClientRecvOnce();
    void OnClient_ResetedDom();
    void OnClient_UpdatedDom(const Context& json);
    void OnClient_RestoredDom(const Context& json);
    void OnClient_CanceledReserve(const Context& json);
    void OnClient_PlayedSound(const Context& json);
    void OnClient_StoppedSound(const Context& json);
    void OnClient_SendedSerial(const Context& json);
    void OnClient_RecvedSerial(const Context& json);
    void OnClient_SendedView(const Context& json);
    void OnClient_CalledGate(const Context& json);
    void OnClient_ExitedAll(const Context& json);
    void ClientSend(const Context& json);

public: // 윈도우
    static const sint32 mMinWindowWidth = 400;
    static const sint32 mMinWindowHeight = 400;
    CursorRole mNowCursor {CR_Arrow};
    bool mNcLeftBorder {false};
    bool mNcTopBorder {false};
    bool mNcRightBorder {false};
    bool mNcBottomBorder {false};
    bool mIsFullScreen {false};
    bool mIsWindowMoving {false};
    rect128 mSavedNormalRect {0, 0, 0, 0};
    String mDirectlyWidget;
    Strings mDebugLogs;

public: // 위젯
    uint64 mUpdateMsec {0};
    uint64 mLastModifyTime {0};
    ZayWidget* mWidgetMain {nullptr};
    ZayWidget* mWidgetSub {nullptr};
    rect128 mWidgetSubRect {0, 0, 0, 0};
    Map<ReservedValues> mReservedValues; // [Key][N] Value/FlushMsec
    Map<ReservedValues> mReservedSounds; // [FileName][N] Speaker/FlushMsec
    Map<ReservedValues> mReservedSerials; // [Packet][N] Device/FlushMsec
    Map<Strings> mStackedValues;
    HtmlViewes mHtmlViewes;
    sint32 mReservedEnterWidget {-1};
    sint32 mReservedChangeWidget {-1};
    id_sound mSounds[4] {nullptr, nullptr, nullptr, nullptr};
    sint32 mSoundFocus {0};

public: // 녹화
    sint32 mRecFrame {-1};
    String mRecFolder;
    rect128 mRecRect {0, 0, 0, 0};
    sint32 mEncCount {0};
    sint32 mEncFrame {-1};
    uint64 mEncTimeMs {0};
    id_flash mEncFLV {nullptr};
    id_h264 mEncH264 {nullptr};

public: // 서버통신
    id_server mServer {nullptr};
    sint32s mServerPeers;
    bool mWasExitAll {false};

public: // 클라이언트통신
    id_socket mClient {nullptr};
    bool mClientConnected {false};
    uint08s mClientQueue;
    Strings mClientSendTasks;

public: // 시리얼통신
    id_serial mSerialPass1 {nullptr};
    id_serial mSerialPass2 {nullptr};
    bool mUseSizeField {false};
};
