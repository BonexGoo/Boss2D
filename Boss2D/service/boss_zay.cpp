#include <boss.hpp>
#include "boss_zay.hpp"

ZAY_DECLARE_VIEW("_defaultview_")
ZAY_VIEW_API OnCommand(CommandType, chars, id_share, id_cloned_share*) {}
ZAY_VIEW_API OnNotify(NotifyType, chars, id_share, id_cloned_share*) {}
ZAY_VIEW_API OnGesture(GestureType, sint32, sint32) {}
ZAY_VIEW_API OnRender(ZayPanel& panel)
{ZAY_RGB(panel, 0x80, 0x80, 0x80) panel.fill();}

namespace BOSS
{
    class TouchRect
    {
    public:
        TouchRect() {BOSS_ASSERT("잘못된 시나리오입니다", false);}
        TouchRect(chars uiname, float l, float t, float r, float b, float zoom, ZayPanel::SubGestureCB cb, sint32 scrollsense, bool hoverpass)
        {
            mName = uiname;
            mL = l;
            mT = t;
            mR = r;
            mB = b;
            mZoom = zoom;
            mCB = cb;
            mScrollSence = scrollsense;
            mHoverPass = hoverpass;
        }
        ~TouchRect() {}
        TouchRect(const TouchRect& rhs) {operator=(rhs);}
        TouchRect& operator=(const TouchRect& rhs)
        {BOSS_ASSERT("잘못된 시나리오입니다", false); return *this;}

    public:
        static buffer Create(chars uiname, float l, float t, float r, float b, float zoom, ZayPanel::SubGestureCB cb, sint32 scrollsense, bool hoverpass)
        {
            buffer NewBuffer = Buffer::AllocNoConstructorOnce<TouchRect>(BOSS_DBG 1);
            BOSS_CONSTRUCTOR(NewBuffer, 0, TouchRect, uiname, l, t, r, b, zoom, cb, scrollsense, hoverpass);
            return NewBuffer;
        }

    public:
        String mName;
        float mL;
        float mT;
        float mR;
        float mB;
        float mZoom;
        ZayPanel::SubGestureCB mCB;
        sint32 mScrollSence; // -1은 스크롤아님, 0~N : 스크롤민감도
        bool mHoverPass;
    };
    typedef Object<TouchRect> TouchRectObject;
    typedef Array<TouchRectObject> TouchRectObjects;

    class TouchCollector
    {
    public:
        TouchCollector()
        {
            mWidth = 0;
            mHeight = 0;
            mRefTouch = nullptr;
            mDirty = true;
        }
        ~TouchCollector() {}

    public:
        static Map<TouchCollector>& STMAP()
        {return *BOSS_STORAGE_SYS(Map<TouchCollector>);}
        static TouchCollector* ST(chars name)
        {return &STMAP()(name);}

    public:
        float mWidth;
        float mHeight;
        void* mRefTouch;
        mutable bool mDirty;
        TouchRectObjects mTouchRects;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // ZayObject
    ////////////////////////////////////////////////////////////////////////////////
    ZayObject::ZayObject()
    {
        m_resource = nullptr;
        m_frame_counter = 0;
        m_finder = nullptr;
        m_finder_data = nullptr;
        m_updater = nullptr;
        m_updater_data = nullptr;
        m_resizing_width = -1;
        m_resizing_height = -1;
    }

    ZayObject::~ZayObject()
    {
    }

    void ZayObject::bind(void* resource)
    {
        m_resource = resource;
    }

    void ZayObject::initialize()
    {
        BOSS_ASSERT("This function should not be called directly.", false);
    }

    void ZayObject::synchronize(SynchronizeModel model, sint32 param) const
    {
        BOSS_ASSERT("This function should not be called directly.", false);
    }

    void ZayObject::invalidate(sint32 count) const
    {
        BOSS_ASSERT("Updater가 없습니다", m_updater);
        if(m_updater && 0 < count)
            m_updater(m_updater_data, count);
    }

    void ZayObject::invalidate(chars uigroup) const
    {
        if(uigroup)
            TouchCollector::ST(uigroup)->mDirty = true;
        invalidate();
    }

    void ZayObject::invalidator(payload data, chars uigroup)
    {
        ((const ZayObject*) data)->invalidate(uigroup);
    }

    ZayObject* ZayObject::next(chars viewclass)
    {
        ZayObject* Result = nullptr;
        if(m_view.get())
            Result = (ZayObject*) Platform::SetNextViewClass(m_view, viewclass);
        return Result;
    }

    bool ZayObject::next(View* viewmanager)
    {
        bool Result = false;
        if(m_view.get())
            Result = Platform::SetNextViewManager(m_view, viewmanager);
        return Result;
    }

    void ZayObject::exit()
    {
        BOSS_ASSERT("Updater가 없습니다", m_updater);
        if(m_updater)
            m_updater(m_updater_data, -1);
    }

    void ZayObject::hide()
    {
        BOSS_ASSERT("Updater가 없습니다", m_updater);
        if(m_updater)
            m_updater(m_updater_data, -2);
    }

    bool ZayObject::valid(chars uiname) const
    {
        BOSS_ASSERT("Finder가 없습니다", m_finder);
        return !!m_finder(m_finder_data, uiname);
    }

    const rect128& ZayObject::rect(chars uiname) const
    {
        BOSS_ASSERT("Finder가 없습니다", m_finder);
        if(auto CurElement = (const ZayView::Element*) m_finder(m_finder_data, uiname))
            return CurElement->m_rect;
        static const rect128 NullRect = {0, 0, 0, 0};
        return NullRect;
    }

    const float ZayObject::zoom(chars uiname) const
    {
        BOSS_ASSERT("Finder가 없습니다", m_finder);
        if(auto CurElement = (const ZayView::Element*) m_finder(m_finder_data, uiname))
            return CurElement->m_zoom;
        return 1;
    }

    const point64& ZayObject::oldxy(chars uiname) const
    {
        BOSS_ASSERT("Finder가 없습니다", m_finder);
        if(auto CurElement = (const ZayView::Element*) m_finder(m_finder_data, uiname))
            return CurElement->m_saved_xy;
        static const point64 NullPoint = {0, 0};
        return NullPoint;
    }

    Point ZayObject::scrollpos(chars uiname) const
    {
        if(auto CurTouch = (const ZayView::Touch*) ((const ZayView*) m_finder_data)->m_touch)
        if(auto CurScroll = CurTouch->getscroll_const(uiname))
            return Point(CurScroll->m_pos->x(), CurScroll->m_pos->y());
        return Point();
    }

    void ZayObject::setscroller(chars uiname, ScrollerCB cb)
    {
        if(auto CurTouch = (ZayView::Touch*) ((ZayView*) m_finder_data)->m_touch)
        if(auto CurScroll = CurTouch->getscroll(uiname))
            CurScroll->m_cb = cb;
    }

    bool ZayObject::isScrollSensing(chars uiname) const
    {
        if(auto CurTouch = (ZayView::Touch*) ((ZayView*) m_finder_data)->m_touch)
        if(auto CurScroll = CurTouch->getscroll(uiname))
            return CurScroll->m_sense;
        return false;
    }

    bool ZayObject::isScrollTouched(chars uiname) const
    {
        if(auto CurTouch = (ZayView::Touch*) ((ZayView*) m_finder_data)->m_touch)
        if(auto CurScroll = CurTouch->getscroll(uiname))
            return CurScroll->m_usercontrol;
        return false;
    }

    void ZayObject::clearScrollTouch(chars uiname)
    {
        if(auto CurTouch = (ZayView::Touch*) ((ZayView*) m_finder_data)->m_touch)
        if(auto CurScroll = CurTouch->getscroll(uiname))
            CurScroll->m_usercontrol = false;
    }

    void ZayObject::moveScroll(chars uiname, float ox, float oy, float x, float y, float sec, bool touch)
    {
        if(auto CurTouch = (ZayView::Touch*) ((ZayView*) m_finder_data)->m_touch)
        if(auto CurScroll = CurTouch->getscroll(uiname))
        {
            CurScroll->Reset(ox, oy);
            CurScroll->Moving(x, y, sec, 0.1, 0.2);
            CurScroll->m_sense = false;
            CurScroll->m_usercontrol |= touch;
        }
    }

    void ZayObject::resetScroll(chars uiname, float x, float y, bool touch)
    {
        if(auto CurTouch = (ZayView::Touch*) ((ZayView*) m_finder_data)->m_touch)
        if(auto CurScroll = CurTouch->getscroll(uiname))
        {
            CurScroll->Reset(x, y);
            CurScroll->m_sense = false;
            CurScroll->m_usercontrol |= touch;
        }
    }

    void ZayObject::stopScroll(chars uiname, bool touch)
    {
        if(auto CurTouch = (ZayView::Touch*) ((ZayView*) m_finder_data)->m_touch)
        if(auto CurScroll = CurTouch->getscroll(uiname))
        {
            CurScroll->Stop();
            CurScroll->m_sense = false;
            CurScroll->m_usercontrol |= touch;
        }
    }

    void ZayObject::resizeForced(sint32 w, sint32 h)
    {
        m_resizing_width = w;
        m_resizing_height = h;
    }

    bool ZayObject::getResizingValue(sint32& w, sint32& h)
    {
        bool Result = false;
        if(m_resizing_width != -1)
        {
            Result = true;
            w = m_resizing_width;
            m_resizing_width = -1;
        }
        if(m_resizing_height != -1)
        {
            Result = true;
            h = m_resizing_height;
            m_resizing_height = -1;
        }
        return Result;
    }

    void ZayObject::setCapture(chars uiname)
    {
        if(auto CurTouch = (ZayView::Touch*) ((ZayView*) m_finder_data)->m_touch)
            CurTouch->setcapture(uiname);
    }

    void ZayObject::clearCapture()
    {
        if(auto CurTouch = (ZayView::Touch*) ((ZayView*) m_finder_data)->m_touch)
            CurTouch->clearcapture();
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ZayPanel
    ////////////////////////////////////////////////////////////////////////////////
    ZayPanel::ZayPanel(FrameUpdater* updater, float width, float height, const buffer touch)
        : m_updater(updater), m_width(width), m_height(height)
    {
        m_dirty = false;
        m_ref_surface = nullptr;
        m_ref_touch = touch;
        m_ref_touch_collector = nullptr;
        ((ZayView::Touch*) m_ref_touch)->ready((sint32) m_width, (sint32) m_height);

        m_stack_clip.AtAdding() = Clip(0, 0, m_width, m_height, true);
        m_stack_scissor.AtAdding() = Rect(0, 0, m_width, m_height);
        m_stack_color.AtAdding() = Color(Color::ColoringDefault);
        m_stack_mask.AtAdding() = MR_Default;
        m_stack_font.AtAdding();
        m_stack_zoom.AtAdding() = 1;

        m_clipped_width = m_width;
        m_clipped_height = m_height;
        m_test_scissor = true;

        Platform::Graphics::SetScissor(0, 0, m_width, m_height);
        Platform::Graphics::SetColor(0xFF, 0xFF, 0xFF, 0xFF);
        Platform::Graphics::SetFont("Arial", 10);
        Platform::Graphics::SetZoom(1);
    }

    ZayPanel::ZayPanel(id_surface surface, float width, float height, chars uigroup)
        : m_updater(nullptr), m_width(width), m_height(height)
    {
        if(surface == nullptr) // OpenGL모드가 아니라면 nullptr이 올 수 있음
        {
            m_dirty = false; // ZAY_MAKE_SUB계열에서의 자연스런 탈출을 유발
            m_ref_surface = nullptr;
            m_ref_touch = nullptr;
            m_ref_touch_collector = nullptr;
            m_clipped_width = m_width;
            m_clipped_height = m_height;
            m_test_scissor = false;
            return;
        }

        if(uigroup)
        {
            m_dirty = TouchCollector::ST(uigroup)->mDirty;
            TouchCollector::ST(uigroup)->mDirty = false;
        }
        else m_dirty = true;

        if(m_dirty)
        {
            m_ref_surface = surface;
            m_ref_touch = (uigroup)? TouchCollector::ST(uigroup)->mRefTouch : nullptr;
            m_ref_touch_collector = (uigroup)? TouchCollector::ST(uigroup) : nullptr;
            Platform::Graphics::BindSurface(m_ref_surface);
            if(m_ref_touch_collector)
            {
                ((TouchCollector*) m_ref_touch_collector)->mWidth = m_width;
                ((TouchCollector*) m_ref_touch_collector)->mHeight = m_height;
                ((TouchCollector*) m_ref_touch_collector)->mTouchRects.SubtractionAll();
            }

            m_stack_clip.AtAdding() = Clip(0, 0, m_width, m_height, true);
            m_stack_scissor.AtAdding() = Rect(0, 0, m_width, m_height);
            m_stack_color.AtAdding() = Color(Color::ColoringDefault);
            m_stack_mask.AtAdding() = MR_Default;
            m_stack_font.AtAdding();
            m_stack_zoom.AtAdding() = 1;

            m_clipped_width = m_width;
            m_clipped_height = m_height;
            m_test_scissor = true;

            Platform::Graphics::SetScissor(0, 0, m_width, m_height);
            Platform::Graphics::SetColor(0xFF, 0xFF, 0xFF, 0xFF);
            Platform::Graphics::SetFont("Arial", m_stack_font[-1].system_size);
            Platform::Graphics::SetZoom(1);
        }
        else m_ref_surface = nullptr;
    }

    ZayPanel::~ZayPanel()
    {
        Platform::Graphics::UnbindSurface(m_ref_surface);
    }

    void ZayPanel::erase() const
    {
        const Clip& LastClip = m_stack_clip[-1];
        Platform::Graphics::EraseRect(LastClip.l, LastClip.t, LastClip.Width(), LastClip.Height());
    }

    void ZayPanel::fill() const
    {
        const Clip& LastClip = m_stack_clip[-1];
        Platform::Graphics::FillRect(LastClip.l, LastClip.t, LastClip.Width(), LastClip.Height());
    }

    void ZayPanel::rect(float thick) const
    {
        const Clip& LastClip = m_stack_clip[-1];
        Platform::Graphics::DrawRect(LastClip.l, LastClip.t, LastClip.Width(), LastClip.Height(), thick);
    }

    void ZayPanel::line(const Point& begin, const Point& end, float thick) const
    {
        const Clip& LastClip = m_stack_clip[-1];
        const Point PointAdd = Point(LastClip.l, LastClip.t);
        Platform::Graphics::DrawLine(begin + PointAdd, end + PointAdd, thick);
    }

    void ZayPanel::circle() const
    {
        const Clip& LastClip = m_stack_clip[-1];
        Platform::Graphics::FillCircle(LastClip.l, LastClip.t, LastClip.Width(), LastClip.Height());
    }

    void ZayPanel::bezier(const Vector& begin, const Vector& end, float thick) const
    {
        const Clip& LastClip = m_stack_clip[-1];
        const Point PointAdd = Point(LastClip.l, LastClip.t);
        Platform::Graphics::DrawBezier(begin + PointAdd, end + PointAdd, thick);
    }

    void ZayPanel::polygon(Points p) const
    {
        const Clip& LastClip = m_stack_clip[-1];
        Platform::Graphics::FillPolygon(LastClip.l, LastClip.t, p);
    }

    void ZayPanel::polyline(Points p, float thick) const
    {
        const Clip& LastClip = m_stack_clip[-1];
        Platform::Graphics::DrawPolyLine(LastClip.l, LastClip.t, p, thick);
    }

    void ZayPanel::polybezier(Points p, float thick, bool showfirst, bool showlast) const
    {
        const Clip& LastClip = m_stack_clip[-1];
        Platform::Graphics::DrawPolyBezier(LastClip.l, LastClip.t, p, thick, showfirst, showlast);
    }

    void ZayPanel::ringbezier(Points p, float thick, float curve) const
    {
        const Clip& LastClip = m_stack_clip[-1];
        Platform::Graphics::DrawRingBezier(LastClip.l, LastClip.t, p, thick, curve);
    }

    void ZayPanel::polyimage(const Point (&ps)[3], const Image& image, Image::Build build, const Point (&ips)[3]) const
    {
        const Color& LastColor = m_stack_color[-1];
        const Color MultiplyBlend = {
            (uint08) Math::Min(LastColor.r * 2, 255), (uint08) Math::Min(LastColor.g * 2, 255),
            (uint08) Math::Min(LastColor.b * 2, 255), (uint08) Math::Min(LastColor.a * 2, 255)};
        const Color MultiplyBlends[3] = {MultiplyBlend, MultiplyBlend, MultiplyBlend};

        if(!polyimageNative(ps, image, build, ips, MultiplyBlends))
        {
            Points Dots;
            Dots.AtAdding() = ps[0];
            Dots.AtAdding() = ps[1];
            Dots.AtAdding() = ps[2];
            Platform::Graphics::SetColor(255, 0, 0, 128);
            polygon(Dots);
            Platform::Graphics::SetColor(LastColor.r, LastColor.g, LastColor.b, LastColor.a);
        }
    }

    bool ZayPanel::polyimageNative(const Point (&ps)[3], const Image& image, Image::Build build, const Point (&ips)[3], const Color (&colors)[3]) const
    {
        const Clip& LastClip = m_stack_clip[-1];
        if(Platform::Graphics::BeginGL())
        {
            const sint32 ImageWidth = image.GetWidth();
            const sint32 ImageHeight = image.GetHeight();
            const float XRate = LastClip.Width() / ImageWidth;
            const float YRate = LastClip.Height() / ImageHeight;
            const sint32 DstWidth = (sint32) (image.GetImageWidth() * XRate + 0.5);
            const sint32 DstHeight = (sint32) (image.GetImageHeight() * YRate + 0.5);

            if(build != Image::Build::Null)
            {
                if(id_image_read RebuildImage = image.GetBuildImage(DstWidth, DstHeight, build))
                    Platform::Graphics::DrawPolyImageToFBO(RebuildImage, ips,
                        LastClip.l, LastClip.t, ps, colors, fbo());
                if(m_updater && !image.IsBuildFinished())
                    m_updater->RepaintOnce();
            }
            else Platform::Graphics::DrawPolyImageToFBO(image.GetImage(), ips,
                LastClip.l, LastClip.t, ps, colors, fbo());
            Platform::Graphics::EndGL();
            return true;
        }
        return false;
    }

    static inline sint32 GetXAlignCode(UIAlign align)
    {
        sint32 XAlignCode = 0;
        switch(align)
        {
        case UIA_LeftTop:    XAlignCode = 0; break; case UIA_CenterTop:    XAlignCode = 1; break; case UIA_RightTop:    XAlignCode = 2; break;
        case UIA_LeftMiddle: XAlignCode = 0; break; case UIA_CenterMiddle: XAlignCode = 1; break; case UIA_RightMiddle: XAlignCode = 2; break;
        case UIA_LeftBottom: XAlignCode = 0; break; case UIA_CenterBottom: XAlignCode = 1; break; case UIA_RightBottom: XAlignCode = 2; break;
        }
        return XAlignCode;
    }

    static inline sint32 GetYAlignCode(UIAlign align)
    {
        sint32 YAlignCode = 0;
        switch(align)
        {
        case UIA_LeftTop:    YAlignCode = 0; break; case UIA_CenterTop:    YAlignCode = 0; break; case UIA_RightTop:    YAlignCode = 0; break;
        case UIA_LeftMiddle: YAlignCode = 1; break; case UIA_CenterMiddle: YAlignCode = 1; break; case UIA_RightMiddle: YAlignCode = 1; break;
        case UIA_LeftBottom: YAlignCode = 2; break; case UIA_CenterBottom: YAlignCode = 2; break; case UIA_RightBottom: YAlignCode = 2; break;
        }
        return YAlignCode;
    }

    ZayPanel::InsideBinder ZayPanel::icon(const Image& image, UIAlign align, bool visible)
    {
        if(!image.HasBitmap())
            return InsideBinder(nullptr); // 안쪽영역없음

        const Clip& LastClip = m_stack_clip[-1];
        const sint32 XAlignCode = GetXAlignCode(align);
        const sint32 YAlignCode = GetYAlignCode(align);
        const float DstX = ((XAlignCode == 0)? 0 : ((XAlignCode == 1)? (LastClip.Width() - image.GetWidth()) / 2 : LastClip.Width() - image.GetWidth()));
        const float DstY = ((YAlignCode == 0)? 0 : ((YAlignCode == 1)? (LastClip.Height() - image.GetHeight()) / 2 : LastClip.Height() - image.GetHeight()));

        if(visible)
        {
            const Color& LastColor = m_stack_color[-1];
            Platform::Graphics::DrawImage(image.GetBuildImage(LastColor),
                0, 0, image.GetImageWidth(), image.GetImageHeight(),
                LastClip.l + DstX - image.L(), LastClip.t + DstY - image.T(),
                image.GetImageWidth(), image.GetImageHeight());
        }

        if(image.HasChild())
            return InsideBinder(&image, Rect(Point(LastClip.l + DstX, LastClip.t + DstY), Size(image.GetWidth(), image.GetHeight())));
        return InsideBinder(nullptr); // 안쪽영역없음
    }

    ZayPanel::InsideBinder ZayPanel::icon(float x, float y, const Image& image, UIAlign align, bool visible)
    {
        if(!image.HasBitmap())
            return InsideBinder(nullptr); // 안쪽영역없음

        const Clip& LastClip = m_stack_clip[-1];
        const sint32 XAlignCode = GetXAlignCode(align);
        const sint32 YAlignCode = GetYAlignCode(align);
        const float DstX = ((XAlignCode == 0)? x : ((XAlignCode == 1)? x - image.GetWidth() / 2 : x - image.GetWidth()));
        const float DstY = ((YAlignCode == 0)? y : ((YAlignCode == 1)? y - image.GetHeight() / 2 : y - image.GetHeight()));

        if(visible)
        {
            const Color& LastColor = m_stack_color[-1];
            Platform::Graphics::DrawImage(image.GetBuildImage(LastColor),
                0, 0, image.GetImageWidth(), image.GetImageHeight(),
                LastClip.l + DstX - image.L(), LastClip.t + DstY - image.T(),
                image.GetImageWidth(), image.GetImageHeight());
        }

        if(image.HasChild())
            return InsideBinder(&image, Rect(Point(LastClip.l + DstX, LastClip.t + DstY), Size(image.GetWidth(), image.GetHeight())));
        return InsideBinder(nullptr); // 안쪽영역없음
    }

    ZayPanel::InsideBinder ZayPanel::iconNative(id_image_read image, UIAlign align)
    {
        const Clip& LastClip = m_stack_clip[-1];
        const sint32 XAlignCode = GetXAlignCode(align);
        const sint32 YAlignCode = GetYAlignCode(align);
        const sint32 ImageWidth = Platform::Graphics::GetImageWidth(image);
        const sint32 ImageHeight = Platform::Graphics::GetImageHeight(image);
        const float DstX = ((XAlignCode == 0)? 0 : ((XAlignCode == 1)? (LastClip.Width() - ImageWidth) / 2 : LastClip.Width() - ImageWidth));
        const float DstY = ((YAlignCode == 0)? 0 : ((YAlignCode == 1)? (LastClip.Height() - ImageHeight) / 2 : LastClip.Height() - ImageHeight));

        Platform::Graphics::DrawImage(image, 0, 0, ImageWidth, ImageHeight,
            LastClip.l + DstX, LastClip.t + DstY, ImageWidth, ImageHeight);
        return InsideBinder(nullptr); // 안쪽영역없음
    }

    ZayPanel::InsideBinder ZayPanel::iconNative(float x, float y, id_image_read image, UIAlign align)
    {
        const Clip& LastClip = m_stack_clip[-1];
        const sint32 XAlignCode = GetXAlignCode(align);
        const sint32 YAlignCode = GetYAlignCode(align);
        const sint32 ImageWidth = Platform::Graphics::GetImageWidth(image);
        const sint32 ImageHeight = Platform::Graphics::GetImageHeight(image);
        const float DstX = ((XAlignCode == 0)? x : ((XAlignCode == 1)? x - ImageWidth / 2 : x - ImageWidth));
        const float DstY = ((YAlignCode == 0)? y : ((YAlignCode == 1)? y - ImageHeight / 2 : y - ImageHeight));

        Platform::Graphics::DrawImage(image, 0, 0, ImageWidth, ImageHeight,
            LastClip.l + DstX, LastClip.t + DstY, ImageWidth, ImageHeight);
        return InsideBinder(nullptr); // 안쪽영역없음
    }

    ZayPanel::InsideBinder ZayPanel::stretch(const Image& image, Image::Build build, UIStretchForm form, bool visible)
    {
        if(!image.HasBitmap())
            return InsideBinder(nullptr); // 안쪽영역없음

        Clip LastClip = m_stack_clip[-1];
        const sint32 ImageWidth = image.GetWidth();
        const sint32 ImageHeight = image.GetHeight();
        if(form != UISF_Strong)
        {
            float Rate = 0;
            switch(form)
            {
            case UISF_Inner: Rate = Math::MinF(LastClip.Width() / ImageWidth, LastClip.Height() / ImageHeight); break;
            case UISF_Outer: Rate = Math::MaxF(LastClip.Width() / ImageWidth, LastClip.Height() / ImageHeight); break;
            case UISF_Width: Rate = LastClip.Width() / ImageWidth; break;
            case UISF_Height: Rate = LastClip.Height() / ImageHeight; break;
            }
            const float InnerX = (LastClip.Width() - ImageWidth * Rate) * 0.5;
            const float InnerY = (LastClip.Height() - ImageHeight * Rate) * 0.5;
            LastClip.l += InnerX;
            LastClip.t += InnerY;
            LastClip.r -= InnerX;
            LastClip.b -= InnerY;
        }

        if(visible)
        {
            const float XRate = LastClip.Width() / ImageWidth;
            const float YRate = LastClip.Height() / ImageHeight;
            const float DstX = -image.L() * XRate;
            const float DstY = -image.T() * YRate;
            const sint32 DstWidth = (sint32) (image.GetImageWidth() * XRate + 0.5);
            const sint32 DstHeight = (sint32) (image.GetImageHeight() * YRate + 0.5);
            if(build != Image::Build::Null)
            {
                const Color& LastColor = m_stack_color[-1];
                if(auto RebuildImage = image.GetBuildImage(DstWidth, DstHeight, LastColor, build))
                {
                    auto RebuildWidth = Platform::Graphics::GetImageWidth(RebuildImage);
                    auto RebuildHeight = Platform::Graphics::GetImageHeight(RebuildImage);
                    Platform::Graphics::DrawImage(RebuildImage, 0, 0, RebuildWidth, RebuildHeight,
                        LastClip.l + DstX, LastClip.t + DstY, DstWidth, DstHeight);
                }
                if(m_updater && !image.IsBuildFinished())
                    m_updater->RepaintOnce();
            }
            else Platform::Graphics::DrawImage(image.GetImage(), 0, 0, image.GetImageWidth(), image.GetImageHeight(),
                LastClip.l + DstX, LastClip.t + DstY, DstWidth, DstHeight);
        }

        if(image.HasChild())
            return InsideBinder(&image, Rect(LastClip.l, LastClip.t, LastClip.r, LastClip.b));
        return InsideBinder(nullptr); // 안쪽영역없음
    }

    ZayPanel::InsideBinder ZayPanel::stretchNative(id_image_read image, UIStretchForm form) const
    {
        Clip LastClip = m_stack_clip[-1];
        const sint32 ImageWidth = Platform::Graphics::GetImageWidth(image);
        const sint32 ImageHeight = Platform::Graphics::GetImageHeight(image);
        if(form != UISF_Strong)
        {
            float Rate = 0;
            switch(form)
            {
            case UISF_Inner: Rate = Math::MinF(LastClip.Width() / ImageWidth, LastClip.Height() / ImageHeight); break;
            case UISF_Outer: Rate = Math::MaxF(LastClip.Width() / ImageWidth, LastClip.Height() / ImageHeight); break;
            case UISF_Width: Rate = LastClip.Width() / ImageWidth; break;
            case UISF_Height: Rate = LastClip.Height() / ImageHeight; break;
            }
            const float InnerX = (LastClip.Width() - ImageWidth * Rate) * 0.5;
            const float InnerY = (LastClip.Height() - ImageHeight * Rate) * 0.5;
            LastClip.l += InnerX;
            LastClip.t += InnerY;
            LastClip.r -= InnerX;
            LastClip.b -= InnerY;
        }

        const float XRate = LastClip.Width() / ImageWidth;
        const float YRate = LastClip.Height() / ImageHeight;
        const sint32 DstWidth = (sint32) (ImageWidth * XRate + 0.5);
        const sint32 DstHeight = (sint32) (ImageHeight * YRate + 0.5);
        Platform::Graphics::DrawImage(image, 0, 0, ImageWidth, ImageHeight,
            LastClip.l, LastClip.t, DstWidth, DstHeight);
        return InsideBinder(nullptr); // 안쪽영역없음
    }

    ZayPanel::InsideBinder ZayPanel::ninepatch(const Image& image, bool visible)
    {
        if(!image.HasBitmap())
            return InsideBinder(nullptr); // 안쪽영역없음

        const Clip& LastClip = m_stack_clip[-1];
        if(visible && image.UpdatePatchBy(LastClip.Width(), LastClip.Height()))
        {
            const sint32* PatchSrcX = image.PatchSrcXArray();
            const sint32* PatchSrcY = image.PatchSrcYArray();
            const sint32* PatchDstX = image.PatchDstXArray();
            const sint32* PatchDstY = image.PatchDstYArray();

            const Color& LastColor = m_stack_color[-1];
            for(sint32 y = 0, yend = image.PatchSrcYCount() - 1; y < yend; ++y)
            for(sint32 x = 0, xend = image.PatchSrcXCount() - 1; x < xend; ++x)
                Platform::Graphics::DrawImage(image.GetBuildImage(LastColor),
                    PatchSrcX[x], PatchSrcY[y], PatchSrcX[x + 1] - PatchSrcX[x], PatchSrcY[y + 1] - PatchSrcY[y],
                    LastClip.l + PatchDstX[x], LastClip.t + PatchDstY[y], PatchDstX[x + 1] - PatchDstX[x], PatchDstY[y + 1] - PatchDstY[y]);
        }

        if(image.HasChild())
            return InsideBinder(&image, Rect(LastClip.l, LastClip.t, LastClip.r, LastClip.b));
        return InsideBinder(nullptr); // 안쪽영역없음
    }

    void ZayPanel::pattern(const Image& image, UIAlign align, bool reversed_xorder, bool reversed_yorder) const
    {
        if(!image.HasBitmap()) return;

        sint32 XAlignCode = 0;
        switch(align)
        {
        case UIA_LeftTop:    XAlignCode = 0; break; case UIA_CenterTop:    XAlignCode = 1; break; case UIA_RightTop:    XAlignCode = 2; break;
        case UIA_LeftMiddle: XAlignCode = 0; break; case UIA_CenterMiddle: XAlignCode = 1; break; case UIA_RightMiddle: XAlignCode = 2; break;
        case UIA_LeftBottom: XAlignCode = 0; break; case UIA_CenterBottom: XAlignCode = 1; break; case UIA_RightBottom: XAlignCode = 2; break;
        }
        sint32 YAlignCode = 0;
        switch(align)
        {
        case UIA_LeftTop:    YAlignCode = 0; break; case UIA_CenterTop:    YAlignCode = 0; break; case UIA_RightTop:    YAlignCode = 0; break;
        case UIA_LeftMiddle: YAlignCode = 1; break; case UIA_CenterMiddle: YAlignCode = 1; break; case UIA_RightMiddle: YAlignCode = 1; break;
        case UIA_LeftBottom: YAlignCode = 2; break; case UIA_CenterBottom: YAlignCode = 2; break; case UIA_RightBottom: YAlignCode = 2; break;
        }

        const Clip& LastClip = m_stack_clip[-1];
        const float DstXAdd = (XAlignCode == 0)? 0 : ((XAlignCode == 1)?
            Math::Mod((LastClip.Width() + image.GetWidth()) * 0.5f, image.GetWidth()) - image.GetWidth() :
            Math::Mod(LastClip.Width(), image.GetWidth()) - image.GetWidth());
        const float DstYAdd = (YAlignCode == 0)? 0 : ((YAlignCode == 1)?
            Math::Mod((LastClip.Height() + image.GetHeight()) * 0.5f, image.GetHeight()) - image.GetHeight() :
            Math::Mod(LastClip.Height(), image.GetHeight()) - image.GetHeight());
        const sint32 XCount = (sint32) ((LastClip.Width() + image.GetWidth() - DstXAdd) / image.GetWidth());
        const sint32 YCount = (sint32) ((LastClip.Height() + image.GetHeight() - DstYAdd) / image.GetHeight());
        const float XBegin = DstXAdd + ((reversed_xorder)? image.GetWidth() * (XCount - 1) : 0) - image.L();
        const float YBegin = DstYAdd + ((reversed_yorder)? image.GetHeight() * (YCount - 1) : 0) - image.T();
        const sint32 XStep = (reversed_xorder)? -image.GetWidth() : image.GetWidth();
        const sint32 YStep = (reversed_yorder)? -image.GetHeight() : image.GetHeight();

        const Color& LastColor = m_stack_color[-1];
        float DstY = YBegin;
        for(sint32 y = 0; y < YCount; ++y)
        {
            float DstX = XBegin;
            for(sint32 x = 0; x < XCount; ++x)
            {
                Platform::Graphics::DrawImage(image.GetBuildImage(LastColor),
                    0, 0, image.GetImageWidth(), image.GetImageHeight(),
                    LastClip.l + DstX, LastClip.t + DstY,
                    image.GetImageWidth(), image.GetImageHeight());
                DstX += XStep;
            }
            DstY += YStep;
        }
    }

    bool ZayPanel::text(chars string, UIFontAlign align, UIFontElide elide) const
    {
        return text(string, -1, align, elide);
    }

    bool ZayPanel::text(chars string, sint32 count, UIFontAlign align, UIFontElide elide) const
    {
        const Clip& LastClip = m_stack_clip[-1];
        return Platform::Graphics::DrawString(LastClip.l, LastClip.t, LastClip.Width(), LastClip.Height(), string, count, align, elide);
    }

    void ZayPanel::text(float x, float y, chars string, UIFontAlign align) const
    {
        text(x, y, string, -1, align);
    }

    void ZayPanel::text(float x, float y, chars string, sint32 count, UIFontAlign align) const
    {
        x += m_stack_clip[-1].l;
        y += m_stack_clip[-1].t;

        bool NeedWidth = false;
        switch(align)
        {
        case UIFA_LeftTop: break; case UIFA_CenterTop: case UIFA_RightTop: case UIFA_JustifyTop: NeedWidth = true; break;
        case UIFA_LeftMiddle: break; case UIFA_CenterMiddle: case UIFA_RightMiddle: case UIFA_JustifyMiddle: NeedWidth = true; break;
        case UIFA_LeftAscent: break; case UIFA_CenterAscent: case UIFA_RightAscent: case UIFA_JustifyAscent: NeedWidth = true; break;
        case UIFA_LeftBottom: break; case UIFA_CenterBottom: case UIFA_RightBottom: case UIFA_JustifyBottom: NeedWidth = true; break;
        }
        bool NeedHeight = false, NeedAscent = false;
        switch(align)
        {
        case UIFA_LeftTop: case UIFA_CenterTop: case UIFA_RightTop: case UIFA_JustifyTop: break;
        case UIFA_LeftMiddle: case UIFA_CenterMiddle: case UIFA_RightMiddle: case UIFA_JustifyMiddle: NeedHeight = true; break;
        case UIFA_LeftAscent: case UIFA_CenterAscent: case UIFA_RightAscent: case UIFA_JustifyAscent: NeedAscent = true; break;
        case UIFA_LeftBottom: case UIFA_CenterBottom: case UIFA_RightBottom: case UIFA_JustifyBottom: NeedHeight = true; break;
        }
        const sint32 Width = (NeedWidth)? Platform::Graphics::GetStringWidth(string, count) : 0;
        const sint32 Height = (NeedHeight)? Platform::Graphics::GetStringHeight() : 0;
        const sint32 Ascent = (NeedAscent)? Platform::Graphics::GetStringAscent() : 0;

        Rect CalcRect;
        switch(align)
        {
        case UIFA_LeftTop: CalcRect.l = x; CalcRect.t = y; break;
        case UIFA_CenterTop: CalcRect.l = x - Width / 2; CalcRect.t = y; break;
        case UIFA_RightTop: CalcRect.l = x - Width; CalcRect.t = y; break;
        case UIFA_JustifyTop: CalcRect.l = x - Width / 2; CalcRect.t = y; break;
        case UIFA_LeftMiddle: CalcRect.l = x; CalcRect.t = y - Height / 2; break;
        case UIFA_CenterMiddle: CalcRect.l = x - Width / 2; CalcRect.t = y - Height / 2; break;
        case UIFA_RightMiddle: CalcRect.l = x - Width; CalcRect.t = y - Height / 2; break;
        case UIFA_JustifyMiddle: CalcRect.l = x - Width / 2; CalcRect.t = y - Height / 2; break;
        case UIFA_LeftAscent: CalcRect.l = x; CalcRect.t = y - Ascent; break;
        case UIFA_CenterAscent: CalcRect.l = x - Width / 2; CalcRect.t = y - Ascent; break;
        case UIFA_RightAscent: CalcRect.l = x - Width; CalcRect.t = y - Ascent; break;
        case UIFA_JustifyAscent: CalcRect.l = x - Width / 2; CalcRect.t = y - Ascent; break;
        case UIFA_LeftBottom: CalcRect.l = x; CalcRect.t = y - Height; break;
        case UIFA_CenterBottom: CalcRect.l = x - Width / 2; CalcRect.t = y - Height; break;
        case UIFA_RightBottom: CalcRect.l = x - Width; CalcRect.t = y - Height; break;
        case UIFA_JustifyBottom: CalcRect.l = x - Width / 2; CalcRect.t = y - Height; break;
        }
        CalcRect.r = CalcRect.l + 0x2000; // 0xFFFF였으나 cocos2dx의 한계로 인하여 적당히 수정
        CalcRect.b = CalcRect.t + 0x2000;
        Platform::Graphics::DrawString(CalcRect.l, CalcRect.t, CalcRect.Width(), CalcRect.Height(), string, count, UIFA_LeftTop);
    }

    bool ZayPanel::textbox(chars string, sint32 linegap) const
    {
        String MultiText(string);
        MultiText.Replace("\\\\", "\\");
        MultiText.Replace("\\n", "\n");
        WString MultiTextW = WString::FromChars(MultiText);
        wchars TextW = MultiTextW;

        const Clip& LastClip = m_stack_clip[-1];
        sint32 AddY = 0;
        while(*TextW)
        {
            const sint32 CurHeight = Platform::Graphics::GetStringHeight();
            sint32 CurLength = Platform::Graphics::GetLengthOfStringW(true, LastClip.Width(), TextW);
            Platform::Graphics::DrawStringW(LastClip.l, LastClip.t + AddY, LastClip.Width(), LastClip.Height(), TextW, CurLength, UIFA_LeftTop);
            AddY += CurHeight + linegap;
            TextW += CurLength + (TextW[CurLength] == L' ');
        }
        return false;
    }

    void ZayPanel::sub(chars uigroup, id_surface surface) const
    {
        auto CurCollector = TouchCollector::ST(uigroup);
        bool DirtyTest = CurCollector->mDirty;
        if(auto CurTouch = (ZayView::Touch*) m_ref_touch)
        {
            const Clip& LastClip = m_stack_clip[-1];
            const float& LastZoom = m_stack_zoom[-1];
            const float HRate = LastClip.Width() / CurCollector->mWidth;
            const float VRate = LastClip.Height() / CurCollector->mHeight;
            for(sint32 i = 0, iend = CurCollector->mTouchRects.Count(); i < iend; ++i)
            {
                const TouchRect& CurTouchRect = CurCollector->mTouchRects[i].ConstValue();
                CurTouch->update(CurTouchRect.mName,
                    LastClip.l + CurTouchRect.mL * CurTouchRect.mZoom * HRate,
                    LastClip.t + CurTouchRect.mT * CurTouchRect.mZoom * VRate,
                    LastClip.l + CurTouchRect.mR * CurTouchRect.mZoom * HRate,
                    LastClip.t + CurTouchRect.mB * CurTouchRect.mZoom * VRate,
                    LastZoom, CurTouchRect.mCB, CurTouchRect.mScrollSence, CurTouchRect.mHoverPass, &DirtyTest);
            }
        }
        CurCollector->mRefTouch = m_ref_touch;
        CurCollector->mDirty = DirtyTest;

        if(surface)
        {
            const Clip& LastClip = m_stack_clip[-1];
            const sint32 SurfaceWidth = Platform::Graphics::GetSurfaceWidth(surface);
            const sint32 SurfaceHeight = Platform::Graphics::GetSurfaceHeight(surface);
            const float XRate = LastClip.Width() / SurfaceWidth;
            const float YRate = LastClip.Height() / SurfaceHeight;
            const sint32 DstWidth = (sint32) (SurfaceWidth * XRate);
            const sint32 DstHeight = (sint32) (SurfaceHeight * YRate);
            Platform::Graphics::DrawSurfaceToFBO(surface, 0, 0, SurfaceWidth, SurfaceHeight,
                orientationtype_normal0, false, LastClip.l, LastClip.t, DstWidth, DstHeight, fbo());
        }
    }

    PanelState ZayPanel::state(chars uiname) const
    {
        if(auto CurTouch = (const ZayView::Touch*) m_ref_touch)
        {
            if(uiname == nullptr)
                return CurTouch->background()->GetState(m_ref_touch);
            else if(auto CurElement = CurTouch->find(uiname, 1))
                return CurElement->GetState(m_ref_touch) | CurTouch->capturetest(uiname);
        }
        return PS_Null;
    }

    Point ZayPanel::toview(float x, float y) const
    {
        const Clip& LastClip = m_stack_clip[-1];
        return Point(LastClip.l + x, LastClip.t + y);
    }

    void ZayPanel::test(UITestOrder order)
    {
        switch(order)
        {
        case UITO_ScissorOn: m_test_scissor = true; break;
        case UITO_ScissorOff: m_test_scissor = false; break;
        }
    }

    void ZayPanel::repaintOnce()
    {
        if(m_updater)
            m_updater->RepaintOnce();
    }

    VisibleState ZayPanel::visible() const
    {
        const Clip& LastClip = m_stack_clip[-1];
        const Rect& LastScissor = m_stack_scissor[-1];

        VisibleState Result = VS_Visible;
        Result = (VisibleState) (Result | (VS_Left * (LastClip.r <= LastScissor.l)));
        Result = (VisibleState) (Result | (VS_Top * (LastClip.b <= LastScissor.t)));
        Result = (VisibleState) (Result | (VS_Right * (LastScissor.r <= LastClip.l)));
        Result = (VisibleState) (Result | (VS_Bottom * (LastScissor.b <= LastClip.t)));
        return Result;
    }

    uint32 ZayPanel::fbo() const
    {
        if(m_ref_surface)
            return Platform::Graphics::GetSurfaceFBO(m_ref_surface);
        return 0;
    }

    ZayPanel::StackBinder ZayPanel::_push_clip(float l, float t, float r, float b, bool doScissor)
    {
        Clip& NewClip = m_stack_clip.AtAdding();
        const Clip& LastClip = m_stack_clip[-2];
        NewClip = Clip(LastClip.l + l, LastClip.t + t, LastClip.l + r, LastClip.t + b, doScissor);

        if(doScissor && !_push_scissor(NewClip.l, NewClip.t, NewClip.r, NewClip.b))
        {
            m_stack_clip.SubtractionOne();
            return StackBinder(this); // 하위진입불가
        }

        m_clipped_width = NewClip.Width();
        m_clipped_height = NewClip.Height();
        return StackBinder(this, ST_Clip);
    }

    ZayPanel::StackBinder ZayPanel::_push_clip_ui(float l, float t, float r, float b, bool doScissor, chars uiname, SubGestureCB cb, bool hoverpass)
    {
        if(auto CurBinder = _push_clip(l, t, r, b, doScissor))
        {
            _add_ui(uiname, cb, -1, hoverpass);
            return StackBinder(ToReference(CurBinder));
        }
        return StackBinder(this); // 하위진입불가
    }

    ZayPanel::StackBinder ZayPanel::_push_clip_by_rect(const Rect& r, bool doScissor)
    {
        return _push_clip(r.l, r.t, r.r, r.b, doScissor);
    }

    ZayPanel::StackBinder ZayPanel::_push_clip_ui_by_rect(const Rect& r, bool doScissor, chars uiname, SubGestureCB cb, bool hoverpass)
    {
        return _push_clip_ui(r.l, r.t, r.r, r.b, doScissor, uiname, cb, hoverpass);
    }

    ZayPanel::StackBinder ZayPanel::_push_clip_by_inside(const InsideBinder& inside, sint32 ix, sint32 iy, sint32 xcount, sint32 ycount, bool doScissor)
    {
        if(!inside)
            return StackBinder(this); // 하위진입불가
        const Rect CalcedRect = inside.flush(ix, iy, xcount, ycount);

        const Clip& LastClip = m_stack_clip[-1];
        const float l = CalcedRect.l - LastClip.l;
        const float t = CalcedRect.t - LastClip.t;
        const float r = CalcedRect.r - LastClip.l;
        const float b = CalcedRect.b - LastClip.t;
        return _push_clip(l, t, r, b, doScissor);
    }

    ZayPanel::StackBinder ZayPanel::_push_clip_ui_by_inside(const InsideBinder& inside, sint32 ix, sint32 iy, sint32 xcount, sint32 ycount, bool doScissor, chars uiname, SubGestureCB cb, bool hoverpass)
    {
        if(auto CurBinder = _push_clip_by_inside(inside, ix, iy, xcount, ycount, doScissor))
        {
            _add_ui(uiname, cb, -1, hoverpass);
            return StackBinder(ToReference(CurBinder));
        }
        return StackBinder(this); // 하위진입불가
    }

    ZayPanel::StackBinder ZayPanel::_push_scroll_ui(float contentw, float contenth, chars uiname, SubGestureCB cb, sint32 sensitive, sint32 senseborder, bool loop, float loopw, float looph)
    {
        if(auto CurTouch = (ZayView::Touch*) m_ref_touch)
        {
            ZayView::Scroll* CurScroll = nullptr;
            if(!(CurScroll = CurTouch->getscroll(uiname)))
            {
                CurScroll = CurTouch->getscroll_valid(uiname);
                CurScroll->Init(m_updater, 0, 0);
            }

            const Clip& LastClip = m_stack_clip[-1];
            sint32 ScrollX = CurScroll->m_pos->x();
            sint32 ScrollY = CurScroll->m_pos->y();

            if(loop)
            {
                // 루프처리
                sint32 BeginIndexX = 0, BeginIndexY = 0;
                if(0 < loopw)
                {
                    if(ScrollX <= -loopw) BeginIndexX = (sint32) (ScrollX / -loopw);
                    else if(0 < ScrollX) BeginIndexX = (ScrollX + loopw - 1) / -loopw;
                    ScrollX += loopw * BeginIndexX;
                }
                if(0 < looph)
                {
                    if(ScrollY <= -looph) BeginIndexY = (sint32) (ScrollY / -looph);
                    else if(0 < ScrollY) BeginIndexY = (ScrollY + looph - 1) / -looph;
                    ScrollY += looph * BeginIndexY;
                }
                CurScroll->SetInfo(0, 0, BeginIndexX, BeginIndexY);

                if(auto CurBinder = _push_clip(ScrollX, ScrollY, LastClip.r - LastClip.l, LastClip.b - LastClip.t, false))
                {
                    _add_ui(uiname, cb, sensitive, false);
                    return StackBinder(ToReference(CurBinder));
                }
            }
            else
            {
                const float ContentWidth = Math::MaxF(contentw, LastClip.Width());
                const float ContentHeight = Math::MaxF(contenth, LastClip.Height());
                // 한계적용
                CurScroll->SetInfo(ContentWidth - LastClip.Width(), ContentHeight - LastClip.Height(), 0, 0);
                const bool OverSense =
                    ScrollX < -CurScroll->m_size.w - senseborder || senseborder < ScrollX ||
                    ScrollY < -CurScroll->m_size.h - senseborder || senseborder < ScrollY;
                ScrollX = Math::Clamp(ScrollX, -CurScroll->m_size.w - senseborder, senseborder);
                ScrollY = Math::Clamp(ScrollY, -CurScroll->m_size.h - senseborder, senseborder);

                // 센스기능(벽튕기기)
                if(!CurScroll->m_sense && (CurScroll->m_pos->IsArrivedAlmost(1.8) || OverSense))
                {
                    if(senseborder == 0)
                    {
                        CurScroll->Reset(ScrollX, ScrollY);
                        CurScroll->m_sense = true;
                    }
                    else
                    {
                        uint08 UpdateCode = 0xFF;
                        float NewX = ScrollX;
                        if(0 < ScrollX) NewX = 0;
                        else if(ScrollX < -CurScroll->m_size.w) NewX = -CurScroll->m_size.w;
                        else UpdateCode &= 0xF0;
                        float NewY = ScrollY;
                        if(0 < ScrollY) NewY = 0;
                        else if(ScrollY < -CurScroll->m_size.h) NewY = -CurScroll->m_size.h;
                        else UpdateCode &= 0x0F;
                        if(UpdateCode)
                        {
                            CurScroll->Reset(ScrollX, ScrollY);
                            CurScroll->Moving(NewX, NewY, 0.5, 0.1, 0.9);
                            CurScroll->m_sense = true;
                        }
                    }
                }

                if(auto CurBinder = _push_clip(ScrollX, ScrollY, ScrollX + ContentWidth, ScrollY + ContentHeight, false))
                {
                    _add_ui(uiname, cb, sensitive, false);
                    return StackBinder(ToReference(CurBinder));
                }
            }
        }
        return StackBinder(this); // 하위진입불가
    }

    ZayPanel::StackBinder ZayPanel::_push_color(sint32 r, sint32 g, sint32 b, sint32 a)
    {
        Color& NewColor = m_stack_color.AtAdding();
        const Color& LastColor = m_stack_color[-2];
        NewColor = LastColor * Color(r, g, b, a);

        // 직접 입력방식
        if(r < 0) NewColor.r = -r;
        if(g < 0) NewColor.g = -g;
        if(b < 0) NewColor.b = -b;
        if(a < 0) NewColor.a = -a;

        Platform::Graphics::SetColor(NewColor.r, NewColor.g, NewColor.b, NewColor.a);
        return StackBinder(this, ST_Color);
    }

    ZayPanel::StackBinder ZayPanel::_push_color(const Color& color)
    {
        return _push_color(color.r, color.g, color.b, color.a);
    }

    ZayPanel::StackBinder ZayPanel::_push_color_clear()
    {
        Color& NewColor = m_stack_color.AtAdding();
        NewColor = Color(Color::ColoringDefault);

        Platform::Graphics::SetColor(NewColor.r, NewColor.g, NewColor.b, NewColor.a);
        return StackBinder(this, ST_Color);
    }

    ZayPanel::StackBinder ZayPanel::_push_mask(MaskRole role)
    {
        MaskRole& NewMask = m_stack_mask.AtAdding();
        NewMask = role;

        Platform::Graphics::SetMask(NewMask);
        return StackBinder(this, ST_Mask);
    }

    ZayPanel::StackBinder ZayPanel::_push_sysfont(float size, chars name)
    {
        Font& NewFont = m_stack_font.AtAdding();
        const Font& LastFont = m_stack_font[-2];
        NewFont.is_freefont = false;
        NewFont.system_name = (name)? name : (chars) LastFont.system_name;
        NewFont.system_size = LastFont.system_size * size;
        NewFont.freefont_nickname = LastFont.freefont_nickname;
        NewFont.freefont_height = LastFont.freefont_height;

        Platform::Graphics::SetFont(NewFont.system_name, NewFont.system_size);
        return StackBinder(this, ST_Font);
    }

    ZayPanel::StackBinder ZayPanel::_push_freefont(sint32 height, chars nickname)
    {
        Font& NewFont = m_stack_font.AtAdding();
        const Font& LastFont = m_stack_font[-2];
        NewFont.is_freefont = true;
        NewFont.system_name = LastFont.system_name;
        NewFont.system_size = LastFont.system_size;
        NewFont.freefont_nickname = (nickname)? nickname : (chars) LastFont.freefont_nickname;
        NewFont.freefont_height = height;

        Platform::Graphics::SetFontForFreeType(NewFont.freefont_nickname, NewFont.freefont_height);
        return StackBinder(this, ST_Font);
    }

    ZayPanel::StackBinder ZayPanel::_push_zoom(float zoom)
    {
        float& NewZoom = m_stack_zoom.AtAdding();
        const float LastZoom = m_stack_zoom[-2];
        NewZoom = LastZoom * zoom;
        Platform::Graphics::SetZoom(NewZoom);

        Clip& NewClip = m_stack_clip.AtAdding();
        const Clip& LastClip = m_stack_clip[-2];
        NewClip = Clip(LastClip.l / zoom, LastClip.t / zoom, LastClip.r / zoom, LastClip.b / zoom, true);
        m_clipped_width = NewClip.Width();
        m_clipped_height = NewClip.Height();

        Rect& NewScissor = m_stack_scissor.AtAdding();
        const Rect& LastScissor = m_stack_scissor[-2];
        NewScissor = Rect(LastScissor.l / zoom, LastScissor.t / zoom, LastScissor.r / zoom, LastScissor.b / zoom);

        Platform::Graphics::SetScissor(NewScissor.l, NewScissor.t, NewScissor.Width(), NewScissor.Height());
        return StackBinder(this, ST_Zoom);
    }

    ZayPanel::StackBinder ZayPanel::_push_zoom_clear()
    {
        m_stack_zoom.AtAdding() = 1;
        const float LastZoom = m_stack_zoom[-2];
        Platform::Graphics::SetZoom(1);

        Clip& NewClip = m_stack_clip.AtAdding();
        const Clip& LastClip = m_stack_clip[-2];
        NewClip = Clip(LastClip.l * LastZoom, LastClip.t * LastZoom, LastClip.r * LastZoom, LastClip.b * LastZoom, true);
        m_clipped_width = NewClip.Width();
        m_clipped_height = NewClip.Height();

        Rect& NewScissor = m_stack_scissor.AtAdding();
        const Rect& LastScissor = m_stack_scissor[-2];
        NewScissor = Rect(LastScissor.l * LastZoom, LastScissor.t * LastZoom, LastScissor.r * LastZoom, LastScissor.b * LastZoom);

        Platform::Graphics::SetScissor(NewScissor.l, NewScissor.t, NewScissor.Width(), NewScissor.Height());
        return StackBinder(this, ST_Zoom);
    }

    ZayPanel::StackBinder ZayPanel::_push_pass()
    {
        return StackBinder(this, ST_Pass);
    }

    void ZayPanel::_pop_clip()
    {
        BOSS_ASSERT("Pop할 잔여스택이 없습니다", 1 < m_stack_clip.Count());
        if(m_stack_clip[-1].didscissor) _pop_scissor();
        m_stack_clip.SubtractionOne();

        const Clip& LastClip = m_stack_clip[-1];
        m_clipped_width = LastClip.Width();
        m_clipped_height = LastClip.Height();
    }

    void ZayPanel::_pop_color()
    {
        BOSS_ASSERT("Pop할 잔여스택이 없습니다", 1 < m_stack_color.Count());
        m_stack_color.SubtractionOne();

        const Color& LastColor = m_stack_color[-1];
        Platform::Graphics::SetColor(LastColor.r, LastColor.g, LastColor.b, LastColor.a);
    }

    void ZayPanel::_pop_mask()
    {
        BOSS_ASSERT("Pop할 잔여스택이 없습니다", 1 < m_stack_mask.Count());
        m_stack_mask.SubtractionOne();

        const MaskRole& LastMask = m_stack_mask[-1];
        Platform::Graphics::SetMask(LastMask);
    }

    void ZayPanel::_pop_font()
    {
        BOSS_ASSERT("Pop할 잔여스택이 없습니다", 1 < m_stack_font.Count());
        m_stack_font.SubtractionOne();

        const Font& LastFont = m_stack_font[-1];
        if(LastFont.is_freefont)
            Platform::Graphics::SetFontForFreeType(LastFont.freefont_nickname, LastFont.freefont_height);
        else Platform::Graphics::SetFont(LastFont.system_name, LastFont.system_size);
    }

    void ZayPanel::_pop_zoom()
    {
        BOSS_ASSERT("Pop할 잔여스택이 없습니다", 1 < m_stack_zoom.Count());
        m_stack_zoom.SubtractionOne();

        const float& LastZoom = m_stack_zoom[-1];
        Platform::Graphics::SetZoom(LastZoom);
        _pop_clip();
    }

    void ZayPanel::_add_ui(chars uiname, SubGestureCB cb, sint32 scrollsense, bool hoverpass)
    {
        if(uiname && uiname[0])
        {
            Clip LastClip = m_stack_clip[-1];
            const Rect& LastScissor = m_stack_scissor[-1];
            LastClip.l = Math::MaxF(LastClip.l, LastScissor.l);
            LastClip.t = Math::MaxF(LastClip.t, LastScissor.t);
            LastClip.r = Math::MinF(LastClip.r, LastScissor.r);
            LastClip.b = Math::MinF(LastClip.b, LastScissor.b);
            if(0 < LastClip.Width() && 0 < LastClip.Height())
            {
                const float& LastZoom = m_stack_zoom[-1];
                if(auto CurCollector = (TouchCollector*) m_ref_touch_collector)
                    CurCollector->mTouchRects.AtAdding() =
                        TouchRect::Create(uiname, LastClip.l, LastClip.t, LastClip.r, LastClip.b, LastZoom, cb, scrollsense, hoverpass);
                else if(auto CurTouch = (ZayView::Touch*) m_ref_touch)
                    CurTouch->update(uiname, LastClip.l, LastClip.t, LastClip.r, LastClip.b, LastZoom, cb, scrollsense, hoverpass);
            }
        }
    }

    bool ZayPanel::_push_scissor(float l, float t, float r, float b)
    {
        Rect& NewScissor = m_stack_scissor.AtAdding();
        const Rect& LastScissor = m_stack_scissor[-2];
        NewScissor.l = Math::MaxF(l, LastScissor.l);
        NewScissor.t = Math::MaxF(t, LastScissor.t);
        NewScissor.r = Math::MinF(r, LastScissor.r);
        NewScissor.b = Math::MinF(b, LastScissor.b);

        if(m_test_scissor && (NewScissor.r <= NewScissor.l || NewScissor.b <= NewScissor.t))
        {
            m_stack_scissor.SubtractionOne();
            return false;
        }

        Platform::Graphics::SetScissor(NewScissor.l, NewScissor.t, NewScissor.Width(), NewScissor.Height());
        return true;
    }

    void ZayPanel::_pop_scissor()
    {
        BOSS_ASSERT("Pop할 잔여스택이 없습니다", 1 < m_stack_scissor.Count());
        m_stack_scissor.SubtractionOne();

        const Rect& LastScissor = m_stack_scissor[-1];
        Platform::Graphics::SetScissor(LastScissor.l, LastScissor.t, LastScissor.Width(), LastScissor.Height());
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ZayExtend
    ////////////////////////////////////////////////////////////////////////////////
    extern void ZayExtendCursor(CursorRole role, sint32 elementid);
    extern void ZayExtendTouch(chars uiname, sint32 elementid);

    ZayExtend::ZayExtend(ComponentType type, ComponentCB ccb, GlueCB gcb)
    {
        mComponentType = type;
        mComponentCB = ccb;
        mGlueCB = gcb;
    }

    ZayExtend::~ZayExtend()
    {
    }

    ZayExtend::Payload::Payload(const ZayExtend* owner, chars uiname, sint32 elementid)
        : mElementID(elementid)
    {
        mOwner = owner;
        mUIName = uiname;
    }

    ZayExtend::Payload::Payload(const ZayExtend* owner, const SolverValue& param, chars uiname, sint32 elementid)
        : mElementID(elementid)
    {
        mOwner = owner;
        mUIName = uiname;
        AddParam(param);
    }

    ZayExtend::Payload::~Payload()
    {
        if(mOwner->HasGlue()) // 소멸시 함수호출
            mOwner->mGlueCB(*this);
    }

    ZayExtend::Payload& ZayExtend::Payload::operator()(const SolverValue& value)
    {
        AddParam(value);
        return *this;
    }

    ZayExtend::Payload& ZayExtend::Payload::operator()(sint32 value)
    {
        AddParam(SolverValue::MakeByInteger(value));
        return *this;
    }

    ZayExtend::Payload& ZayExtend::Payload::operator()(sint64 value)
    {
        AddParam(SolverValue::MakeByInteger(value));
        return *this;
    }

    ZayExtend::Payload& ZayExtend::Payload::operator()(SolverValue::Float value)
    {
        AddParam(SolverValue::MakeByFloat(value));
        return *this;
    }

    ZayExtend::Payload& ZayExtend::Payload::operator()(SolverValue::Text value)
    {
        AddParam(SolverValue::MakeByText(value));
        return *this;
    }

    ZayPanel::StackBinder ZayExtend::Payload::operator>>(ZayPanel& panel) const
    {
        if(mOwner->HasComponent())
            return mOwner->mComponentCB(panel, *this);
        return panel._push_pass();
    }

    chars ZayExtend::Payload::UIName() const
    {
        return mUIName;
    }

    ZayPanel::SubGestureCB ZayExtend::Payload::MakeGesture() const
    {
        auto ElementID = mElementID;
        return ZAY_GESTURE_NT(n, t, ElementID)
            {
                if(t == GT_Moving || t == GT_MovingIdle)
                    ZayExtendCursor(CR_PointingHand, ElementID);
                else if(t == GT_MovingLosed)
                    ZayExtendCursor(CR_Arrow, ElementID);
                else if(t == GT_InReleased)
                {
                    ZayExtendTouch(n, ElementID);
                    ZayExtendCursor(CR_Busy, ElementID);
                }
            };
    }

    sint32 ZayExtend::Payload::ParamCount() const
    {
        return mParams.Count();
    }

    const SolverValue& ZayExtend::Payload::Param(sint32 i) const
    {
        return mParams[i];
    }

    bool ZayExtend::Payload::ParamToBool(sint32 i) const
    {
        if(mParams[i].GetType() == SolverValueType::Text)
        {
            if(!mParams[i].ToText().CompareNoCase("true"))
                return true;
            if(!mParams[i].ToText().CompareNoCase("false"))
                return false;
            BOSS_ASSERT("알 수 없는 Bool입니다", false);
        }
        return !!mParams[i].ToInteger();
    }

    UIAlign ZayExtend::Payload::ParamToUIAlign(sint32 i) const
    {
        String Result = mParams[i].ToText();
        if(!String::CompareNoCase(Result, "UIA_", 4))
            Result = Result.Right(Result.Length() - 4);

        branch;
        jump(!Result.CompareNoCase("LeftTop")) return UIA_LeftTop;
        jump(!Result.CompareNoCase("CenterTop")) return UIA_CenterTop;
        jump(!Result.CompareNoCase("RightTop")) return UIA_RightTop;
        jump(!Result.CompareNoCase("LeftMiddle")) return UIA_LeftMiddle;
        jump(!Result.CompareNoCase("CenterMiddle")) return UIA_CenterMiddle;
        jump(!Result.CompareNoCase("RightMiddle")) return UIA_RightMiddle;
        jump(!Result.CompareNoCase("LeftBottom")) return UIA_LeftBottom;
        jump(!Result.CompareNoCase("CenterBottom")) return UIA_CenterBottom;
        jump(!Result.CompareNoCase("RightBottom")) return UIA_RightBottom;
        BOSS_ASSERT("알 수 없는 UIAlign입니다", false);
        return UIA_LeftTop;
    }

    UIStretchForm ZayExtend::Payload::ParamToUIStretchForm(sint32 i) const
    {
        String Result = mParams[i].ToText();
        if(!String::CompareNoCase(Result, "UISF_", 5))
            Result = Result.Right(Result.Length() - 5);

        branch;
        jump(!Result.CompareNoCase("Strong")) return UISF_Strong;
        jump(!Result.CompareNoCase("Inner")) return UISF_Inner;
        jump(!Result.CompareNoCase("Outer")) return UISF_Outer;
        jump(!Result.CompareNoCase("Width")) return UISF_Width;
        jump(!Result.CompareNoCase("Height")) return UISF_Height;
        BOSS_ASSERT("알 수 없는 UIStretchForm입니다", false);
        return UISF_Strong;
    }

    UIFontAlign ZayExtend::Payload::ParamToUIFontAlign(sint32 i) const
    {
        String Result = mParams[i].ToText();
        if(!String::CompareNoCase(Result, "UIFA_", 5))
            Result = Result.Right(Result.Length() - 5);

        branch;
        jump(!Result.CompareNoCase("LeftTop")) return UIFA_LeftTop;
        jump(!Result.CompareNoCase("CenterTop")) return UIFA_CenterTop;
        jump(!Result.CompareNoCase("RightTop")) return UIFA_RightTop;
        jump(!Result.CompareNoCase("JustifyTop")) return UIFA_JustifyTop;
        jump(!Result.CompareNoCase("LeftMiddle")) return UIFA_LeftMiddle;
        jump(!Result.CompareNoCase("CenterMiddle")) return UIFA_CenterMiddle;
        jump(!Result.CompareNoCase("RightMiddle")) return UIFA_RightMiddle;
        jump(!Result.CompareNoCase("JustifyMiddle")) return UIFA_JustifyMiddle;
        jump(!Result.CompareNoCase("LeftAscent")) return UIFA_LeftAscent;
        jump(!Result.CompareNoCase("CenterAscent")) return UIFA_CenterAscent;
        jump(!Result.CompareNoCase("RightAscent")) return UIFA_RightAscent;
        jump(!Result.CompareNoCase("JustifyAscent")) return UIFA_JustifyAscent;
        jump(!Result.CompareNoCase("LeftBottom")) return UIFA_LeftBottom;
        jump(!Result.CompareNoCase("CenterBottom")) return UIFA_CenterBottom;
        jump(!Result.CompareNoCase("RightBottom")) return UIFA_RightBottom;
        jump(!Result.CompareNoCase("JustifyBottom")) return UIFA_JustifyBottom;
        BOSS_ASSERT("알 수 없는 UIFontAlign입니다", false);
        return UIFA_LeftTop;
    }

    UIFontElide ZayExtend::Payload::ParamToUIFontElide(sint32 i) const
    {
        String Result = mParams[i].ToText();
        if(!String::CompareNoCase(Result, "UIFE_", 5))
            Result = Result.Right(Result.Length() - 5);

        branch;
        jump(!Result.CompareNoCase("None")) return UIFE_None;
        jump(!Result.CompareNoCase("Left")) return UIFE_Left;
        jump(!Result.CompareNoCase("Center")) return UIFE_Center;
        jump(!Result.CompareNoCase("Right")) return UIFE_Right;
        BOSS_ASSERT("알 수 없는 UIFontElide입니다", false);
        return UIFE_None;
    }

    void ZayExtend::Payload::AddParam(const SolverValue& value)
    {
        mParams.AtAdding() = value;
    }

    const ZayExtend::Payload ZayExtend::operator()() const
    {
        return Payload(this);
    }

    ZayExtend::Payload ZayExtend::operator()(const SolverValue& value) const
    {
        return Payload(this, value);
    }

    ZayExtend::Payload ZayExtend::operator()(sint32 value) const
    {
        return Payload(this, SolverValue::MakeByInteger(value));
    }

    ZayExtend::Payload ZayExtend::operator()(sint64 value) const
    {
        return Payload(this, SolverValue::MakeByInteger(value));
    }

    ZayExtend::Payload ZayExtend::operator()(SolverValue::Float value) const
    {
        return Payload(this, SolverValue::MakeByFloat(value));
    }

    ZayExtend::Payload ZayExtend::operator()(SolverValue::Text value) const
    {
        return Payload(this, SolverValue::MakeByText(value));
    }

    bool ZayExtend::HasComponent() const
    {
        return (mComponentCB != nullptr);
    }

    bool ZayExtend::HasContentComponent() const
    {
        return (mComponentType == ComponentType::Content || mComponentType == ComponentType::ContentWithParameter);
    }

    bool ZayExtend::HasGlue() const
    {
        return (mGlueCB != nullptr);
    }

    void ZayExtend::ResetForComponent(ComponentType type, ComponentCB cb)
    {
        mComponentType = type;
        mComponentCB = cb;
    }

    void ZayExtend::ResetForGlue(GlueCB cb)
    {
        mGlueCB = cb;
    }

    ZayExtend::Payload ZayExtend::MakePayload(chars uiname, sint32 elementid) const
    {
        return Payload(this, uiname, elementid);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ZayView
    ////////////////////////////////////////////////////////////////////////////////
    class ZayController : public ZayObject
    {
    private:
        ZayController() {}
        ~ZayController() {}

    public:
        void nextFrame()
        {
            m_frame_counter++;
            m_frame_updater.Flush(ZayObject::invalidator, this);
        }

        void wakeUpCheck()
        {
            m_frame_updater.WakeUp(ZayObject::invalidator, this);
        }

        void setCallback(ZayObject::FinderCB fcb, void* fdata, ZayObject::UpdaterCB icb, void* idata)
        {
            m_finder = fcb;
            m_finder_data = fdata;
            m_updater = icb;
            m_updater_data = idata;
        }
    };

    ZayView::ZayView(chars viewclass) : View(),
        m_ref_func(ZayView::_accessfunc(viewclass, false)), m_viewclass((viewclass)? viewclass : "")
    {
        BOSS_ASSERT(String::Format("존재하지 않는 뷰(%s)를 생성하려 합니다", viewclass), m_ref_func);
        m_class = m_ref_func->m_alloc();
        m_touch = Buffer::Alloc<Touch>(BOSS_DBG 1);
        m_agreed_quit = false;
    }

    ZayView::~ZayView()
    {
        m_ref_func->m_free(m_class);
        Buffer::Free(m_touch);
    }

    View* ZayView::Creator(chars viewclass)
    {
        return new ZayView(viewclass);
    }

    h_view ZayView::SetView(h_view view)
    {
        h_view OldViewHandle = m_class->m_view;
        m_class->m_view = view;
        return OldViewHandle;
    }

    bool ZayView::IsNative()
    {
        return m_ref_func->m_isnative;
    }

    void* ZayView::GetClass()
    {
        return m_class;
    }

    void ZayView::SendNotify(NotifyType type, chars topic, id_share in, id_cloned_share* out)
    {
        auto LockID = m_ref_func->m_lock(m_class);
        m_ref_func->m_notify(type, topic, in, out);
        m_ref_func->m_unlock(LockID);
    }

    void ZayView::SetCallback(UpdaterCB cb, payload data)
    {
        ((ZayController*) m_class)->setCallback(_finder, this, cb, data);
    }

    void ZayView::DirtyAllSurfaces()
    {
        TouchCollector::STMAP().AccessByCallback(
            [](const MapPath*, TouchCollector* collector, payload param)->void
            {
                collector->mDirty = true;
            }, nullptr);
    }

    void ZayView::OnCreate()
    {
        BOSS_ASSERT("브로드캐스트 등록에 실패하였습니다", m_class);
        View::Regist(m_viewclass, m_class->m_view);

        auto LockID = m_ref_func->m_lock(m_class);
        m_ref_func->m_command(CT_Create, "", nullptr, nullptr);
        m_ref_func->m_unlock(LockID);
    }

    bool ZayView::OnCanQuit()
    {
        if(!m_agreed_quit)
        {
            auto LockID = m_ref_func->m_lock(m_class);
            id_cloned_share out = nullptr;
            m_ref_func->m_command(CT_CanQuit, "(Can you quit?)", nullptr, &out);
            m_ref_func->m_unlock(LockID);

            bool Result = true;
            if(out)
            {
                bools OutResult((id_cloned_share_read) out);
                Result = (m_agreed_quit = !!OutResult[0]);
            }
            return Result;
        }
        return m_agreed_quit;
    }

    void ZayView::OnDestroy()
    {
        auto LockID = m_ref_func->m_lock(m_class);
        m_ref_func->m_command(CT_Destroy, "", nullptr, nullptr);
        m_ref_func->m_unlock(LockID);

        BOSS_ASSERT("브로드캐스트 해제에 실패하였습니다", m_class);
        View::Unregist(m_viewclass, m_class->m_view);
    }

    void ZayView::OnSize(sint32 w, sint32 h)
    {
        sint32s WH;
        WH.AtAdding() = w;
        WH.AtAdding() = h;

        auto LockID = m_ref_func->m_lock(m_class);
        m_ref_func->m_command(CT_Size, "", WH, nullptr);
        m_ref_func->m_unlock(LockID);
    }

    void ZayView::OnTick()
    {
        auto LockID = m_ref_func->m_lock(m_class);
        m_ref_func->m_command(CT_Tick, "", nullptr, nullptr);
        m_ref_func->m_unlock(LockID);

        ((ZayController*) m_class)->wakeUpCheck();
    }

    void ZayView::OnRender(sint32 width, sint32 height, float l, float t, float r, float b)
    {
        ZayPanel NewPanel(((ZayController*) m_class)->updater(), width, height, m_touch);
        ZAY_LTRB_SCISSOR(NewPanel, l, t, r, b)
        ZAY_XYWH(NewPanel, -l, -t, r, b)
        {
            auto LockID = m_ref_func->m_lock(m_class);
            Platform::Graphics::UpdateImageRoutineTimeout(10);
            m_ref_func->m_render(NewPanel);
            m_ref_func->m_unlock(LockID);
        }
        ((ZayController*) m_class)->nextFrame();

        // 마우스이벤트 처리
        Touch* CurTouch = (Touch*) m_touch;
        point64 CursorPos;
        if(!Platform::Utility::GetCursorPosInWindow(CursorPos))
        {
            // 포커스 제거
            if(CurTouch->changefocus(nullptr))
                m_class->invalidate();
            // MovingLosed와 DroppingLosed의 처리
            _checklose(GT_Null, nullptr, CursorPos.x, CursorPos.y);
        }
        // 마우스이동없이 엘리먼트 스스로 움직일 가능성
        else OnTouch(TT_Render, 0, CursorPos.x, CursorPos.y);
    }

    void ZayView::OnTouch(TouchType type, sint32 id, sint32 x, sint32 y)
    {
        Touch* CurTouch = (Touch*) m_touch;
        const Element* ScrollElement = nullptr;
        const Element* PressElement = CurTouch->getpress(); // Press가 존재해야 Scroll도 찾음
        const Element& CurElement = CurTouch->get(x, y, PressElement, ScrollElement);
        GestureType SavedType = GT_Null;

        bool NeedUpdate = CurTouch->hovertest(x, y);
        NeedUpdate |= CurTouch->changefocus(&CurElement);
        if(type == TT_Press)
        {
            if(CurTouch->changepress(&CurElement))
            {
                CurTouch->setpress_xy(x, y);
                NeedUpdate = true;
            }
        }
        else if(type == TT_Release)
            NeedUpdate |= CurTouch->changepress(nullptr);

        // 랜더이벤트의 치환
        if(type == TT_Render)
        {
            switch(CurTouch->getlasttouch())
            {
            case TT_Press: type = TT_DraggingIdle; break;
            case TT_ExtendPress: type = TT_ExtendDraggingIdle; break;
            case TT_WheelPress: type = TT_WheelDraggingIdle; break;
            default: type = TT_MovingIdle; break;
            }
        }
        else if(type == TT_Press || type == TT_ExtendPress || type == TT_WheelPress)
            CurTouch->setlasttouch(type);
        else if((CurTouch->getlasttouch() == TT_Press && type == TT_Release)
            || (CurTouch->getlasttouch() == TT_ExtendPress && type == TT_ExtendRelease)
            || (CurTouch->getlasttouch() == TT_WheelPress && type == TT_WheelRelease))
            CurTouch->setlasttouch(TT_Null);

        if(CurElement.m_cb)
        {
            auto LockID = m_ref_func->m_lock(m_class);
            const bool IsSameElement = (&CurElement == PressElement);
            switch(type)
            {
            case TT_Moving: CurElement.m_cb(this, &CurElement, SavedType = GT_Moving, x, y); break;
            case TT_MovingIdle: CurElement.m_cb(this, &CurElement, GT_MovingIdle, x, y); break;
            case TT_Press: CurElement.m_cb(this, &CurElement, SavedType = GT_Pressed, x, y); break;
            case TT_Dragging:
                // 관련 스크롤 존재시 sensitive초과된 Dragging발생한 경우 이벤트전이
                if(ScrollElement && ScrollElement != PressElement && ScrollElement->m_cb
                    && ScrollElement->m_scrollsence < CurTouch->press_to_xy(x, y))
                {
                    if(PressElement && PressElement->m_cb)
                        PressElement->m_cb(this, PressElement, GT_OutReleased, x, y);
                    ScrollElement->m_cb(this, ScrollElement, SavedType = GT_Pressed, x, y);
                    // 백스크롤을 강제로 Press화
                    CurTouch->changepress(ScrollElement);
                    CurTouch->setpress_xy(x, y);
                    NeedUpdate = true;
                }
                else
                {
                    if(!IsSameElement && PressElement && PressElement->m_cb)
                        PressElement->m_cb(this, PressElement, GT_OutDragging, x, y);
                    CurElement.m_cb(this, &CurElement, SavedType = ((IsSameElement)? GT_InDragging : GT_Dropping), x, y);
                }
                break;
            case TT_DraggingIdle:
                if(!IsSameElement && PressElement && PressElement->m_cb)
                    PressElement->m_cb(this, PressElement, GT_OutDraggingIdle, x, y);
                CurElement.m_cb(this, &CurElement, (IsSameElement)? GT_InDraggingIdle : GT_DroppingIdle, x, y);
                break;
            case TT_Release:
                CurElement.m_cb(this, &CurElement, SavedType = ((IsSameElement)? GT_InReleased : GT_Dropped), x, y);
                if(!IsSameElement && PressElement && PressElement->m_cb)
                    PressElement->m_cb(this, PressElement, GT_OutReleased, x, y);
                break;
            case TT_WheelUp: case TT_WheelDown:
            case TT_WheelPress: case TT_WheelDragging: case TT_WheelDraggingIdle: case TT_WheelRelease:
            case TT_ExtendPress: case TT_ExtendDragging: case TT_ExtendDraggingIdle: case TT_ExtendRelease:
                CurElement.m_cb(this, &CurElement, (GestureType) (type - TT_WheelUp + GT_WheelUp), x, y);
                // 스크롤처리
                if(ScrollElement && ScrollElement != PressElement && ScrollElement->m_cb)
                {
                    ScrollElement->m_cb(this, ScrollElement,
                        (GestureType) (type - TT_WheelUp + GT_WheelUp), x, y);
                    NeedUpdate = true;
                }
                // Peek처리
                if(&CurElement != CurTouch->background())
                    CurTouch->background()->m_cb(this, CurTouch->background(),
                        (GestureType) (type - TT_WheelUp + GT_WheelUpPeeked), x, y);
                break;
            case TT_ToolTip:
                CurElement.m_cb(this, &CurElement, GT_ToolTip, x, y);
                break;
            case TT_LongPress:
                CurElement.m_cb(this, &CurElement, GT_LongPressed, x, y);
                break;
            }
            m_ref_func->m_unlock(LockID);
        }

        // MovingLosed와 DroppingLosed의 처리
        if(SavedType != GT_Null)
            _checklose(SavedType, &CurElement, x, y);

        if(NeedUpdate)
            m_class->invalidate();
    }

    void ZayView::OnKey(sint32 code, chars text, bool pressed)
    {
        Touch* CurTouch = (Touch*) m_touch;
        if(const Element* CurElement = CurTouch->getcapture())
        {
            auto LockID = m_ref_func->m_lock(m_class);
            if(pressed)
                CurElement->m_cb(this, CurElement, GT_KeyPressed, code, text[0]);
            else CurElement->m_cb(this, CurElement, GT_KeyReleased, code, text[0]);
            m_ref_func->m_unlock(LockID);
        }
        else
        {
            auto LockID = m_ref_func->m_lock(m_class);
            if(pressed)
                m_ref_func->m_notify(NT_KeyPress, text, sint32o(code), nullptr);
            else m_ref_func->m_notify(NT_KeyRelease, text, sint32o(code), nullptr);
            m_ref_func->m_unlock(LockID);
        }
    }

    void ZayView::_checklose(GestureType type, const Element* element, sint32 x, sint32 y)
    {
        Touch* CurTouch = (Touch*) m_touch;
        if(const Element* OldMover = CurTouch->changemoving(element, type))
        {
            if(OldMover->m_cb)
            {
                auto LockID = m_ref_func->m_lock(m_class);
                OldMover->m_cb(this, OldMover, GT_MovingLosed, x, y);
                m_ref_func->m_unlock(LockID);
            }
        }
        if(const Element* OldDropper = CurTouch->changedropping(element, type))
        {
            if(OldDropper->m_cb)
            {
                auto LockID = m_ref_func->m_lock(m_class);
                OldDropper->m_cb(this, OldDropper, GT_DroppingLosed, x, y);
                m_ref_func->m_unlock(LockID);
            }
        }
    }

    void ZayView::_gesture(GestureType type, sint32 x, sint32 y)
    {
        m_ref_func->m_gesture(type, x, y);
    }

    autorun ZayView::_makefunc(bool isnative, chars viewclass,
        ZayObject::CommandCB c, ZayObject::NotifyCB n, ZayPanel::GestureCB g, ZayPanel::RenderCB r,
        ZayObject::LockCB l, ZayObject::UnlockCB u, ZayObject::AllocCB a, ZayObject::FreeCB f)
    {
        BOSS_ASSERT("중복된 이름의 뷰가 존재합니다", !_accessfunc(viewclass, false));
        Function* NewFunction = _accessfunc(viewclass, true);
        NewFunction->m_isnative = isnative;
        NewFunction->m_viewclass = viewclass;
        NewFunction->m_command = c;
        NewFunction->m_notify = n;
        NewFunction->m_gesture = g;
        NewFunction->m_render = r;
        NewFunction->m_lock = l;
        NewFunction->m_unlock = u;
        NewFunction->m_alloc = a;
        NewFunction->m_free = f;
        return true;
    }

    ZayView::Function* ZayView::_accessfunc(chars viewclass, bool creatable)
    {
        static Map<Function> AllFunctions;
        // 본 함수는 아래와 같은 TLS기법으로 AllFunctions의 싱글톤을 관리하지 않는다.
        // "Map<Function>& AllFunctions = *BOSS_STORAGE_SYS(Map<Function>);"
        // 그 이유는 안드로이드 DLL인 so와 같이 main()진입전의 전역변수등의 초기화수행시
        // main스레드가 아닌 곳에서 수행하기 때문이다.
        // 즉, BOSS_DECLARE_VIEW_CLASS(...)으로 뷰를 자동등록하는 과정의
        // 시나리오에 혼선을 야기하는 결과를 초래하기 때문이다. 또한 안드로이드 디버깅의 경우,
        // main()진입전에 일어난 초기화수행의 과정은 브레이크조차 잡히지 않는다.

        chars ViewName = (viewclass && *viewclass)? viewclass : "_defaultview_";
        return (creatable)? &AllFunctions(ViewName) : AllFunctions.Access(ViewName);
    }

    const void* ZayView::_finder(void* data, chars uiname)
    {
        Touch* CurTouch = (Touch*) ((ZayView*) data)->m_touch;
        return (uiname != nullptr)? CurTouch->find(uiname, 0) : CurTouch->background();
    }

    ZayView::Element::Element()
    {
        m_updateid = -1;
        m_rect.l = 0;
        m_rect.t = 0;
        m_rect.r = 0;
        m_rect.b = 0;
        m_zoom = 1;
        m_cb = nullptr;
        m_subcb = nullptr;
        m_scrollsence = -1;
        m_hoverpass = false;
        m_hoverid = -1;
        m_saved_xy.x = 0;
        m_saved_xy.y = 0;
        m_saved_updateid_for_state = -1;
        m_saved_state = PS_Null;
        m_saved_state_old = PS_Null;
    }

    ZayView::Element::~Element()
    {
    }

    ZayView::Element& ZayView::Element::operator=(const Element& rhs)
    {
        m_updateid = rhs.m_updateid;
        m_name = rhs.m_name;
        m_rect.l = rhs.m_rect.l;
        m_rect.t = rhs.m_rect.t;
        m_rect.r = rhs.m_rect.r;
        m_rect.b = rhs.m_rect.b;
        m_zoom = rhs.m_zoom;
        m_cb = rhs.m_cb;
        m_subcb = rhs.m_subcb;
        m_scrollsence = rhs.m_scrollsence;
        m_hoverpass = rhs.m_hoverpass;
        m_hoverid = rhs.m_hoverid;
        m_saved_xy.x = rhs.m_saved_xy.x;
        m_saved_xy.y = rhs.m_saved_xy.y;
        m_saved_updateid_for_state = rhs.m_saved_updateid_for_state;
        m_saved_state = rhs.m_saved_state;
        m_saved_state_old = rhs.m_saved_state_old;
        return *this;
    }

    PanelState ZayView::Element::GetState(void* touch) const
    {
        if(m_updateid != m_saved_updateid_for_state)
        {
            PanelState StateCollector = PS_Null;
            auto CurTouch = (const ZayView::Touch*) touch;
            if(CurTouch->ishovered(m_hoverid))
                StateCollector = StateCollector | PS_Hovered;
            if(CurTouch->getfocus() == this)
            {
                StateCollector = StateCollector | PS_Focused;
                if(CurTouch->getpress())
                {
                    if(CurTouch->getpress() == this)
                        StateCollector = StateCollector | PS_Pressed;
                    else StateCollector = StateCollector | PS_Dropping;
                }
            }
            if(CurTouch->getpress() == this)
                StateCollector = StateCollector | PS_Dragging;
            // 처리결과를 캐시에 저장
            m_saved_updateid_for_state = m_updateid;
            m_saved_state_old = m_saved_state;
            m_saved_state = StateCollector;
        }
        return m_saved_state;
    }

    bool ZayView::Element::IsStateChanged(void* touch) const
    {
        return (GetState(touch) != m_saved_state_old);
    }

    ZayView::Scroll::Scroll()
    {
        m_pos = nullptr;
        m_size.w = 0;
        m_size.h = 0;
        m_idx.x = 0;
        m_idx.y = 0;
        m_cb = nullptr;
        m_sense = false;
        m_usercontrol = false;
    }

    ZayView::Scroll::~Scroll()
    {
        delete m_pos;
    }

    ZayView::Scroll& ZayView::Scroll::operator=(const Scroll& rhs)
    {
        delete m_pos; m_pos = nullptr;
        if(rhs.m_pos) m_pos = new Tween2D(*rhs.m_pos);
        m_size = rhs.m_size;
        m_idx = rhs.m_idx;
        m_cb = rhs.m_cb;
        m_sense = rhs.m_sense;
        m_usercontrol = rhs.m_usercontrol;
        return *this;
    }

    void ZayView::Scroll::Init(Updater* updater, float x, float y)
    {
        delete m_pos;
        m_pos = new Tween2D(updater);
        m_pos->Reset(x, y);
    }

    void ZayView::Scroll::Move(float x, float y, float sec)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_pos);
        m_pos->MoveTo(x, y, sec);
    }

    void ZayView::Scroll::Moving(float x, float y, float sec, float unitsec, float unitrate)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_pos);
        float OldX = m_pos->x();
        float OldY = m_pos->y();
        while(unitsec < sec && 1 < Math::Distance(x, y, OldX, OldY))
        {
            const float CurX = OldX + (x - OldX) * unitrate;
            const float CurY = OldY + (y - OldY) * unitrate;
            m_pos->MoveTo(CurX, CurY, unitsec);
            OldX = CurX;
            OldY = CurY;
            sec -= unitsec;
        }
        m_pos->MoveTo(x, y, sec);
    }

    void ZayView::Scroll::Stop()
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_pos);
        m_pos->ResetPathes();
    }

    void ZayView::Scroll::Reset(float x, float y)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", m_pos);
        m_pos->Reset(x, y);
    }

    void ZayView::Scroll::SetInfo(sint32 width, sint32 height, sint32 ix, sint32 iy)
    {
        m_size.w = width;
        m_size.h = height;
        m_idx.x = ix;
        m_idx.y = iy;
        if(m_cb) m_cb(m_size, m_idx);
    }

    ZayView::Touch::Touch()
    {
        m_updateid = 0;
        m_hoverid = 0;
        m_captured_uiname.Empty();
        m_block_width = 0;
        m_block_height = 0;
        m_focus = nullptr;
        m_press = nullptr;
        m_moving = nullptr;
        m_dropping = nullptr;
        m_press_x = 0;
        m_press_y = 0;
        m_hover_x = -1;
        m_hover_y = -1;
        m_lasttouch = TT_Null;
    }

    ZayView::Touch::~Touch()
    {
    }

    ZayView::Touch& ZayView::Touch::operator=(const ZayView::Touch& rhs)
    {
        m_updateid = rhs.m_updateid;
        m_hoverid = rhs.m_hoverid;
        m_captured_uiname = rhs.m_captured_uiname;
        m_block_width = rhs.m_block_width;
        m_block_height = rhs.m_block_height;
        m_element = rhs.m_element;
        m_map = rhs.m_map;
        m_cell = rhs.m_cell;
        m_focus = rhs.m_focus;
        m_press = rhs.m_press;
        m_moving = rhs.m_moving;
        m_dropping = rhs.m_dropping;
        m_press_x = rhs.m_press_x;
        m_press_y = rhs.m_press_y;
        m_hover_x = rhs.m_hover_x;
        m_hover_y = rhs.m_hover_y;
        m_scrollmap = rhs.m_scrollmap;
        m_lasttouch = rhs.m_lasttouch;
        return *this;
    }

    void ZayView::Touch::ready(sint32 width, sint32 height)
    {
        m_updateid++;
        m_block_width = (width + Cell::Size - 1) / Cell::Size;
        m_block_height = (height + Cell::Size - 1) / Cell::Size;
        m_element.m_rect.l = 0;
        m_element.m_rect.t = 0;
        m_element.m_rect.r = width;
        m_element.m_rect.b = height;
        m_element.m_cb = GestureCB;
    }

    void ZayView::Touch::update(chars uiname, float l, float t, float r, float b,
        float zoom, ZayPanel::SubGestureCB cb, sint32 scrollsense, bool hoverpass, bool* dirtytest)
    {
        if(uiname == nullptr || uiname[0] == '\0' || r <= l || b <= t)
            return;

        Element& CurElement = m_map(uiname);
        BOSS_ASSERT(String::Format("동일한 uiname(%s)이 여러번 사용되고 있습니다", uiname), CurElement.m_updateid < m_updateid);
        CurElement.m_updateid = m_updateid;
        CurElement.m_name = uiname;
        CurElement.m_rect.l = (sint32) (l * zoom);
        CurElement.m_rect.t = (sint32) (t * zoom);
        CurElement.m_rect.r = (sint32) (r * zoom);
        CurElement.m_rect.b = (sint32) (b * zoom);
        CurElement.m_zoom = zoom;
        CurElement.m_cb = SubGestureCB;
        CurElement.m_subcb = cb;
        CurElement.m_scrollsence = scrollsense;
        CurElement.m_hoverpass = hoverpass;

        const sint32 CellL = Math::Max(0, CurElement.m_rect.l / Cell::Size);
        const sint32 CellT = Math::Max(0, CurElement.m_rect.t / Cell::Size);
        const sint32 CellR = Math::Min(m_block_width, (CurElement.m_rect.r + Cell::Size - 1) / Cell::Size);
        const sint32 CellB = Math::Min(m_block_height, (CurElement.m_rect.b + Cell::Size - 1) / Cell::Size);
        for(sint32 y = CellT; y < CellB; ++y)
        for(sint32 x = CellL; x < CellR; ++x)
        {
            Cell& CurCell = m_cell.AtWherever(y).AtWherever(x);
            if(CurCell.m_updateid != m_updateid)
            {
                CurCell.m_updateid = m_updateid;
                CurCell.m_validlength = 0;
            }
            CurCell.m_elements.AtWherever(CurCell.m_validlength++) = &CurElement;
        }

        if(dirtytest && !*dirtytest)
            *dirtytest = CurElement.IsStateChanged(this);
    }

    const ZayView::Element* ZayView::Touch::background() const
    {
        return &m_element;
    }

    const ZayView::Element* ZayView::Touch::find(chars uiname, sint32 lag) const
    {
        BOSS_ASSERT("uiname이 nullptr입니다", uiname);
        if(const Element* CurElement = m_map.Access(uiname))
        if(m_updateid <= CurElement->m_updateid + lag)
            return CurElement;
        return nullptr;
    }

    const ZayView::Element& ZayView::Touch::get(sint32 x, sint32 y, const Element* press, const Element*& scroll) const
    {
        if(const Cell* CurCell = getcell_const(x, y))
        {
            for(sint32 i = CurCell->m_validlength - 1; 0 <= i; --i)
            {
                const Element& CurElement = *CurCell->m_elements[i];
                const rect128& CurRect = CurElement.m_rect;
                if(CurRect.l <= x && CurRect.t <= y && x < CurRect.r && y < CurRect.b)
                {
                    // 관련된 스크롤조사
                    const Element* ScrollBase = (press)? press : &CurElement;
                    if(ScrollBase->m_scrollsence == -1) // 스크롤이 아닌 눌러진 영역이 있고
                    for(sint32 j = i; 0 <= j; --j) // 터치영역을 포함하여 바닥까지 조사
                    {
                        Element& ScrollElement = *CurCell->m_elements[j];
                        if(ScrollElement.m_scrollsence != -1) // 대상 영역이 스크롤영역이고
                        {
                            const rect128& NextRect = ScrollElement.m_rect;
                            if(NextRect.l <= x && NextRect.t <= y && x < NextRect.r && y < NextRect.b) // 영역에 들어왔고
                            {
                                // 스크롤의 명칭이 CurElement의 명칭에 포함관계일때 스크롤찾음
                                if(!String::Compare(ScrollBase->m_name, ScrollElement.m_name, ScrollElement.m_name.Length()))
                                    scroll = &ScrollElement;
                                break; // 관련된 스크롤을 못찾더라도 스크롤영역을 만나면 break
                            }
                        }
                    }
                    return CurElement;
                }
            }
        }
        return m_element;
    }

    bool ZayView::Touch::hovertest(sint32 x, sint32 y)
    {
        m_hoverid++;
        bool NeedUpdate = false;

        Cell* CurCell = getcell(x, y);
        if(CurCell)
        for(sint32 i = CurCell->m_validlength - 1; 0 <= i; --i)
        {
            Element* CurElement = CurCell->m_elements.At(i);
            const rect128& CurRect = CurElement->m_rect;
            if(CurRect.l <= x && CurRect.t <= y && x < CurRect.r && y < CurRect.b)
            {
                NeedUpdate |= (CurElement->m_hoverid < m_hoverid - 1);
                CurElement->m_hoverid = m_hoverid;
                if(!CurElement->m_hoverpass) break;
            }
        }

        if(!NeedUpdate)
        {
            const Cell* OldCell = CurCell;
            if(x / Cell::Size != m_hover_x / Cell::Size || y / Cell::Size != m_hover_y / Cell::Size)
                OldCell = getcell_const(m_hover_x, m_hover_y);
            if(OldCell)
            for(sint32 i = 0; i < OldCell->m_validlength; ++i)
            {
                const Element* CurElement = OldCell->m_elements[i];
                const rect128& CurRect = CurElement->m_rect;
                if(CurRect.l <= m_hover_x && CurRect.t <= m_hover_y && m_hover_x < CurRect.r && m_hover_y < CurRect.b)
                {
                    NeedUpdate |= (CurElement->m_hoverid == m_hoverid - 1);
                    if(NeedUpdate) break;
                }
            }
        }

        m_hover_x = x;
        m_hover_y = y;
        return NeedUpdate;
    }

    void ZayView::Touch::setcapture(chars uiname)
    {
        BOSS_ASSERT("uiname이 nullptr입니다", uiname);
        m_captured_uiname = uiname;
    }

    void ZayView::Touch::clearcapture()
    {
        m_captured_uiname.Empty();
    }

    const ZayView::Element* ZayView::Touch::getcapture() const
    {
        if(0 < m_captured_uiname.Length())
            return m_map.Access(m_captured_uiname);
        return nullptr;
    }

    PanelState ZayView::Touch::capturetest(chars uiname) const
    {
        BOSS_ASSERT("uiname이 nullptr입니다", uiname);
        if(!m_captured_uiname.Compare(uiname))
            return PS_Captured;
        return PS_Null;
    }

    ZayView::Touch::Cell* ZayView::Touch::getcell(sint32 x, sint32 y)
    {
        const sint32 CellX = x / Cell::Size;
        const sint32 CellY = y / Cell::Size;
        if(0 <= CellX && 0 <= CellY && CellX < m_block_width && CellY < m_block_height)
        {
            if(CellY < m_cell.Count())
            {
                CellRow& CurCellRow = m_cell.At(CellY);
                if(CellX < CurCellRow.Count())
                {
                    Cell& CurCell = CurCellRow.At(CellX);
                    if(CurCell.m_updateid == m_updateid)
                        return &CurCell;
                }
            }
        }
        return nullptr;
    }

    const ZayView::Touch::Cell* ZayView::Touch::getcell_const(sint32 x, sint32 y) const
    {
        const sint32 CellX = x / Cell::Size;
        const sint32 CellY = y / Cell::Size;
        if(0 <= CellX && 0 <= CellY && CellX < m_block_width && CellY < m_block_height)
        {
            if(CellY < m_cell.Count())
            {
                const CellRow& CurCellRow = m_cell[CellY];
                if(CellX < CurCellRow.Count())
                {
                    const Cell& CurCell = CurCellRow[CellX];
                    if(CurCell.m_updateid == m_updateid)
                        return &CurCell;
                }
            }
        }
        return nullptr;
    }

    void ZayView::Touch::GestureCB(ZayView* manager, const Element* data, GestureType type, sint32 x, sint32 y)
    {
        manager->_gesture(type, x, y);
        switch(type)
        {
        case GT_MovingIdle: case GT_InDraggingIdle: case GT_OutDraggingIdle: case GT_DroppingIdle:
        case GT_WheelDraggingIdle: case GT_ExtendDraggingIdle: case GT_WheelDraggingIdlePeeked: case GT_ExtendDraggingIdlePeeked:
            break;
        default:
            data->m_saved_xy.x = x;
            data->m_saved_xy.y = y;
            break;
        }
    }

    void ZayView::Touch::SubGestureCB(ZayView* manager, const Element* data, GestureType type, sint32 x, sint32 y)
    {
        if(data->m_subcb)
        {
            data->m_subcb((ZayObject*) manager->GetClass(), data->m_name, type, x, y);
            switch(type)
            {
            case GT_MovingIdle: case GT_InDraggingIdle: case GT_OutDraggingIdle: case GT_DroppingIdle:
            case GT_WheelDraggingIdle: case GT_ExtendDraggingIdle: case GT_WheelDraggingIdlePeeked: case GT_ExtendDraggingIdlePeeked:
                break;
            default:
                data->m_saved_xy.x = x;
                data->m_saved_xy.y = y;
                break;
            }
        }
    }

    ZayView::Function::Function()
    {
        m_isnative = false;
        m_command = nullptr;
        m_notify = nullptr;
        m_gesture = nullptr;
        m_render = nullptr;
        m_lock = nullptr;
        m_unlock = nullptr;
        m_alloc = nullptr;
        m_free = nullptr;
    }

    ZayView::Function::~Function()
    {
    }
}
