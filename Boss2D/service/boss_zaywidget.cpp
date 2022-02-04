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

    private:
        static void RemoveCB(chars itemname, payload data);
    };

    ZayWidgetDocumentP::ZayWidgetDocumentP() : ZaySonDocument("d")
    {
    }

    ZayWidgetDocumentP::~ZayWidgetDocumentP()
    {
    }

    ZayWidgetDocumentP::ZayWidgetDocumentP(ZayWidgetDocumentP&& rhs) : ZaySonDocument("d")
    {
        operator=(ToReference(rhs));
    }

    ZayWidgetDocumentP& ZayWidgetDocumentP::operator=(ZayWidgetDocumentP&& rhs)
    {
        ZaySonDocument::operator=(ToReference(rhs));
        return *this;
    }

    void ZayWidgetDocumentP::RemoveCB(chars itemname, payload data)
    {
        if(Platform::File::ExistForDir(itemname))
            Platform::File::Search(itemname, RemoveCB, nullptr, true);
        Platform::File::Remove(WString::FromChars(itemname), true);
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
                Json.At("title").Set(title);
                Json.At("detail").Set(detail);
                if(type == ZaySon::LogType::Warning)
                    Json.At("type").Set("warning");
                else if(type == ZaySon::LogType::Error)
                    Json.At("type").Set("error");
                else if(type == ZaySon::LogType::Info)
                    Json.At("type").Set("info");
                else Json.At("type").Set("unknown");
                if(mPipe) Platform::Pipe::SendJson(mPipe, Json.SaveJson());
                else mPipeReservers.AtAdding() = Json.SaveJson();
            }
        });
        mZaySonModifyTime = 0;
        mResourceCB = nullptr;
        mProcedureID = 0;
        mPipe = nullptr;
        mPipeModifyTime = 0;
    }

    ZayWidget::~ZayWidget()
    {
        Platform::SubProcedure(mProcedureID);
        ZayWidgetDOM::UnbindPipe(mPipe);
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

            Platform::SubProcedure(mProcedureID);
            mProcedureID = Platform::AddProcedure(PE_1SEC,
                [](payload data)->void
                {
                    auto Self = (ZayWidget*) data;
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
                            ZayWidgetDOM::UnbindPipe(Self->mPipe);
                            Platform::Pipe::Close(Self->mPipe);
                            Self->mPipe = Platform::Pipe::Open(PipeName);
                            ZayWidgetDOM::BindPipe(Self->mPipe);
                            // 그동안 쌓인 송신내용
                            for(sint32 i = 0, iend = Self->mPipeReservers.Count(); i < iend; ++i)
                                Platform::Pipe::SendJson(Self->mPipe, Self->mPipeReservers[i]);
                            Self->mPipeReservers.Clear();
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
                panel.line(Point(0.5, 0.5), Point(panel.w() - 0.5, panel.h() - 0.5), 1);
                panel.line(Point(panel.w() - 0.5, 0.5), Point(0.5, panel.h() - 0.5), 1);
            }
        };

        interface.AddComponent(ZayExtend::ComponentType::Code, "code → Input",
            ZAY_DECLARE_COMPONENT(panel, params)
            {
                BOSS_ASSERT("No conditional expression for 'code'", false);
                return panel._push_pass();
            });

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

        interface.AddComponent(ZayExtend::ComponentType::Option, "vscroll",
            ZAY_DECLARE_COMPONENT(panel, params, ViewName)
            {
                if(params.ParamCount() != 2 && params.ParamCount() != 3 && params.ParamCount() != 4 && params.ParamCount() != 5)
                    return panel._push_pass();
                auto UIName = ViewName + '.' + params.Param(0).ToText();
                auto ContentSize = params.Param(1).ToInteger();
                auto Sensitive = (params.ParamCount() < 3)? 0 : params.Param(2).ToInteger();
                auto SenseBorder = (params.ParamCount() < 4)? 0 : params.Param(3).ToInteger();
                auto LoopingSize = (params.ParamCount() < 5)? 0 : params.Param(4).ToInteger();
                return panel._push_scroll_ui(0, ContentSize, UIName,
                    ZAY_GESTURE_VNTXY(v, n, t, x, y)
                    {
                        static sint32 FirstY = 0;
                        if(t == GT_Pressed)
                            FirstY = v->scrollpos(n).y - y;
                        else if(t == GT_InDragging || t == GT_OutDragging)
                        {
                            if(v->isScrollSensing(n))
                                FirstY = v->scrollpos(n).y - y;
                            const sint32 VectorY = (y - v->oldxy(n).y) * 20;
                            v->moveScroll(n, 0, FirstY + y, 0, FirstY + y + VectorY, 2.0, true);
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
                if(*pcb && !String::Compare(ImageName, strpair("r.")))
                    CurImage = (*pcb)(ImageName.Offset(2));
                else HasError = true;

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
                if(*pcb && !String::Compare(ImageName, strpair("r.")))
                    CurImage = (*pcb)(ImageName.Offset(2));
                else HasError = true;

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
                bool HasError = false;
                auto ImageName = params.Param(0).ToText();

                const Image* CurImage = nullptr;
                if(*pcb && !String::Compare(ImageName, strpair("r.")))
                    CurImage = (*pcb)(ImageName.Offset(2));
                else HasError = true;

                if(CurImage)
                    panel.ninepatch(*CurImage);

                if(HasError)
                    RenderErrorBox(panel);
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
                if(*pcb && !String::Compare(ImageName, strpair("r.")))
                    CurImage = (*pcb)(ImageName.Offset(2));
                else HasError = true;

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

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParameter, "text_box",
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

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParameter, "edit_box",
            ZAY_DECLARE_COMPONENT(panel, params, pcb, ViewName)
            {
                if(params.ParamCount() != 2 && params.ParamCount() != 3 && params.ParamCount() != 4)
                    return panel._push_pass();
                bool HasError = false;
                auto UIName = ViewName + '.' + params.Param(0).ToText();
                auto DOMName = params.Param(1).ToText();
                auto Enabled = (params.ParamCount() < 3)? true : params.ParamToBool(2, HasError);
                auto IsPassword = (params.ParamCount() < 4)? false : params.ParamToBool(3, HasError);

                if(ZayControl::RenderEditBox(panel, UIName, DOMName, Enabled, IsPassword))
                    panel.repaintOnce();

                if(HasError)
                    RenderErrorBox(panel);
                return panel._push_pass();
            },
            "[UIName]"
            "[DomName:group.name]#"
            "[EnableFlag:true|false]"
            "[PasswordFlag:false|true]");

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
                if(*pcb && !String::Compare(ImageName, strpair("r.")))
                {
                    PanelState CurState = panel.state(UIName);
                    if(CurState & PS_Dragging) ImageName += "_p";
                    else if(CurState & PS_Focused) ImageName += "_o";
                    else ImageName += "_d";
                    CurImage = (*pcb)(ImageName.Offset(2));
                }
                else HasError = true;

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

    bool ZayWidgetDOM::ExistValue(chars variable)
    {
        auto& Self = ST();
        return Self.mDocument->ExistValue(variable);
    }

    SolverValue ZayWidgetDOM::GetValue(chars variable)
    {
        auto& Self = ST();
        return Self.mDocument->GetValue(variable);
    }

    void ZayWidgetDOM::SetValue(chars variable, chars formula)
    {
        auto& Self = ST();
        Self.mDocument->SetValue(variable, formula);
        Self.UpdateFlush();
    }

    void ZayWidgetDOM::SetJson(const Context& json, const String nameheader)
    {
        auto& Self = ST();
        Self.mDocument->SetJson(json, nameheader);
        Self.UpdateFlush();
    }

    void ZayWidgetDOM::RemoveVariables(chars keyword)
    {
        auto& Self = ST();
        const Strings OldVariables = Self.mDocument->MatchedVariables(keyword);
        for(sint32 i = 0, iend = OldVariables.Count(); i < iend; ++i)
        {
            Self.mDocument->Remove(OldVariables[i]);
            Self.RemoveFlush(OldVariables[i]);
        }
    }

    void ZayWidgetDOM::BindPipe(id_pipe pipe)
    {
        auto& Self = ST();
        auto& NewPipe = Self.mPipeMap[PtrToUint64(pipe)];
        NewPipe.mRefPipe = pipe;
        NewPipe.mMsec = Platform::Utility::CurrentTimeMsec();

        // 최초 전달
        Self.mDocument->CheckUpdatedSolvers(0, [pipe](const Solver* solver)->void
        {UpdateToPipe(pipe, solver);});
    }

    void ZayWidgetDOM::UnbindPipe(id_pipe pipe)
    {
        if(pipe)
        {
            auto& Self = ST();
            Self.mPipeMap.Remove(PtrToUint64(pipe));
        }
    }

    void ZayWidgetDOM::UpdateFlush()
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
                {UpdateToPipe(PipeID, solver);});
            }
        }
    }

    void ZayWidgetDOM::UpdateToPipe(id_pipe pipe, const Solver* solver)
    {
        Context Json;
        Json.At("type").Set("dom_updated");
        Json.At("variable").Set(solver->linked_variable());
        Json.At("result").Set(solver->result().ToText(true));
        Json.At("formula").Set(solver->parsed_formula());
        Platform::Pipe::SendJson(pipe, Json.SaveJson());
    }

    void ZayWidgetDOM::RemoveFlush(chars variable)
    {
        for(sint32 i = 0, iend = mPipeMap.Count(); i < iend; ++i)
        {
            auto CurPipe = *mPipeMap.AccessByOrder(i);
            id_pipe PipeID = CurPipe.mRefPipe;
            RemoveToPipe(PipeID, variable);
        }
    }

    void ZayWidgetDOM::RemoveToPipe(id_pipe pipe, chars variable)
    {
        Context Json;
        Json.At("type").Set("dom_removed");
        Json.At("variable").Set(variable);
        Platform::Pipe::SendJson(pipe, Json.SaveJson());
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ZayControl
    ////////////////////////////////////////////////////////////////////////////////
    bool ZayControl::RenderEditBox(ZayPanel& panel, const String& uiname, const String& domname, bool enabled, bool ispassword)
    {
        auto& Self = ST();
        const sint32 CursorWidth = 4;
        const sint32 CursorHeight = Math::Min(Platform::Graphics::GetStringHeight(), panel.h());
        const sint32 CursorEffect = 20;
        const PanelState UIState = panel.state(uiname);
        const bool Captured = !!(UIState & PS_Captured);
        const bool Focused = !!(UIState & PS_Focused);

        // 비활성처리
        if(!enabled)
        {
            ZAY_INNER_SCISSOR(panel, 0)
            {
                const String FieldText = (ZayWidgetDOM::ExistValue(domname))? ZayWidgetDOM::GetValue(domname).ToText() : String();
                const String VisualText = Self.SecretFilter(ispassword, FieldText);
                sint32 iCursor = 0;
                Self.RenderText(panel, uiname, VisualText, iCursor, 0, 0);
            }
            return false;
        }

        // 캡쳐처리
        if(Captured)
        {
            // 커서애니메이션 처리
            Self.mFieldAni = (Self.mFieldAni + 1) % 50;
            const double AniValue = Math::Abs(Self.mFieldAni - 25) / 25.0;
            // 반복키 처리
            if(Self.mLastPressCode != 0 && Self.mLastPressMsec + 40 < Platform::Utility::CurrentTimeMsec()) // 반복주기
            {
                Self.mLastPressMsec += 40;
                Self.SendIme(nullptr, uiname, domname, Self.mLastPressCode, Self.mLastPressKey);
            }

            // 캡쳐된 컨트롤
            ZAY_INNER_UI_SCISSOR(panel, 0, uiname,
                ZAY_GESTURE_VNTXY(v, n, t, x, y, domname)
                {
                    if(t == GT_KeyPressed)
                    {
                        auto& Self = ST();
                        Self.SendIme(v, n, domname, x, (char) y);
                        Self.mLastPressCode = x;
                        Self.mLastPressKey = (char) y;
                        Self.mLastPressMsec = Platform::Utility::CurrentTimeMsec() + 300; // 첫반복
                    }
                    else if(t == GT_KeyReleased)
                    {
                        auto& Self = ST();
                        if(Self.mLastPressCode == x)
                            Self.mLastPressCode = 0; // 키해제
                    }
                    // 커서이동
                    else if(t == GT_Pressed)
                    {
                        auto& Self = ST();
                        const String FieldText = (ZayWidgetDOM::ExistValue(domname))? ZayWidgetDOM::GetValue(domname).ToText() : String();
                        if(auto CurCursor = Self.mFieldSavedCursor.Access(n))
                            Self.mFieldTextFocus = CurCursor->GetFocusBy(FieldText);
                        else Self.mFieldTextFocus = 0;
                    }
                    // 구간복사
                    else if(t == GT_ExtendPressed)
                    {
                        auto& Self = ST();
                        const String FieldText = (ZayWidgetDOM::ExistValue(domname))? ZayWidgetDOM::GetValue(domname).ToText() : String();
                        if(auto CurCursor = Self.mFieldSavedCursor.Access(n))
                        {
                            const sint32 CopyFocus = CurCursor->GetFocusBy(FieldText);
                            if(Self.mFieldTextFocus != CopyFocus)
                            {
                                const sint32 FocusBegin = Math::Min(Self.mFieldTextFocus, CopyFocus);
                                const sint32 FocusEnd = Math::Max(Self.mFieldTextFocus, CopyFocus);
                                Self.mCopyAni = mCopyAniMax;
                                Self.mCopyAreaX = CurCursor->GetPos(FocusBegin);
                                Self.mCopyAreaWidth = CurCursor->GetPos(FocusEnd - 1) - Self.mCopyAreaX;
                                // 클립보드 송신
                                const String FieldText = (ZayWidgetDOM::ExistValue(domname))? ZayWidgetDOM::GetValue(domname).ToText() : String();
                                Platform::Utility::SendToTextClipboard(FieldText.Middle(FocusBegin, FocusEnd - FocusBegin));
                            }
                        }
                    }
                    // 커서포커스
                    else if(t == GT_Moving)
                    {
                        auto& Self = ST();
                        if(auto CurCursor = Self.mFieldSavedCursor.Access(n))
                        if(CurCursor->SetFocusText(x))
                            v->invalidate();
                    }
                    else if(t == GT_MovingLosed)
                    {
                        auto& Self = ST();
                        if(auto CurCursor = Self.mFieldSavedCursor.Access(n))
                        {
                            CurCursor->ClearFocus();
                            v->invalidate();
                        }
                    }
                })
            {
                const String FieldText = (ZayWidgetDOM::ExistValue(domname))? ZayWidgetDOM::GetValue(domname).ToText() : String();
                const String VisualFrontText = Self.SecretFilter(ispassword, FieldText.Left(Self.mFieldTextFocus));
                const String VisualRearText = Self.SecretFilter(ispassword, FieldText.Right(Math::Max(0, FieldText.Length() - Self.mFieldTextFocus)));
                sint32 CurCursor = 0;
                sint32 CurPos = 0;

                // 커서이전 텍스트
                if(0 < VisualFrontText.Length())
                    CurPos = Self.RenderText(panel, uiname, VisualFrontText, CurCursor, CurPos, CursorHeight);

                // 커서
                ZAY_XYWH(panel, CurPos, (panel.h() - CursorHeight) / 2, CursorWidth, CursorHeight)
                {
                    ZAY_RGBA(panel, 128, 128, 128, 10 + 110 * AniValue)
                        panel.fill();

                    for(sint32 i = 0, iend = 5; i < iend; ++i)
                    {
                        ZAY_RGBA(panel, 128, 128, 128, 40 * (iend - i) / iend)
                        {
                            ZAY_XYWH(panel, 0, i, panel.w(), 1)
                                panel.fill();
                            ZAY_XYWH(panel, 0, panel.h() - 1 - i, panel.w(), 1)
                                panel.fill();
                        }
                    }
                }

                // 커서이후 텍스트
                if(0 < VisualRearText.Length())
                    Self.RenderText(panel, uiname, VisualRearText, CurCursor, CurPos + CursorWidth, CursorHeight);
            }

            // 복사애니메이션 처리
            if(0 < Self.mCopyAni)
            {
                ZAY_RGBA(panel, 128, 128, 128, 128 * Self.mCopyAni / mCopyAniMax)
                ZAY_MOVE(panel, -panel.toview(0, 0).x, 0)
                ZAY_XYWH(panel, Self.mCopyAreaX, (panel.h() - CursorHeight) / 2, Self.mCopyAreaWidth, CursorHeight)
                ZAY_INNER(panel, CursorEffect * Self.mCopyAni / mCopyAniMax - CursorEffect)
                    panel.rect(CursorWidth);
                Self.mCopyAni--;
            }
            return true;
        }

        // 비캡쳐처리
        ZAY_INNER_UI_SCISSOR(panel, 0, uiname,
            ZAY_GESTURE_VNTXY(v, n, t, x, y, domname)
            {
                // 캡쳐화
                if(t == GT_Pressed)
                {
                    auto& Self = ST();
                    v->setCapture(n);
                    Self.mFieldAni = 0;
                    const String FieldText = (ZayWidgetDOM::ExistValue(domname))? ZayWidgetDOM::GetValue(domname).ToText() : String();
                    if(auto CurCursor = Self.mFieldSavedCursor.Access(n))
                        Self.mFieldTextFocus = CurCursor->GetFocusBy(FieldText);
                    else Self.mFieldTextFocus = 0;
                    Self.mFieldTextLength = boss_strlen(FieldText);
                    Self.mFieldSavedText = FieldText;
                    Self.mCopyAni = 0; // 복사애니중단
                    Self.mLastPressCode = 0; // 키해제
                }
                // 커서포커스
                else if(t == GT_Moving)
                {
                    auto& Self = ST();
                    if(auto CurCursor = Self.mFieldSavedCursor.Access(n))
                    if(CurCursor->SetFocusText(x))
                        v->invalidate();
                }
                else if(t == GT_MovingLosed)
                {
                    auto& Self = ST();
                    if(auto CurCursor = Self.mFieldSavedCursor.Access(n))
                    {
                        CurCursor->ClearFocus();
                        v->invalidate();
                    }
                }
            })
        {
            const String FieldText = (ZayWidgetDOM::ExistValue(domname))? ZayWidgetDOM::GetValue(domname).ToText() : String();
            const String VisualText = Self.SecretFilter(ispassword, FieldText);
            sint32 iCursor = 0;
            Self.RenderText(panel, uiname, VisualText, iCursor, 0, CursorHeight);
        }
        return false;
    }

    const String ZayControl::SecretFilter(bool ispassword, chars text) const
    {
        if(ispassword)
        {
            String Collector;
            while(*(text++)) Collector += "●";
            return Collector;
        }
        return text;
    }

    sint32 ZayControl::RenderText(ZayPanel& panel, const String& uiname, chars text, sint32& cursor, sint32 pos, sint32 height)
    {
        auto& SavedCursor = mFieldSavedCursor(uiname);
        while(*text)
        {
            // 한 글자
            const sint32 LetterLength = String::GetLengthOfFirstLetter(text);
            panel.text(pos, panel.h() / 2, text, LetterLength, UIFA_LeftMiddle);

            const sint32 OldPos = pos;
            pos += Platform::Graphics::GetStringWidth(text, LetterLength);
            text += LetterLength;
            SavedCursor.mPosArray.AtWherever(cursor) = panel.toview((OldPos + pos) / 2, 0).x;
            SavedCursor.mPosLength = ++cursor;

            // 포커싱된 커서표현
            if(height != 0 && cursor == SavedCursor.mCursor && *text)
            ZAY_XYWH(panel, pos - 1, (panel.h() - height) / 2, 2, height)
            ZAY_RGBA(panel, 128, 128, 128, 32)
                panel.fill();
        }
        return pos;
    }

    void ZayControl::SendIme(ZayObject* view, const String& uiname, const String& domname, sint32 code, char key)
    {
        branch;
        jump(code == 0x01000012) // Left
            mFieldTextFocus = Math::Max(0, mFieldTextFocus - 1);
        jump(code == 0x01000014) // Right
            mFieldTextFocus = Math::Min(mFieldTextFocus + 1, mFieldTextLength);
        jump(code == 0x01000010) // Home
            mFieldTextFocus = 0;
        jump(code == 0x01000011) // End
            mFieldTextFocus = mFieldTextLength;
        jump(code == 0x01000003) // BackSpace
        {
            if(0 < mFieldTextFocus)
            {
                const String FieldText = (ZayWidgetDOM::ExistValue(domname))? ZayWidgetDOM::GetValue(domname).ToText() : String();
                const String FrontText = FieldText.Left(mFieldTextFocus - 1);
                const String RearText = FieldText.Right(Math::Max(0, FieldText.Length() - mFieldTextFocus));
                ZayWidgetDOM::SetValue(domname, '\'' + FrontText + RearText + '\'');
                mFieldTextFocus--;
                mFieldTextLength--;
            }
        }
        jump(code == 0x01000007) // Delete
        {
            if(mFieldTextFocus < mFieldTextLength)
            {
                const String FieldText = (ZayWidgetDOM::ExistValue(domname))? ZayWidgetDOM::GetValue(domname).ToText() : String();
                const String FrontText = FieldText.Left(mFieldTextFocus);
                const String RearText = FieldText.Right(Math::Max(0, FieldText.Length() - mFieldTextFocus - 1));
                ZayWidgetDOM::SetValue(domname, '\'' + FrontText + RearText + '\'');
                mFieldTextLength--;
            }
        }
        jump(code == 0x01000000) // Esc
        {
            if(view) view->clearCapture();
            ZayWidgetDOM::SetValue(domname, '\'' + mFieldSavedText + '\'');
            mFieldSavedCursor.Remove(uiname);
            mCopyAni = 0; // 복사애니중단
            mLastPressCode = 0; // 키해제
        }
        jump(code == 0x01000004) // Enter
        {
            if(view) view->clearCapture();
            mCopyAni = 0; // 복사애니중단
            mLastPressCode = 0; // 키해제
        }
        jump(code == 0x01000031) // F2
        {
            String PastedText = Platform::Utility::RecvFromTextClipboard();
            if(0 < PastedText.Length())
            {
                PastedText.Replace('\'', '"');
                const String FieldText = (ZayWidgetDOM::ExistValue(domname))? ZayWidgetDOM::GetValue(domname).ToText() : String();
                const String FrontText = FieldText.Left(mFieldTextFocus);
                const String RearText = FieldText.Right(Math::Max(0, FieldText.Length() - mFieldTextFocus));
                ZayWidgetDOM::SetValue(domname, '\'' + FrontText + PastedText + RearText + '\'');
                mFieldTextFocus += PastedText.Length();
                mFieldTextLength += PastedText.Length();
            }
        }
        jump(0 < key) // Ascii
        {
            const String FieldText = (ZayWidgetDOM::ExistValue(domname))? ZayWidgetDOM::GetValue(domname).ToText() : String();
            const String FrontText = FieldText.Left(mFieldTextFocus);
            const String RearText = FieldText.Right(Math::Max(0, FieldText.Length() - mFieldTextFocus));
            ZayWidgetDOM::SetValue(domname, '\'' + FrontText + key + RearText + '\'');
            mFieldTextFocus++;
            mFieldTextLength++;
        }
        else return;

        // 포커스커서 초기화
        if(auto CurCursor = mFieldSavedCursor.Access(uiname))
            CurCursor->ClearFocus();
    }
}
