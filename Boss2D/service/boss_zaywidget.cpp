#include <boss.hpp>
#include "boss_zaywidget.hpp"

namespace BOSS
{
    ////////////////////////////////////////////////////////////////////////////////
    // ZayWidgetDocumentP
    ////////////////////////////////////////////////////////////////////////////////
    class ZayWidgetDocumentP : public ZaySonDocument
    {
        BOSS_DECLARE_NONCOPYABLE_INITIALIZED_CLASS(ZayWidgetDocumentP, ZaySonDocument("d"))
    public:
        ZayWidgetDocumentP();
        ~ZayWidgetDocumentP() override;
        ZayWidgetDocumentP(ZayWidgetDocumentP&& rhs);
        ZayWidgetDocumentP& operator=(ZayWidgetDocumentP&& rhs);

    public:
        void PostProcess(const String key, const String value) override;
        void ReserverFlush();

    private:
        static void RemoveCB(chars itemname, payload data);

    private:
        class DownloadReserver
        {
            BOSS_DECLARE_STANDARD_CLASS(DownloadReserver)
        public:
            DownloadReserver();
            ~DownloadReserver();
        public:
            static sint32 OnTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common);
        public:
            bool mHtml;
            String mPath;
            String mFileName;
            String mContent;
        };
        typedef Array<DownloadReserver> DownloadReservers;

    private:
        DownloadReservers mReservers;
        id_tasking mReserverTask;
    };

    ZayWidgetDocumentP::ZayWidgetDocumentP() : ZaySonDocument("d")
    {
        mReserverTask = nullptr;
    }

    ZayWidgetDocumentP::~ZayWidgetDocumentP()
    {
        Tasking::Release(mReserverTask, true);
    }

    ZayWidgetDocumentP::ZayWidgetDocumentP(ZayWidgetDocumentP&& rhs) : ZaySonDocument("d")
    {
        operator=(ToReference(rhs));
    }

    ZayWidgetDocumentP& ZayWidgetDocumentP::operator=(ZayWidgetDocumentP&& rhs)
    {
        ZaySonDocument::operator=(ToReference(rhs));
        mReservers = ToReference(rhs.mReservers);
        Tasking::Release(mReserverTask, true);
        mReserverTask = rhs.mReserverTask;
        rhs.mReserverTask = nullptr;
        return *this;
    }

    void ZayWidgetDocumentP::PostProcess(const String key, const String value)
    {
        // 마지막 키명 추출
        sint32 DotPos = -1, NextDotPos = -1;
        while((NextDotPos = key.Find(DotPos + 1, ".")) != -1) DotPos = NextDotPos;
        const String LastKey = (DotPos == -1)? key : key.Right(key.Length() - (DotPos + 1));

        // 같은 폴더내의 리소스가 최신상태가 아니면 전부 지워준다
        if(!LastKey.Compare("updated_at"))
        {
            String MidPath = key;
            MidPath.Replace(".", "/");
            const String NewUpdate = value;
            if(0 < NewUpdate.Length())
            {
                const String OldUpdate = String::FromAsset("web-cache/" + MidPath + ".txt");
                if(!!OldUpdate.Compare(NewUpdate))
                    Platform::File::Search(Platform::File::RootForAssetsRem() + "web-cache/" + MidPath.Left(MidPath.Length() - LastKey.Length() - 1), RemoveCB, nullptr, true);
                NewUpdate.ToAsset("web-cache/" + MidPath + ".txt", true);
            }
        }
        // 리소스 다운로드예약
        else if(LastKey.Find(0, "_url") != -1)
        {
            const String Value = value;
            String FileExt = "";
            branch;
            jump(!Value.Right(4).CompareNoCase(".jpg")) FileExt = ".jpg";
            jump(!Value.Right(4).CompareNoCase(".mp4")) FileExt = ".mp4";
            jump(!Value.Right(4).CompareNoCase(".ogg")) FileExt = ".ogg";
            jump(!Value.Right(5).CompareNoCase(".webm")) FileExt = ".webm";
            if(0 < FileExt.Length())
            {
                auto& NewReserver = mReservers.AtAdding();
                String MidPath = key;
                NewReserver.mHtml = !!FileExt.Compare(".jpg");
                NewReserver.mPath = "web-cache/" + MidPath.Replace(".", "/").Left(MidPath.Length() - LastKey.Length());
                if(NewReserver.mHtml)
                {
                    NewReserver.mFileName = LastKey + ".html";
                    String NewHtml = String::FromAsset("html/movie.html");
                    NewHtml.Replace("#####URL#####", Value);
                    NewReserver.mContent = NewHtml;
                }
                else
                {
                    NewReserver.mFileName = LastKey + FileExt;
                    NewReserver.mContent = Value;
                }
            }
        }
    }

    void ZayWidgetDocumentP::ReserverFlush()
    {
        if(!mReserverTask)
        {
            mReserverTask = Tasking::Create(DownloadReserver::OnTask);
            // 정보파일
            for(sint32 i = 0, iend = mReservers.Count(); i < iend; ++i)
            {
                if(mReservers[i].mHtml)
                {
                    buffer NewQuery = Buffer::Alloc<DownloadReserver>(BOSS_DBG 1);
                    *((DownloadReserver*) NewQuery) = mReservers[i];
                    Tasking::SendQuery(mReserverTask, NewQuery);
                }
            }
            // 실파일
            for(sint32 i = 0, iend = mReservers.Count(); i < iend; ++i)
            {
                if(!mReservers[i].mHtml)
                {
                    buffer NewQuery = Buffer::Alloc<DownloadReserver>(BOSS_DBG 1);
                    *((DownloadReserver*) NewQuery) = mReservers[i];
                    Tasking::SendQuery(mReserverTask, NewQuery);
                }
            }
        }
        mReservers.Clear();
    }

    void ZayWidgetDocumentP::RemoveCB(chars itemname, payload data)
    {
        if(Platform::File::ExistForDir(itemname))
            Platform::File::Search(itemname, RemoveCB, nullptr, true);
        Platform::File::Remove(WString::FromChars(itemname), true);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ZayWidgetDocumentP::DownloadReserver
    ////////////////////////////////////////////////////////////////////////////////
    ZayWidgetDocumentP::DownloadReserver::DownloadReserver()
    {
        mHtml = false;
    }

    ZayWidgetDocumentP::DownloadReserver::~DownloadReserver()
    {
    }

    sint32 ZayWidgetDocumentP::DownloadReserver::OnTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)
    {
        if(Tasking::IsReleased(common))
            return -1;
        if(buffer CurQuery = query.Dequeue(nullptr))
        {
            auto& CurReserver = *((DownloadReserver*) CurQuery);
            if(!Asset::Exist(CurReserver.mPath + CurReserver.mFileName))
            {
                // HTML파일
                if(CurReserver.mHtml)
                {
                    auto NewAsset = Asset::OpenForWrite(CurReserver.mPath + CurReserver.mFileName, true);
                    Asset::Write(NewAsset, (bytes)(chars) CurReserver.mContent, CurReserver.mContent.Length());
                    Asset::Close(NewAsset);
                }
                // 실파일
                else if(id_curl NewCurl = AddOn::Curl::Create(3000))
                {
                    sint32 BufferSize = 0;
                    bytes DataBuffer = AddOn::Curl::GetBytes(NewCurl, CurReserver.mContent, &BufferSize);
                    auto NewAsset = Asset::OpenForWrite(CurReserver.mPath + CurReserver.mFileName, true);
                    Asset::Write(NewAsset, DataBuffer, BufferSize);
                    Asset::Close(NewAsset);
                    AddOn::Curl::Release(NewCurl);
                }
            }
            Buffer::Free(CurQuery);
            return 5;
        }
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ZayWidget
    ////////////////////////////////////////////////////////////////////////////////
    ZayWidget::ZayWidget()
    {
        mZaySon.SetLogger([this](ZaySon::LogType type, chars title, chars detail)->void
        {
            if(type == ZaySon::LogType::Option)
            {
                if(!String::Compare(title, "Cursor"))
                    Platform::BroadcastNotify("SetCursor", sint32o(Parser::GetInt(detail)));
            }
            else
            {
                Context Json;
                if(type == ZaySon::LogType::Warning)
                    Json.At("type").Set("warning");
                else if(type == ZaySon::LogType::Error)
                    Json.At("type").Set("error");
                else Json.At("type").Set("info");
                Json.At("title").Set(title);
                Json.At("detail").Set(detail);
                if(mPipe) Platform::Pipe::SendJson(mPipe, Json.SaveJson());
                else mPipeReservers.AtAdding() = Json.SaveJson();
            }
        });
        mZaySonModifyTime = 0;
        mResourceCB = nullptr;
        mProcedureID = 0;
        mLastProcedureTime = 0;
        mPipe = nullptr;
        mPipeModifyTime = 0;
    }

    ZayWidget::~ZayWidget()
    {
        Platform::SubWindowProcedure(mProcedureID);
        Platform::Pipe::Close(mPipe);
    }

    ZayWidget::ZayWidget(ZayWidget&& rhs)
    {
        operator=(ToReference(rhs));
    }

    ZayWidget& ZayWidget::operator=(ZayWidget&& rhs)
    {
        mZaySon = ToReference(rhs.mZaySon);
        mZaySonAssetName = ToReference(rhs.mZaySonAssetName);
        mZaySonFileSize = ToReference(rhs.mZaySonFileSize);
        mZaySonModifyTime = ToReference(rhs.mZaySonModifyTime);
        mResourceCB = ToReference(rhs.mResourceCB);
        mProcedureID = ToReference(rhs.mProcedureID); rhs.mProcedureID = 0; // 이관
        mLastProcedureTime = ToReference(rhs.mLastProcedureTime);
        mPipe = rhs.mPipe; rhs.mPipe = nullptr; // 이관
        mPipeModifyTime = ToReference(rhs.mPipeModifyTime);
        return *this;
    }

    ZaySon& ZayWidget::Init(chars viewname, chars assetname, ResourceCB cb)
    {
        BuildComponents(viewname, mZaySon, &mResourceCB);
        BuildGlues(viewname, mZaySon, &mResourceCB);
        mZaySonViewName = viewname;
        mResourceCB = cb;

        if(assetname)
            Reload(assetname);
        return mZaySon;
    }

    void ZayWidget::Reload(chars assetname)
    {
        BOSS_ASSERT("Reload는 Init후 호출가능합니다", 0 < mZaySonViewName.Length());
        if(assetname != nullptr) mZaySonAssetName = assetname;
        BOSS_ASSERT("assetname이 존재해야 합니다", 0 < mZaySonAssetName.Length());

        if(Asset::Exist(mZaySonAssetName, nullptr, &mZaySonFileSize, nullptr, nullptr, &mZaySonModifyTime))
        {
            Context Json(ST_Json, SO_NeedCopy, String::FromAsset(mZaySonAssetName));
            mZaySon.Load(mZaySonViewName, Json);

            Platform::SubWindowProcedure(mProcedureID);
            mProcedureID = Platform::AddWindowProcedure(WE_Tick,
                [](payload data)->void
                {
                    auto Self = (ZayWidget*) data;
                    uint64 CurUpdateCheckTime = Platform::Utility::CurrentTimeMsec();
                    if(Self->mLastProcedureTime + 1000 < CurUpdateCheckTime)
                    {
                        Self->mLastProcedureTime = CurUpdateCheckTime;

                        // Json체크
                        uint64 FileSize = 0, ModifyTime = 0;
                        if(Asset::Exist(Self->mZaySonAssetName, nullptr, &FileSize, nullptr, nullptr, &ModifyTime))
                        if(Self->mZaySonFileSize != FileSize || Self->mZaySonModifyTime != ModifyTime)
                        {
                            Self->mZaySonFileSize = FileSize;
                            Self->mZaySonModifyTime = ModifyTime;
                            Context Json(ST_Json, SO_NeedCopy, String::FromAsset(Self->mZaySonAssetName));
                            Self->mZaySon.Reload(Json);
                            Platform::UpdateAllViews();
                        }

                        // Pipe체크
                        if(!Asset::Exist(Self->mZaySonAssetName + ".pipe", nullptr, nullptr, nullptr, nullptr, &ModifyTime))
                        {
                            if(Self->mPipeModifyTime != 0)
                            {
                                ZayWidgetDOM::UnbindPipe(Self->mPipe);
                                Platform::Pipe::Close(Self->mPipe);
                                Self->mPipe = nullptr;
                                Self->mPipeModifyTime = 0;
                            }
                        }
                        else
                        {
                            if(Self->mPipeModifyTime != ModifyTime)
                            {
                                Self->mPipeModifyTime = ModifyTime;
                                Context Json(ST_Json, SO_NeedCopy, String::FromAsset(Self->mZaySonAssetName + ".pipe"));
                                const String PipeName = Json("pipe").GetText();
                                Platform::Pipe::Close(Self->mPipe);
                                Self->mPipe = Platform::Pipe::Open(PipeName);
                                ZayWidgetDOM::BindPipe(Self->mPipe);
                                // 그동안 쌓인 송신내용
                                for(sint32 i = 0, iend = Self->mPipeReservers.Count(); i < iend; ++i)
                                    Platform::Pipe::SendJson(Self->mPipe, Self->mPipeReservers[i]);
                                Self->mPipeReservers.Clear();
                            }
                        }
                    }
                }, this);
        }
    }

    void ZayWidget::Render(ZayPanel& panel)
    {
        mZaySon.Render(panel);
    }

    bool ZayWidget::TickOnce()
    {
        while(auto NewJson = Platform::Pipe::RecvJson(mPipe))
        {
            const String Type = (*NewJson)("type").GetText();
            branch;
            jump(!String::Compare(Type, "CompFocusIn"))
            {
                const sint32 CompID = (*NewJson)("value").GetInt(-1);
                mZaySon.SetFocusCompID(CompID);
                return true;
            }
            jump(!String::Compare(Type, "CompFocusOut"))
            {
                mZaySon.ClearFocusCompID();
                return true;
            }
        }
        return false;
    }

    void ZayWidget::BuildComponents(chars viewname, ZaySonInterface& interface, ResourceCB* pcb)
    {
        const String ViewName = viewname;

        static auto RenderErrorBox = [](ZayPanel& panel)->void
        {
            ZAY_RGBA(panel, 0, 0, 0, 128)
                panel.fill();
            ZAY_RGB(panel, 255, 0, 0)
            {
                ZAY_INNER(panel, 1)
                    panel.rect(1);
                panel.line(Point(0, 0), Point(panel.w(), panel.h()), 1);
                panel.line(Point(panel.w(), 0), Point(0, panel.h()), 1);
            }
        };

        interface.AddComponent(ZayExtend::ComponentType::Loop, "loop → Repeat",
            ZAY_DECLARE_COMPONENT(panel, params)
            {
                BOSS_ASSERT("No conditional expression for 'loop'", false);
                return panel._push_pass();
            });

        interface.AddComponent(ZayExtend::ComponentType::Layout, "ltrb",
            ZAY_DECLARE_COMPONENT(panel, params)
            {
                if(params.ParamCount() != 4 && params.ParamCount() != 5)
                    return panel._push_pass();
                bool HasError = false;
                auto L = params.Param(0).ToInteger();
                auto T = params.Param(1).ToInteger();
                auto R = params.Param(2).ToInteger();
                auto B = params.Param(3).ToInteger();
                auto Scissor = (params.ParamCount() < 5)? false : params.ParamToBool(4, HasError);
                return panel._push_clip_ui(L, T, R, B, Scissor, params.UIName(), params.MakeGesture());
            },
            "[Left]"
            "[Top]"
            "[Right]"
            "[Bottom]#"
            "[ScissorFlag:false|true]");

        interface.AddComponent(ZayExtend::ComponentType::Layout, "xywh",
            ZAY_DECLARE_COMPONENT(panel, params)
            {
                if(params.ParamCount() != 4 && params.ParamCount() != 5)
                    return panel._push_pass();
                bool HasError = false;
                auto X = params.Param(0).ToInteger();
                auto Y = params.Param(1).ToInteger();
                auto W = params.Param(2).ToInteger();
                auto H = params.Param(3).ToInteger();
                auto Scissor = (params.ParamCount() < 5)? false : params.ParamToBool(4, HasError);
                return panel._push_clip_ui(X, Y, X + W, Y + H, Scissor, params.UIName(), params.MakeGesture());
            },
            "[X]"
            "[Y]"
            "[Width]"
            "[Height]#"
            "[ScissorFlag:false|true]");

        interface.AddComponent(ZayExtend::ComponentType::Layout, "xyrr",
            ZAY_DECLARE_COMPONENT(panel, params)
            {
                if(params.ParamCount() != 4 && params.ParamCount() != 5)
                    return panel._push_pass();
                bool HasError = false;
                auto X = params.Param(0).ToInteger();
                auto Y = params.Param(1).ToInteger();
                auto RX = params.Param(2).ToInteger();
                auto RY = params.Param(3).ToInteger();
                auto Scissor = (params.ParamCount() < 5)? false : params.ParamToBool(4, HasError);
                return panel._push_clip_ui(X - RX, Y - RY, X + RX, Y + RY, Scissor, params.UIName(), params.MakeGesture());
            },
            "[X]"
            "[Y]"
            "[RadiusX]"
            "[RadiusY]#"
            "[ScissorFlag:false|true]");

        interface.AddComponent(ZayExtend::ComponentType::Layout, "inner",
            ZAY_DECLARE_COMPONENT(panel, params)
            {
                if(params.ParamCount() != 1 && params.ParamCount() != 2)
                    return panel._push_pass();
                bool HasError = false;
                auto V = params.Param(0).ToInteger();
                auto Scissor = (params.ParamCount() < 2)? false : params.ParamToBool(1, HasError);
                return panel._push_clip_ui(V, V, panel.w() - V, panel.h() - V, Scissor, params.UIName(), params.MakeGesture());
            },
            "[Value:1]#"
            "[ScissorFlag:false|true]");

        interface.AddComponent(ZayExtend::ComponentType::Layout, "move",
            ZAY_DECLARE_COMPONENT(panel, params)
            {
                if(params.ParamCount() != 2 && params.ParamCount() != 3)
                    return panel._push_pass();
                bool HasError = false;
                auto X = params.Param(0).ToInteger();
                auto Y = params.Param(1).ToInteger();
                auto Scissor = (params.ParamCount() < 3)? false : params.ParamToBool(2, HasError);
                return panel._push_clip_ui(X, Y, panel.w() + X, panel.h() + Y, Scissor, params.UIName(), params.MakeGesture());
            },
            "[X:0]"
            "[Y:0]#"
            "[ScissorFlag:false|true]");

        interface.AddComponent(ZayExtend::ComponentType::Option, "hscroll",
            ZAY_DECLARE_COMPONENT(panel, params, ViewName)
            {
                if(params.ParamCount() != 2 && params.ParamCount() != 3 && params.ParamCount() != 4 && params.ParamCount() != 5)
                    return panel._push_pass();
                auto UIName = ViewName + '.' + params.Param(0).ToText();
                auto ContentSize = params.Param(1).ToInteger();
                auto Sensitive = (params.ParamCount() < 3)? 0 : params.Param(2).ToInteger();
                auto SenseBorder = (params.ParamCount() < 4)? 0 : params.Param(3).ToInteger();
                auto LoopingSize = (params.ParamCount() < 5)? 0 : params.Param(4).ToInteger();
                return panel._push_scroll_ui(ContentSize, 0, UIName,
                    ZAY_GESTURE_VNTXY(v, n, t, x, y)
                    {
                        static sint32 FirstX = 0;
                        if(t == GT_Pressed)
                            FirstX = v->scrollpos(n).x - x;
                        else if(t == GT_InDragging || t == GT_OutDragging)
                        {
                            if(v->isScrollSensing(n))
                                FirstX = v->scrollpos(n).x - x;
                            const sint32 VectorX = (x - v->oldxy(n).x) * 20;
                            v->moveScroll(n, FirstX + x, 0, FirstX + x + VectorX, 0, 2.0, true);
                        }
                    }, Sensitive, SenseBorder, 0 < LoopingSize, LoopingSize, 0);
            },
            "[UIName]"
            "[ContentSize:0]#"
            "[Sensitive:0]"
            "[SenseBorder:0]"
            "[LoopingSize:0]");

        interface.AddComponent(ZayExtend::ComponentType::Option, "color",
            ZAY_DECLARE_COMPONENT(panel, params)
            {
                if(params.ParamCount() != 3 && params.ParamCount() != 4)
                    return panel._push_pass();
                auto R = params.Param(0).ToInteger();
                auto G = params.Param(1).ToInteger();
                auto B = params.Param(2).ToInteger();
                auto A = (params.ParamCount() < 4)? 255 : params.Param(3).ToInteger();
                return panel._push_color(R, G, B, A);
            },
            "[Red:0~255]"
            "[Green:0~255]"
            "[Blue:0~255]#"
            "[Opacity:0~255]");

        interface.AddComponent(ZayExtend::ComponentType::Option, "hexcolor",
            ZAY_DECLARE_COMPONENT(panel, params)
            {
                if(params.ParamCount() != 1 && params.ParamCount() != 2)
                    return panel._push_pass();
                auto HexColor = params.Param(0).ToText();
                auto Alpha = (params.ParamCount() < 2)? 1.0 : params.Param(1).ToFloat();
                if(!String::Compare(HexColor, "#", 1))
                    return panel._push_color(Color(HexColor, Alpha));
                return panel._push_pass();
            },
            "[HexColor:#ffffff]#"
            "[OpacityRate:1.0]");

        interface.AddComponent(ZayExtend::ComponentType::Option, "font",
            ZAY_DECLARE_COMPONENT(panel, params)
            {
                if(params.ParamCount() != 1 && params.ParamCount() != 2)
                    return panel._push_pass();
                auto Size = params.Param(0).ToFloat();
                chars Name = (params.ParamCount() < 2)? nullptr : (chars) params.Param(1).ToText();
                return panel._push_sysfont(Size, Name);
            },
            "[SizeRate:1.0]#"
            "[FontName]");

        interface.AddComponent(ZayExtend::ComponentType::Option, "freefont",
            ZAY_DECLARE_COMPONENT(panel, params)
            {
                if(params.ParamCount() != 1 && params.ParamCount() != 2)
                    return panel._push_pass();
                auto Height = (sint32) params.Param(0).ToInteger();
                chars NickName = (params.ParamCount() < 2)? nullptr : (chars) params.Param(1).ToText();
                return panel._push_freefont(Height, NickName);
            },
            "[Height:10]#"
            "[FontNick]");

        interface.AddComponent(ZayExtend::ComponentType::Option, "zoom",
            ZAY_DECLARE_COMPONENT(panel, params)
            {
                if(params.ParamCount() != 1)
                    return panel._push_pass();
                auto Zoom = params.Param(0).ToFloat();
                return panel._push_zoom(Zoom);
            },
            "[ZoomRate:1.0]");

        interface.AddComponent(ZayExtend::ComponentType::Content, "fill",
            ZAY_DECLARE_COMPONENT(panel, params)
            {
                panel.fill();
                return panel._push_pass();
            });

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParameter, "rect",
            ZAY_DECLARE_COMPONENT(panel, params)
            {
                if(params.ParamCount() != 1)
                    return panel._push_pass();
                auto Thick = params.Param(0).ToFloat();
                panel.rect(Thick);
                return panel._push_pass();
            },
            "[Thick:1]");

        interface.AddComponent(ZayExtend::ComponentType::Content, "circle",
            ZAY_DECLARE_COMPONENT(panel, params)
            {
                panel.circle();
                return panel._push_pass();
            });

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParameter, "icon",
            ZAY_DECLARE_COMPONENT(panel, params, pcb)
            {
                if(params.ParamCount() != 2)
                    return panel._push_pass();
                bool HasError = false;
                auto ImageName = params.Param(0).ToText();
                auto Align = params.ParamToUIAlign(1, HasError);

                const Image* CurImage = nullptr;
                if(*pcb && !String::Compare(ImageName, "r.", 2))
                    CurImage = (*pcb)(((chars) ImageName) + 2);

                if(CurImage)
                    panel.icon(*CurImage, Align);

                if(HasError)
                    RenderErrorBox(panel);
                return panel._push_pass();
            },
            "[RName:r.name]"
            "[UIAlign:LeftTop|CenterTop|RightTop|LeftMiddle|CenterMiddle|RightMiddle|LeftBottom|CenterBottom|RightBottom]");

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParameter, "stretch",
            ZAY_DECLARE_COMPONENT(panel, params, pcb)
            {
                if(params.ParamCount() != 2 && params.ParamCount() != 3)
                    return panel._push_pass();
                bool HasError = false;
                auto ImageName = params.Param(0).ToText();
                auto Rebuild = params.ParamToBool(1, HasError);
                auto Form = (params.ParamCount() < 3)? UISF_Strong : params.ParamToUIStretchForm(2, HasError);

                const Image* CurImage = nullptr;
                if(*pcb && !String::Compare(ImageName, "r.", 2))
                    CurImage = (*pcb)(((chars) ImageName) + 2);

                if(CurImage)
                    panel.stretch(*CurImage, (Rebuild)? Image::Build::AsyncNotNull : Image::Build::Null, Form);

                if(HasError)
                    RenderErrorBox(panel);
                return panel._push_pass();
            },
            "[RName:r.name]"
            "[RebuildFlag:true|false]#"
            "[Form:Strong|Inner|Outer|Width|Height]");

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParameter, "ninepatch",
            ZAY_DECLARE_COMPONENT(panel, params, pcb)
            {
                if(params.ParamCount() != 1)
                    return panel._push_pass();
                auto ImageName = params.Param(0).ToText();

                const Image* CurImage = nullptr;
                if(*pcb && !String::Compare(ImageName, "r.", 2))
                    CurImage = (*pcb)(((chars) ImageName) + 2);

                if(CurImage)
                    panel.ninepatch(*CurImage);
                return panel._push_pass();
            },
            "[RName:r.name]");

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParameter, "pattern",
            ZAY_DECLARE_COMPONENT(panel, params, pcb)
            {
                if(params.ParamCount() != 2 && params.ParamCount() != 3 && params.ParamCount() != 4)
                    return panel._push_pass();
                bool HasError = false;
                auto ImageName = params.Param(0).ToText();
                auto Align = params.ParamToUIAlign(1, HasError);
                auto ReversedXorder = (params.ParamCount() < 3)? false : params.ParamToBool(2, HasError);
                auto ReversedYorder = (params.ParamCount() < 4)? false : params.ParamToBool(3, HasError);

                const Image* CurImage = nullptr;
                if(*pcb && !String::Compare(ImageName, "r.", 2))
                    CurImage = (*pcb)(((chars) ImageName) + 2);

                if(CurImage)
                    panel.pattern(*CurImage, Align, ReversedXorder, ReversedYorder);

                if(HasError)
                    RenderErrorBox(panel);
                return panel._push_pass();
            },
            "[RName:r.name]"
            "[UIAlign:LeftTop|CenterTop|RightTop|LeftMiddle|CenterMiddle|RightMiddle|LeftBottom|CenterBottom|RightBottom]#"
            "[XReverseFlag:false|true]"
            "[YReverseFlag:false|true]");

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParameter, "text",
            ZAY_DECLARE_COMPONENT(panel, params)
            {
                if(params.ParamCount() != 1 && params.ParamCount() != 2 && params.ParamCount() != 3)
                    return panel._push_pass();
                bool HasError = false;
                auto Text = params.Param(0).ToText();
                auto Align = (params.ParamCount() < 2)? UIFA_CenterMiddle : params.ParamToUIFontAlign(1, HasError);
                auto Elide = (params.ParamCount() < 3)? UIFE_None : params.ParamToUIFontElide(2, HasError);

                panel.text(Text, Align, Elide);

                if(HasError)
                    RenderErrorBox(panel);
                return panel._push_pass();
            },
            "[Text]#"
            "[FontAlign:"
                "CenterMiddle|LeftTop|CenterTop|RightTop|JustifyTop|"
                "LeftMiddle|RightMiddle|JustifyMiddle|"
                "LeftAscent|CenterAscent|RightAscent|JustifyAscent|"
                "LeftBottom|CenterBottom|RightBottom|JustifyBottom]"
            "[Elide:None|Left|Center|Right]");

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParameter, "multi_text",
            ZAY_DECLARE_COMPONENT(panel, params)
            {
                if(params.ParamCount() != 2 && params.ParamCount() != 3)
                    return panel._push_pass();
                bool HasError = false;
                auto Text = params.Param(0).ToText();
                auto LineGap = params.Param(1).ToInteger();
                auto Align = (params.ParamCount() < 3)? UIA_LeftTop : params.ParamToUIAlign(2, HasError);

                panel.textbox(Text, LineGap, Align);

                if(HasError)
                    RenderErrorBox(panel);
                return panel._push_pass();
            },
            "[Text]"
            "[LineGap:10]#"
            "[UIAlign:LeftTop|CenterTop|RightTop|LeftMiddle|CenterMiddle|RightMiddle|LeftBottom|CenterBottom|RightBottom]");

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParameter, "button_pod",
            ZAY_DECLARE_COMPONENT(panel, params, pcb, ViewName)
            {
                if(params.ParamCount() != 2 && params.ParamCount() != 3)
                    return panel._push_pass();
                bool HasError = false;
                auto UIName = ViewName + '.' + params.Param(0).ToText();
                auto ImageName = params.Param(1).ToText();
                auto NinePatch = (params.ParamCount() < 3)? false : params.ParamToBool(2, HasError);

                const Image* CurImage = nullptr;
                if(*pcb && !String::Compare(ImageName, "r.", 2))
                {
                    PanelState CurState = panel.state(UIName);
                    if(CurState & PS_Dragging) ImageName += "_p";
                    else if(CurState & PS_Focused) ImageName += "_o";
                    else ImageName += "_d";
                    CurImage = (*pcb)(((chars) ImageName) + 2);
                }

                if(CurImage)
                {
                    if(NinePatch)
                        panel.ninepatch(*CurImage);
                    else panel.stretch(*CurImage, Image::Build::AsyncNotNull);
                }

                if(HasError)
                    RenderErrorBox(panel);
                return panel._push_pass();
            },
            "[UIName]"
            "[RName:r.name]#"
            "[NinepatchFlag:false|true]");

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParameter, "user_content",
            ZAY_DECLARE_COMPONENT(panel, params, pcb)
            {
                BOSS_ASSERT("Register 'AddComponent' as 'user_content' once more and use it for replacement", false);
                return panel._push_pass();
            },
            "[According to customization]");
    }

    void ZayWidget::BuildGlues(chars viewname, ZaySonInterface& interface, ResourceCB* pcb)
    {
        interface.AddGlue("exit",
            ZAY_DECLARE_GLUE(params)
            {
                Platform::Utility::ExitProgram();
            });

        interface.AddGlue("minimize",
            ZAY_DECLARE_GLUE(params)
            {
                Platform::Utility::SetMinimize();
            });

        interface.AddGlue("maximize",
            ZAY_DECLARE_GLUE(params)
            {
                if(Platform::Utility::IsFullScreen())
                    Platform::Utility::SetNormalWindow();
                else Platform::Utility::SetFullScreen();
            });
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ZayWidgetDOM
    ////////////////////////////////////////////////////////////////////////////////
    ZayWidgetDOM::ZayWidgetDOM() : mDocument(new ZayWidgetDocumentP())
    {
    }

    ZayWidgetDOM::~ZayWidgetDOM()
    {
        delete mDocument;
    }

    void ZayWidgetDOM::Add(chars variable, chars formula)
    {
        auto& Self = ST();
        Self.mDocument->Add(variable, formula);
    }

    void ZayWidgetDOM::AddJson(const Context& json)
    {
        auto& Self = ST();
        Self.mDocument->AddJson(json);
    }

    void ZayWidgetDOM::AddFlush()
    {
        auto& Self = ST();
        Self.mDocument->AddFlush();
        Self.SendFlush();
    }

    void ZayWidgetDOM::ReserverFlush()
    {
        auto& Self = ST();
        Self.mDocument->ReserverFlush();
    }

    void ZayWidgetDOM::Update(chars variable, chars formula)
    {
        auto& Self = ST();
        Self.mDocument->Update(variable, formula);
        Self.SendFlush();
    }

    void ZayWidgetDOM::UpdateJson(const Context& json)
    {
        auto& Self = ST();
        Self.mDocument->UpdateJson(json);
        Self.SendFlush();
    }

    void ZayWidgetDOM::BindPipe(id_pipe pipe)
    {
        auto& Self = ST();
        auto& NewPipe = Self.mPipeMap[PtrToUint64(pipe)];
        NewPipe.mRefPipe = pipe;
        NewPipe.mMsec = Platform::Utility::CurrentTimeMsec();

        // 최초 전달
        Self.mDocument->CheckUpdatedSolvers(0, [pipe](const Solver* solver)->void
        {SendToPipe(pipe, solver);});
    }

    void ZayWidgetDOM::UnbindPipe(id_pipe pipe)
    {
        auto& Self = ST();
        Self.mPipeMap.Remove(PtrToUint64(pipe));
    }

    void ZayWidgetDOM::SendFlush()
    {
        const uint64 CurMsec = Platform::Utility::CurrentTimeMsec();
        for(sint32 i = 0, iend = mPipeMap.Count(); i < iend; ++i)
        {
            auto CurPipe = *mPipeMap.AccessByOrder(i);
            if(CurPipe.mMsec < CurMsec)
            {
                CurPipe.mMsec = CurMsec;
                id_pipe PipeID = CurPipe.mRefPipe;
                mDocument->CheckUpdatedSolvers(CurMsec, [PipeID](const Solver* solver)->void
                {SendToPipe(PipeID, solver);});
            }
        }
    }

    void ZayWidgetDOM::SendToPipe(id_pipe pipe, const Solver* solver)
    {
        Context Json;
        Json.At("type").Set("dom");
        Json.At("variable").Set(solver->linked_variable());
        Json.At("result").Set(solver->result().ToText(true));
        Json.At("formula").Set(solver->parsed_formula());
        Platform::Pipe::SendJson(pipe, Json.SaveJson());
    }
}
