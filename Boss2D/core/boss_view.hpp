#pragma once
#include <boss_map.hpp>
#include <boss_queue.hpp>
#include <boss_share.hpp>
#include <boss_string.hpp>

namespace BOSS
{
    BOSS_DECLARE_HANDLE(h_view);
    enum NotifyType {NT_Normal, NT_KeyPress, NT_KeyRelease,
        NT_FileContent, NT_SocketReceive, NT_Summit, NT_GLState,
        NT_BluetoothSearch, NT_BluetoothDevice, NT_BluetoothService, NT_BluetoothReceive,
        NT_CameraCapture, NT_AddOn};
    enum TouchType {TT_Null,
        // 일반
        TT_Moving, TT_MovingIdle, TT_Press, TT_Dragging, TT_DraggingIdle, TT_Release,
        // 마우스휠/확장(우클릭드래그)
        TT_WheelUp, TT_WheelDown, TT_WheelPress, TT_WheelDragging, TT_WheelDraggingIdle, TT_WheelRelease,
        TT_ExtendPress, TT_ExtendDragging, TT_ExtendDraggingIdle, TT_ExtendRelease,
        // 특수
        TT_ToolTip, TT_LongPress, TT_Render};

    /// @brief 뷰지원
    class View
    {
    public:
        typedef View* (*CreatorCB)(chars);
        typedef void (*UpdaterCB)(payload, sint32, chars);

    protected:
        View();
    public:
        virtual ~View();

    public:
        static View* Creator(chars viewclass);
        static void Regist(chars viewclass, h_view view);
        static void Unregist(chars viewclass, h_view view);
        static const Map<h_view>* SearchBegin(chars viewclass);
        static void SearchEnd();

    public:
        virtual h_view SetView(h_view view);
        virtual bool IsNative();
        virtual void* GetClass();
        void SendNotify(NotifyType type, chars topic, id_share in, id_cloned_share* out, bool direct);
        virtual void SetCallback(UpdaterCB cb, payload data);
        virtual void DirtyAllSurfaces();

    public:
        virtual void OnCreate();
        virtual bool OnCanQuit();
        virtual void OnDestroy();
        virtual void OnActivate(bool actived);
        virtual void OnSize(sint32 w, sint32 h);
        virtual void OnTick();
        virtual void OnNotify(NotifyType type, chars topic, id_share in, id_cloned_share* out);
        virtual void OnRender(sint32 width, sint32 height, float l, float t, float r, float b);
        virtual void OnTouch(TouchType type, sint32 id, sint32 x, sint32 y);
        virtual void OnKey(sint32 code, chars text, bool pressed);

    private:
        struct Notify
        {
            NotifyType mType;
            String mTopic;
            id_cloned_share mClonedIn;
        };
        Queue<Notify*> mNotifyQueue;
        sint32 mNotifyProcedure;
    };
}
