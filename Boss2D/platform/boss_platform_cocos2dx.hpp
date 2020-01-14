#pragma once
#include <platform/boss_platform.hpp>
#include <platform/boss_platform_impl.hpp>

#ifdef BOSS_PLATFORM_COCOS2DX

    // 코코스2DX #include관련
    #undef ssize_t
    #define ssize_t __w64 long

    #if BOSS_WINDOWS
        #define WIN32_LEAN_AND_MEAN
        #include <windows.h>
        #include <tchar.h>
    #endif

    #include "CCStdC.h"
    #include "cocos2d.h"
    #include "cocostudio/CocoStudio.h"
    #include "ui/CocosGUI.h"
    #include "base/CCUtils.h"

    class MainData;
    class MainWindow;

    extern MainData* g_data;
    extern MainWindow* g_window;
    extern cocos2d::Node* g_view;

    class ViewAPI
    {
    public:
        enum ParentType {PT_Null, PT_GenericView};

    public:
        inline ParentType getParentType() const
        {
            return m_parent_type;
        }

        inline void* getParent() const
        {
            BOSS_ASSERT("잘못된 시나리오입니다", (m_parent_buf != nullptr) != (m_parent_ptr != nullptr));
            return (void*) (((ublock) m_parent_buf) | ((ublock) m_parent_ptr));
        }

        inline void renewParent(void* ptr)
        {
            m_parent_buf = nullptr;
            m_parent_ptr = ptr;

            if(getParent())
                sendCreate();
        }

        inline bool parentIsPtr() const
        {
            return (m_parent_ptr != nullptr);
        }

        inline bool hasViewManager() const
        {
            return (m_view_manager != nullptr);
        }

        void setViewAndCreateAndSize(h_view view)
        {
            BOSS_ASSERT("잘못된 시나리오입니다", m_view_manager);
            g_view = getWidget();
            m_view_manager->SetView(view);

            sendCreate();
            sendSizeWhenValid();
        }

        h_view changeViewManagerAndDestroy(View* manager)
        {
            sendDestroy();
            h_view OldViewHandle = h_view::null();
            if(m_view_manager)
            {
                OldViewHandle = m_view_manager->SetView(h_view::null());
                delete m_view_manager;
            }

            m_view_manager = manager;
            m_view_manager->SetCallback(m_view_cb, m_view_data);
            return OldViewHandle;
        }

        inline void setNextViewManager(View* manager)
        {
            delete m_next_manager;
            m_next_manager = manager;
        }

        inline void changeViewData(cocos2d::Node* data)
        {
            m_view_data = data;
            if(m_view_manager)
                m_view_manager->SetCallback(m_view_cb, m_view_data);
        }

        inline ViewClass* getClass() const
        {
            return (ViewClass*) m_view_manager->GetClass();
        }

        inline cocos2d::Node* getWidget() const
        {
            if(m_view_manager)
            {
                if(m_view_data)
                    return m_view_data;
                if(m_view_manager->IsNative())
                    return (cocos2d::Node*) m_view_manager->GetClass();
            }
            return (cocos2d::Node*) getParent();
        }

        inline void render(sint32 width, sint32 height, const Rect& viewport)
        {
            BOSS_ASSERT("잘못된 시나리오입니다", m_view_manager);
            g_view = getWidget();
            m_view_manager->OnRender(width, height, viewport.l, viewport.t, viewport.r, viewport.b);

            if(m_next_manager)
            {
                h_view OldViewHandle = changeViewManagerAndDestroy(m_next_manager);
                setViewAndCreateAndSize(OldViewHandle);
                m_next_manager = nullptr;
            }
        }

        inline void touch(TouchType type, sint32 id, sint32 x, sint32 y)
        {
            BOSS_ASSERT("잘못된 시나리오입니다", m_view_manager);
            g_view = getWidget();
            m_view_manager->OnTouch(type, id, x, y);
        }

        inline void sendCreate()
        {
            if(m_view_manager != nullptr)
            {
                g_view = getWidget();
                m_view_manager->OnCreate();
            }
        }

        inline bool canQuit()
        {
            if(m_view_manager != nullptr)
            {
                g_view = getWidget();
                return m_view_manager->OnCanQuit();
            }
            return true;
        }

        inline void sendDestroy()
        {
            if(m_view_manager != nullptr)
            {
                g_view = getWidget();
                m_view_manager->OnDestroy();
            }
        }

        inline void sendSizeWhenValid()
        {
            if(0 < m_width && 0 < m_height)
            if(m_view_manager != nullptr)
            {
                g_view = getWidget();
                m_view_manager->OnSize(m_width, m_height);
            }
        }

        inline void sendTick() const
        {
            if(m_view_manager != nullptr)
            {
                g_view = getWidget();
                m_view_manager->OnTick();
            }
        }

        inline void sendNotify(chars topic, id_share in, id_cloned_share* out) const
        {
            BOSS_ASSERT("잘못된 시나리오입니다", m_view_manager);
            g_view = getWidget();
            m_view_manager->SendNotify("(platform)", topic, in, out);
        }

    public:
        void resize(sint32 width, sint32 height, bool doCommand)
        {
            m_width = width;
            m_height = height;
            if(doCommand) sendSizeWhenValid();
        }
        sint32 width() const {return m_width;}
        sint32 height() const {return m_height;}

        void paint()
        {
            // for assert dialog
            if(CurPainter()) return;

            getWidget()->removeAllChildren();
            auto NewClipper = cocos2d::ClippingRectangleNode::create();
            getWidget()->addChild(NewClipper);

            float ViewportL = 0;
            float ViewportT = 0;
            float ViewportR = 0;
            float ViewportB = 0;
            if(auto Layout = (cocos2d::ui::Layout*) getWidget()->getParent())
            if(auto Scroll = dynamic_cast<cocos2d::ui::ScrollView*>(Layout->getParent()))
            {
                const cocos2d::Vec2& LayoutPos = Layout->getPosition();
                const cocos2d::Size& ScrollSize = Scroll->getContentSize();
                const float PosL = LayoutPos.x - m_width / 2.0f;
                const float PosT = ScrollSize.height - LayoutPos.y - m_height / 2.0f;

                ViewportL = Math::MaxF(0, PosL) - PosL;
                ViewportT = Math::MaxF(0, PosT) - PosT;
                ViewportR = Math::MinF(PosL + m_width, ScrollSize.width) - PosL;
                ViewportB = Math::MinF(PosT + m_height, ScrollSize.height) - PosT;
            }

            CurPainter() = NewClipper;
            render(m_width, m_height, Rect(ViewportL, ViewportT, ViewportR, ViewportB));
            CurPainter() = nullptr;
        }
        void nextPaint() {}

    public:
        static inline cocos2d::Node*& CurPainter()
        {static cocos2d::Node* _ = nullptr; return _;}
        static inline cocos2d::Color4B& CurColor()
        {static cocos2d::Color4B _(0, 0, 0, 0); return _;}
        static inline cocos2d::FontDefinition& CurFont()
        {static cocos2d::FontDefinition _; return _;}
        static inline float CalcY(float y)
        {return g_view->getContentSize().height - y;}

    public:
        ViewAPI(ParentType type, buffer buf, ViewManager* manager, ViewClass::UpdaterCB cb, cocos2d::Node* data)
        {
            m_parent_type = type;
            m_parent_buf = buf;
            m_parent_ptr = nullptr;
            m_view_manager = manager;
            m_next_manager = nullptr;
            m_view_cb = cb;
            m_view_data = data;
            if(manager)
                manager->SetCallback(m_view_cb, m_view_data);

            m_width = 0;
            m_height = 0;
        }

        ~ViewAPI()
        {
            if(getParent()) sendDestroy();
            Buffer::Free(m_parent_buf);
            delete m_view_manager;
            delete m_next_manager;
        }

        ViewAPI(const ViewAPI& rhs) = delete;
        ViewAPI& operator=(const ViewAPI& rhs) = delete;

    private:
        ParentType m_parent_type;
        buffer m_parent_buf;
        void* m_parent_ptr;
        View* m_view_manager;
        View* m_next_manager;
        ViewClass::UpdaterCB m_view_cb;
        cocos2d::Node* m_view_data;

    private:
        sint32 m_width;
        sint32 m_height;
    };

    class CCHelper
    {
    public:
        typedef cocos2d::Scene* (*CreateCB)();

    public:
        CCHelper() {m_cb = nullptr;}
        CCHelper(CreateCB cb) {m_cb = cb;}
        ~CCHelper() {}

    public:
        void SetName(chars name) {m_name = name;}
        void SetCreator(CreateCB cb) {m_cb = cb;}

    public:
        cocos2d::Scene* CreateWithBoss()
        {
            BOSS_ASSERT("m_cb가 nullptr입니다", m_cb);
            cocos2d::Scene* NewScene = m_cb();
            BossCreateCore(NewScene->getChildren());
            return NewScene;
        }

        static void ResizeWithBoss(cocos2d::Node* node, sint32 w, sint32 h)
        {
            node->setContentSize(cocos2d::Size(w, h));
            cocos2d::ui::Helper::doLayout(node);
            Platform::PassAllViews([](void* api, payload data)->bool
            {ResizeViewWithBoss((ViewAPI*) api); return false;}, nullptr);
        }

        static void ResizeViewWithBoss(ViewAPI* api)
        {
            if(auto Layer = (cocos2d::Layer*) api->getWidget())
            {
                Layer->setPosition(-api->width() / 2, -api->height() / 2);
                Layer->setContentSize(cocos2d::Size(api->width(), api->height()));
                cocos2d::ui::Helper::doLayout(Layer);

                if(auto Layout = Layer->getParent())
                if(auto Scroll = dynamic_cast<cocos2d::ui::ScrollView*>(Layout->getParent()))
                {
                    Scroll->getInnerContainer()->ignoreAnchorPointForPosition(true);
                    Scroll->getInnerContainer()->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
                    Scroll->setInnerContainerSize(cocos2d::Size(api->width(), api->height()));
                }
            }
        }

    private:
        void BossCreateCore(const cocos2d::Vector<cocos2d::Node*>& children)
        {
            for(auto child : children)
            {
                chars CurChildName = child->getName().c_str();
                if(!String::Compare("boss:", CurChildName, 4))
                {
                    Context Json(ST_Json, SO_OnlyReference, &CurChildName[4]);
                    chars ViewClass = Json("view").GetString(nullptr);
                    h_view NewView = Platform::CreateView("", 300, 300, nullptr, ViewClass);

                    if(auto NewLayer = (cocos2d::Layer*) ((ViewAPI*) NewView->get())->getParent())
                    {
                        NewLayer->setName(ViewClass);
                        NewLayer->setPosition(-600, -400);
                        NewLayer->setContentSize(cocos2d::Size(1200, 800));
                        cocos2d::ui::Helper::doLayout(NewLayer);

                        if(auto View = dynamic_cast<cocos2d::ui::ScrollView*>(child))
                        {
                            View->getInnerContainer()->addChild(NewLayer);
                            View->setScrollBarEnabled(false);
                        }
                        else child->addChild(NewLayer);
                    }
                }
                else BossCreateCore(child->getChildren());
            }
        }

    public:
        static CCHelper* Pool(chars scene, bool creatable)
        {
            static Map<CCHelper> AllFunctions;
            BOSS_ASSERT("파라미터가 nullptr입니다", scene);
            return (creatable)? &AllFunctions(scene) : AllFunctions.Access(scene);
        }

        static CCHelper* Blank()
        {
            static CCHelper _([]()->cocos2d::Scene*{return cocos2d::Scene::create();});
            return &_;
        }

        static CCHelper*& Intro()
        {
            static CCHelper* _ = Blank();
            return _;
        }

    private:
        String m_name;
        CreateCB m_cb;
    };

    class MainData
    {
    public:
        void initForGL()
        {
        }

        void initForMDI()
        {
        }

    public:
        buffer regist(buffer handle)
        {
            m_handles.AtAdding() = handle;
            return handle;
        }

        void unregist(buffer handle)
        {
            if(handle)
            {
                // 해당 항목제거
                for(sint32 i = m_handles.Count() - 1; 0 <= i; --i)
                    if(m_handles[i] == handle)
                    {
                        m_handles.At(i) = nullptr;
                        break;
                    }
                // 배열최적화
                while(0 < m_handles.Count() && !m_handles[-1])
                    m_handles.SubtractionOne();
            }
        }

    public:
        MainData()
        {
        }

        ~MainData()
        {
            for(sint32 i = m_handles.Count() - 1; 0 <= i; --i)
                m_handles[i].set_buf(nullptr);
        }

    private:
        Array<buffer> m_handles;
    };

    class GenericView : public cocos2d::Layer
    {
    public:
        virtual bool init() override
        {
            return Layer::init();
        }

        virtual void setContentSize(const cocos2d::Size& contentSize) override
        {
            Layer::setContentSize(contentSize);
            m_api->resize((sint32) contentSize.width, (sint32) contentSize.height, true);
        }

        virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4 &transform, uint32_t flags) override
        {
            Layer::draw(renderer, transform, flags);
            cocos2d::kmGLPushMatrix();
            cocos2d::kmGLLoadMatrix(&transform);

            if(m_api->hasViewManager())
            {
                if(ViewClass* View = m_api->getClass())
                {
                    sint32 w = m_api->width();
                    sint32 h = m_api->height();
                    if(View->getResizingValue(w, h))
                    {
                        m_api->resize(w, h, false);
                        CCHelper::ResizeViewWithBoss(m_api);
                    }
                }
                m_api->paint();
            }
            else Layer::draw(renderer, transform, flags);
            m_api->nextPaint();
        }

        virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event) override
        {
            const cocos2d::Vec2& Location = getParent()->convertToNodeSpace(touch->getLocation());
            const cocos2d::Rect& BoundingBox = getBoundingBox();
            if(BoundingBox.containsPoint(Location))
            {
                m_touchPressed = true;
                m_api->touch(TT_Press, 0,
                    Location.x - BoundingBox.getMinX(), BoundingBox.getMaxY() - Location.y);
            }
            return false;
        }
        virtual void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event) override {}
        virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event) override {}
        virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *unused_event) override {}

        bool hookTouchMoved(float x, float y)
        {
            const cocos2d::Vec2& Location = getParent()->convertToNodeSpace(cocos2d::Vec2(x, y));
            const cocos2d::Rect& BoundingBox = getBoundingBox();
            if(m_touchPressed || BoundingBox.containsPoint(Location))
            {
                m_api->touch((m_touchPressed)? TT_Dragging : TT_Moving, 0,
                    Location.x - BoundingBox.getMinX(), BoundingBox.getMaxY() - Location.y);
                return true;
            }
            return false;
        }

        bool hookTouchEnded(float x, float y)
        {
            if(m_touchPressed)
            {
                m_touchPressed = false;
                const cocos2d::Vec2& Location = getParent()->convertToNodeSpace(cocos2d::Vec2(x, y));
                const cocos2d::Rect& BoundingBox = getBoundingBox();
                m_api->touch(TT_Release, 0,
                    Location.x - BoundingBox.getMinX(), BoundingBox.getMaxY() - Location.y);
                return true;
            }
            return false;
        }

        void onTick(float deltaTime)
        {
            m_api->sendTick();
        }

    protected:
        static void updater(void* data, sint32 count) {}

    public:
        GenericView()
        {
            m_api = nullptr;
            m_name = "";
            m_firstwidth = 0;
            m_firstheight = 0;
            m_touchPressed = false;
        }

        GenericView(View* manager, chars name, sint32 width, sint32 height)
        {
            BOSS_DECLARE_BUFFERED_CLASS(BufferedViewAPI, ViewAPI, PT_Null, nullptr, nullptr, nullptr, nullptr);
            buffer NewAPI = Buffer::AllocNoConstructorOnce<BufferedViewAPI>(BOSS_DBG 1);
            BOSS_CONSTRUCTOR(NewAPI, 0, ViewAPI, ViewAPI::PT_GenericView, (buffer) this, manager, updater, this);
            m_api = (ViewAPI*) NewAPI;

            m_name = name;
            m_firstwidth = width;
            m_firstheight = height;
            m_touchPressed = false;

            init();
            setTouchMode(cocos2d::Touch::DispatchMode::ONE_BY_ONE);
            setTouchEnabled(true);
            schedule(schedule_selector(GenericView::onTick), 0.1f);
        }

        virtual ~GenericView()
        {
            unschedule(schedule_selector(GenericView::onTick));
            if(m_api && m_api->parentIsPtr())
                Buffer::Free((buffer) m_api);
        }

        GenericView(const GenericView* rhs) {BOSS_ASSERT("사용금지", false);}
        GenericView& operator=(const GenericView& rhs) {BOSS_ASSERT("사용금지", false); return *this;}

        CREATE_FUNC(GenericView);

    public:
        ViewAPI* m_api;

    private:
        String m_name;
        sint32 m_firstwidth;
        sint32 m_firstheight;
        bool m_touchPressed;
    };

    class MainWindow : private cocos2d::Application
    {
    public:
        MainWindow()
        {
            g_data = new MainData();
            WindowName = "noname";
            ClientSize.w = 480;
            ClientSize.h = 320;
            s_self() = this;
        }

        virtual ~MainWindow()
        {
            delete g_data;
            g_data = nullptr;
            if(s_self() == this)
                s_self() = nullptr;

            #if BOSS_WINDOWS
                UnhookWindowsHookEx(s_hook_resize_handle());
                UnhookWindowsHookEx(s_hook_mouse_handle());
            #endif
        }

    private:
        #if BOSS_WINDOWS
            static MainWindow*& s_self() {static MainWindow* _ = nullptr; return _;}
            static HWND& s_win_handle() {static HWND _ = nullptr; return _;}
            static HHOOK& s_hook_resize_handle() {static HHOOK _ = nullptr; return _;}
            static HHOOK& s_hook_mouse_handle() {static HHOOK _ = nullptr; return _;}
        #endif

    public:
        void resizeByClient(sint32 client_w, sint32 client_h)
        {
            auto director = cocos2d::Director::getInstance();
            auto glview = director->getOpenGLView();

            // 윈도우 재설정(forced code)
            const cocos2d::Size& framesize = glview->getFrameSize();
            const cocos2d::Size& designsize = glview->getDesignResolutionSize();
            ((cocos2d::Size*) &framesize)->setSize(client_w, client_h);
            ((cocos2d::Size*) &designsize)->setSize(client_w, client_h);
            class GLView_forced : public cocos2d::GLView
            {public: void updateDesignResolutionSize_forced() {updateDesignResolutionSize();}};
            ((GLView_forced*) glview)->updateDesignResolutionSize_forced();

            // 클라이언트 사이즈계산
            ClientSize.w = client_w;
            ClientSize.h = client_h;
            recalcLayout(director->getRunningScene());

            director->drawScene();
        }

        void resizeByWindow(sint32 window_w, sint32 window_h)
        {
            const sint32 CxFrame = GetSystemMetrics(SM_CXSIZEFRAME);
            const sint32 CyFrame = GetSystemMetrics(SM_CYSIZEFRAME);
            const sint32 CyCaption = GetSystemMetrics(SM_CYCAPTION);
            resizeByClient(window_w - CxFrame * 2, window_h - CyFrame * 2 - CyCaption);
        }

        void recalcLayout(cocos2d::Scene* scene)
        {
            // 해당씬
            CCHelper::ResizeWithBoss(scene, ClientSize.w, ClientSize.h);

            // 자식노드
            cocos2d::Vector<cocos2d::Node*>& childs = scene->getChildren();
            for(sint32 i = 0, iend = childs.size(); i < iend; ++i)
            {
                cocos2d::Node* curchild = childs.at(i);
                CCHelper::ResizeWithBoss(curchild, ClientSize.w, ClientSize.h);
            }
        }

        virtual void initGLContextAttrs()
        {
            GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};
            cocos2d::GLView::setGLContextAttrs(glContextAttrs);
        }

        virtual bool applicationDidFinishLaunching()
        {
            auto director = cocos2d::Director::getInstance();
            auto glview = director->getOpenGLView();
            if(!glview)
            {
                glview = cocos2d::GLViewImpl::createWithRect((chars) WindowName,
                    cocos2d::Rect(0, 0, ClientSize.w, ClientSize.h));
                director->setOpenGLView(glview);
            }

            // 윈도우 후킹
            #if BOSS_WINDOWS
                s_win_handle() = glview->getWin32Window();
                LONG style = GetWindowLong(s_win_handle(), GWL_STYLE);
                SetWindowLong(s_win_handle(), GWL_STYLE, style | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
                s_hook_resize_handle() = SetWindowsHookEx(WH_CALLWNDPROCRET | WH_CALLWNDPROC,
                    [](int code, WPARAM wParam, LPARAM lParam)->LRESULT
                    {
                        if(code == HC_ACTION)
                        {
                            PCWPRETSTRUCT pmsg = (PCWPRETSTRUCT) lParam;
                            if(pmsg->hwnd == s_win_handle())
                            {
                                if(pmsg->message == WM_SIZE)
                                {
                                    WORD width = LOWORD(pmsg->lParam);
                                    WORD height = HIWORD(pmsg->lParam);
                                    s_self()->resizeByClient(width, height);
                                }
                                else if(pmsg->message == WM_SIZING)
                                {
                                    LPRECT prect = (LPRECT) pmsg->lParam;
                                    s_self()->resizeByWindow(prect->right - prect->left, prect->bottom - prect->top);
                                }
                            }
                        }
                        return CallNextHookEx(s_hook_resize_handle(), code, wParam, lParam);
                    }, NULL, GetCurrentThreadId());
                s_hook_mouse_handle() = SetWindowsHookEx(WH_MOUSE,
                    [](int code, WPARAM wParam, LPARAM lParam)->LRESULT
                    {
                        if(code == HC_ACTION)
                        {
                            PMOUSEHOOKSTRUCT pmsg = (PMOUSEHOOKSTRUCT) lParam;
                            if(pmsg->hwnd == s_win_handle())
                            {
                                if(wParam == WM_MOUSEMOVE || wParam == WM_LBUTTONUP)
                                {
                                    POINT MousePos = {pmsg->pt.x, pmsg->pt.y};
                                    ScreenToClient(pmsg->hwnd, &MousePos);
                                    RECT ClientRect;
                                    GetClientRect(pmsg->hwnd, &ClientRect);
                                    MousePos.y = ClientRect.bottom - ClientRect.top - MousePos.y;
                                    struct Payload {float x; float y; bool moved;} Param
                                        = {MousePos.x, MousePos.y, (wParam == WM_MOUSEMOVE)};
                                    BOSS::Platform::PassAllViews([](void* api, payload data)->bool
                                    {
                                        ViewAPI* CurApi = (ViewAPI*) api;
                                        Payload* Param = (Payload*) data;
                                        if(Param->moved)
                                            return ((GenericView*) CurApi->getWidget())->hookTouchMoved(Param->x, Param->y);
                                        return ((GenericView*) CurApi->getWidget())->hookTouchEnded(Param->x, Param->y);
                                    }, &Param);
                                }
                            }
                        }
                        return CallNextHookEx(s_hook_mouse_handle(), code, wParam, lParam);
                    }, NULL, GetCurrentThreadId());
            #endif

            director->setDisplayStats(false);
            director->setAnimationInterval(1 / 60.0f);
            cocos2d::FileUtils::getInstance()->addSearchPath("res");

            // Intro씬 탑재
            cocos2d::Scene* scene = CCHelper::Intro()->CreateWithBoss();
            director->runWithScene(scene);
            recalcLayout(scene);
            return true;
        }

        virtual void applicationDidEnterBackground()
        {
            cocos2d::Director::getInstance()->stopAnimation();
        }

        virtual void applicationWillEnterForeground()
        {
            cocos2d::Director::getInstance()->startAnimation();
        }

    public:
        String WindowName;
        size64 ClientSize;
    };

#endif
