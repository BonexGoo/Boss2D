#pragma once
#include <platform/boss_platform.hpp>
#include <element/boss_clip.hpp>
#include <element/boss_color.hpp>
#include <element/boss_font.hpp>
#include <element/boss_image.hpp>
#include <element/boss_point.hpp>
#include <element/boss_rect.hpp>
#include <element/boss_size.hpp>
#include <element/boss_solver.hpp>
#include <element/boss_tween.hpp>
#include <element/boss_vector.hpp>
#include <functional>

// 옵션스택 - 영역
#define ZAY_LTRB(PANEL, L, T, R, B) \
    if(auto _ = (PANEL)._push_clip(L, T, R, B, false))
#define ZAY_LTRB_UI(PANEL, L, T, R, B, ...) \
    if(auto _ = (PANEL)._push_clip_ui(L, T, R, B, false, __VA_ARGS__))
#define ZAY_LTRB_SCISSOR(PANEL, L, T, R, B) \
    if(auto _ = (PANEL)._push_clip(L, T, R, B, true))
#define ZAY_LTRB_UI_SCISSOR(PANEL, L, T, R, B, ...) \
    if(auto _ = (PANEL)._push_clip_ui(L, T, R, B, true, __VA_ARGS__))

#define ZAY_XYWH(PANEL, X, Y, W, H) \
    if(auto _ = (PANEL)._push_clip(X, Y, (X) + (W), (Y) + (H), false))
#define ZAY_XYWH_UI(PANEL, X, Y, W, H, ...) \
    if(auto _ = (PANEL)._push_clip_ui(X, Y, (X) + (W), (Y) + (H), false, __VA_ARGS__))
#define ZAY_XYWH_SCISSOR(PANEL, X, Y, W, H) \
    if(auto _ = (PANEL)._push_clip(X, Y, (X) + (W), (Y) + (H), true))
#define ZAY_XYWH_UI_SCISSOR(PANEL, X, Y, W, H, ...) \
    if(auto _ = (PANEL)._push_clip_ui(X, Y, (X) + (W), (Y) + (H), true, __VA_ARGS__))

#define ZAY_XYRR(PANEL, X, Y, RX, RY) \
    if(auto _ = (PANEL)._push_clip((X) - (RX), (Y) - (RY), (X) + (RX), (Y) + (RY), false))
#define ZAY_XYRR_UI(PANEL, X, Y, RX, RY, ...) \
    if(auto _ = (PANEL)._push_clip_ui((X) - (RX), (Y) - (RY), (X) + (RX), (Y) + (RY), false, __VA_ARGS__))
#define ZAY_XYRR_SCISSOR(PANEL, X, Y, RX, RY) \
    if(auto _ = (PANEL)._push_clip((X) - (RX), (Y) - (RY), (X) + (RX), (Y) + (RY), true))
#define ZAY_XYRR_UI_SCISSOR(PANEL, X, Y, RX, RY, ...) \
    if(auto _ = (PANEL)._push_clip_ui((X) - (RX), (Y) - (RY), (X) + (RX), (Y) + (RY), true, __VA_ARGS__))

#define ZAY_RECT(PANEL, R) \
    if(auto _ = (PANEL)._push_clip_by_rect(R, false))
#define ZAY_RECT_UI(PANEL, R, ...) \
    if(auto _ = (PANEL)._push_clip_ui_by_rect(R, false, __VA_ARGS__))
#define ZAY_RECT_SCISSOR(PANEL, R) \
    if(auto _ = (PANEL)._push_clip_by_rect(R, true))
#define ZAY_RECT_UI_SCISSOR(PANEL, R, ...) \
    if(auto _ = (PANEL)._push_clip_ui_by_rect(R, true, __VA_ARGS__))

#define ZAY_INNER(PANEL, V) \
    if(auto _ = (PANEL)._push_clip(V, V, (PANEL).w() - (V), (PANEL).h() - (V), false))
#define ZAY_INNER_UI(PANEL, V, ...) \
    if(auto _ = (PANEL)._push_clip_ui(V, V, (PANEL).w() - (V), (PANEL).h() - (V), false, __VA_ARGS__))
#define ZAY_INNER_SCISSOR(PANEL, V) \
    if(auto _ = (PANEL)._push_clip(V, V, (PANEL).w() - (V), (PANEL).h() - (V), true))
#define ZAY_INNER_UI_SCISSOR(PANEL, V, ...) \
    if(auto _ = (PANEL)._push_clip_ui(V, V, (PANEL).w() - (V), (PANEL).h() - (V), true, __VA_ARGS__))

#define ZAY_MOVE(PANEL, X, Y) \
    if(auto _ = (PANEL)._push_clip(X, Y, (PANEL).w() + (X), (PANEL).h() + (Y), false))
#define ZAY_MOVE_UI(PANEL, X, Y, ...) \
    if(auto _ = (PANEL)._push_clip_ui(X, Y, (PANEL).w() + (X), (PANEL).h() + (Y), false, __VA_ARGS__))
#define ZAY_MOVE_SCISSOR(PANEL, X, Y) \
    if(auto _ = (PANEL)._push_clip(X, Y, (PANEL).w() + (X), (PANEL).h() + (Y), true))
#define ZAY_MOVE_UI_SCISSOR(PANEL, X, Y, ...) \
    if(auto _ = (PANEL)._push_clip_ui(X, Y, (PANEL).w() + (X), (PANEL).h() + (Y), true, __VA_ARGS__))
#define ZAY_MOVE_IF(PANEL, X, Y, QUERY) \
    if(auto _ = (QUERY)? (PANEL)._push_clip(X, Y, (PANEL).w() + (X), (PANEL).h() + (Y), false) : (PANEL)._push_pass())

// 옵션스택 - 인사이드
#define ZAY_BIND_INSIDE(CODE) \
    if(auto _inside_ = CODE)

#define ZAY_INSIDE(PANEL) \
    if(auto _ = (PANEL)._push_clip_by_inside(_inside_, 0, 0, 0xFFFF, 0xFFFF, false))
#define ZAY_INSIDE_UI(PANEL, ...) \
    if(auto _ = (PANEL)._push_clip_ui_by_inside(_inside_, 0, 0, 0xFFFF, 0xFFFF, false, __VA_ARGS__))
#define ZAY_INSIDE_SCISSOR(PANEL) \
    if(auto _ = (PANEL)._push_clip_by_inside(_inside_, 0, 0, 0xFFFF, 0xFFFF, true))
#define ZAY_INSIDE_UI_SCISSOR(PANEL, ...) \
    if(auto _ = (PANEL)._push_clip_ui_by_inside(_inside_, 0, 0, 0xFFFF, 0xFFFF, true, __VA_ARGS__))

#define ZAY_INSIDE_AT(PANEL, IX, IY) \
    if(auto _ = (PANEL)._push_clip_by_inside(_inside_, IX, IY, 1, 1, false))
#define ZAY_INSIDE_AT_UI(PANEL, IX, IY, ...) \
    if(auto _ = (PANEL)._push_clip_ui_by_inside(_inside_, IX, IY, 1, 1, false, __VA_ARGS__))
#define ZAY_INSIDE_AT_SCISSOR(PANEL, IX, IY) \
    if(auto _ = (PANEL)._push_clip_by_inside(_inside_, IX, IY, 1, 1, true))
#define ZAY_INSIDE_AT_UI_SCISSOR(PANEL, IX, IY, ...) \
    if(auto _ = (PANEL)._push_clip_ui_by_inside(_inside_, IX, IY, 1, 1, true, __VA_ARGS__))

#define ZAY_INSIDE_SECTION(PANEL, IX, IY, XC, YC) \
    if(auto _ = (PANEL)._push_clip_by_inside(_inside_, IX, IY, XC, YC, false))
#define ZAY_INSIDE_SECTION_UI(PANEL, IX, IY, XC, YC, ...) \
    if(auto _ = (PANEL)._push_clip_ui_by_inside(_inside_, IX, IY, XC, YC, false, __VA_ARGS__))
#define ZAY_INSIDE_SECTION_SCISSOR(PANEL, IX, IY, XC, YC) \
    if(auto _ = (PANEL)._push_clip_by_inside(_inside_, IX, IY, XC, YC, true))
#define ZAY_INSIDE_SECTION_UI_SCISSOR(PANEL, IX, IY, XC, YC, ...) \
    if(auto _ = (PANEL)._push_clip_ui_by_inside(_inside_, IX, IY, XC, YC, true, __VA_ARGS__))

// 옵션스택 - 스크롤
#define ZAY_SCROLL_UI(PANEL, CW, CH, ...) \
    if(auto _ = (PANEL)._push_scroll_ui(CW, CH, __VA_ARGS__))

// 옵션스택 - 색상
#define ZAY_RGB(PANEL, R, G, B) \
    if(auto _ = (PANEL)._push_color(R, G, B, 0xFF))
#define ZAY_RGB_IF(PANEL, R, G, B, QUERY) \
    if(auto _ = (QUERY)? (PANEL)._push_color(R, G, B, 0xFF) : (PANEL)._push_pass())
#define ZAY_RGBA(PANEL, R, G, B, A) \
    if(auto _ = (PANEL)._push_color(R, G, B, A))
#define ZAY_RGBA_IF(PANEL, R, G, B, A, QUERY) \
    if(auto _ = (QUERY)? (PANEL)._push_color(R, G, B, A) : (PANEL)._push_pass())
#define ZAY_COLOR(PANEL, ...) \
    if(auto _ = (PANEL)._push_color(Color(__VA_ARGS__)))
#define ZAY_COLOR_IF(PANEL, COLOR, QUERY) \
    if(auto _ = (QUERY)? (PANEL)._push_color(COLOR) : (PANEL)._push_pass())
#define ZAY_COLOR_CLEAR(PANEL) \
    if(auto _ = (PANEL)._push_color_clear())

// 옵션스택 - 기타
#define ZAY_MASK(PANEL, MASK) \
    if(auto _ = (PANEL)._push_mask(MASK))
#define ZAY_SHADER(PANEL, SHADER) \
    if(auto _ = (PANEL)._push_shader(SHADER))
#define ZAY_FONT(PANEL, ...) \
    if(auto _ = (PANEL)._push_sysfont(__VA_ARGS__))
#define ZAY_FREEFONT(PANEL, ...) \
    if(auto _ = (PANEL)._push_freefont(__VA_ARGS__))
#define ZAY_ZOOM(PANEL, ...) \
    if(auto _ = (PANEL)._push_zoom(__VA_ARGS__))
#define ZAY_ZOOM_CLEAR(PANEL) \
    if(auto _ = (PANEL)._push_zoom_clear())
#define ZAY_SCISSOR_CLEAR(PANEL) \
    if(auto _ = (PANEL)._push_scissor_clear())
#define ZAY_SCISSOR_CLEAR_IF(PANEL, QUERY) \
    if(auto _ = (QUERY)? (PANEL)._push_scissor_clear() : (PANEL)._push_pass())

// 확장형 옵션스택
// ZayExtend A가 파라미터가 없는 경우 : ZAY_EXTEND(A() >> panel) {...}
// ZayExtend B가 파라미터가 하나인 경우 : ZAY_EXTEND(B(1) >> panel) {...}
// ZayExtend C가 파라미터가 다양한 경우 : ZAY_EXTEND(C(1)(2.5)("abc") >> panel) {...}
// ZayExtend D가 하위옵션이 필요없는 경우 : ZAY_EXTEND(D(1)(2.5)("abc") >> panel);
#define ZAY_EXTEND(...)                        if(auto _ = (__VA_ARGS__))
#define ZAY_DECLARE_COMPONENT(PANEL, PAY, ...) [__VA_ARGS__](ZayPanel& PANEL, const ZayExtend::Payload& PAY)->ZayPanel::StackBinder
#define ZAY_DECLARE_GLUE(PAY, ...)             [__VA_ARGS__](const ZayExtend::Payload& PAY)->void

// 서브패널
#define ZAY_MAKE_SUBPANEL(PANEL, SURFACE) \
    for(ZayPanel PANEL(SURFACE, Platform::Graphics::GetSurfaceWidth(SURFACE), Platform::Graphics::GetSurfaceHeight(SURFACE)); (PANEL)._is_dirty(); (PANEL)._clear_me())
#define ZAY_MAKE_SUBPANEL_UI(PANEL, SURFACE, UIGROUP) \
    for(ZayPanel PANEL(SURFACE, Platform::Graphics::GetSurfaceWidth(SURFACE), Platform::Graphics::GetSurfaceHeight(SURFACE), UIGROUP); (PANEL)._is_dirty(); (PANEL)._clear_me())
#define ZAY_MAKE_SUBPANEL_WH(PANEL, SURFACE, W, H) \
    for(ZayPanel PANEL(SURFACE, W, H); (PANEL)._is_dirty(); (PANEL)._clear_me())
#define ZAY_MAKE_SUBPANEL_UI_WH(PANEL, SURFACE, UIGROUP, W, H) \
    for(ZayPanel PANEL(SURFACE, W, H, UIGROUP); (PANEL)._is_dirty(); (PANEL)._clear_me())

// 뷰등록
#define ZAY_VIEW_API static void
#define ZAY_DECLARE_VIEW(NAME) ZAY_DECLARE_VIEW_CLASS(NAME, ZayObject)
#define ZAY_DECLARE_VIEW_CLASS(NAME, CLASS) \
    ZAY_VIEW_API OnCommand(CommandType, id_share, id_cloned_share*); \
    ZAY_VIEW_API OnNotify(NotifyType, chars, id_share, id_cloned_share*); \
    ZAY_VIEW_API OnGesture(GestureType, sint32, sint32); \
    ZAY_VIEW_API OnRender(ZayPanel&); \
    static ZayInstance<CLASS> m; \
    static void _Lock(ZayObject* ptr) {m._lock((CLASS*) ptr);} \
    static void _Unlock() {m._unlock();} \
    static ZayObject* _Alloc() {return (ZayObject*) Buffer::Alloc<CLASS>(BOSS_DBG 1);} \
    static void _Free(ZayObject* ptr) {Buffer::Free((buffer) ptr);} \
    static autorun _ = ZayView::_makefunc(false, "" NAME, OnCommand, OnNotify, \
        OnGesture, OnRender, _Lock, _Unlock, _Alloc, _Free);
#define ZAY_DECLARE_VIEW_NATIVE(NAME, CLASS) \
    ZAY_VIEW_API OnCommand(CommandType, id_share, id_cloned_share*); \
    ZAY_VIEW_API OnNotify(NotifyType, chars, id_share, id_cloned_share*); \
    ZAY_VIEW_API OnGesture(GestureType, sint32, sint32) {BOSS_ASSERT("This function should not be called directly.", false);} \
    ZAY_VIEW_API OnRender(ZayPanel&) {BOSS_ASSERT("This function should not be called directly.", false);} \
    static ZayInstance<CLASS> m; \
    static void _Lock(ZayObject* ptr) {m._lock((CLASS*) ptr);} \
    static void _Unlock() {m._unlock();} \
    static ZayObject* _Alloc() {return (ZayObject*) new CLASS;} \
    static void _Free(ZayObject* ptr) {delete (CLASS*) ptr;} \
    static autorun _ = ZayView::_makefunc(true, "" NAME, OnCommand, OnNotify, \
        OnGesture, OnRender, _Lock, _Unlock, _Alloc, _Free);

// 제스처/랜더 람다함수
#define ZAY_GESTURE_T(TYPE, ...)                       [__VA_ARGS__](ZayObject*, chars, GestureType TYPE, sint32, sint32)->void
#define ZAY_GESTURE_TXY(TYPE, X, Y, ...)               [__VA_ARGS__](ZayObject*, chars, GestureType TYPE, sint32 X, sint32 Y)->void
#define ZAY_GESTURE_NT(NAME, TYPE, ...)                [__VA_ARGS__](ZayObject*, chars NAME, GestureType TYPE, sint32, sint32)->void
#define ZAY_GESTURE_NTXY(NAME, TYPE, X, Y, ...)        [__VA_ARGS__](ZayObject*, chars NAME, GestureType TYPE, sint32 X, sint32 Y)->void
#define ZAY_GESTURE_VNT(VIEW, NAME, TYPE, ...)         [__VA_ARGS__](ZayObject* VIEW, chars NAME, GestureType TYPE, sint32, sint32)->void
#define ZAY_GESTURE_VNTXY(VIEW, NAME, TYPE, X, Y, ...) [__VA_ARGS__](ZayObject* VIEW, chars NAME, GestureType TYPE, sint32 X, sint32 Y)->void
#define ZAY_RENDER_PN(PANEL, NAME, ...)                [__VA_ARGS__](ZayPanel& PANEL, chars NAME)->void

namespace BOSS
{
    enum CommandType {CT_Create, CT_CanQuit, CT_Destroy, CT_Activate, CT_DeviceArrival, CT_Size, CT_Touch, CT_Tick};
    enum GestureType {GT_Null,
        GT_Moving, GT_MovingIdle, GT_MovingLosed, // 마우스전용
        GT_Pressed, GT_InReleased, GT_OutReleased, GT_CancelReleased, // 터치
        GT_InDragging, GT_InDraggingIdle, GT_OutDragging, GT_OutDraggingIdle, // 드래그
        GT_Dropping, GT_DroppingIdle, GT_DroppingLosed, GT_Dropped, // 드롭
        // 마우스휠/확장(우클릭드래그)
        GT_WheelUp, GT_WheelDown, GT_WheelPressed, GT_WheelDragging, GT_WheelDraggingIdle, GT_WheelRelease,
        GT_ExtendPressed, GT_ExtendDragging, GT_ExtendDraggingIdle, GT_ExtendRelease,
        // 마우스휠/확장-피크(자식패널에게 이벤트가 전달될 경우, 최상단인 OnPanel에도 전달)
        GT_WheelUpPeeked, GT_WheelDownPeeked, GT_WheelPressedPeeked, GT_WheelDraggingPeeked, GT_WheelDraggingIdlePeeked, GT_WheelReleasePeeked,
        GT_ExtendPressedPeeked, GT_ExtendDraggingPeeked, GT_ExtendDraggingIdlePeeked, GT_ExtendReleasePeeked,
        // 드래그-피크(자식패널에게 이벤트가 전달될 경우, 최상단인 OnPanel에도 전달)
        GT_DraggingPeeked, GT_DraggingIdlePeeked, GT_DraggingIsOverPeeked,
        // 키보드
        GT_KeyPressed, GT_KeyReleased,
        // 특수
        GT_ToolTip, GT_LongPressed, GT_RepeatPressed};
    enum PanelState {PS_Null = 0x00, PS_Hovered = 0x01, PS_Focused = 0x02,
        PS_Pressed = 0x04, PS_Dragging = 0x08, PS_Dropping = 0x10, PS_Captured = 0x20};
    static PanelState operator|(PanelState a, PanelState b) {return (PanelState) (int(a) | int(b));}
    enum VisibleState {VS_Visible = 0x00,
        VS_Left = 0x01, VS_Top = 0x10, VS_Right = 0x02, VS_Bottom = 0x20,
        VS_LeftTop = 0x11, VS_RightTop = 0x12, VS_LeftBottom = 0x21, VS_RightBottom = 0x22};
    enum SynchronizeModel {SM_Null, SM_UpdateAll, // 전체 동기화
        SM_Rename, SM_Move, SM_Modify, // 자신의 에디트
        SM_AddChild, SM_InsertChild, SM_RemoveChild, // 자식의 개체관리
        SM_RenameChild, SM_MoveChild, SM_ModifyChild}; // 자식의 에디트

    /// @brief 뷰의 데이터모델
    class ZayObject
    {
        friend class ZayView;
        friend class ZayObjectData;

    public:
        typedef void (*CommandCB)(CommandType, id_share, id_cloned_share*);
        typedef void (*NotifyCB)(NotifyType, chars, id_share, id_cloned_share*);
        typedef void (*LockCB)(ZayObject*);
        typedef void (*UnlockCB)();
        typedef ZayObject* (*AllocCB)();
        typedef void (*FreeCB)(ZayObject*);

    public:
        typedef const void* (*FinderCB)(void*, chars);
        typedef void (*UpdaterCB)(void*, sint32, chars);
        typedef void (*ScrollerCB)(const size64&, const point64&);
        typedef void (*ReleaseCaptureCB)(payload olddata, payload newdata);

    public:
        ZayObject();
        virtual ~ZayObject();

    public:
        void bind(void* resource);
        virtual void initialize();
        virtual void synchronize(SynchronizeModel model, sint32 param) const;

    public:
        void invalidate(sint32 count = 1) const;
        void invalidate(chars uigroup) const;
    protected:
        static void invalidator(payload data, chars uigroup);

    public:
        ZayObject* next(chars viewclass);
        bool next(View* viewmanager);
        void exit();
        void hide();
        void show();
        void showby(sint32 x, sint32 y, sint32 w, sint32 h);
        bool valid(chars uiname = nullptr) const;
        const rect128& rect(chars uiname = nullptr) const;
        const float zoom(chars uiname = nullptr) const;
        // 스크롤
        Point scrollpos(chars uiname) const;
        void setscroller(chars uiname, ScrollerCB cb);
        bool isScrollSensing(chars uiname) const; // 스크롤이 벽에 의해 튕겨지고 있음
        bool isScrollTouched(chars uiname) const; // 스크롤이 사용자에 의해 터치됨
        void clearScrollTouch(chars uiname); // 터치여부를 초기화
        void moveScroll(chars uiname, float ox, float oy, float x, float y, float sec, bool touch);
        void resetScroll(chars uiname, float x, float y, bool touch);
        void stopScroll(chars uiname, bool touch);
        // 리사이징
        void resizeForced(sint32 w = -1, sint32 h = -1);
        bool getResizingValue(sint32& w, sint32& h);
        // 캡쳐
        void setCapture(chars uiname, ReleaseCaptureCB cb_once = nullptr, payload data = nullptr);
        void clearCapture();
        void eraseCapture(payload condition = nullptr);

    public:
        inline h_view view() const
        {return m_view;}
        inline uint32 frame() const
        {return m_frame_counter;}
        inline FrameUpdater* updater()
        {return &m_frame_updater;}

    private:
        h_view m_view;
        void* m_resource;
        uint32 m_frame_counter;
        FrameUpdater m_frame_updater;
        FinderCB m_finder_cb;
        void* m_finder_data;
        UpdaterCB m_updater_cb;
        void* m_updater_data;
        sint32 m_resizing_width;
        sint32 m_resizing_height;
    };

    /// @brief 뷰패널
    class ZayPanel
    {
    public:
        typedef void (*GestureCB)(GestureType, sint32, sint32);
        typedef std::function<void(ZayObject*, chars, GestureType, sint32, sint32)> SubGestureCB;
        typedef void (*RenderCB)(ZayPanel&);
        typedef std::function<void(ZayPanel&, chars)> SubRenderCB;

    public:
        ZayPanel(FrameUpdater* updater, float width, float height, const buffer touch);
        ZayPanel(id_surface surface, float width, float height, chars uigroup = nullptr);
        ~ZayPanel();

    public:
        class InsideBinder
        {
            BOSS_DECLARE_NONCOPYABLE_INITIALIZED_CLASS(InsideBinder, mRefImage(rhs.mRefImage), mGuideRect(rhs.mGuideRect))
            friend class ZayPanel;
        private:
            InsideBinder(rect128 rect) : mRefImage((const Image*) 1), mGuideRect(rect) {}
            InsideBinder(const Image* image, rect128 rect = {0, 0, 0, 0}) : mRefImage(image), mGuideRect(rect) {}
        public:
            ~InsideBinder() {}
        public:
            InsideBinder(InsideBinder&& rhs) : mRefImage(ToReference(rhs.mRefImage)), mGuideRect(ToReference(rhs.mGuideRect)) {}
            InsideBinder& operator=(InsideBinder&& rhs)
            {
                mRefImage = ToReference(rhs.mRefImage);
                mGuideRect = ToReference(rhs.mGuideRect);
                return *this;
            }
            operator bool() const {return (mRefImage != nullptr);}
        public:
            Rect flush(sint32 ix, sint32 iy, sint32 xcount, sint32 ycount) const
            {
                if(mRefImage == (const Image*) 1)
                    return mGuideRect;
                return mRefImage->CalcChildRect(mGuideRect, ix, iy, xcount, ycount);
            }
        private:
            const Image* mRefImage;
            rect128 mGuideRect;
        };

    public:
        void erase(sint32 round = 0) const;
        void fill() const;
        void rect(float thick) const;
        void line(const Point& begin, const Point& end, float thick) const;
        void circle() const;
        void circleline(float thick) const;
        void bezier(const Vector& begin, const Vector& end, float thick) const;
        void polygon(Points p) const;
        void polyline(Points p, float thick, bool ring = false) const;
        void polybezier(Points p, float thick, bool showfirst, bool showlast) const;
        void ringbezier(Points p, float thick, float curve = 0.2) const;
        void fillbezier(Points p, float curve = 0.2) const;
        void polyimage(const Point (&ps)[3], const Image& image, Image::Build build, const Point (&ips)[3]) const;
        bool polyimageNative(const Point (&ps)[3], const Image& image, Image::Build build, const Point (&ips)[3], const Color (&colors)[3]) const;
        InsideBinder icon(const Image& image, UIAlign align, float degree = 0.0f, bool visible = true);
        InsideBinder icon(float x, float y, const Image& image, UIAlign align, float degree = 0.0f, bool visible = true); // 중점식
        InsideBinder iconNative(id_image_read image, UIAlign align, float degree = 0.0f);
        InsideBinder iconNative(float x, float y, id_image_read image, UIAlign align, float degree = 0.0f); // 중점식
        InsideBinder stretch(const Image& image, Image::Build build, UIStretchForm form = UISF_Strong, bool visible = true);
        InsideBinder stretchNative(id_image_read image, UIStretchForm form = UISF_Strong) const;
        InsideBinder ninepatch(const Image& image, bool visible = true);
        void pattern(const Image& image, UIAlign align, bool reversed_xorder = false, bool reversed_yorder = false) const;
        bool text(chars string, UIFontAlign align = UIFA_CenterMiddle, UIFontElide elide = UIFE_None) const;
        bool text(chars string, sint32 count, UIFontAlign align = UIFA_CenterMiddle, UIFontElide elide = UIFE_None) const;
        void text(float x, float y, chars string, UIFontAlign align = UIFA_CenterMiddle) const; // 중점식
        void text(float x, float y, chars string, sint32 count, UIFontAlign align = UIFA_CenterMiddle) const; // 중점식
        bool textbox(chars string, sint32 linegap, UIAlign align) const;
        void subpanel(id_surface surface = nullptr, chars uigroup = nullptr) const;
        PanelState state(chars uiname = nullptr) const;
        Point toview(float x, float y) const;
        void test(UITestOrder order);
        void repaint(sint32 count = 1);
        VisibleState visible() const;
        uint32 fbo() const;

    public:
        inline const Rect scissor() const
        {return m_stack_scissor[-1];}
        inline const Color color() const
        {return m_stack_color[-1];}
        inline const ZoomState zoom() const
        {return m_stack_zoom[-1];}
        inline const float w() const
        {return m_clipped_width;}
        inline const float h() const
        {return m_clipped_height;}
        inline const float screen_w() const
        {return m_stack_clip[0].Width();}
        inline const float screen_h() const
        {return m_stack_clip[0].Height();}

    public:
        enum StackType {ST_Null, ST_Pass, ST_Clip, ST_Color, ST_Mask, ST_Shader, ST_Font, ST_Zoom};
        class StackBinder
        {
            BOSS_DECLARE_NONCOPYABLE_INITIALIZED_CLASS(StackBinder, mPanel(rhs.mPanel))
            friend class ZayPanel;
        private:
            StackBinder(ZayPanel* panel, StackType type = ST_Null) : mPanel(panel) {mType = type;}
        public:
            ~StackBinder()
            {
                switch(mType)
                {
                case ST_Clip: mPanel->_pop_clip(); break;
                case ST_Color: mPanel->_pop_color(); break;
                case ST_Mask: mPanel->_pop_mask(); break;
                case ST_Shader: mPanel->_pop_shader(); break;
                case ST_Font: mPanel->_pop_font(); break;
                case ST_Zoom: mPanel->_pop_zoom(); break;
                }
            }
        public:
            StackBinder(StackBinder&& rhs) : mPanel(rhs.mPanel) {mType = rhs.mType; rhs.mType = ST_Null;}
            StackBinder& operator=(StackBinder&& rhs) {mType = rhs.mType; rhs.mType = ST_Null; return *this;}
            operator bool() const {return (mType != ST_Null);}
        private:
            ZayPanel* const mPanel;
            StackType mType;
        };

    public:
        StackBinder _push_clip(float l, float t, float r, float b, bool doScissor);
        StackBinder _push_clip_ui(float l, float t, float r, float b, bool doScissor, chars uiname, SubGestureCB cb = nullptr, bool hoverpass = true);
        StackBinder _push_clip_by_rect(const Rect& r, bool doScissor);
        StackBinder _push_clip_ui_by_rect(const Rect& r, bool doScissor, chars uiname, SubGestureCB cb = nullptr, bool hoverpass = true);
        StackBinder _push_clip_by_inside(const InsideBinder& inside, sint32 ix, sint32 iy, sint32 xcount, sint32 ycount, bool doScissor);
        StackBinder _push_clip_ui_by_inside(const InsideBinder& inside, sint32 ix, sint32 iy, sint32 xcount, sint32 ycount, bool doScissor, chars uiname, SubGestureCB cb = nullptr, bool hoverpass = true);
        StackBinder _push_scroll_ui(float contentw, float contenth, chars uiname, SubGestureCB cb, point64 sensitive, sint32 senseborder = 0, bool loop = false, float loopw = 0, float looph = 0);
        StackBinder _push_color(sint32 r, sint32 g, sint32 b, sint32 a);
        StackBinder _push_color(const Color& color);
        StackBinder _push_color_clear();
        StackBinder _push_mask(MaskRole role);
        StackBinder _push_shader(ShaderRole role);
        StackBinder _push_sysfont(float size, chars name = nullptr, float space = 1.0);
        StackBinder _push_freefont(sint32 height, chars nickname = nullptr);
        StackBinder _push_zoom(float zoom, OrientationRole orientation = OR_Normal);
        StackBinder _push_zoom_clear();
        StackBinder _push_scissor_clear();
        StackBinder _push_pass();

    private:
        void _pop_clip();
        void _pop_color();
        void _pop_mask();
        void _pop_shader();
        void _pop_font();
        void _pop_zoom();
        void _add_ui(chars uiname, SubGestureCB cb, point64 scrollsense, bool hoverpass);

    private:
        bool _push_scissor_intersect(double l, double t, double r, double b);
        bool _push_scissor_copy(double l, double t, double r, double b);
        void _pop_scissor();

    public:
        inline bool _is_dirty() const
        {return m_dirty;}
        inline void _clear_me()
        {m_dirty = false;}

    protected:
        bool m_dirty;
        FrameUpdater* const m_updater;
        const float m_width;
        const float m_height;
        id_surface m_ref_surface;
        void* m_ref_touch;
        void* m_ref_touch_collector;
        Clips m_stack_clip;
        Rects m_stack_scissor;
        Colors m_stack_color;
        Array<MaskRole, datatype_pod_canmemcpy> m_stack_mask;
        Array<ShaderRole, datatype_pod_canmemcpy> m_stack_shader;
        Fonts m_stack_font;
        ZoomStates m_stack_zoom;

    protected:
        float m_clipped_width;
        float m_clipped_height;
        bool m_test_scissor;
    };

    /// @brief 뷰패널의 확장모델
    class ZayExtend
    {
        BOSS_DECLARE_STANDARD_CLASS(ZayExtend)
    public:
        class Payload;
        enum class ComponentType {Unknown,
            Content, ContentWithParameter, ContentWithInsider, ContentWithParamAndInsider, ContentWithParamAndSetter,
            Option, OptionWithoutParameter, Layout, CodeInput, JumpCall, JumpMethod,
            Loop, Condition, ConditionWithOperation, ConditionWithEvent};
        typedef std::function<ZayPanel::StackBinder(ZayPanel& panel, const Payload& pay)> ComponentCB;
        typedef std::function<void(const Payload& pay)> GlueCB;

    public:
        ZayExtend(ComponentType type = ComponentType::Unknown, ComponentCB ccb = nullptr, GlueCB gcb = nullptr);
        ~ZayExtend();

    public: // 랜더러
        class Renderer
        {
        public:
            virtual chars DomName(chars uiname) const {return nullptr;}
            virtual chars GetText(chars uiname) const {return nullptr;}
            virtual void SetText(chars uiname, chars text) {}
            virtual void ShowTip(chars uiname) const {}
        public:
            virtual bool HasInsider(chars uiname, chars rendername) const = 0;
            virtual bool RenderInsider(chars uiname, chars rendername, ZayPanel& panel, sint32 pv = -1) const = 0;
        };

    public: // 페이로드
        class Payload
        {
            BOSS_DECLARE_NONCOPYABLE_INITIALIZED_CLASS(Payload, mElementID(-1), mRefLValue(nullptr), mRefRenderer(nullptr))
        public:
            Payload(const ZayExtend* owner, chars uiname = nullptr, sint32 elementid = -1, Solver* lvalue = nullptr, const Renderer* renderer = nullptr);
            Payload(const ZayExtend* owner, const SolverValue& param);
            ~Payload();

        public:
            Payload& operator()(const SolverValue& value);
            Payload& operator()(sint32 value);
            Payload& operator()(sint64 value);
            Payload& operator()(SolverValue::Float value);
            Payload& operator()(SolverValue::Text value);
            ZayPanel::StackBinder operator>>(ZayPanel& panel) const;

        public:
            chars UIName() const;
            sint32 ElementID() const;
            ZayPanel::SubGestureCB MakeGesture() const;
            sint32 ParamCount() const;
            const SolverValue& Param(sint32 i) const;
            bool ParamToBool(sint32 i, bool& error) const;
            UILayout ParamToUILayout(sint32 i, bool& error) const;
            UIAlign ParamToUIAlign(sint32 i, bool& error) const;
            UIStretchForm ParamToUIStretchForm(sint32 i, bool& error) const;
            UIFontAlign ParamToUIFontAlign(sint32 i, bool& error) const;
            UIFontElide ParamToUIFontElide(sint32 i, bool& error) const;
            MaskRole ParamToMask(sint32 i, bool& error) const;
            OrientationRole ParamToOrientation(sint32 i, bool& error) const;
            bool CanReturn(SolverValue* out_lvalue = nullptr) const;
            void Return(const String& formula) const;
            const Renderer* TakeRenderer() const;

        private:
            void AddParam(const SolverValue& value);

        private:
            const ZayExtend* mOwner;
            chars mUIName;
            const sint32 mElementID;
            SolverValues mParams;
            Solver* const mRefLValue;
            const Renderer* const mRefRenderer;
        };
        const Payload operator()() const;
        Payload operator()(const SolverValue& value) const;
        Payload operator()(sint32 value) const;
        Payload operator()(sint64 value) const;
        Payload operator()(SolverValue::Float value) const;
        Payload operator()(SolverValue::Text value) const;

    public:
        bool HasComponent() const;
        bool HasContentComponent() const;
        bool HasGlue() const;
        void ResetForComponent(ComponentType type, ComponentCB cb);
        void ResetForGlue(GlueCB cb);
        Payload MakePayload(chars uiname, Solver* lvalue, sint32 elementid = -1, const Renderer* renderer = nullptr) const;

    private:
        ComponentType mComponentType;
        ComponentCB mComponentCB;
        GlueCB mGlueCB;
    };

    /// @brief 뷰의 m인스턴스
    template<typename TYPE>
    class ZayInstance
    {
    public:
        ZayInstance()
        {
            m_ref_data_last = nullptr;
            m_mutex = Mutex::Open();
        }
        ~ZayInstance()
        {
            Mutex::Close(m_mutex);
        }

    public:
        inline TYPE* operator->()
        {
            BOSS_ASSERT("현재 위치에서는 m이 사용될 수 없습니다", m_ref_data_last);
            return m_ref_data_last;
        }
        inline TYPE& operator()()
        {
            BOSS_ASSERT("현재 위치에서는 m이 사용될 수 없습니다", m_ref_data_last);
            return *m_ref_data_last;
        }
        inline void _lock(TYPE* ptr)
        {
            BOSS_ASSERT("ptr값은 nullptr가 될 수 없습니다", ptr);
            Mutex::LocalLock(m_mutex);
            m_ref_datas.AtAdding() = ptr;
            m_ref_data_last = ptr;
        }
        inline void _unlock()
        {
            m_ref_datas.SubtractionOne();
            if(0 < m_ref_datas.Count())
                m_ref_data_last = m_ref_datas[-1];
            else m_ref_data_last = nullptr;
            Mutex::LocalUnlock(m_mutex);
        }

    private:
        Array<TYPE*, datatype_pod_canmemcpy> m_ref_datas;
        TYPE* m_ref_data_last;
        id_mutex m_mutex;
    };

    /// @brief 제이뷰
    class ZayView : public View
    {
        friend class ZayObject;
        friend class ZayPanel;

    private:
        // 영역모델
        class Element
        {
        public:
            typedef void (*GestureCB)(ZayView*, const Element*, GestureType, sint32, sint32);

        public:
            Element();
            ~Element();
            Element& operator=(const Element& rhs);

        public:
            PanelState GetState(void* touch) const;
            bool IsStateChanged(void* touch) const;

        public:
            sint32 m_updateid;
            String m_name;
            rect128 m_rect;
            float m_zoom;
            GestureCB m_cb;
            ZayPanel::SubGestureCB m_subcb;
            point64 m_scrollsense; // -1은 스크롤아님, 0~N : 스크롤민감도
            bool m_hoverpass;
            sint32 m_hoverid;
            mutable bool m_peekdragging;

        private:
            mutable sint32 m_saved_updateid_for_state;
            mutable PanelState m_saved_state;
            mutable PanelState m_saved_state_old;
        };

    private:
        // 스크롤모델
        class Scroll
        {
        public:
            Scroll();
            ~Scroll();
            Scroll& operator=(const Scroll& rhs);

        public:
            void Init(Updater* updater, float x, float y);
            void Move(float x, float y, float sec);
            void Moving(float x, float y, float sec, float unitsec, float unitrate);
            void Stop();
            void Reset(float x, float y);
            void SetInfo(sint32 width, sint32 height, sint32 ix, sint32 iy);

        public:
            Tween2D* m_pos;
            size64 m_size;
            point64 m_idx;
            ZayObject::ScrollerCB m_cb;
            bool m_sense;
            bool m_usercontrol;
        };

    private:
        // 터치/스크롤 관리
        class Touch
        {
        public:
            Touch();
            ~Touch();
            Touch& operator=(const Touch& rhs);

        private:
            class Cell
            {
            public:
                enum {Size = 128};
            public:
                sint32 m_updateid;
                sint32 m_validlength;
                Array<Element*> m_elements;
            public:
                Cell() {m_updateid = -1; m_validlength = 0;}
                ~Cell() {}
                Cell& operator=(const Cell& rhs)
                {
                    m_updateid = rhs.m_updateid;
                    m_validlength = rhs.m_validlength;
                    m_elements = rhs.m_elements;
                    return *this;
                }
            };
            typedef Array<Cell> CellRow;

        public:
            void ready(sint32 width, sint32 height);
            void update(chars uiname, float l, float t, float r, float b,
                float zoom, ZayPanel::SubGestureCB cb, point64 scrollsense, bool hoverpass, bool* dirtytest = nullptr);
            const Element* background() const;
            const Element* find(chars uiname, sint32 lag) const;
            const Element& get(sint32 x, sint32 y, const Element* press, const Element*& backscroll) const;
            bool hovertest(sint32 x, sint32 y);

        public:
            void setcapture(chars uiname, ZayObject::ReleaseCaptureCB cb_once = nullptr, payload data = nullptr);
            void clearcapture();
            void erasecapture(payload condition = nullptr);
            const Element* getcapture() const;
            PanelState capturetest(chars uiname) const;

        private:
            Cell* getcell(sint32 x, sint32 y);
            const Cell* getcell_const(sint32 x, sint32 y) const;

        public:
            inline bool ishovered(sint32 hoverid) const {return (hoverid == m_hoverid);}
            inline sint32 hoverx() const {return m_hover_x;}
            inline sint32 hovery() const {return m_hover_y;}
            inline const Element* getfocus() const {return m_focus;}
            inline const Element* getpress() const {return m_press;}
            inline void setpress_xy(sint32 x, sint32 y) {m_press_x = x; m_press_y = y;}
            inline sint32 press_to_x(sint32 x) const {return Math::Abs(x - m_press_x);}
            inline sint32 press_to_y(sint32 y) const {return Math::Abs(y - m_press_y);}
            inline Scroll* getscroll(chars uiname) {return m_scrollmap.Access(uiname);}
            inline const Scroll* getscroll_const(chars uiname) const {return m_scrollmap.Access(uiname);}
            inline Scroll* getscroll_valid(chars uiname) {return &m_scrollmap(uiname);}
            inline void setlasttouch(TouchType type) {m_lasttouch = type;}
            inline TouchType getlasttouch() const {return m_lasttouch;}

        public:
            inline bool changefocus(const Element* element)
            {
                const bool Result = (m_focus != element);
                m_focus = element;
                return Result;
            }
            inline bool changepress(const Element* element)
            {
                const bool Result = (m_press != element);
                m_press = element;
                return Result;
            }
            inline const Element* changemoving(const Element* element, GestureType type)
            {
                const Element* Result = (m_moving == element)? nullptr : m_moving;
                m_moving = (type == GT_Moving)? element : nullptr;
                return Result;
            }
            inline const Element* changedropping(const Element* element, GestureType type)
            {
                const Element* Result = (m_dropping == element)? nullptr : m_dropping;
                m_dropping = (type == GT_Dropping)? element : nullptr;
                return Result;
            }

        private:
            static void GestureCB(ZayView* manager, const Element* data, GestureType type, sint32 x, sint32 y);
            static void SubGestureCB(ZayView* manager, const Element* data, GestureType type, sint32 x, sint32 y);

        private:
            sint32 m_updateid;
            sint32 m_hoverid;
            String m_captured_uiname;
            ZayObject::ReleaseCaptureCB m_captured_cb_once;
            payload m_captured_data;
            sint32 m_block_width;
            sint32 m_block_height;
            Element m_element;
            Map<Element> m_map;
            Array<CellRow> m_cell;
            const Element* m_focus;
            const Element* m_press;
            const Element* m_moving;
            const Element* m_dropping;
            sint32 m_press_x;
            sint32 m_press_y;
            sint32 m_hover_x;
            sint32 m_hover_y;
            Map<Scroll> m_scrollmap;
            TouchType m_lasttouch;
        };

    private:
        // 구동함수 관리
        class Function
        {
        public:
            Function();
            ~Function();

        public:
            static Function* GetForced(chars viewclass);

        public:
            bool m_isnative;
            String m_viewclass;
            ZayObject::CommandCB m_command;
            ZayObject::NotifyCB m_notify;
            ZayPanel::GestureCB m_gesture;
            ZayPanel::RenderCB m_render;
            ZayObject::LockCB m_lock;
            ZayObject::UnlockCB m_unlock;
            ZayObject::AllocCB m_alloc;
            ZayObject::FreeCB m_free;
        };

    protected:
        ZayView(chars viewclass = nullptr);
    public:
        ~ZayView() override;

    public:
        static View* Creator(chars viewclass);

    public:
        h_view SetView(h_view view) override;
        bool IsNative() override;
        void* GetClass() override;
        void SetCallback(UpdaterCB cb, payload data) override;
        void DirtyAllSurfaces() override;

    public:
        void OnCreate() override;
        bool OnCanQuit() override;
        void OnDestroy() override;
        void OnActivate(bool actived) override;
        void OnDeviceArrival(bool connected) override;
        void OnSize(sint32 w, sint32 h) override;
        void OnTick() override;
        void OnNotify(NotifyType type, chars topic, id_share in, id_cloned_share* out) override;
        void OnRender(sint32 width, sint32 height, float l, float t, float r, float b) override;
        void OnTouch(TouchType type, sint32 id, sint32 x, sint32 y) override;
        void OnKey(sint32 code, chars text, bool pressed) override;

    private:
        void _checklose(GestureType type, const Element* element, sint32 x, sint32 y);
        void _gesture(GestureType type, sint32 x, sint32 y);

    public:
        static autorun _makefunc(bool isnative, chars viewclass,
            ZayObject::CommandCB c, ZayObject::NotifyCB n, ZayPanel::GestureCB g, ZayPanel::RenderCB r,
            ZayObject::LockCB l, ZayObject::UnlockCB u, ZayObject::AllocCB a, ZayObject::FreeCB f);
        static Function* _accessfunc(chars viewclass, bool creatable);

    private:
        static const void* _finder(void* data, chars uiname);

    public:
        static void _compfocusing();

    private:
        const Function& m_viewfunc;
        const String m_viewclass;
        ZayObject* m_data;
        buffer m_touch;
        sint32 m_touchin; // -1:초기값, 0:뷰바깥쪽, 1:뷰안쪽
        bool m_agreed_quit;
    };
}
