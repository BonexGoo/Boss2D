#include <boss.hpp>
#include "boss_zaywidget.hpp"

namespace BOSS
{
    static id_assetpath gAssetPath = nullptr;

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
                    Platform::BroadcastNotify("SetCursor", sint32o(Parser::GetInt(detail)), NT_ZayWidget);
                else if(!String::Compare(title, "Atlas"))
                {
                    Context Json;
                    Json.At("type").Set("atlas_updated");
                    Json.At("atlas").LoadJson(SO_OnlyReference, detail);
                    if(mPipe) Platform::Pipe::SendJson(mPipe, Json.SaveJson());
                    else mPipeReservers.AtAdding() = Json.SaveJson();
                }
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
        mProcedureID = -1;
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
        mViewName = ToReference(rhs.mViewName);
        mDomHeader = ToReference(rhs.mDomHeader);
        mZaySon = ToReference(rhs.mZaySon);
        mZaySonAssetName = ToReference(rhs.mZaySonAssetName);
        mZaySonFileSize = ToReference(rhs.mZaySonFileSize);
        mZaySonModifyTime = ToReference(rhs.mZaySonModifyTime);
        mProcedureID = ToReference(rhs.mProcedureID); rhs.mProcedureID = -1; // 이관
        mPipe = rhs.mPipe; rhs.mPipe = nullptr; // 이관
        mPipeModifyTime = ToReference(rhs.mPipeModifyTime);
        // this가 변경되면 Reload가 필요
        if(0 < mZaySonAssetName.Length())
            Reload(mZaySonAssetName);
        return *this;
    }

    ZaySon& ZayWidget::Init(chars viewname, chars assetname, ResourceCB cb, chars domheader)
    {
        mViewName = viewname;
        mDomHeader = (domheader)? domheader : "";
        BuildComponents(viewname, mZaySon, cb);
        BuildGlues(viewname, mZaySon, cb);
        if(assetname)
            Reload(assetname);
        return mZaySon;
    }

    void ZayWidget::Reload(chars assetname)
    {
        BOSS_ASSERT("Reload는 Init후 호출가능합니다", 0 < mViewName.Length());
        if(assetname != nullptr) mZaySonAssetName = assetname;
        BOSS_ASSERT("assetname이 존재해야 합니다", 0 < mZaySonAssetName.Length());

        if(Asset::Exist(mZaySonAssetName, gAssetPath, &mZaySonFileSize, nullptr, nullptr, &mZaySonModifyTime))
        {
            Context Json(ST_Json, SO_NeedCopy, String::FromAsset(mZaySonAssetName, gAssetPath));
            mZaySon.Load(mViewName, mDomHeader, Json);
        }
        else mZaySon.SetViewAndDom(mViewName, mDomHeader);

        Platform::SubProcedure(mProcedureID);
        mProcedureID = Platform::AddProcedure(PE_1SEC,
            [](payload data)->void
            {
                auto Self = (ZayWidget*) data;
                // Json체크
                uint64 FileSize = 0, ModifyTime = 0;
                if(Asset::Exist(Self->mZaySonAssetName, gAssetPath, &FileSize, nullptr, nullptr, &ModifyTime))
                if(Self->mZaySonFileSize != FileSize || Self->mZaySonModifyTime != ModifyTime)
                {
                    Self->mZaySonFileSize = FileSize;
                    Self->mZaySonModifyTime = ModifyTime;
                    Context Json(ST_Json, SO_NeedCopy, String::FromAsset(Self->mZaySonAssetName, gAssetPath));
                    Self->mZaySon.Reload(Json);
                    Platform::UpdateAllViews();
                }

                // Pipe체크
                if(!Asset::Exist(Self->mZaySonAssetName + ".pipe", gAssetPath, nullptr, nullptr, nullptr, &ModifyTime))
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
                        Context Json(ST_Json, SO_NeedCopy, String::FromAsset(Self->mZaySonAssetName + ".pipe", gAssetPath));
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

    bool ZayWidget::Render(ZayPanel& panel)
    {
        return mZaySon.Render(panel);
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
            jump(!String::Compare(Type, "CompFocusing"))
            {
                ZayView::_compfocusing();
                return true;
            }
        }
        return false;
    }

    bool ZayWidget::GlueCall(chars name, const Strings params)
    {
        if(auto OneGlue = mZaySon.FindGlue(name))
        {
            ZayExtend::Payload ParamCollector = OneGlue->MakePayload();
            for(sint32 i = 0, iend = params.Count(); i < iend; ++i)
                ParamCollector(params[i]);
            // ParamCollector가 소멸되면서 Glue함수가 호출됨
            return true;
        }
        return false;
    }

    void ZayWidget::JumpCall(chars name, sint32 count)
    {
        mZaySon.JumpCall(name, count);
    }

    void ZayWidget::UpdateAtlas(chars json)
    {
        mZaySon.SendAtlas(json);
    }

    void ZayWidget::SendLog(chars text)
    {
        mZaySon.SendInfoLog("Message", text);
    }

    void ZayWidget::SetAssetPath(id_assetpath assetpath)
    {
        AssetPath::Release(gAssetPath);
        gAssetPath = assetpath;
    }

    void ZayWidget::BuildComponents(chars viewname, ZaySonInterface& interface, ResourceCB pcb)
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

        interface.AddComponent(ZayExtend::ComponentType::CodeInput, "code",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                BOSS_ASSERT("No conditional expression for 'code'", false);
                return panel._push_pass();
            });

        interface.AddComponent(ZayExtend::ComponentType::JumpCall, "jump",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                BOSS_ASSERT("No conditional expression for 'jump'", false);
                return panel._push_pass();
            });

        interface.AddComponent(ZayExtend::ComponentType::JumpMethod, "gate",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                BOSS_ASSERT("No conditional expression for 'jumpgate'", false);
                return panel._push_pass();
            });

        interface.AddComponent(ZayExtend::ComponentType::Layout, "ltrb",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                if(pay.ParamCount() != 4 && pay.ParamCount() != 5)
                    return panel._push_pass();
                bool HasError = false;
                auto L = pay.Param(0).ToInteger();
                auto T = pay.Param(1).ToInteger();
                auto R = pay.Param(2).ToInteger();
                auto B = pay.Param(3).ToInteger();
                auto Scissor = (pay.ParamCount() < 5)? false : pay.ParamToBool(4, HasError);
                return panel._push_clip_ui(L, T, R, B, Scissor, pay.UIName(), pay.MakeGesture());
            },
            "[Left]"
            "[Top]"
            "[Right]"
            "[Bottom]#"
            "[ScissorFlag:false|true]");

        interface.AddComponent(ZayExtend::ComponentType::Layout, "xywh",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                if(pay.ParamCount() != 4 && pay.ParamCount() != 5)
                    return panel._push_pass();
                bool HasError = false;
                auto X = pay.Param(0).ToInteger();
                auto Y = pay.Param(1).ToInteger();
                auto W = pay.Param(2).ToInteger();
                auto H = pay.Param(3).ToInteger();
                auto Scissor = (pay.ParamCount() < 5)? false : pay.ParamToBool(4, HasError);
                return panel._push_clip_ui(X, Y, X + W, Y + H, Scissor, pay.UIName(), pay.MakeGesture());
            },
            "[X]"
            "[Y]"
            "[Width]"
            "[Height]#"
            "[ScissorFlag:false|true]");

        interface.AddComponent(ZayExtend::ComponentType::Layout, "xyrr",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                if(pay.ParamCount() != 4 && pay.ParamCount() != 5)
                    return panel._push_pass();
                bool HasError = false;
                auto X = pay.Param(0).ToInteger();
                auto Y = pay.Param(1).ToInteger();
                auto RX = pay.Param(2).ToInteger();
                auto RY = pay.Param(3).ToInteger();
                auto Scissor = (pay.ParamCount() < 5)? false : pay.ParamToBool(4, HasError);
                return panel._push_clip_ui(X - RX, Y - RY, X + RX, Y + RY, Scissor, pay.UIName(), pay.MakeGesture());
            },
            "[X]"
            "[Y]"
            "[RadiusX]"
            "[RadiusY]#"
            "[ScissorFlag:false|true]");

        interface.AddComponent(ZayExtend::ComponentType::Layout, "inner",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                if(pay.ParamCount() != 1 && pay.ParamCount() != 2)
                    return panel._push_pass();
                bool HasError = false;
                auto V = pay.Param(0).ToInteger();
                auto Scissor = (pay.ParamCount() < 2)? false : pay.ParamToBool(1, HasError);
                return panel._push_clip_ui(V, V, panel.w() - V, panel.h() - V, Scissor, pay.UIName(), pay.MakeGesture());
            },
            "[Value:1]#"
            "[ScissorFlag:false|true]");

        interface.AddComponent(ZayExtend::ComponentType::Layout, "move",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                if(pay.ParamCount() != 2 && pay.ParamCount() != 3)
                    return panel._push_pass();
                bool HasError = false;
                auto X = pay.Param(0).ToInteger();
                auto Y = pay.Param(1).ToInteger();
                auto Scissor = (pay.ParamCount() < 3)? false : pay.ParamToBool(2, HasError);
                return panel._push_clip_ui(X, Y, panel.w() + X, panel.h() + Y, Scissor, pay.UIName(), pay.MakeGesture());
            },
            "[X:0]"
            "[Y:0]#"
            "[ScissorFlag:false|true]");

        interface.AddComponent(ZayExtend::ComponentType::Option, "hscroll",
            ZAY_DECLARE_COMPONENT(panel, pay, ViewName)
            {
                if(pay.ParamCount() != 2 && pay.ParamCount() != 3 && pay.ParamCount() != 4 && pay.ParamCount() != 5)
                    return panel._push_pass();
                auto UIName = ViewName + '.' + pay.Param(0).ToText();
                auto ContentSize = pay.Param(1).ToInteger();
                auto Sensitive = (pay.ParamCount() < 3)? 0 : pay.Param(2).ToInteger();
                auto SenseBorder = (pay.ParamCount() < 4)? 0 : pay.Param(3).ToInteger();
                auto LoopingSize = (pay.ParamCount() < 5)? 0 : pay.Param(4).ToInteger();
                return panel._push_scroll_ui(ContentSize, 0, UIName,
                    ZAY_GESTURE_VNTXY(v, n, t, x, y)
                    {
                        static float OldPosX;
                        static sint32 FirstX = 0;
                        if(t == GT_Pressed)
                        {
                            OldPosX = x;
                            FirstX = v->scrollpos(n).x - x;
                        }
                        else if(t == GT_InDragging || t == GT_OutDragging)
                        {
                            if(v->isScrollSensing(n))
                                FirstX = v->scrollpos(n).x - x;
                            const sint32 VectorX = (x - OldPosX) * 20;
                            v->moveScroll(n, FirstX + x, 0, FirstX + x + VectorX, 0, 2.0, true);
                            OldPosX = x;
                        }
                    }, {(sint32) Sensitive, -1}, SenseBorder, 0 < LoopingSize, LoopingSize, 0);
            },
            "[UIName]"
            "[ContentSize:0]#"
            "[Sensitive:0]"
            "[SenseBorder:0]"
            "[LoopingSize:0]");

        interface.AddComponent(ZayExtend::ComponentType::Option, "vscroll",
            ZAY_DECLARE_COMPONENT(panel, pay, ViewName)
            {
                if(pay.ParamCount() != 2 && pay.ParamCount() != 3 && pay.ParamCount() != 4 && pay.ParamCount() != 5)
                    return panel._push_pass();
                auto UIName = ViewName + '.' + pay.Param(0).ToText();
                auto ContentSize = pay.Param(1).ToInteger();
                auto Sensitive = (pay.ParamCount() < 3)? 0 : pay.Param(2).ToInteger();
                auto SenseBorder = (pay.ParamCount() < 4)? 0 : pay.Param(3).ToInteger();
                auto LoopingSize = (pay.ParamCount() < 5)? 0 : pay.Param(4).ToInteger();
                return panel._push_scroll_ui(0, ContentSize, UIName,
                    ZAY_GESTURE_VNTXY(v, n, t, x, y)
                    {
                        static float OldPosY;
                        static sint32 FirstY = 0;
                        if(t == GT_Pressed)
                        {
                            OldPosY = y;
                            FirstY = v->scrollpos(n).y - y;
                        }
                        else if(t == GT_InDragging || t == GT_OutDragging)
                        {
                            if(v->isScrollSensing(n))
                                FirstY = v->scrollpos(n).y - y;
                            const sint32 VectorY = (y - OldPosY) * 20;
                            v->moveScroll(n, 0, FirstY + y, 0, FirstY + y + VectorY, 2.0, true);
                            OldPosY = y;
                        }
                    }, {-1, (sint32) Sensitive}, SenseBorder, 0 < LoopingSize, LoopingSize, 0);
            },
            "[UIName]"
            "[ContentSize:0]#"
            "[Sensitive:0]"
            "[SenseBorder:0]"
            "[LoopingSize:0]");

        interface.AddComponent(ZayExtend::ComponentType::Option, "color",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                if(pay.ParamCount() != 3 && pay.ParamCount() != 4)
                    return panel._push_pass();
                auto R = pay.Param(0).ToInteger();
                auto G = pay.Param(1).ToInteger();
                auto B = pay.Param(2).ToInteger();
                auto A = (pay.ParamCount() < 4)? 255 : pay.Param(3).ToInteger();
                return panel._push_color(R, G, B, A);
            },
            "[Red:0~255]"
            "[Green:0~255]"
            "[Blue:0~255]#"
            "[Opacity:0~255]");

        interface.AddComponent(ZayExtend::ComponentType::Option, "hexcolor",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                if(pay.ParamCount() != 1 && pay.ParamCount() != 2)
                    return panel._push_pass();
                auto HexColor = pay.Param(0).ToText();
                auto Alpha = (pay.ParamCount() < 2)? 1.0 : pay.Param(1).ToFloat();
                if(!String::Compare(HexColor, "#", 1))
                    return panel._push_color(Color(HexColor, Alpha));
                return panel._push_pass();
            },
            "[HexColor:#ffffff]#"
            "[OpacityRate:1.0]");

        interface.AddComponent(ZayExtend::ComponentType::OptionWithoutParameter, "color_clear",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                return panel._push_color_clear();
            });

        interface.AddComponent(ZayExtend::ComponentType::Option, "shader",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                if(pay.ParamCount() != 1)
                    return panel._push_pass();
                bool HasError = false;
                auto Shader = pay.ParamToShader(0, HasError);
                if(!HasError)
                    return panel._push_shader(Shader);
                return panel._push_pass();
            },
            "[Shader:Normal|Nv21|BlurWeak|BlurMedium|BlurStrong]");

        interface.AddComponent(ZayExtend::ComponentType::Option, "font",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                if(pay.ParamCount() != 1 && pay.ParamCount() != 2)
                    return panel._push_pass();
                auto Size = pay.Param(0).ToFloat();
                chars Name = (pay.ParamCount() < 2)? nullptr : (chars) pay.Param(1).ToText();
                return panel._push_sysfont(Size, Name);
            },
            "[SizeRate:1.0]#"
            "[FontName]");

        interface.AddComponent(ZayExtend::ComponentType::Option, "freefont",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                if(pay.ParamCount() != 1 && pay.ParamCount() != 2)
                    return panel._push_pass();
                auto Height = (sint32) pay.Param(0).ToInteger();
                chars NickName = (pay.ParamCount() < 2)? nullptr : (chars) pay.Param(1).ToText();
                return panel._push_freefont(Height, NickName);
            },
            "[Height:10]#"
            "[FontNick]");

        interface.AddComponent(ZayExtend::ComponentType::Option, "zoom",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                if(pay.ParamCount() != 1 && pay.ParamCount() != 2)
                    return panel._push_pass();
                bool HasError = false;
                auto Zoom = pay.Param(0).ToFloat();
                auto Orientation = (pay.ParamCount() < 2)? OR_Normal : pay.ParamToOrientation(1, HasError);
                if(!HasError)
                    return panel._push_zoom(Zoom, Orientation);
                return panel._push_pass();
            },
            "[ZoomRate:1.0]#"
            "[Orientation:Normal|CW90|CW180|CW270]");

        interface.AddComponent(ZayExtend::ComponentType::OptionWithoutParameter, "zoom_clear",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                return panel._push_zoom_clear();
            });

        interface.AddComponent(ZayExtend::ComponentType::OptionWithoutParameter, "scissor_clear",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                return panel._push_scissor_clear();
            });

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParameter, "erase",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                if(pay.ParamCount() != 0 && pay.ParamCount() != 1)
                    return panel._push_pass();
                auto Round = (pay.ParamCount() < 1)? 0 : pay.Param(0).ToInteger();
                panel.erase(Round);
                return panel._push_pass();
            },
            "#[Round:0]");

        interface.AddComponent(ZayExtend::ComponentType::Content, "fill",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                panel.fill();
                return panel._push_pass();
            });

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParameter, "rect",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                if(pay.ParamCount() != 1)
                    return panel._push_pass();
                auto Thick = pay.Param(0).ToFloat();
                panel.rect(Thick);
                return panel._push_pass();
            },
            "[Thick:1]");

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParameter, "circle",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                if(pay.ParamCount() != 0 && pay.ParamCount() != 1)
                    return panel._push_pass();
                auto Thick = (pay.ParamCount() < 1)? 0.0f : pay.Param(0).ToFloat();
                if(Thick == 0.0f)
                    panel.circle();
                else panel.circleline(Thick);
                return panel._push_pass();
            },
            "#[Thick:0]");

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParameter, "line",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                if(pay.ParamCount() != 4 && pay.ParamCount() != 5)
                    return panel._push_pass();
                auto X1 = pay.Param(0).ToFloat();
                auto Y1 = pay.Param(1).ToFloat();
                auto X2 = pay.Param(2).ToFloat();
                auto Y2 = pay.Param(3).ToFloat();
                auto Thick = (pay.ParamCount() < 5)? 1.0f : pay.Param(4).ToFloat();
                panel.line(Point(X1, Y1), Point(X2, Y2), Thick);
                return panel._push_pass();
            },
            "[X1]"
            "[Y1]"
            "[X2]"
            "[Y2]#"
            "[Thick:1]");

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParameter, "icon",
            ZAY_DECLARE_COMPONENT(panel, pay, pcb)
            {
                if(pay.ParamCount() != 2 && pay.ParamCount() != 3)
                    return panel._push_pass();
                bool HasError = false;
                auto ImageName = pay.Param(0).ToText();
                auto Align = pay.ParamToUIAlign(1, HasError);
                auto Degree = (pay.ParamCount() < 3)? 0.0f : pay.Param(2).ToFloat();

                const Image* CurImage = nullptr;
                if(pcb && !String::Compare(ImageName, strpair("r.")))
                    CurImage = (pcb)(ImageName.Offset(2));
                else HasError = true;

                if(CurImage)
                    panel.icon(*CurImage, Align, Degree);

                if(HasError)
                    RenderErrorBox(panel);
                return panel._push_pass();
            },
            "[RName:r.name]"
            "[UIAlign:LeftTop|CenterTop|RightTop|LeftMiddle|CenterMiddle|RightMiddle|LeftBottom|CenterBottom|RightBottom]#"
            "[Degree:0.0]");

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParameter, "stretch",
            ZAY_DECLARE_COMPONENT(panel, pay, pcb)
            {
                if(pay.ParamCount() != 2 && pay.ParamCount() != 3)
                    return panel._push_pass();
                bool HasError = false;
                auto ImageName = pay.Param(0).ToText();
                auto Rebuild = pay.ParamToBool(1, HasError);
                auto Form = (pay.ParamCount() < 3)? UISF_Strong : pay.ParamToUIStretchForm(2, HasError);

                const Image* CurImage = nullptr;
                if(pcb && !String::Compare(ImageName, strpair("r.")))
                    CurImage = (pcb)(ImageName.Offset(2));
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

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParamAndInsider, "ninepatch",
            ZAY_DECLARE_COMPONENT(panel, pay, pcb)
            {
                if(pay.ParamCount() != 1)
                    return panel._push_pass();
                bool HasError = false;
                auto ImageName = pay.Param(0).ToText();

                const Image* CurImage = nullptr;
                if(pcb && !String::Compare(ImageName, strpair("r.")))
                    CurImage = (pcb)(ImageName.Offset(2));
                else HasError = true;

                if(CurImage)
                    panel.ninepatch(*CurImage);

                if(HasError)
                    RenderErrorBox(panel);
                return panel._push_pass();
            },
            "[RName:r.name]",
            "child|at:X/Y|section:X/Y/XC/YC");

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParameter, "pattern",
            ZAY_DECLARE_COMPONENT(panel, pay, pcb)
            {
                if(pay.ParamCount() != 2 && pay.ParamCount() != 3 && pay.ParamCount() != 4)
                    return panel._push_pass();
                bool HasError = false;
                auto ImageName = pay.Param(0).ToText();
                auto Align = pay.ParamToUIAlign(1, HasError);
                auto ReversedXorder = (pay.ParamCount() < 3)? false : pay.ParamToBool(2, HasError);
                auto ReversedYorder = (pay.ParamCount() < 4)? false : pay.ParamToBool(3, HasError);

                const Image* CurImage = nullptr;
                if(pcb && !String::Compare(ImageName, strpair("r.")))
                    CurImage = (pcb)(ImageName.Offset(2));
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
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                if(pay.ParamCount() != 1 && pay.ParamCount() != 2 && pay.ParamCount() != 3)
                    return panel._push_pass();
                bool HasError = false;
                auto Text = pay.Param(0).ToText();
                auto Align = (pay.ParamCount() < 2)? UIFA_CenterMiddle : pay.ParamToUIFontAlign(1, HasError);
                auto Elide = (pay.ParamCount() < 3)? UIFE_None : pay.ParamToUIFontElide(2, HasError);

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
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                if(pay.ParamCount() != 2 && pay.ParamCount() != 3)
                    return panel._push_pass();
                bool HasError = false;
                auto Text = pay.Param(0).ToText();
                auto LineGap = pay.Param(1).ToInteger();
                auto Align = (pay.ParamCount() < 3)? UIA_LeftTop : pay.ParamToUIAlign(2, HasError);

                panel.textbox(Text, LineGap, Align);

                if(HasError)
                    RenderErrorBox(panel);
                return panel._push_pass();
            },
            "[Text]"
            "[LineGap:10]#"
            "[UIAlign:LeftTop|CenterTop|RightTop|LeftMiddle|CenterMiddle|RightMiddle|LeftBottom|CenterBottom|RightBottom]");

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParamAndInsider, "edit_box",
            ZAY_DECLARE_COMPONENT(panel, pay, ViewName)
            {
                if(pay.ParamCount() != 3 && pay.ParamCount() != 4 && pay.ParamCount() != 5 && pay.ParamCount() != 6)
                    return panel._push_pass();
                bool HasError = false;
                auto UIName = ViewName + '.' + pay.Param(0).ToText();
                auto DOMName = pay.Param(1).ToText();
                auto Border = pay.Param(2).ToInteger();
                auto Enabled = (pay.ParamCount() < 4)? true : pay.ParamToBool(3, HasError);
                auto Password = (pay.ParamCount() < 5)? false : pay.ParamToBool(4, HasError);
                auto DualSave = (pay.ParamCount() < 6)? false : pay.ParamToBool(5, HasError);

                if(ZayControl::RenderEditBox(panel, UIName, DOMName, Border, Enabled, Password, DualSave, pay.TakeRenderer()))
                    panel.repaint();

                if(HasError)
                    RenderErrorBox(panel);
                return panel._push_pass();
            },
            "[UIName]"
            "[DomName:group.name]"
            "[Border:10]#"
            "[EnableFlag:true|false]"
            "[PasswordFlag:false|true]"
            "[DualSaveFlag:false|true]",
            "cursor|focus|copying|default");

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParamAndInsider, "button",
            ZAY_DECLARE_COMPONENT(panel, pay, ViewName)
            {
                if(pay.ParamCount() != 1)
                    return panel._push_pass();
                auto UIName = ViewName + '.' + pay.Param(0).ToText();
                PanelState CurState = panel.state(UIName);
                chars InsiderName = "normal";
                if(CurState & PS_Pressed) InsiderName = "press";
                else if(CurState & (PS_Focused | PS_Dragging)) InsiderName = "focus";

                ZAY_INNER_UI(panel, 0, UIName)
                if(!pay.TakeRenderer() || !pay.TakeRenderer()->RenderInsider(UIName, InsiderName, panel))
                    RenderErrorBox(panel);
                return panel._push_pass();
            },
            "[UIName]",
            "normal|focus|press");

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParamAndInsider, "scroll_view",
            ZAY_DECLARE_COMPONENT(panel, pay, ViewName)
            {
                if(pay.ParamCount() < 3 || 10 < pay.ParamCount())
                    return panel._push_pass();
                bool HasError = false;
                auto UIName = ViewName + '.' + pay.Param(0).ToText();
                auto UIScrollBgName = ViewName + ".scrollbg." + pay.Param(0).ToText(); // UIName에 제스처 전이방지
                auto UIScrollBarName = ViewName + ".scrollbar." + pay.Param(0).ToText(); // UIName에 제스처 전이방지
                auto ContentSize = pay.Param(1).ToInteger();
                auto ContentCount = pay.Param(2).ToInteger();
                auto Layout = (pay.ParamCount() < 4)? UIL_Vertical : pay.ParamToUILayout(3, HasError);
                auto StageCount = (pay.ParamCount() < 5)? 1 : Math::Max(1, pay.Param(4).ToInteger());
                auto ScrollBorder = (pay.ParamCount() < 6)? 0 : pay.Param(5).ToInteger();
                auto ContentBorder = (pay.ParamCount() < 7)? 0 : pay.Param(6).ToInteger();
                auto Sensitive = (pay.ParamCount() < 8)? 20 : pay.Param(7).ToInteger();
                auto SenseBorder = (pay.ParamCount() < 9)? 50 : pay.Param(8).ToInteger();
                auto WheelSpeed = (pay.ParamCount() < 10)? 200 : pay.Param(9).ToInteger();
                const sint32 TotalContentSize = ContentBorder * 2 + ContentSize * ((ContentCount + StageCount - 1) / StageCount);

                if(HasError)
                    RenderErrorBox(panel);
                else
                {
                    if(Layout == UIL_Vertical)
                    {
                        // 컨텐츠
                        sint32 ContentPos = panel.toview(0, 0).y;
                        ZAY_XYWH(panel, 0, 0, panel.w() - ScrollBorder, panel.h())
                        ZAY_SCROLL_UI(panel, 0, TotalContentSize, UIName,
                            ZAY_GESTURE_VNTXY(v, n, t, x, y, WheelSpeed)
                            {
                                static float OldPosY;
                                static sint32 FirstY = 0;
                                if(t == GT_Pressed)
                                {
                                    OldPosY = y;
                                    FirstY = v->scrollpos(n).y - y;
                                }
                                else if(t == GT_InDragging || t == GT_OutDragging)
                                {
                                    if(v->isScrollSensing(n))
                                        FirstY = v->scrollpos(n).y - y;
                                    const sint32 VectorY = (y - OldPosY) * 20;
                                    v->moveScroll(n, 0, FirstY + y, 0, FirstY + y + VectorY, 2.0, true);
                                    OldPosY = y;
                                }
                                else if(t == GT_InReleased || t == GT_OutReleased)
                                    v->clearScrollTouch(n);
                                else if(t == GT_WheelUp || t == GT_WheelDown)
                                {
                                    const sint32 ScrollPos = v->scrollpos(n).y;
                                    const sint32 ScrollTarget = ScrollPos + ((t == GT_WheelUp)? WheelSpeed : -WheelSpeed);
                                    v->moveScroll(n, 0, ScrollPos, 0, ScrollTarget, 1.0, false);
                                }
                            }, {-1, (sint32) Sensitive}, SenseBorder, false, 0, 0)
                        {
                            ContentPos -= panel.toview(0, 0).y;
                            for(sint32 y = 0, i = 0; i < ContentCount; ++y)
                            for(sint32 x = 0; x < StageCount && i < ContentCount; ++x, ++i)
                            {
                                ZAY_LTRB(panel, panel.w() * x / StageCount, ContentBorder + ContentSize * y,
                                    panel.w() * (x + 1) / StageCount, ContentBorder + ContentSize * (y + 1))
                                    if(!pay.TakeRenderer() || !pay.TakeRenderer()->RenderInsider(UIName, "content", panel, i))
                                        RenderErrorBox(panel);
                            }
                        }

                        // 스크롤영역
                        if(0 < ScrollBorder && 0 < panel.h())
                        ZAY_XYWH_UI(panel, panel.w() - ScrollBorder, 0, ScrollBorder, panel.h(), UIScrollBgName,
                            ZAY_GESTURE_VNTXY(v, n, t, x, y, UIName, TotalContentSize, WheelSpeed)
                            {
                                if(t == GT_Pressed || t == GT_InDragging || t == GT_OutDragging)
                                {
                                    auto CurRect = v->rect(n);
                                    const float TargetRate = (y - CurRect.t) / float(CurRect.b - CurRect.t);
                                    const sint32 ScrollPos = v->scrollpos(UIName).y;
                                    const sint32 ScrollTarget = (CurRect.b - CurRect.t - TotalContentSize) * TargetRate;
                                    v->moveScroll(UIName, 0, ScrollPos, 0, ScrollTarget, 1.5, false);
                                }
                                else if(t == GT_WheelUp || t == GT_WheelDown)
                                {
                                    const sint32 ScrollPos = v->scrollpos(UIName).y;
                                    const sint32 ScrollTarget = ScrollPos + ((t == GT_WheelUp)? WheelSpeed : -WheelSpeed);
                                    v->moveScroll(UIName, 0, ScrollPos, 0, ScrollTarget, 1.0, false);
                                }
                            })
                        {
                            if(!pay.TakeRenderer() || !pay.TakeRenderer()->RenderInsider(UIScrollBgName, "scrollbg", panel))
                                RenderErrorBox(panel);

                            // 스크롤바
                            if(panel.h() < TotalContentSize)
                            {
                                const sint32 BarSizeMin = Math::Min(20, panel.h());
                                const float ScrollRate = TotalContentSize / panel.h();
                                const sint32 ScrollSize = Math::Max(BarSizeMin, panel.h() * panel.h() / TotalContentSize);
                                const sint32 ScrollPos = (panel.h() - ScrollSize) * ContentPos / (TotalContentSize - panel.h());
                                const sint32 ScrollTop = Math::Clamp(ScrollPos, 0, panel.h() - BarSizeMin);
                                const sint32 ScrollBottom = Math::Clamp(ScrollPos + ScrollSize, BarSizeMin, panel.h());
                                ZAY_LTRB_UI(panel, 0, ScrollTop, panel.w(), ScrollBottom, UIScrollBarName,
                                    ZAY_GESTURE_VNTXY(v, n, t, x, y, UIName, TotalContentSize, ScrollRate, WheelSpeed)
                                    {
                                        static sint32 FirstY = 0;
                                        static sint32 OldY = 0;
                                        if(t == GT_Pressed)
                                        {
                                            FirstY = v->scrollpos(UIName).y;
                                            OldY = y;
                                        }
                                        else if(t == GT_InDragging || t == GT_OutDragging)
                                        {
                                            const sint32 ScrollTarget = FirstY + (OldY - y) * ScrollRate;
                                            v->moveScroll(UIName, 0, ScrollTarget, 0, ScrollTarget, 0.1, true);
                                        }
                                        else if(t == GT_InReleased || t == GT_OutReleased)
                                            v->clearScrollTouch(UIName);
                                        else if(t == GT_WheelUp || t == GT_WheelDown)
                                        {
                                            const sint32 ScrollPos = v->scrollpos(UIName).y;
                                            const sint32 ScrollTarget = ScrollPos + ((t == GT_WheelUp)? WheelSpeed : -WheelSpeed);
                                            v->moveScroll(UIName, 0, ScrollPos, 0, ScrollTarget, 1.0, false);
                                        }
                                    })
                                    if(!pay.TakeRenderer() || !pay.TakeRenderer()->RenderInsider(UIScrollBarName, "scrollbar", panel))
                                        RenderErrorBox(panel);
                            }
                        }
                    }
                    else
                    {
                        // 컨텐츠
                        sint32 ContentPos = panel.toview(0, 0).x;
                        ZAY_XYWH(panel, 0, 0, panel.w(), panel.h() - ScrollBorder)
                        ZAY_SCROLL_UI(panel, TotalContentSize, 0, UIName,
                            ZAY_GESTURE_VNTXY(v, n, t, x, y, WheelSpeed)
                            {
                                static float OldPosX;
                                static sint32 FirstX = 0;
                                if(t == GT_Pressed)
                                {
                                    OldPosX = x;
                                    FirstX = v->scrollpos(n).x - x;
                                }
                                else if(t == GT_InDragging || t == GT_OutDragging)
                                {
                                    if(v->isScrollSensing(n))
                                        FirstX = v->scrollpos(n).x - x;
                                    const sint32 VectorX = (x - OldPosX) * 20;
                                    v->moveScroll(n, FirstX + x, 0, FirstX + x + VectorX, 0, 2.0, true);
                                    OldPosX = x;
                                }
                                else if(t == GT_InReleased || t == GT_OutReleased)
                                    v->clearScrollTouch(n);
                                else if(t == GT_WheelUp || t == GT_WheelDown)
                                {
                                    const sint32 ScrollPos = v->scrollpos(n).x;
                                    const sint32 ScrollTarget = ScrollPos + ((t == GT_WheelUp)? WheelSpeed : -WheelSpeed);
                                    v->moveScroll(n, ScrollPos, 0, ScrollTarget, 0, 1.0, false);
                                }
                            }, {(sint32) Sensitive, -1}, SenseBorder, false, 0, 0)
                        {
                            ContentPos -= panel.toview(0, 0).x;
                            for(sint32 x = 0, i = 0; i < ContentCount; ++x)
                            for(sint32 y = 0; y < StageCount && i < ContentCount; ++y, ++i)
                            {
                                ZAY_LTRB(panel, ContentBorder + ContentSize * x, panel.h() * y / StageCount,
                                    ContentBorder + ContentSize * (x + 1), panel.h() * (y + 1) / StageCount)
                                    if(!pay.TakeRenderer() || !pay.TakeRenderer()->RenderInsider(UIName, "content", panel, i))
                                        RenderErrorBox(panel);
                            }
                        }

                        // 스크롤영역
                        if(0 < ScrollBorder && 0 < panel.w())
                        ZAY_XYWH_UI(panel, 0, panel.h() - ScrollBorder, panel.w(), ScrollBorder, UIScrollBgName,
                            ZAY_GESTURE_VNTXY(v, n, t, x, y, UIName, TotalContentSize, WheelSpeed)
                            {
                                if(t == GT_Pressed || t == GT_InDragging || t == GT_OutDragging)
                                {
                                    auto CurRect = v->rect(n);
                                    const float TargetRate = (x - CurRect.l) / float(CurRect.r - CurRect.l);
                                    const sint32 ScrollPos = v->scrollpos(UIName).x;
                                    const sint32 ScrollTarget = (CurRect.r - CurRect.l - TotalContentSize) * TargetRate;
                                    v->moveScroll(UIName, ScrollPos, 0, ScrollTarget, 0, 1.5, false);
                                }
                                else if(t == GT_WheelUp || t == GT_WheelDown)
                                {
                                    const sint32 ScrollPos = v->scrollpos(UIName).x;
                                    const sint32 ScrollTarget = ScrollPos + ((t == GT_WheelUp)? WheelSpeed : -WheelSpeed);
                                    v->moveScroll(UIName, ScrollPos, 0, ScrollTarget, 0, 1.0, false);
                                }
                            })
                        {
                            if(!pay.TakeRenderer() || !pay.TakeRenderer()->RenderInsider(UIScrollBgName, "scrollbg", panel))
                                RenderErrorBox(panel);

                            // 스크롤바
                            if(panel.w() < TotalContentSize)
                            {
                                const sint32 BarSizeMin = Math::Min(20, panel.w());
                                const float ScrollRate = TotalContentSize / panel.w();
                                const sint32 ScrollSize = Math::Max(BarSizeMin, panel.w() * panel.w() / TotalContentSize);
                                const sint32 ScrollPos = (panel.w() - ScrollSize) * ContentPos / (TotalContentSize - panel.w());
                                const sint32 ScrollLeft = Math::Clamp(ScrollPos, 0, panel.w() - BarSizeMin);
                                const sint32 ScrollRight = Math::Clamp(ScrollPos + ScrollSize, BarSizeMin, panel.w());
                                ZAY_LTRB_UI(panel, ScrollLeft, 0, ScrollRight, panel.h(), UIScrollBarName,
                                    ZAY_GESTURE_VNTXY(v, n, t, x, y, UIName, TotalContentSize, ScrollRate, WheelSpeed)
                                    {
                                        static sint32 FirstX = 0;
                                        static sint32 OldX = 0;
                                        if(t == GT_Pressed)
                                        {
                                            FirstX = v->scrollpos(UIName).x;
                                            OldX = x;
                                        }
                                        else if(t == GT_InDragging || t == GT_OutDragging)
                                        {
                                            const sint32 ScrollTarget = FirstX + (OldX - x) * ScrollRate;
                                            v->moveScroll(UIName, ScrollTarget, 0, ScrollTarget, 0, 0.1, true);
                                        }
                                        else if(t == GT_InReleased || t == GT_OutReleased)
                                            v->clearScrollTouch(UIName);
                                        else if(t == GT_WheelUp || t == GT_WheelDown)
                                        {
                                            const sint32 ScrollPos = v->scrollpos(UIName).x;
                                            const sint32 ScrollTarget = ScrollPos + ((t == GT_WheelUp)? WheelSpeed : -WheelSpeed);
                                            v->moveScroll(UIName, ScrollPos, 0, ScrollTarget, 0, 1.0, false);
                                        }
                                    })
                                    if(!pay.TakeRenderer() || !pay.TakeRenderer()->RenderInsider(UIScrollBarName, "scrollbar", panel))
                                        RenderErrorBox(panel);
                            }
                        }
                    }
                }
                return panel._push_pass();
            },
            "[UIName]"
            "[ContentSize:100]"
            "[ContentCount:5]#"
            "[Layout:vertical|horizontal]"
            "[StageCount:1]"
            "[ScrollBorder:0]"
            "[ContentBorder:0]"
            "[Sensitive:20]"
            "[SenseBorder:50]"
            "[WheelSpeed:200]",
            "scrollbg|scrollbar|content");

        interface.AddComponent(ZayExtend::ComponentType::ContentWithInsider, "back_buffer",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                // 백버퍼정보
                class BackBufferInfo
                {
                public:
                    BackBufferInfo() {}
                    ~BackBufferInfo() {Platform::Graphics::RemoveSurface(mSurface);}
                public:
                    id_surface mSurface {nullptr};
                    Image mLastImage;
                };
                Map<BackBufferInfo>& BackBufferMap = *BOSS_STORAGE(Map<BackBufferInfo>);

                const String UIName = String::Format("back_buffer_%d", pay.ElementID());
                hook(BackBufferMap(UIName))
                {
                    if(!fish.mSurface ||
                        Platform::Graphics::GetSurfaceWidth(fish.mSurface) != panel.w() ||
                        Platform::Graphics::GetSurfaceHeight(fish.mSurface) != panel.h())
                    {
                        fish.mSurface = Platform::Graphics::CreateSurface(panel.w(), panel.h());
                        fish.mLastImage.Clear();
                    }

                    if(pay.TakeRenderer())
                    {
                        if(pay.TakeRenderer()->HasInsider(UIName, "always"))
                        {
                            ZAY_MAKE_SUBPANEL(panel, fish.mSurface)
                            {
                                pay.TakeRenderer()->RenderInsider(UIName, "always", panel);
                                auto LastBitmap = Platform::Graphics::GetBitmapFromSurface(fish.mSurface);
                                fish.mLastImage.LoadBitmap(LastBitmap);
                            }
                        }
                        else if(pay.TakeRenderer()->HasInsider(UIName, "once"))
                        {
                            ZAY_MAKE_SUBPANEL_UI(panel, fish.mSurface, UIName)
                            {
                                pay.TakeRenderer()->RenderInsider(UIName, "once", panel);
                                auto LastBitmap = Platform::Graphics::GetBitmapFromSurface(fish.mSurface);
                                fish.mLastImage.LoadBitmap(LastBitmap);
                            }
                        }
                        else
                        {
                            RenderErrorBox(panel);
                            return panel._push_pass();
                        }
                        const Point PSA[3] = {Point(0, 0), Point(panel.w(), 0), Point(panel.w(), panel.h())};
                        const Point IPSA[3] = {Point(0, 0), Point(1, 0), Point(1, 1)};
                        panel.polyimage(PSA, fish.mLastImage, Image::Build::Null, IPSA);
                        const Point PSB[3] = {Point(panel.w(), panel.h()), Point(0, panel.h()), Point(0, 0)};
                        const Point IPSB[3] = {Point(1, 1), Point(0, 1), Point(0, 0)};
                        panel.polyimage(PSB, fish.mLastImage, Image::Build::Null, IPSB);
                    }
                    else RenderErrorBox(panel);
                }
                return panel._push_pass();
            },
            nullptr,
            "once|always");

        interface.AddComponent(ZayExtend::ComponentType::ContentWithParamAndSetter, "user_content",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                BOSS_ASSERT("Register 'AddComponent' as 'user_content' once more and use it for replacement", false);
                return panel._push_pass();
            },
            "[According to customization]");

        interface.AddComponent(ZayExtend::ComponentType::Loop, "loop",
            ZAY_DECLARE_COMPONENT(panel, pay)
            {
                BOSS_ASSERT("No conditional expression for 'loop'", false);
                return panel._push_pass();
            });
    }

    void ZayWidget::BuildGlues(chars viewname, ZaySonInterface& interface, ResourceCB pcb)
    {
        interface.AddGlue("exit",
            ZAY_DECLARE_GLUE(pay)
            {
                Platform::Utility::ExitProgram();
            });

        interface.AddGlue("minimize",
            ZAY_DECLARE_GLUE(pay)
            {
                Platform::Utility::SetMinimize();
            });

        interface.AddGlue("maximize",
            ZAY_DECLARE_GLUE(pay)
            {
                if(Platform::Utility::IsFullScreen())
                    Platform::Utility::SetNormalWindow();
                else Platform::Utility::SetFullScreen();
            });

        ZaySonInterface* RefZaySon = &interface;
        interface.AddGlue("jumpcall",
            ZAY_DECLARE_GLUE(pay, RefZaySon)
            {
                if(0 < pay.ParamCount())
                {
                    const String GateName = pay.Param(0).ToText();
                    const sint32 RunCount = (1 < pay.ParamCount())? pay.Param(1).ToInteger() : 1;
                    if(pay.ParamCount() == 6)
                    {
                        const float X = pay.Param(2).ToFloat();
                        const float Y = pay.Param(3).ToFloat();
                        const float W = pay.Param(4).ToFloat();
                        const float H = pay.Param(5).ToFloat();
                        RefZaySon->JumpCallWithArea(GateName, RunCount, X, Y, W, H);
                    }
                    else RefZaySon->JumpCall(GateName, RunCount);
                }
            });

        interface.AddGlue("jumpclear",
            ZAY_DECLARE_GLUE(pay, RefZaySon)
            {
                RefZaySon->JumpClear();
            });
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ZayWidgetDOM
    ////////////////////////////////////////////////////////////////////////////////
    ZayWidgetDOM::ZayWidgetDOM() : mDocument(new ZayWidgetDocumentP())
    {
        mHasUpdate = false;
        mHasRemove = false;
        mHasFocusedCompID = -1;
        mUpdateProcedure = Platform::AddProcedure(PE_100MSEC,
            [](payload data)->void
            {
                auto Self = (ZayWidgetDOM*) data;
                const uint64 NowMsec = Platform::Utility::CurrentTimeMsec();

                // 우선적으로 삭제처리
                if(Self->mHasRemove)
                for(sint32 i = 0, iend = Self->mPipeMap.Count(); i < iend; ++i)
                {
                    auto CurPipe = *Self->mPipeMap.AccessByOrder(i);
                    id_pipe PipeID = CurPipe.mRefPipe;
                    for(sint32 j = 0, jend = Self->mRemoveVariables.Count(); j < jend; ++j)
                    {
                        chararray CurVariable;
                        if(Self->mRemoveVariables.AccessByOrder(j, &CurVariable))
                            RemoveToPipe(PipeID, &CurVariable[0]);
                    }
                }

                // 업데이트처리
                if(Self->mHasUpdate)
                for(sint32 i = 0, iend = Self->mPipeMap.Count(); i < iend; ++i)
                {
                    if(auto CurPipe = Self->mPipeMap.AccessByOrder(i))
                    if(CurPipe->mUpdatedMsec < NowMsec)
                    {
                        id_pipe CurPipeID = CurPipe->mRefPipe;
                        Self->mDocument->CheckUpdatedSolvers(CurPipe->mUpdatedMsec, [CurPipeID](const Solver* solver)->void
                        {UpdateToPipe(CurPipeID, solver);});
                        CurPipe->mUpdatedMsec = NowMsec;
                    }
                }

                // 포커싱처리
                if(Self->mHasFocusedCompID != -1)
                for(sint32 i = 0, iend = Self->mPipeMap.Count(); i < iend; ++i)
                {
                    auto CurPipe = *Self->mPipeMap.AccessByOrder(i);
                    id_pipe PipeID = CurPipe.mRefPipe;
                    FocusToPipe(PipeID, Self->mHasFocusedCompID);
                }

                // 초기화
                Self->mHasUpdate = false;
                Self->mHasRemove = false;
                Self->mHasFocusedCompID = -1;
                Self->mRemoveVariables.Reset();
            }, this);
    }

    ZayWidgetDOM::~ZayWidgetDOM()
    {
        Platform::SubProcedure(mUpdateProcedure);
        delete mDocument;
    }

    bool ZayWidgetDOM::ExistVariable(chars variable)
    {
        auto& Self = ST();
        return Self.mDocument->ExistVariable(variable);
    }

    void ZayWidgetDOM::SetVariableFilter(chars variable, SolverValueCB cb)
    {
        auto& Self = ST();
        Self.mDocument->SetVariableFilter(variable, cb);
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
        Self.ConfirmUpdate();
    }

    String ZayWidgetDOM::GetComment(chars variable)
    {
        auto& Self = ST();
        return Self.mDocument->GetComment(variable);
    }

    void ZayWidgetDOM::SetComment(chars variable, chars text)
    {
        auto& Self = ST();
        Self.mDocument->SetComment(variable, text);
        Self.ConfirmUpdate();
    }

    void ZayWidgetDOM::SetJson(const Context& json, const String nameheader)
    {
        auto& Self = ST();
        Self.mDocument->SetJson(json, nameheader);
        Self.ConfirmUpdate();
    }

    void ZayWidgetDOM::GetJson(Context& json, const String nameheader)
    {
        auto& Self = ST();
        Self.mDocument->GetJson(json, nameheader);
    }

    void ZayWidgetDOM::RemoveVariables(chars keyword)
    {
        auto& Self = ST();
        Array<id_pipe> Pipes;
        for(sint32 i = 0, iend = Self.mPipeMap.Count(); i < iend; ++i)
        {
            auto CurPipe = *Self.mPipeMap.AccessByOrder(i);
            Pipes.AtAdding() = CurPipe.mRefPipe;
        }

        if(0 < Pipes.Count())
        {
            const Array<id_pipe>* PtrPipes = &Pipes;
            Self.mDocument->RemoveMatchedVariables(keyword,
                [PtrPipes](const String& variable, const SolverChainPair* pair)->void
                {
                    for(sint32 i = 0, iend = PtrPipes->Count(); i < iend; ++i)
                        RemoveToPipe((*PtrPipes)[i], variable);
                });
        }
        else Self.mDocument->RemoveMatchedVariables(keyword);
    }

    void ZayWidgetDOM::BindPipe(id_pipe pipe)
    {
        auto& Self = ST();
        auto& NewPipe = Self.mPipeMap[PtrToUint64(pipe)];
        NewPipe.mRefPipe = pipe;
        NewPipe.mUpdatedMsec = Platform::Utility::CurrentTimeMsec();

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

    void ZayWidgetDOM::SetFocus(sint32 compid)
    {
        auto& Self = ST();
        Self.mHasFocusedCompID = compid;
    }

    void ZayWidgetDOM::ConfirmUpdate()
    {
        mHasUpdate = true;
    }

    void ZayWidgetDOM::ConfirmRemove(chars variable)
    {
        mHasRemove = true;
        mRemoveVariables(variable) = true;
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

    void ZayWidgetDOM::RemoveToPipe(id_pipe pipe, chars variable)
    {
        Context Json;
        Json.At("type").Set("dom_removed");
        Json.At("variable").Set(variable);
        Platform::Pipe::SendJson(pipe, Json.SaveJson());
    }

    void ZayWidgetDOM::FocusToPipe(id_pipe pipe, sint32 compid)
    {
        Context Json;
        Json.At("type").Set("comp_focused");
        Json.At("compid").Set(String::FromInteger(compid));
        Platform::Pipe::SendJson(pipe, Json.SaveJson());
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ZayControl
    ////////////////////////////////////////////////////////////////////////////////
    bool ZayControl::RenderEditBox(ZayPanel& panel, const String& uiname, const String& domname,
        sint32 border, bool enabled, bool password, bool dualsave, const ZayExtend::Renderer* renderer)
    {
        auto& Self = ST();
        const sint32 CursorWidth = (Self.mLastLanguage == LM_English)? 4 : 2 + Platform::Graphics::GetStringWidth("가");
        const sint32 CursorHeight = Math::Min(Platform::Graphics::GetStringHeight(), panel.h());
        const sint32 CursorEffect = 20;
        const sint32 CursorEffectWidth = 5;
        const PanelState UIState = panel.state(uiname);
        const bool Captured = !!(UIState & PS_Captured);
        const bool Focused = !!(UIState & PS_Focused);
        sint32 ScrollPos = 0;
        bool RepaintOnce = Captured;

        // 스크롤처리
        const bool ScrollDisabled = (Captured && 0 < Self.mCopyAni);
        sint32 CalcedScrollBar = 0;
        sint32 CalcedContentSize = 0;
        if(auto CurRenderInfo = Self.mRenderInfoMap.Access(uiname))
        {
            if(ScrollDisabled) // 캡쳐이면서 복사애니가 돌아가면 일시 스크롤멈춤
                RepaintOnce |= CurRenderInfo->GetScroll(ScrollPos);
            else RepaintOnce |= CurRenderInfo->UpdateScroll(ScrollPos);
        }

        // 비활성처리
        if(!enabled)
        {
            ZAY_INNER(panel, 0)
            {
                const String FieldText = ZayWidgetDOM::GetComment(domname);
                const String VisualText = Self.SecretFilter(password, FieldText);
                sint32 iCursor = 0;
                Self.RenderText(panel, uiname, VisualText, iCursor, border + ScrollPos, 0, renderer);
            }
        }

        // 캡쳐처리
        else if(Captured)
        {
            // 커서애니메이션 처리
            Self.mCapturedCursorAni = (Self.mCapturedCursorAni + 1) % 50;
            const double CursorAniValue = Math::Abs(Self.mCapturedCursorAni - 25) / 25.0;
            // 반복키 처리
            if(Self.mLastPressCode != 0 && Self.mLastPressMsec + 40 < Platform::Utility::CurrentTimeMsec()) // 반복주기
            {
                Self.mLastPressMsec += 40;
                Self.OnKeyPressed(nullptr, uiname, domname, Self.mLastPressCode, Self.mLastPressKey, dualsave);
            }

            // 캡쳐된 컨트롤
            ZAY_INNER_UI(panel, 0, uiname,
                ZAY_GESTURE_VNTXY(v, n, t, x, y, domname, dualsave)
                {
                    if(t == GT_KeyPressed)
                    {
                        auto& Self = ST();
                        Self.OnKeyPressed(v, n, domname, x, (char) y, dualsave);
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
                        const String FieldText = ZayWidgetDOM::GetComment(domname);
                        sint32 NewCursorIndex = 0;
                        if(auto CurRenderInfo = Self.mRenderInfoMap.Access(n))
                        {
                            NewCursorIndex = CurRenderInfo->GetIndex(FieldText);
                            CurRenderInfo->FlushScrollWheel(); // 스크롤휠 무효화
                        }
                        else Self.mCapturedCursorIndex = 0; // 제이프로에 의한 예외상황

                        if(NewCursorIndex < Self.mCapturedCursorIndex)
                        {
                            Self.FlushSavedIME(domname, dualsave);
                            Self.mCapturedCursorIndex = NewCursorIndex;
                        }
                        else if(Self.mCapturedCursorIndex < NewCursorIndex)
                        {
                            NewCursorIndex -= Self.mCapturedCursorIndex;
                            Self.FlushSavedIME(domname, dualsave);
                            Self.mCapturedCursorIndex += NewCursorIndex;
                        }

                        // 포커싱커서도 맞춤
                        if(auto CurRenderInfo = Self.mRenderInfoMap.Access(n))
                            CurRenderInfo->SetFocusByIndex(FieldText, Self.mCapturedCursorIndex);
                    }
                    // 구간복사
                    else if(t == GT_ExtendPressed)
                    {
                        auto& Self = ST();
                        if(!Self.FlushSavedIME(domname, dualsave))
                        {
                            if(auto CurRenderInfo = Self.mRenderInfoMap.Access(n))
                            {
                                const String FieldText = ZayWidgetDOM::GetComment(domname);
                                const sint32 FieldTextLength = FieldText.Length();
                                const sint32 CopyFocus = CurRenderInfo->GetIndex(FieldText);
                                if(Self.mCapturedCursorIndex != CopyFocus)
                                {
                                    Self.mCopyAni = mCopyAniMax;
                                    if(Self.mCapturedCursorIndex < FieldTextLength)
                                    {
                                        Self.mCopyAreaA = CurRenderInfo->GetPos(FieldText, Self.mCapturedCursorIndex);
                                        Self.mCopyAreaB = CurRenderInfo->GetPos(FieldText, CopyFocus);
                                    }
                                    else // FieldTextFocus가 마지막이면 커서영역의 보정이 불필요
                                    {
                                        Self.mCopyAreaA = CurRenderInfo->GetPos(FieldText, CopyFocus);
                                        Self.mCopyAreaB = CurRenderInfo->GetPos(FieldText, Self.mCapturedCursorIndex);
                                    }
                                    // 클립보드 송신
                                    const sint32 FocusBegin = Math::Min(Self.mCapturedCursorIndex, CopyFocus);
                                    const sint32 FocusEnd = Math::Max(Self.mCapturedCursorIndex, CopyFocus);
                                    Platform::Utility::SendToTextClipboard(FieldText.Middle(FocusBegin, FocusEnd - FocusBegin));
                                }
                            }
                        }
                    }
                    // 커서포커스
                    else if(t == GT_Moving)
                    {
                        auto& Self = ST();
                        if(auto CurRenderInfo = Self.mRenderInfoMap.Access(n))
                        if(CurRenderInfo->SetFocusByPos(x))
                            v->invalidate();
                    }
                    else if(t == GT_MovingLosed)
                    {
                        auto& Self = ST();
                        if(auto CurRenderInfo = Self.mRenderInfoMap.Access(n))
                        {
                            CurRenderInfo->ClearFocus();
                            CurRenderInfo->ZeroScrollWheel();
                            v->invalidate();
                        }
                    }
                    else if(t == GT_WheelUp || t == GT_WheelDown)
                    {
                        auto& Self = ST();
                        if(auto CurRenderInfo = Self.mRenderInfoMap.Access(n))
                        {
                            CurRenderInfo->MoveScrollWheel((t == GT_WheelUp)? 50 : -50);
                            v->invalidate();
                        }
                    }
                })
            {
                const String FieldText = ZayWidgetDOM::GetComment(domname);
                const String VisualFrontText = Self.SecretFilter(password, FieldText.Left(Self.mCapturedCursorIndex));
                const String VisualRearText = Self.SecretFilter(password, FieldText.Right(Math::Max(0, FieldText.Length() - Self.mCapturedCursorIndex)));
                sint32 CurCursor = 0;
                sint32 CurPos = border + ScrollPos;

                // 커서이전 텍스트
                if(0 < VisualFrontText.Length())
                    CurPos = Self.RenderText(panel, uiname, VisualFrontText, CurCursor, CurPos, CursorHeight, renderer);
                CalcedScrollBar = CurPos; // 스크롤보정용

                // 커서
                if(Self.mCopyAni == 0)
                {
                    ZAY_XYWH(panel, CurPos, (panel.h() - CursorHeight) / 2, CursorWidth, CursorHeight)
                    {
                        if(!renderer || !renderer->RenderInsider(uiname, "cursor", panel))
                        {
                            ZAY_RGBA(panel, 128, 128, 128, 10 + 110 * CursorAniValue)
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

                            // 커서IME
                            if(Self.mCapturedIMEChar != L'\0')
                                panel.text(panel.w() / 2, panel.h() / 2, String::FromWChars(WString(Self.mCapturedIMEChar)));

                            // 커서명칭
                            switch(Self.mLastLanguage)
                            {
                            case LM_Korean:
                                ZAY_FONT(panel, 0.3, "arial black")
                                    panel.text(panel.w() / 2, 0, "K O R", UIFA_CenterBottom);
                                break;
                            }
                        }
                    }
                }

                // 커서이후 텍스트
                if(0 < VisualRearText.Length())
                    CurPos = Self.RenderText(panel, uiname, VisualRearText, CurCursor, CurPos + CursorWidth, CursorHeight, renderer);
                CalcedContentSize = CurPos - ScrollPos + border; // 스크롤보정용

                // 복사애니메이션 처리
                if(0 < Self.mCopyAni)
                {
                    // 커서영역의 산정 및 보정
                    const sint32 CopyX = Math::Min(Self.mCopyAreaA, Self.mCopyAreaB) / panel.zoom().scale;
                    sint32 CopyWidth = Math::Max(Self.mCopyAreaA, Self.mCopyAreaB) / panel.zoom().scale - CopyX;
                    if(Self.mCopyAreaB < Self.mCopyAreaA)
                        CopyWidth -= CursorWidth;

                    ZAY_MOVE(panel, -panel.toview(0, 0).x, 0)
                    ZAY_XYWH(panel, CopyX, (panel.h() - CursorHeight) / 2, CopyWidth, CursorHeight)
                    {
                        if(!renderer || !renderer->RenderInsider(uiname, "copying", panel))
                        ZAY_RGBA(panel, 128, 128, 128, 128 * Self.mCopyAni / mCopyAniMax)
                        ZAY_INNER(panel, CursorEffect * Self.mCopyAni / mCopyAniMax - CursorEffect + CursorEffectWidth)
                            panel.rect(CursorEffectWidth);
                    }
                    if(--Self.mCopyAni == 0)
                        Self.mCapturedCursorAni = 0;
                }
            }
        }

        // 비캡쳐처리
        else
        {
            ZAY_INNER_UI(panel, 0, uiname,
                ZAY_GESTURE_VNTXY(v, n, t, x, y, domname, renderer, dualsave)
                {
                    // 캡쳐화
                    if(t == GT_Pressed)
                    {
                        auto& Self = ST();
                        // ReleaseCapture시 방식결정(에디터 → DOM)
                        if(renderer && renderer->DomName(n))
                        {
                            auto NewInfo = (ReleaseCaptureInfo*) Buffer::Alloc<ReleaseCaptureInfo>(BOSS_DBG 1);
                            NewInfo->mUIName = n;
                            NewInfo->mRenderer = (ZayExtend::Renderer*) renderer;
                            if(dualsave)
                                v->setCapture(n, Self.OnReleaseCaptureByRenderer<true>, (payload) NewInfo);
                            else v->setCapture(n, Self.OnReleaseCaptureByRenderer<false>, (payload) NewInfo);
                        }
                        else
                        {
                            if(dualsave)
                                v->setCapture(n, Self.OnReleaseCapture<true>, (id_cloned_share) domname);
                            else v->setCapture(n, Self.OnReleaseCapture<false>, (id_cloned_share) domname);
                        }
                        // 가능하다면 DOM업데이트
                        if(chars CurText = (renderer)? renderer->GetText(n) : nullptr)
                        {
                            ZayWidgetDOM::SetComment(domname, CurText);
                            if(dualsave) ZayWidgetDOM::SetValue(domname + ".text", String::Format("'%s'", CurText));
                        }
                        // DOM → 에디터
                        const String FieldText = ZayWidgetDOM::GetComment(domname);
                        if(auto CurRenderInfo = Self.mRenderInfoMap.Access(n))
                        {
                            Self.mCapturedCursorIndex = CurRenderInfo->GetIndex(FieldText);
                            CurRenderInfo->FlushScrollWheel(); // 스크롤휠 무효화
                        }
                        else Self.mCapturedCursorIndex = 0; // 제이프로에 의한 예외상황
                        Self.mCapturedCursorAni = 0;
                        Self.mCapturedSavedText = FieldText;
                        Self.mCopyAni = 0; // 복사애니중단
                        Self.mLastPressCode = 0; // 키해제
                        Platform::BroadcastNotify("CapturePressing", String(domname), NT_ZayWidget);
                    }
                    // 커서포커스
                    else if(t == GT_Moving)
                    {
                        auto& Self = ST();
                        if(auto CurRenderInfo = Self.mRenderInfoMap.Access(n))
                        if(CurRenderInfo->SetFocusByPos(x))
                            v->invalidate();
                    }
                    else if(t == GT_MovingLosed)
                    {
                        auto& Self = ST();
                        if(auto CurRenderInfo = Self.mRenderInfoMap.Access(n))
                        {
                            CurRenderInfo->ClearFocus();
                            CurRenderInfo->ZeroScrollWheel();
                            v->invalidate();
                        }
                    }
                    else if(t == GT_WheelUp || t == GT_WheelDown)
                    {
                        auto& Self = ST();
                        if(auto CurRenderInfo = Self.mRenderInfoMap.Access(n))
                        {
                            CurRenderInfo->MoveScrollWheel((t == GT_WheelUp)? 50 : -50);
                            v->invalidate();
                        }
                    }
                    else if(t == GT_ToolTip)
                    {
                        if(renderer)
                            renderer->ShowTip(n);
                    }
                })
            {
                String FieldText;
                if(chars CurText = (renderer)? renderer->GetText(uiname) : nullptr)
                    FieldText = CurText;
                else FieldText = ZayWidgetDOM::GetComment(domname);
                const String VisualText = Self.SecretFilter(password, FieldText);
                sint32 iCursor = 0;
                if(!renderer || !renderer->RenderInsider(uiname, (0 < VisualText.Length())? "content" : "default", panel))
                    Self.RenderText(panel, uiname, VisualText, iCursor, border + ScrollPos, CursorHeight, renderer);
            }
        }

        // 스크롤보정
        if(auto CurRenderInfo = Self.mRenderInfoMap.Access(uiname))
        {
            if(Captured)
            {
                if(!ScrollDisabled)
                {
                    const sint32 LimitValue = Math::Max(0, panel.w() / 2 - border) / 2;
                    const sint32 LeftLimit = panel.w() / 2 - LimitValue;
                    const sint32 RightLimit = panel.w() / 2 + LimitValue;
                    const sint32 LeftMove = LeftLimit - CalcedScrollBar;
                    const sint32 RightMove = (CalcedScrollBar + CursorWidth) - RightLimit;
                    if(0 < LeftMove || 0 < RightMove)
                    {
                        const sint32 ScrollMax = Math::Max(0, CalcedContentSize - panel.w());
                        CurRenderInfo->MoveScrollTarget((0 < LeftMove)? LeftMove : -RightMove, ScrollMax);
                    }
                }
            }
            // 캡쳐가 아니면
            else
            {
                // 스크롤타겟 초기화
                CurRenderInfo->ZeroScrollTarget();
                if(!enabled) // 비활성상태는 스크롤휠도 초기화
                    CurRenderInfo->ZeroScrollWheel();
            }
        }
        return RepaintOnce;
    }

    void ZayControl::KeyPressing(const String& domname, wchar_t code, bool dualsave)
    {
        auto& Self = ST();
        const String IMEResult = Self.AddCodeToIME(code);
        if(0 < IMEResult.Length())
            Self.FlushIME(domname, IMEResult, dualsave);
    }

    void ZayControl::KeyPressingBack(const String& domname, bool dualsave)
    {
        auto& Self = ST();
        if(Self.mCapturedIMEChar != L'\0')
            Self.mCapturedIMEChar = WString::BreakKorean(Self.mCapturedIMEChar);
        else if(0 < Self.mCapturedCursorIndex)
        {
            const String FieldText = ZayWidgetDOM::GetComment(domname);
            const sint32 LetterSize = String::GetLengthOfLastLetter(FieldText, Self.mCapturedCursorIndex);
            const String FrontText = FieldText.Left(Self.mCapturedCursorIndex - LetterSize);
            const String RearText = FieldText.Right(Math::Max(0, FieldText.Length() - Self.mCapturedCursorIndex));
            const String MergedText = FrontText + RearText;
            ZayWidgetDOM::SetComment(domname, MergedText);
            if(dualsave) ZayWidgetDOM::SetValue(domname + ".text", "'" + MergedText + "'");
            Self.mCapturedCursorIndex -= LetterSize;
        }
    }

    void ZayControl::KeyPressingEnter(const String& domname, bool dualsave, ZayObject* view)
    {
        auto& Self = ST();
        Self.FlushSavedIME(domname, dualsave);
        if(view) view->clearCapture();
        Self.mCopyAni = 0; // 복사애니중단
        Self.mLastPressCode = 0; // 키해제
        Platform::BroadcastNotify("EnterPressing", String(domname), NT_ZayWidget);
    }

    void ZayControl::LangTurn(const String& domname, bool dualsave)
    {
        auto& Self = ST();
        Self.FlushSavedIME(domname, dualsave);
        Self.mLastLanguage = LanguageMode((Self.mLastLanguage + 1) % LM_Max);
        switch(Self.mLastLanguage)
        {
        case LM_English: Platform::BroadcastNotify("LangPressing", String("English"), NT_ZayWidget); break;
        case LM_Korean: Platform::BroadcastNotify("LangPressing", String("Korean"), NT_ZayWidget); break;
        }
    }

    const String ZayControl::SecretFilter(bool ispassword, chars text) const
    {
        if(ispassword)
        {
            String Collector;
            while(*(text++)) Collector += "*";
            return Collector;
        }
        return text;
    }

    sint32 ZayControl::RenderText(ZayPanel& panel, const String& uiname, chars text,
        sint32& cursor, sint32 pos, sint32 height, const ZayExtend::Renderer* renderer)
    {
        auto& CurRenderInfo = mRenderInfoMap(uiname);
        CurRenderInfo.UpdatePos(cursor, panel.toview(pos, 0).x * panel.zoom().scale);
        while(*text)
        {
            // 한 글자
            const sint32 LetterLength = String::GetLengthOfFirstLetter(text);
            panel.text(pos, panel.h() / 2, text, LetterLength, UIFA_LeftMiddle);

            pos += Platform::Graphics::GetStringWidth(text, LetterLength);
            text += LetterLength;
            cursor++;
            CurRenderInfo.UpdatePos(cursor, panel.toview(pos, 0).x * panel.zoom().scale);

            // 포커싱된 커서표현
            if(height != 0 && cursor == CurRenderInfo.GetFocus() && *text)
            ZAY_XYWH(panel, pos - 1, (panel.h() - height) / 2, 2, height)
            {
                if(!renderer || !renderer->RenderInsider(uiname, "focus", panel))
                ZAY_RGBA(panel, 128, 128, 128, 32)
                    panel.fill();
            }
        }
        return pos;
    }

    String ZayControl::AddCodeToIME(wchar_t code)
    {
        String Result;
        if(mLastLanguage == LM_Korean)
        {
            if(code != L'\0')
            {
                if(mCapturedIMEChar != L'\0')
                {
                    wchars MergeResult = WString::MergeKorean(mCapturedIMEChar, code);
                    if(MergeResult[1] != L'\0')
                    {
                        chars_kssm Kssm =  WString::MatchKssm(MergeResult[1]);
                        const bool IsExtended = (Kssm && (Kssm[0] & 0x80));
                        if(IsExtended)
                        {
                            mCapturedIMEChar = MergeResult[1];
                            Result += String::FromWChars(WString(MergeResult[0]));
                        }
                        else
                        {
                            mCapturedIMEChar = L'\0';
                            Result += String::FromWChars(MergeResult);
                        }                        
                    }
                    else mCapturedIMEChar = MergeResult[0];
                }
                else
                {
                    chars_kssm Kssm =  WString::MatchKssm(code);
                    const bool IsExtended = (Kssm && (Kssm[0] & 0x80));
                    if(IsExtended)
                        mCapturedIMEChar = code;
                    else Result += String::FromWChars(WString(code));
                }
            }
            else
            {
                if(mCapturedIMEChar != L'\0')
                {
                    Result += String::FromWChars(WString(mCapturedIMEChar));
                    mCapturedIMEChar = L'\0';
                }
                Result += String::FromWChars((wchars) &code, 1);
            }
        }
        else Result += String::FromWChars((wchars) &code, 1);
        return Result;
    }

    String ZayControl::AddKeyToIME(char key)
    {
        const WString KeyW = WString::FromChars(&key, 1);
        wchar_t Code = ((wchars) KeyW)[0];
        if(mLastLanguage == LM_Korean)
        switch(key)
        {
        case 'A': Code = L'ㅁ'; break; case 'B': Code = L'ㅠ'; break; case 'C': Code = L'ㅊ'; break; case 'D': Code = L'ㅇ'; break;
        case 'E': Code = L'ㄸ'; break; case 'F': Code = L'ㄹ'; break; case 'G': Code = L'ㅎ'; break; case 'H': Code = L'ㅗ'; break;
        case 'I': Code = L'ㅑ'; break; case 'J': Code = L'ㅓ'; break; case 'K': Code = L'ㅏ'; break; case 'L': Code = L'ㅣ'; break;
        case 'M': Code = L'ㅡ'; break; case 'N': Code = L'ㅜ'; break; case 'O': Code = L'ㅒ'; break; case 'P': Code = L'ㅖ'; break;
        case 'Q': Code = L'ㅃ'; break; case 'R': Code = L'ㄲ'; break; case 'S': Code = L'ㄴ'; break; case 'T': Code = L'ㅆ'; break;
        case 'U': Code = L'ㅕ'; break; case 'V': Code = L'ㅍ'; break; case 'W': Code = L'ㅉ'; break; case 'X': Code = L'ㅌ'; break;
        case 'Y': Code = L'ㅛ'; break; case 'Z': Code = L'ㅋ'; break;
        case 'a': Code = L'ㅁ'; break; case 'b': Code = L'ㅠ'; break; case 'c': Code = L'ㅊ'; break; case 'd': Code = L'ㅇ'; break;
        case 'e': Code = L'ㄷ'; break; case 'f': Code = L'ㄹ'; break; case 'g': Code = L'ㅎ'; break; case 'h': Code = L'ㅗ'; break;
        case 'i': Code = L'ㅑ'; break; case 'j': Code = L'ㅓ'; break; case 'k': Code = L'ㅏ'; break; case 'l': Code = L'ㅣ'; break;
        case 'm': Code = L'ㅡ'; break; case 'n': Code = L'ㅜ'; break; case 'o': Code = L'ㅐ'; break; case 'p': Code = L'ㅔ'; break;
        case 'q': Code = L'ㅂ'; break; case 'r': Code = L'ㄱ'; break; case 's': Code = L'ㄴ'; break; case 't': Code = L'ㅅ'; break;
        case 'u': Code = L'ㅕ'; break; case 'v': Code = L'ㅍ'; break; case 'w': Code = L'ㅈ'; break; case 'x': Code = L'ㅌ'; break;
        case 'y': Code = L'ㅛ'; break; case 'z': Code = L'ㅋ'; break;
        }
        return AddCodeToIME(Code);
    }

    void ZayControl::FlushIME(const String& domname, const String added, bool dualsave)
    {
        const String FieldText = ZayWidgetDOM::GetComment(domname);
        const String FrontText = FieldText.Left(mCapturedCursorIndex);
        const String RearText = FieldText.Right(Math::Max(0, FieldText.Length() - mCapturedCursorIndex));
        const String MergedText = FrontText + added + RearText;
        ZayWidgetDOM::SetComment(domname, MergedText);
        if(dualsave) ZayWidgetDOM::SetValue(domname + ".text", "'" + MergedText + "'");
        const sint32 AddedLength = added.Length();
        mCapturedCursorIndex += AddedLength;
    }

    bool ZayControl::FlushSavedIME(const String& domname, bool dualsave)
    {
        if(mCapturedIMEChar != L'\0')
        {
            FlushIME(domname, String::FromWChars(WString(mCapturedIMEChar)), dualsave);
            mCapturedIMEChar = L'\0';
            return true;
        }
        return false;
    }

    void ZayControl::OnKeyPressed(ZayObject* view, const String& uiname, const String& domname, sint32 code, char key, bool dualsave)
    {
        BOSS_TRACE("OnKeyPressed(code=%d, key=%d)", code, key);

        branch;
        jump((code == 37 && key == '\0') || code == 16777234) // Left
        {
            FlushSavedIME(domname, dualsave);
            if(0 < mCapturedCursorIndex)
            {
                const String FieldText = ZayWidgetDOM::GetComment(domname);
                mCapturedCursorIndex = Math::Min(mCapturedCursorIndex, FieldText.Length());
                mCapturedCursorIndex = Math::Max(0, mCapturedCursorIndex - String::GetLengthOfLastLetter(FieldText, mCapturedCursorIndex));
            }
        }
        jump((code == 39 && key == '\0') || code == 16777236) // Right
        {
            if(!FlushSavedIME(domname, dualsave))
            {
                const String FieldText = ZayWidgetDOM::GetComment(domname);
                const sint32 FieldTextLength = FieldText.Length();
                if(mCapturedCursorIndex < FieldTextLength)
                {
                    mCapturedCursorIndex = Math::Min(mCapturedCursorIndex, FieldText.Length());
                    mCapturedCursorIndex = Math::Min(mCapturedCursorIndex + String::GetLengthOfFirstLetter(chars(FieldText) + mCapturedCursorIndex), FieldTextLength);
                }
            }
        }
        jump((code == 36 && key == '\0') || code == 16777232) // Home
        {
            FlushSavedIME(domname, dualsave);
            mCapturedCursorIndex = 0;
        }
        jump((code == 35 && key == '\0') || code == 16777233) // End
        {
            FlushSavedIME(domname, dualsave);
            const String FieldText = ZayWidgetDOM::GetComment(domname);
            const sint32 FieldTextLength = FieldText.Length();
            mCapturedCursorIndex = FieldTextLength;
        }
        jump((code == 21 && key == '\0') || code == 16777301) // 한영키 or 메뉴키
        {
            LangTurn(domname, dualsave);
        }
        jump((code == 113 && key == '\0') || code == 16777265) // F2
        {
            String PastedText = Platform::Utility::RecvFromTextClipboard();
            if(0 < PastedText.Length())
            {
                const String FieldText = ZayWidgetDOM::GetComment(domname);
                const String FrontText = FieldText.Left(mCapturedCursorIndex);
                const String RearText = FieldText.Right(Math::Max(0, FieldText.Length() - mCapturedCursorIndex));
                const String MergedText = FrontText + PastedText + RearText;
                ZayWidgetDOM::SetComment(domname, MergedText);
                if(dualsave) ZayWidgetDOM::SetValue(domname + ".text", "'" + MergedText + "'");
                mCapturedCursorIndex += PastedText.Length();
            }
        }
        jump((code == 16 && key == '\0') || code == 16777248) // Shift
        {
            // 처리없음
        }
        jump(key == 8 || code == 16777219) // BackSpace
        {
            if(mCapturedIMEChar != L'\0')
                mCapturedIMEChar = WString::BreakKorean(mCapturedIMEChar);
            else if(0 < mCapturedCursorIndex)
            {
                const String FieldText = ZayWidgetDOM::GetComment(domname);
                const sint32 LetterSize = String::GetLengthOfLastLetter(FieldText, mCapturedCursorIndex);
                const String FrontText = FieldText.Left(mCapturedCursorIndex - LetterSize);
                const String RearText = FieldText.Right(Math::Max(0, FieldText.Length() - mCapturedCursorIndex));
                const String MergedText = FrontText + RearText;
                ZayWidgetDOM::SetComment(domname, MergedText);
                if(dualsave) ZayWidgetDOM::SetValue(domname + ".text", "'" + MergedText + "'");
                mCapturedCursorIndex -= LetterSize;
            }
        }
        jump(key == 127 || code == 16777223) // Delete
        {
            const String FieldText = ZayWidgetDOM::GetComment(domname);
            const sint32 FieldTextLength = FieldText.Length();
            if(mCapturedCursorIndex < FieldTextLength)
            {
                const sint32 LetterSize = String::GetLengthOfFirstLetter(chars(FieldText) + mCapturedCursorIndex);
                const String FrontText = FieldText.Left(mCapturedCursorIndex);
                const String RearText = FieldText.Right(Math::Max(0, FieldText.Length() - mCapturedCursorIndex - LetterSize));
                const String MergedText = FrontText + RearText;
                ZayWidgetDOM::SetComment(domname, MergedText);
                if(dualsave) ZayWidgetDOM::SetValue(domname + ".text", "'" + MergedText + "'");
            }
        }
        jump(key == 27 || code == 16777216) // Esc
        {
            if(view) view->clearCapture();
            mCapturedIMEChar = L'\0';
            ZayWidgetDOM::SetComment(domname, mCapturedSavedText);
            if(dualsave) ZayWidgetDOM::SetValue(domname + ".text", "'" + mCapturedSavedText + "'");
            mRenderInfoMap.Remove(uiname);
            mCopyAni = 0; // 복사애니중단
            mLastPressCode = 0; // 키해제
        }
        jump(key == 13 || code == 16777220) // Enter
        {
            FlushSavedIME(domname, dualsave);
            if(view) view->clearCapture();
            mCopyAni = 0; // 복사애니중단
            mLastPressCode = 0; // 키해제
            Platform::BroadcastNotify("EnterPressing", String(domname), NT_ZayWidget);
        }
        else // 문자
        {
            const String IMEResult = AddKeyToIME(key);
            if(0 < IMEResult.Length())
                FlushIME(domname, IMEResult, dualsave);
        }

        if(auto CurRenderInfo = mRenderInfoMap.Access(uiname))
        {
            CurRenderInfo->ClearFocus(); // 포커스커서 초기화
            CurRenderInfo->FlushScrollWheel(); // 스크롤휠 무효화
        }
    }

    template<bool DUALSAVE>
    void ZayControl::OnReleaseCapture(payload olddata, payload newdata)
    {
        auto& Self = ST();
        if(olddata)
            Self.FlushSavedIME(String((id_cloned_share) olddata), DUALSAVE);
    }

    template<bool DUALSAVE>
    void ZayControl::OnReleaseCaptureByRenderer(payload olddata, payload newdata)
    {
        auto& Self = ST();
        if(olddata)
        {
            auto OldInfo = (ReleaseCaptureInfo*) olddata;
            const String CurDomName = OldInfo->mRenderer->DomName(OldInfo->mUIName);
            Self.FlushSavedIME(CurDomName, DUALSAVE);
            OldInfo->mRenderer->SetText(OldInfo->mUIName, ZayWidgetDOM::GetComment(CurDomName));
            Buffer::Free((buffer) OldInfo);
        }
    }
}
