#include <boss.hpp>
#include "zaypro.hpp"

#include <resource.hpp>

ZAY_DECLARE_VIEW_CLASS("zayproView", zayproData)

String gTitleFont;
String gBasicFont;
sint32 gZoomPercent = 100;

ZAY_VIEW_API OnCommand(CommandType type, id_share in, id_cloned_share* out)
{
    if(type == CT_Tick)
    {
        // 파이프통신
        if(m->mPipe.TickOnce())
            m->invalidate();
        // 코멘트태그 애니메이션
        if(m->mShowCommentTagMsec != 0)
        {
            if(m->mShowCommentTagMsec <= Platform::Utility::CurrentTimeMsec())
                m->mShowCommentTagMsec = 0;
            m->invalidate();
        }
        // 드래그 애니메이션
        const float DragDist = Math::Distance(0, 0, m->mWorkViewDrag.x, m->mWorkViewDrag.y);
        if(1 <= DragDist)
        {
            const float DragDistMin = Math::Min(20, DragDist);
            Point Drag = Point(m->mWorkViewDrag.x / DragDistMin, m->mWorkViewDrag.y / DragDistMin);
            m->mWorkViewDrag -= Drag;
            m->mWorkViewScroll += Drag;
            m->invalidate();
        }
        else m->mWorkViewDrag = Point();
    }
    else if(type == CT_Activate)
    {
        const boolo Actived(in);
        if(!Actived.ConstValue())
            m->clearCapture();
        
    }
    else if(type == CT_Size)
    {
        m->mWorkViewDrag = Point(); // 드래그하던 것이 있다면 중지
        m->clearCapture();
    }
}

ZAY_VIEW_API OnNotify(NotifyType type, chars topic, id_share in, id_cloned_share* out)
{
    if(type == NT_Normal)
    {
        branch;
        jump(!String::Compare(topic, "Update"))
        {
            m->invalidate(sint32o(in).ConstValue());
        }
        jump(!String::Compare(topic, "SetWindowName"))
        {
            m->mWindowName = String(in);
            Platform::SetWindowName(m->mWindowName);
            m->invalidate();
        }
        jump(!String::Compare(topic, "HookPressed"))
        {
            m->mDraggingHook = sint32o(in).ConstValue();
        }
        jump(!String::Compare(topic, "HookReleased"))
        {
            m->mDraggingHook = -1;
        }
        jump(!String::Compare(topic, "HookDropped"))
        {
            if(m->mDraggingHook != -1)
            {
                const sint32s Values(in);
                const sint32 CurZayBoxID = Values[0];
                const sint32 GroupIndex = Values[1];
                ZEZayBox::TOP()[CurZayBoxID]->AddChild(ZEZayBox::TOP()[m->mDraggingHook].Value(), GroupIndex);
                ZEZayBox::TOP()[CurZayBoxID]->Sort();
            }
        }
        jump(!String::Compare(topic, "HookClear")) // 부모가 자식 전부와 연결해제
        {
            const sint32s Values(in);
            const sint32 CurZayBoxID = Values[0];
            const sint32 GroupIndex = Values[1];
            ZEZayBox::TOP()[CurZayBoxID]->ClearChildrenHook(GroupIndex, true);
        }
        jump(!String::Compare(topic, "ZayBoxMove"))
        {
            const sint32 CurZayBoxID = sint32o(in).ConstValue();
            ZEZayBox::TOP()[CurZayBoxID]->FlushTitleDrag();
            m->mWorkViewDrag = Point(); // 드래그하던 것이 있다면 중지
        }
        jump(!String::Compare(topic, "ZayBoxMoveWith"))
        {
            const sint32 CurZayBoxID = sint32o(in).ConstValue();
            ZEZayBox::TOP()[CurZayBoxID]->FlushTitleDragWith(true);
            m->mWorkViewDrag = Point(); // 드래그하던 것이 있다면 중지
        }
        jump(!String::Compare(topic, "ZayBoxResize"))
        {
            const sint32s Values(in);
            const sint32 CurZayBoxID = Values[0];
            const sint32 AddValue = Values[1];
            ZEZayBox::TOP()[CurZayBoxID]->Resize(AddValue);
            m->mWorkViewDrag = Point(); // 드래그하던 것이 있다면 중지
        }
        jump(!String::Compare(topic, "ZayBoxCopy"))
        {
            const sint32 CurZayBoxID = sint32o(in).ConstValue();
            const sint32 NewZayBoxID = ZEZayBox::TOP()[CurZayBoxID]->Copy();
            const sint32 ParentZayBoxID = ZEZayBox::TOP()[CurZayBoxID]->parent();
            if(NewZayBoxID != -1 && ParentZayBoxID != -1)
                ZEZayBox::TOP()[ParentZayBoxID]->ChangeChild(
                    ZEZayBox::TOP()[CurZayBoxID].Value(), ZEZayBox::TOP()[NewZayBoxID].Value());
        }
        jump(!String::Compare(topic, "ZayBoxSort"))
        {
            const sint32 CurZayBoxID = sint32o(in).ConstValue();
            ZEZayBox::TOP()[CurZayBoxID]->Sort();
        }
        jump(!String::Compare(topic, "ZayBoxRemove")) // 제이박스 삭제(0-remove)
        {
            const sint32 CurZayBoxID = sint32o(in).ConstValue();
            ZEZayBox::TOP()[CurZayBoxID]->ClearChildrenHookAll();
            const sint32 ParentID = ZEZayBox::TOP()[CurZayBoxID]->parent();
            if(ParentID != -1)
                ZEZayBox::TOP()[ParentID]->SubChild(ZEZayBox::TOP()[CurZayBoxID].Value());
            ZEZayBox::TOP().Remove(CurZayBoxID);
        }
        jump(!String::Compare(topic, "ZayBoxHookRemove"))
        {
            const sint32 CurZayBoxID = sint32o(in).ConstValue();
            ZEZayBox::TOP()[CurZayBoxID]->ClearParentHook();
        }
        jump(!String::Compare(topic, "ZayBoxRemoveGroup")) // 제이박스 및 자식들 삭제(0-remove)
        {
            const sint32 CurZayBoxID = sint32o(in).ConstValue();
            ZEZayBox::TOP()[CurZayBoxID]->RemoveChildrenAll();
            const sint32 ParentID = ZEZayBox::TOP()[CurZayBoxID]->parent();
            if(ParentID != -1)
                ZEZayBox::TOP()[ParentID]->SubChild(ZEZayBox::TOP()[CurZayBoxID].Value());
            ZEZayBox::TOP().Remove(CurZayBoxID);
        }
        jump(!String::Compare(topic, "ZayBoxParamRemove")) // 제이박스 파라미터 삭제(0-param-0-remove)
        {
            sint32s Args(in);
            const sint32 CurZayBoxID = Args[0];
            const sint32 CurParamID = Args[1];
            ZEZayBox::TOP()[CurZayBoxID]->SubParam(CurParamID);
        }
        jump(!String::Compare(topic, "ZayBoxValueRemove")) // 제이박스 밸류 삭제(0-value-0-remove)
        {
            sint32s Args(in);
            const sint32 CurZayBoxID = Args[0];
            const sint32 CurValueID = Args[1];
            ZEZayBox::TOP()[CurZayBoxID]->SubInput(CurValueID);
        }
        jump(!String::Compare(topic, "ZayBoxExtValueRemove")) // 제이박스 확장밸류 삭제(0-extvalue-0-remove)
        {
            sint32s Args(in);
            const sint32 CurZayBoxID = Args[0];
            const sint32 CurExtValueID = Args[1];
            ZEZayBox::TOP()[CurZayBoxID]->SubExtInput(CurExtValueID);
        }
        jump(!String::Compare(topic, "ZayBoxInsiderRemove")) // 제이박스 확장밸류 삭제(0-insider-0-remove)
        {
            sint32s Args(in);
            const sint32 CurZayBoxID = Args[0];
            const sint32 GroupIndex = Args[1] + 1;
            ZEZayBox::TOP()[CurZayBoxID]->SubInsiderBall(GroupIndex);
        }
        jump(!String::Compare(topic, "Pipe:", 5))
        {
            if(m->mPipe.IsConnected())
            {
                Context Json;
                Json.At("type").Set(topic + 5);
                Json.At("value").Set(String(in));
                m->mPipe.SendToClient(Json.SaveJson());
            }
        }
        jump(!String::Compare(topic, "RemoveLog"))
        {
            m->mPipe.RemoveLog(sint32o(in).ConstValue());
            m->invalidate();
        }
    }
    else if(type == NT_KeyPress)
    {
        const sint32 KeyCode = sint32o(in).ConstValue();
        branch;
        jump(KeyCode == 37) // Left
            m->mWorkViewDrag = Point(300, 0);
        jump(KeyCode == 38) // Up
            m->mWorkViewDrag = Point(0, 300);
        jump(KeyCode == 39) // Right
            m->mWorkViewDrag = Point(-300, 0);
        jump(KeyCode == 40) // Down
            m->mWorkViewDrag = Point(0, -300);
        jump(KeyCode == 114) // F3: 포커싱요청
        {
            if(m->mPipe.IsConnected())
            {
                Context Json;
                Json.At("type").Set("CompFocusing");
                m->mPipe.SendToClient(Json.SaveJson());
            }
        }
        jump(KeyCode == 116) // F5: 간단세이브
            m->FastSave();
    }
    else if(type == NT_FileContent)
    {
        String ContentName = topic;
        uint08s Content(in);
        if(auto NewAsset = Asset::OpenForWrite(ContentName, true))
        {
            Asset::Write(NewAsset, &Content[0], Content.Count());
            Asset::Close(NewAsset);
            Content.Clear();
        }

        m->mPipe.ResetJsonPath(ContentName);
        const String JsonText = String::FromAsset(ContentName);
        const Context Json(ST_Json, SO_OnlyReference, (chars) JsonText);
        m->LoadCore(Json);
    }
    else if(type == NT_ZayWidget)
    {
        if(!String::Compare(topic, "SetCursor"))
        {
            auto Cursor = CursorRole(sint32o(in).Value());
            m->SetCursor(Cursor);
        }
    }
}

ZAY_VIEW_API OnGesture(GestureType type, sint32 x, sint32 y)
{
}

ZAY_VIEW_API OnRender(ZayPanel& panel)
{
    ZAY_FONT(panel, 1.0, gBasicFont)
    {
        // 타이틀바
        m->RenderTitleBar(panel);

        // 뷰
        ZAY_LTRB_SCISSOR(panel, 0, m->mTitleHeight, panel.w(), panel.h())
        {
            ZAY_RGB(panel, 51, 61, 73)
                panel.fill();

            // 작업뷰
            ZAY_XYWH_UI(panel, 0, 0, panel.w() - 240, panel.h(), "workspace",
                ZAY_GESTURE_VNTXY(v, n, t, x, y)
                {
                    static Point OldPos;
                    if(t == GT_Moving || t == GT_MovingIdle)
                        m->SetCursor(CR_Arrow);
                    else if(t == GT_Pressed)
                    {
                        OldPos = Point(x, y);
                        m->clearCapture();
                    }
                    else if(t == GT_InDragging || t == GT_OutDragging)
                    {
                        const float DragX = (x - OldPos.x) * 100 / gZoomPercent;
                        const float DragY = (y - OldPos.y) * 100 / gZoomPercent;
                        m->mWorkViewScroll.x += DragX;
                        m->mWorkViewScroll.y += DragY;
                        m->mWorkViewDrag = Point(); // 드래그하던 것이 있다면 중지
                        OldPos = Point(x, y);
                        v->invalidate();
                    }
                    else if(t == GT_Dropped && m->mZaySonAPI.mDraggingComponentID != -1)
                    {
                        // 타이틀바만큼 올림
                        m->mZaySonAPI.mDraggingComponentRect -= m->mWorkViewScroll;
                        m->mZaySonAPI.mDraggingComponentRect.t -= m->mTitleHeight;
                        m->mZaySonAPI.mDraggingComponentRect.b -= m->mTitleHeight;

                        // 컴포넌트 추가
                        auto& CurComponent = m->mZaySonAPI.GetComponent(m->mZaySonAPI.mDraggingComponentID);
                        ZEZayBoxObject NewZayBox = ZEZayBox::CREATOR()(CurComponent.mName);
                        ZEZayBox::TOP()[NewZayBox->id()] = NewZayBox;
                    }
                    else if(t == GT_WheelUp || t == GT_WheelDown)
                    {
                        auto CurRect = v->rect(n);
                        const float OldAddX = (x - CurRect.l) * 100 / gZoomPercent;
                        const float OldAddY = (y - CurRect.t) * 100 / gZoomPercent;

                        // 줌변경
                        if(t == GT_WheelDown)
                            gZoomPercent = Math::Min(gZoomPercent + 5, 600);
                        else gZoomPercent = Math::Max(30, gZoomPercent - 5);

                        // 스크롤조정
                        const float NewAddX = (x - CurRect.l) * 100 / gZoomPercent;
                        const float NewAddY = (y - CurRect.t) * 100 / gZoomPercent;
                        m->mWorkViewScroll.x += NewAddX - OldAddX;
                        m->mWorkViewScroll.y += NewAddY - OldAddY;
                        v->invalidate();
                    }
                })
            {
                ZAY_INNER(panel, 20)
                ZAY_FONT(panel, 2.0)
                {
                    ZAY_RGBA(panel, 31, 198, 253, 32)
                    {
                        // 버전
                        panel.text(m->mBuildTag, UIFA_LeftBottom, UIFE_Right);
                        // 랜더카운트
                        static sint32 RenderCount = 0;
                        panel.text(String::Format("RENDER:%04d", RenderCount++ % 10000), UIFA_RightBottom, UIFE_Left);
                    }
                    // 배율정보(100%가 아닐때만)
                    if(gZoomPercent != 100)
                    ZAY_RGBA(panel, 31, 198, 253, 128)
                        panel.text(String::Format("ZOOM:%03d%%", gZoomPercent), UIFA_CenterTop, UIFE_Right);
                }

                m->mWorkViewSize.w = panel.w() * 100 / gZoomPercent;
                m->mWorkViewSize.h = panel.h() * 100 / gZoomPercent;
                if(m->mZaySonAPI.mDraggingComponentID != -1 && (panel.state("workspace") & PS_Dropping))
                {
                    ZAY_INNER(panel, 4)
                    ZAY_RGBA(panel, 255, 255, 255, 20)
                        panel.rect(4);
                }

                // 박스
                ZAY_ZOOM(panel, gZoomPercent * 0.01)
                ZAY_MOVE(panel, m->mWorkViewScroll.x, m->mWorkViewScroll.y)
                for(sint32 i = 0, iend = ZEZayBox::TOP().Count(); i < iend; ++i)
                {
                    auto CurBox = ZEZayBox::TOP().AccessByOrder(i);
                    (*CurBox)->Render(panel);
                }

                // DOM
                m->RenderDOM(panel);

                // 미니맵
                m->RenderMiniMap(panel);

                // 로그리스트
                m->RenderLogList(panel);

                // 간단세이브 효과
                if(const sint32 EasySaveAni = (sint32) (100 * m->mEasySaveEffect.value()))
                {
                    const sint32 Border = 20 * (100 - EasySaveAni) / 100;
                    ZAY_INNER(panel, Border)
                    ZAY_RGBA(panel, 31, 198, 253, 192 * EasySaveAni / 100)
                        panel.rect(Border);
                }
            }

            // 도구뷰
            ZAY_XYWH_UI(panel, panel.w() - 240, 0, 240, panel.h(), "apispace",
                ZAY_GESTURE_T(t)
                {
                    if(t == GT_Pressed)
                        m->clearCapture();
                })
            {
                // 버튼 구역
                ZAY_LTRB_SCISSOR(panel, 0, 0, panel.w(), 12 + 34 + 6 + 34 + 8)
                {
                    // 버튼 배경
                    ZAY_RGB(panel, 35, 44, 53)
                        panel.fill();

                    // 새프로젝트 버튼
                    ZAY_LTRB_UI(panel, 14, 12, 14 + 100, 12 + 34, "NEW",
                        ZAY_GESTURE_T(t)
                        {
                            if(t == GT_Pressed)
                                m->clearCapture();
                            else if(t == GT_InReleased)
                                m->NewProject();
                        })
                    {
                        const bool IsFocused = ((panel.state("NEW") & (PS_Focused | PS_Dropping)) == PS_Focused);
                        const bool IsPressed = ((panel.state("NEW") & (PS_Pressed | PS_Dragging)) != 0);
                        if(IsPressed) panel.ninepatch(R("btn_new_p"));
                        else if(IsFocused) panel.ninepatch(R("btn_new_o"));
                        else panel.ninepatch(R("btn_new_n"));
                    }

                    // 빠른저장 버튼
                    ZAY_LTRB_UI(panel, 14 + 100 + 12, 12, 14 + 100 + 12 + 100, 12 + 34, "FSAVE",
                        ZAY_GESTURE_T(t)
                        {
                            if(t == GT_Pressed)
                                m->clearCapture();
                            else if(t == GT_InReleased)
                                m->FastSave();
                        })
                    {
                        if(const sint32 EasySaveAni = (sint32) (100 * m->mEasySaveEffect.value()))
                        {
                            panel.ninepatch(R("btn_fsave_n"));
                            ZAY_RGBA(panel, 128, 128, 128, 128 * EasySaveAni / 100)
                                panel.ninepatch(R("btn_fsave_o"));
                        }
                        else
                        {
                            const bool IsFocused = ((panel.state("FSAVE") & (PS_Focused | PS_Dropping)) == PS_Focused);
                            const bool IsPressed = ((panel.state("FSAVE") & (PS_Pressed | PS_Dragging)) != 0);
                            if(IsPressed) panel.ninepatch(R("btn_fsave_p"));
                            else if(IsFocused) panel.ninepatch(R("btn_fsave_o"));
                            else panel.ninepatch(R("btn_fsave_n"));
                        }
                    }

                    // 불러오기 버튼
                    auto LoadGestureCB =
                        #if BOSS_WASM
                            ZAY_GESTURE_T(t)
                            {
                                if(t == GT_Pressed)
                                    m->clearCapture();
                                else if(t == GT_InReleased)
                                    Platform::Popup::FileContentDialog(L"All File(*.*)\0*.*\0Json File\0*.json\0");
                            };
                        #else
                            ZAY_GESTURE_T(t)
                            {
                                if(t == GT_Pressed)
                                    m->clearCapture();
                                else if(t == GT_InReleased)
                                {
                                    String JsonPath;
                                    if(Platform::Popup::FileDialog(DST_FileOpen, JsonPath, nullptr, "Load zui"))
                                    {
                                        m->mPipe.ResetJsonPath(JsonPath);
                                        const String JsonText = String::FromFile(JsonPath);
                                        const Context Json(ST_Json, SO_OnlyReference, (chars) JsonText);
                                        m->LoadCore(Json);
                                    }
                                }
                            };
                        #endif
                    ZAY_LTRB_UI(panel, 14, 12 + 34 + 6, 14 + 100, 12 + 34 + 6 + 34, "LOAD", LoadGestureCB)
                    {
                        const bool IsFocused = ((panel.state("LOAD") & (PS_Focused | PS_Dropping)) == PS_Focused);
                        const bool IsPressed = ((panel.state("LOAD") & (PS_Pressed | PS_Dragging)) != 0);
                        if(IsPressed) panel.ninepatch(R("btn_load_p"));
                        else if(IsFocused) panel.ninepatch(R("btn_load_o"));
                        else panel.ninepatch(R("btn_load_n"));
                    }

                    // 저장 버튼
                    ZAY_LTRB_UI(panel, 14 + 100 + 12, 12 + 34 + 6, 14 + 100 + 12 + 100, 12 + 34 + 6 + 34, "SAVE",
                        ZAY_GESTURE_T(t)
                        {
                            if(t == GT_Pressed)
                                m->clearCapture();
                            else if(t == GT_InReleased)
                            {
                                String JsonPath;
                                if(Platform::Popup::FileDialog(DST_FileSave, JsonPath, nullptr, "Save zui"))
                                {
                                    m->mPipe.ResetJsonPath(JsonPath);
                                    // 기존파일은 old로 보관
                                    if(Platform::File::Exist(JsonPath))
                                    {
                                        String OldPath = JsonPath + ".old";
                                        for(sint32 i = 2; Platform::File::Exist(OldPath); ++i)
                                            OldPath = JsonPath + ".old" + String::FromInteger(i);
                                        Platform::File::Rename(WString::FromChars(JsonPath), WString::FromChars(OldPath));
                                    }
                                    m->SaveCore(JsonPath);
                                }
                            }
                        })
                    {
                        const bool IsFocused = ((panel.state("SAVE") & (PS_Focused | PS_Dropping)) == PS_Focused);
                        const bool IsPressed = ((panel.state("SAVE") & (PS_Pressed | PS_Dragging)) != 0);
                        if(IsPressed) panel.ninepatch(R("btn_save_p"));
                        else if(IsFocused) panel.ninepatch(R("btn_save_o"));
                        else panel.ninepatch(R("btn_save_n"));
                    }
                }

                // 컴포넌트 구역
                ZAY_LTRB_SCISSOR(panel, 0, 12 + 34 + 6 + 34 + 8, panel.w(), panel.h())
                {
                    // 컴포넌트 배경
                    ZAY_RGBA(panel, 35, 44, 53, 192)
                        panel.fill();

                    // 컴포넌트들
                    const sint32 ComponentCount = m->mZaySonAPI.GetComponentCount();
                    const sint32 ScrollHeight = ZEZayBox::TitleBarHeight * ComponentCount;
                    ZAY_SCROLL_UI(panel, 0, ScrollHeight, "apispace-scroll",
                        ZAY_GESTURE_VNT(v, n, t)
                        {
                            if(t == GT_WheelUp || t == GT_WheelDown)
                            {
                                Platform::Popup::CloseAllTracker();
                                const sint32 OldPos = v->scrollpos("apispace-scroll").y;
                                const sint32 NewPos = OldPos + ((t == GT_WheelUp)? 300 : -300);
                                v->moveScroll("apispace-scroll", 0, OldPos, 0, NewPos, 1.0, true);
                                v->invalidate(2);
                            }
                        }, {-1, 5}, 20)
                    for(sint32 i = 0; i < ComponentCount; ++i)
                        ZAY_XYWH(panel, 10, ZEZayBox::TitleBarHeight * i, panel.w() - 20, ZEZayBox::TitleBarHeight)
                            m->RenderComponent(panel, i, true, m->mZaySonAPI.mDraggingComponentID == i);

                    // 부드러운 상하경계선
                    for(sint32 i = 0; i < 5; ++i)
                    {
                        ZAY_RGBA(panel, 35, 44, 53, 255 * (5 - i) / 5)
                        {
                            ZAY_XYWH(panel, 0, i, panel.w(), 1)
                                panel.fill();
                            ZAY_XYWH(panel, 0, panel.h() - 1 - i, panel.w(), 1)
                                panel.fill();
                        }
                    }
                }
            }
        }

        // 드래깅중인 API
        if(m->mZaySonAPI.mDraggingComponentID != -1)
        {
            ZAY_RECT(panel, m->mZaySonAPI.mDraggingComponentRect)
                m->RenderComponent(panel, m->mZaySonAPI.mDraggingComponentID, false, false);
        }
        // 아웃라인
        else if(!Platform::Utility::IsFullScreen())
            m->RenderOutline(panel);
    }
}

ZEFakeZaySon::ZEFakeZaySon()
{
    ZEZayBox::CREATOR() = [this](chars compname)->ZEZayBoxObject
    {
        ZEZayBoxObject NewZayBox;
        String NewName = compname;
        Color NewColor(255, 255, 255, 192);
        String NewColorRes = "box_title_first";

        if(auto CurComponent = GetComponent(compname))
        {
            switch(CurComponent->mType)
            {
            case ZayExtend::ComponentType::Content:
                NewZayBox = ZEZayBoxContent::Create(ZEZayBox::ChildType::None, false, CurComponent->mParamComments, CurComponent->mInsideSamples);
                break;
            case ZayExtend::ComponentType::ContentWithParameter:
                NewZayBox = ZEZayBoxContent::Create(ZEZayBox::ChildType::None, true, CurComponent->mParamComments, CurComponent->mInsideSamples);
                break;
            case ZayExtend::ComponentType::ContentWithInsider:
                NewZayBox = ZEZayBoxContent::Create(ZEZayBox::ChildType::Insider, false, CurComponent->mParamComments, CurComponent->mInsideSamples);
                break;
            case ZayExtend::ComponentType::ContentWithParamAndInsider:
                NewZayBox = ZEZayBoxContent::Create(ZEZayBox::ChildType::Insider, true, CurComponent->mParamComments, CurComponent->mInsideSamples);
                break;
            case ZayExtend::ComponentType::ContentWithParamAndSetter:
                NewZayBox = ZEZayBoxContent::Create(ZEZayBox::ChildType::Setter, true, CurComponent->mParamComments, CurComponent->mInsideSamples);
                break;
            case ZayExtend::ComponentType::Option:
                NewZayBox = ZEZayBoxContent::Create(ZEZayBox::ChildType::Inner, true, CurComponent->mParamComments, CurComponent->mInsideSamples);
                break;
            case ZayExtend::ComponentType::OptionWithoutParameter:
                NewZayBox = ZEZayBoxContent::Create(ZEZayBox::ChildType::Inner, false, CurComponent->mParamComments, CurComponent->mInsideSamples);
                break;
            case ZayExtend::ComponentType::Layout:
                NewZayBox = ZEZayBoxLayout::Create(CurComponent->mParamComments);
                break;
            case ZayExtend::ComponentType::CodeInput:
                NewZayBox = ZEZayBoxCode::Create();
                break;
            case ZayExtend::ComponentType::JumpCall:
                NewZayBox = ZEZayBoxJumpOrGate::Create(false);
                break;
            case ZayExtend::ComponentType::JumpMethod:
                NewZayBox = ZEZayBoxJumpOrGate::Create(true);
                break;
            case ZayExtend::ComponentType::Loop:
                NewZayBox = ZEZayBoxLoop::Create();
                break;
            case ZayExtend::ComponentType::Condition:
                NewZayBox = ZEZayBoxCondition::Create();
                break;
            case ZayExtend::ComponentType::ConditionWithOperation:
                NewZayBox = ZEZayBoxCondition::Create(compname);
                break;
            case ZayExtend::ComponentType::ConditionWithEvent:
                NewZayBox = ZEZayBoxCondition::Create(compname, true);
                break;
            default:
                NewZayBox = ZEZayBoxError::Create();
                break;
            }
            NewName = CurComponent->mName;
            NewColor = GetComponentColor(CurComponent->mType, NewColorRes);
        }
        else NewZayBox = ZEZayBoxError::Create();

        const sint32 NewID = ++mLastBoxID;
        NewZayBox->Init(NewID, NewName, NewColor, NewColorRes, true,
            mDraggingComponentRect.l, mDraggingComponentRect.t);
        return NewZayBox;
    };
    ResetBoxInfo();
}

ZEFakeZaySon::~ZEFakeZaySon()
{
}

Color ZEFakeZaySon::GetComponentColor(ZayExtend::ComponentType type, String& colorres)
{
    switch(type)
    {
    case ZayExtend::ComponentType::Content:
    case ZayExtend::ComponentType::ContentWithParameter:
    case ZayExtend::ComponentType::ContentWithInsider:
    case ZayExtend::ComponentType::ContentWithParamAndInsider:
    case ZayExtend::ComponentType::ContentWithParamAndSetter:
        colorres = "box_title_d";
        return Color(0, 181, 240, 192);
    case ZayExtend::ComponentType::Option:
    case ZayExtend::ComponentType::OptionWithoutParameter:
        colorres = "box_title_c";
        return Color(252, 181, 11, 192);
    case ZayExtend::ComponentType::Layout:
        colorres = "box_title_b";
        return Color(34, 173, 166, 192);
    case ZayExtend::ComponentType::Loop:
        colorres = "box_title_a";
        return Color(178, 134, 195, 192);
    case ZayExtend::ComponentType::Condition:
    case ZayExtend::ComponentType::ConditionWithOperation:
    case ZayExtend::ComponentType::ConditionWithEvent:
        colorres = "box_title_e";
        return Color(255, 42, 106, 192);
    }
    colorres = "box_title_first";
    return Color(255, 255, 255, 192);
}

void ZEFakeZaySon::ResetBoxInfo()
{
    mLastBoxID = 0; // 스타터가 0번
    mDraggingComponentID = -1;
    mDraggingComponentRect = Rect();
}

const String& ZEFakeZaySon::ViewName() const
{
    static String _("FakeZaySon");
    return _;
}

ZaySonInterface& ZEFakeZaySon::AddComponent(ZayExtend::ComponentType type, chars name,
    ZayExtend::ComponentCB cb, chars comments, chars samples)
{
    auto& NewComponent = mComponents.AtAdding();
    NewComponent.mType = type;
    NewComponent.mName = name;
    NewComponent.mColor = GetComponentColor(type, NewComponent.mColorRes);
    NewComponent.mParamComments = (comments)? comments : "";
    NewComponent.mInsideSamples = (samples)? samples : "";
    return *this;
}

ZaySonInterface& ZEFakeZaySon::AddGlue(chars name, ZayExtend::GlueCB cb)
{
    return *this;
}

void ZEFakeZaySon::JumpCall(chars gatename, sint32 runcount)
{
}

void ZEFakeZaySon::JumpCallWithArea(chars gatename, sint32 runcount, float x, float y, float w, float h)
{
}

void ZEFakeZaySon::JumpClear()
{
}

sint32 ZEFakeZaySon::GetComponentCount() const
{
    return mComponents.Count();
}

const ZEFakeZaySon::Component& ZEFakeZaySon::GetComponent(sint32 i) const
{
    return mComponents[i];
}

const ZEFakeZaySon::Component* ZEFakeZaySon::GetComponent(chars name) const
{
    // 검색어 분리 : 예) "iffocused(btn)" → "iffocused"
    String FindName = name;
    const sint32 FindedPos = FindName.Find(0, "(");
    if(FindedPos != -1)
        FindName = FindName.Left(FindedPos);
    // elif에서 el분리
    if(!String::Compare(FindName, "elif", 4))
        FindName = FindName.Right(FindName.Length() - 2);
    // 컴포넌트 찾기
    for(sint32 i = 0, iend = mComponents.Count(); i < iend; ++i)
        if(!String::CompareNoCase(mComponents[i].mName, FindName, FindName.Length()))
            return &mComponents[i];
    return nullptr;
}

ZEWidgetPipe::ZEWidgetPipe()
{
    mPipe = nullptr;
    mJsonPathUpdated = false;
    mConnected = false;
    mDOMCount = 0;
    mExpandedDOM = true;
    mExpandedLog = true;
    mLogTitleTurn = false;
}

ZEWidgetPipe::~ZEWidgetPipe()
{
    StopPipe();
}

void ZEWidgetPipe::RunPipe()
{
    if(0 < mLastJsonPath.Length())
    {
        // 서버형 파이프생성
        String PipeName = "error";
        #if !BOSS_WASM
            do
            {
                Platform::Pipe::Close(mPipe);
                const uint32 RandomValue = Platform::Utility::Random();
                PipeName = String::Format("zaypro:%08X", RandomValue);
                mPipe = Platform::Pipe::Open(PipeName);
            }
            while(!mPipe || !Platform::Pipe::IsServer(mPipe));
        #endif

        // 파이프파일 저장
        Context Json;
        Json.At("pipe").Set(PipeName);
        Json.SaveJson().ToFile(mLastJsonPath + ".pipe", true);
    }
}

void ZEWidgetPipe::StopPipe()
{
    if(0 < mLastJsonPath.Length())
    {
        // 파이프파일 제거
        Platform::File::Remove(WString::FromChars(mLastJsonPath + ".pipe"));
        // 파이프제거
        Platform::Pipe::Close(mPipe);
        mPipe = nullptr;
    }
}

bool ZEWidgetPipe::TickOnce()
{
    bool NeedUpdate = false;
    const bool NewConnected = IsConnected();

    // 윈도우타이틀 변경
    if(mJsonPathUpdated || NewConnected != mConnected)
    {
        mJsonPathUpdated = false;
        if(mLastJsonPath.Length() == 0)
            Platform::BroadcastNotify("SetWindowName", String("ZAYPRO"));
        else
        {
            #if BOSS_WASM
                Platform::BroadcastNotify("SetWindowName", String("[" + mLastJsonPath
                    + ((NewConnected)? "] + ZAYPRO" : "] - ZAYPRO")));
            #else
                Platform::BroadcastNotify("SetWindowName",
                    String("[" + String::FromWChars(Platform::File::GetShortName(WString::FromChars(mLastJsonPath)))
                        + ((NewConnected)? "] + ZAYPRO" : "] - ZAYPRO")));
            #endif
        }
    }

    // 수신내용 처리
    if(!NewConnected)
    {
        if(mConnected)
        {
            mDOM.Reset();
            NeedUpdate = true;
        }
    }
    else while(auto NewJson = Platform::Pipe::RecvJson(mPipe))
    {
        const String Type = (*NewJson)("type").GetText();
        if(!String::Compare(Type, "dom_updated"))
        {
            const String CurVariable = (*NewJson)("variable").GetText();
            auto& CurDOM = mDOM(CurVariable);
            CurDOM.mResult = (*NewJson)("result").GetText();
            CurDOM.mFormula = (*NewJson)("formula").GetText();
            CurDOM.mUpdateMsec = Platform::Utility::CurrentTimeMsec();
        }
        else if(!String::Compare(Type, "dom_removed"))
        {
            const String OldVariable = (*NewJson)("variable").GetText();
            mDOM.Remove(OldVariable);
        }
        else if(!String::Compare(Type, "comp_focused"))
        {
            const sint32 CompID = (*NewJson)("compid").GetInt();
            for(sint32 i = 0, iend = ZEZayBox::TOP().Count(); i < iend; ++i)
            {
                auto CurBox = ZEZayBox::TOP().AccessByOrder(i);
                if((*CurBox)->compid() == CompID)
                {
                    auto BoxPos = (*CurBox)->GetRect().Center();
                    const Point TargetPos = Point(BoxPos.x, BoxPos.y) + m->mWorkViewScroll;
                    m->mWorkViewDrag.x = m->mWorkViewSize.w / 2 - TargetPos.x;
                    m->mWorkViewDrag.y = m->mWorkViewSize.h / 2 - TargetPos.y;
                    break;
                }
            }
        }
        else AddLog(Type, (*NewJson)("title").GetText(), (*NewJson)("detail").GetText());
        NeedUpdate = true;
    }

    // 에러 애니메이션
    NeedUpdate |= (mLogTitleTop.TickOnce() && mExpandedLog);
    NeedUpdate |= (mLogDetailTop.TickOnce() && mExpandedLog);

    mConnected = NewConnected;
    return NeedUpdate;
}

void ZEWidgetPipe::ResetJsonPath(chars jsonpath)
{
    StopPipe();
    mLastJsonPath = jsonpath;
    mJsonPathUpdated = true;
    RunPipe();
}

bool ZEWidgetPipe::IsConnected() const
{
    return (Platform::Pipe::Status(mPipe) == ConnectStatus::CS_Connected);
}

void ZEWidgetPipe::SendToClient(chars json)
{
    Platform::Pipe::SendJson(mPipe, json);
}

void ZEWidgetPipe::SetExpandDOM(bool expand)
{
    mExpandedDOM = expand;
}

void ZEWidgetPipe::SetExpandLog(bool expand)
{
    mExpandedLog = expand;
}

class ZELogTitle : public ZEWidgetPipe::LogElement
{
public:
    ZELogTitle(chars type, chars title) : ZEWidgetPipe::LogElement(title)
    {
        mDetailCount = 0;
        if(!String::Compare(type, "info")) mGroupID |= 0x1;
        else if(!String::Compare(type, "warning")) mGroupID |= 0x2;
    }
    ~ZELogTitle() override {}

private:
    void RenderCore(ZayPanel& panel) override
    {
        const Color UIColors[3] = {Color(255, 0, 0), Color(0, 0, 255), Color(224, 0, 224)};
        const Color& CurFontColor = UIColors[mGroupID & 0xF];
        const String UIName = String::Format("log-%d", mGroupID);

        ZAY_INNER_UI(panel, 0, UIName,
            ZAY_GESTURE_T(t, this)
            {
                if(t == GT_Moving)
                    focusid() = mGroupID;
                else if(t == GT_MovingLosed)
                    focusid() = -1;
            })
        {
            ZAY_RGBA(panel, 0, 0, 0, (focusid() == mGroupID)? 255 : 128)
                panel.fill();
            // 텍스트
            ZAY_COLOR(panel, CurFontColor)
            ZAY_RGB(panel, 224, 224, 224)
            ZAY_LTRB(panel, 10, 0, panel.w() - panel.h(), panel.h())
                panel.text(String::Format("%s (%d)", (chars) mText, mDetailCount), UIFA_LeftMiddle, UIFE_Left);
            // 종료버튼
            const String UIRemoveName = UIName + "-remove";
            ZAY_LTRB_UI(panel, panel.w() - panel.h(), 0, panel.w(), panel.h(), UIRemoveName,
                ZAY_GESTURE_T(t, this)
                {
                    if(t == GT_InReleased)
                        Platform::BroadcastNotify("RemoveLog", sint32o(mGroupID));
                })
            ZAY_INNER(panel, 4)
            {
                ZAY_RGB(panel, 0, 0, 0)
                ZAY_RGB_IF(panel, 180, 180, 180, panel.state(UIRemoveName) & PS_Pressed)
                    panel.fill();
                ZAY_RGB(panel, 255, 255, 255)
                    panel.text("×");
                ZAY_RGBA(panel, 255, 255, 255, (panel.state(UIRemoveName) & PS_Focused)? 255 : 128)
                    panel.rect(1);
            }
            ZAY_RGB(panel, 0, 0, 0)
                panel.rect(1);
        }
    }
    bool valid() const override {return true;}

public:
    sint32 mDetailCount;
};

class ZELogDetail : public ZEWidgetPipe::LogElement
{
public:
    ZELogDetail(sint32 groupid, chars detail) : ZEWidgetPipe::LogElement(detail, groupid),
        mBeginMsec(Platform::Utility::CurrentTimeMsec()), mEndMsec(mBeginMsec + 5000) {}
    ~ZELogDetail() override {}

private:
    void RenderCore(ZayPanel& panel) override
    {
        const Color UIColors[3] = {Color(255, 0, 0), Color(0, 0, 255), Color(224, 0, 224)};
        const Color& CurBGColor = UIColors[mGroupID & 0xF];

        ZAY_COLOR(panel, CurBGColor)
        ZAY_RGBA(panel, 64, 64, 64, 92)
            panel.fill();
        // 텍스트
        ZAY_RGB(panel, 255, 255, 128)
        ZAY_LTRB(panel, 10, 0, panel.w() - 10 - panel.h(), panel.h())
            panel.text(mText, UIFA_LeftMiddle, UIFE_Right);
    }
    bool valid() const override
    {
        if(focusid() != -1)
            return (focusid() == mGroupID);
        const uint64 CurMsec = Platform::Utility::CurrentTimeMsec();
        return (mBeginMsec <= CurMsec && CurMsec < mEndMsec);
    }

public:
    const uint64 mBeginMsec;
    const uint64 mEndMsec;
};

void ZEWidgetPipe::AddLog(chars type, chars title, chars detail)
{
    sint32 GroupID = -1;
    if(auto CurTitle = (ZELogTitle*) mLogTitleTop.Detach(title))
    {
        CurTitle->mDetailCount++;
        GroupID = mLogTitleTop.Attach(CurTitle);
    }
    else
    {
        auto NewTitle = new ZELogTitle(type, title);
        NewTitle->mDetailCount++;
        GroupID = mLogTitleTop.Attach(NewTitle);
    }
    auto NewDetail = new ZELogDetail(GroupID, detail);
    mLogDetailTop.AttachLast(NewDetail);
}

void ZEWidgetPipe::RemoveLog(sint32 groupid)
{
    mLogTitleTop.Remove(groupid);
    mLogDetailTop.Remove(groupid);
}

ZEWidgetPipe::LogElement* ZEWidgetPipe::GetLogElement()
{
    mLogTitleTurn = true;
    return mLogTitleTop.next();
}

ZEWidgetPipe::LogElement* ZEWidgetPipe::NextLogElement(LogElement* element)
{
    ZEWidgetPipe::LogElement* Result = element->next();
    if(!Result && mLogTitleTurn)
    {
        mLogTitleTurn = false;
        Result = mLogDetailTop.next();
    }
    while(Result && !Result->valid())
        Result = Result->next();
    return Result;
}

void ZEWidgetPipe::SetDOMSearch(chars text)
{
    mDOMSearch = text;
}

void ZEWidgetPipe::SetDOMCount(sint32 count)
{
    mDOMCount = count;
}

zayproData::zayproData() : mEasySaveEffect(updater())
{
    String DateText = __DATE__;
    DateText.Replace("Jan", "01"); DateText.Replace("Feb", "02"); DateText.Replace("Mar", "03");
    DateText.Replace("Apr", "04"); DateText.Replace("May", "05"); DateText.Replace("Jun", "06");
    DateText.Replace("Jul", "07"); DateText.Replace("Aug", "08"); DateText.Replace("Sep", "09");
    DateText.Replace("Oct", "10"); DateText.Replace("Nov", "11"); DateText.Replace("Dec", "12");
    const String Day = String::Format("%02d", Parser::GetInt(DateText.Middle(2, DateText.Length() - 6).Trim()));
    DateText = DateText.Right(4) + DateText.Left(2) + Day;
    String TimeText = __TIME__;
    TimeText.Replace(":", "");
    mBuildTag = String::Format("BUILD_%s_%s_KST", (chars) DateText, (chars) TimeText);
    mEasySaveEffect.Reset(0);

    // 시스템폰트 등록
    if(buffer NewFontData = Asset::ToBuffer("font/Jalnan.ttf"))
    {
        gTitleFont = Platform::Utility::CreateSystemFont((bytes) NewFontData, Buffer::CountOf(NewFontData));
        Buffer::Free(NewFontData);
    }
    if(buffer NewFontData = Asset::ToBuffer("font/NanumGothicCoding.ttf"))
    {
        gBasicFont = Platform::Utility::CreateSystemFont((bytes) NewFontData, Buffer::CountOf(NewFontData));
        Platform::Popup::SetTrackerFont(gBasicFont, 10);
        Buffer::Free(NewFontData);
    }

    // 위젯 컴포넌트 추가
    auto ResourceCB = [](chars name)->const Image* {return &((const Image&) R(name));};
    ZayWidget::BuildComponents(mZaySonAPI.ViewName(), mZaySonAPI, ResourceCB);
    ZayWidget::BuildGlues(mZaySonAPI.ViewName(), mZaySonAPI, ResourceCB);

    // 분기문 컴포넌트 추가
    mZaySonAPI.AddComponent(ZayExtend::ComponentType::ConditionWithOperation, "if", nullptr);
    mZaySonAPI.AddComponent(ZayExtend::ComponentType::ConditionWithEvent, "iffocused", nullptr);
    mZaySonAPI.AddComponent(ZayExtend::ComponentType::ConditionWithEvent, "ifhovered", nullptr);
    mZaySonAPI.AddComponent(ZayExtend::ComponentType::ConditionWithEvent, "ifpressed", nullptr);
    mZaySonAPI.AddComponent(ZayExtend::ComponentType::Condition, "else", nullptr);
    mZaySonAPI.AddComponent(ZayExtend::ComponentType::Condition, "endif", nullptr);

    // 초기화
    ResetBoxes();

    // 윈도우상태
    mWindowName = "ZAYPRO";
    mNowCursor = CR_Arrow;
    mNcLeftBorder = false;
    mNcTopBorder = false;
    mNcRightBorder = false;
    mNcBottomBorder = false;
    gZoomPercent = 100;
}

zayproData::~zayproData()
{
}

void zayproData::ResetBoxes()
{
    mZaySonAPI.ResetBoxInfo();
    ZEZayBox::TOP().Reset();

    // 스타터 입력
    ZEZayBoxObject NewZayBox;
    NewZayBox = ZEZayBoxStarter::Create();
    NewZayBox->Init(0, "widget", Color(255, 255, 255, 192), "box_title_first", true, 20, 20);
    ZEZayBox::TOP()[0] = NewZayBox;
    mDraggingHook = -1;
    mShowCommentTagMsec = 0;
    mWorkViewDrag = Point(); // 드래그하던 것이 있다면 중지
    mWorkViewScroll = Point(); // 스크롤정위치
}

void zayproData::NewProject()
{
    mPipe.ResetJsonPath("");
    ResetBoxes();
    ZEZayBox::ResetLastID(); // ID발급기 초기화
}

void zayproData::FastSave()
{
    if(0 < mPipe.jsonpath().Length())
    {
        // 기존파일은 .f5로 보관
        if(Platform::File::Exist(mPipe.jsonpath()))
        {
            String OldPath = mPipe.jsonpath() + ".f5";
            Platform::File::Remove(WString::FromChars(OldPath));
            Platform::File::Rename(WString::FromChars(mPipe.jsonpath()), WString::FromChars(OldPath));
        }
        SaveCore(mPipe.jsonpath());
        // 애니효과
        mEasySaveEffect.Reset(1);
        mEasySaveEffect.MoveTo(0, 1.0);
    }
}

void zayproData::LoadCore(const Context& json)
{
    // 박스리셋 및 ID발급기 초기화
    ResetBoxes();
    ZEZayBox::ResetLastID();

    // 위젯 및 위젯과 연결된 박스로드
    ZEZayBox::TOP()[0]->ReadJson(json);
    ZEZayBox::Load(ZEZayBox::TOP()[0]->children(), json("ui"), ZEZayBox::TOP()[0].ConstPtr(), 0);
    // 스타터위치로 스크롤이동
    auto StarterRect = ZEZayBox::TOP()[0]->GetRect();
    mWorkViewScroll.x = 20 - StarterRect.l;
    mWorkViewScroll.y = 20 - StarterRect.t;

    // 기타 박스로드
    sint32s ExtendIDs;
    ZEZayBox::Load(ExtendIDs, json("extends"), nullptr, 0);
}

void zayproData::SaveCore(chars filename) const
{
    Context Json;
    // 위젯 및 위젯과 연결된 박스저장
    ZEZayBox::TOP()[0]->WriteJson(Json, true);
    if(0 < ZEZayBox::TOP()[0]->children().Count())
        ZEZayBox::Save(ZEZayBox::TOP()[0]->children(), Json.At("ui"), true);
    // 기타 박스저장
    sint32s ExtendIDs;
    for(sint32 i = 0, iend = ZEZayBox::TOP().Count(); i < iend; ++i)
    {
        if(auto CurBox = ZEZayBox::TOP().AccessByOrder(i))
        {
            sint32 CurID = (*CurBox)->id();
            if(CurID != 0 && (*CurBox)->parent() == -1)
            {
                for(sint32 j = 0, jend = ExtendIDs.Count(); j <= jend; ++j)
                {
                    if(j == jend)
                        ExtendIDs.AtAdding() = CurID;
                    else if(CurID < ExtendIDs[j])
                    {
                        ExtendIDs.DeliveryOne(j, ToReference(CurID));
                        break;
                    }
                }
            }
        }
    }
    if(0 < ExtendIDs.Count())
        ZEZayBox::Save(ExtendIDs, Json.At("extends"), true);
    Json.SaveJson().ToFile(filename, true);
}

void zayproData::RenderButton(ZayPanel& panel, chars name, Color color, ZayPanel::SubGestureCB cb)
{
    ZAY_INNER_UI(panel, 0, name, cb)
    {
        ZAY_MOVE(panel, 3, 3)
        ZAY_RGBA(panel, 0, 0, 0, 64)
            panel.fill();

        const sint32 PressAdd = (panel.state(name) & PS_Pressed)? 2 : 0;
        ZAY_MOVE(panel, PressAdd, PressAdd)
        {
            ZAY_COLOR(panel, color)
            ZAY_RGB_IF(panel, 128, 128, 128, panel.state(name) & (PS_Focused | PS_Dragging))
                panel.fill();
            ZAY_LTRB(panel, 3, 0, panel.w() - 3, panel.h())
            {
                ZAY_FONT(panel, 1.2)
                {
                    ZAY_MOVE(panel, 1, 1)
                    ZAY_RGB(panel, 255, 255, 255)
                        panel.text(name, UIFA_CenterMiddle, UIFE_Right);
                    ZAY_RGB(panel, 0, 0, 0)
                        panel.text(name, UIFA_CenterMiddle, UIFE_Right);
                }
            }
            ZAY_RGB(panel, 0, 0, 0)
                panel.rect(1);
        }
    }
}

void zayproData::RenderComponent(ZayPanel& panel, sint32 i, bool enable, bool blank)
{
    auto& CurComponent = mZaySonAPI.GetComponent(i);
    const String UIName = String::Format("%d-component", i);
    const Rect UIRect = Rect(panel.toview(0, 0), Size(panel.w(), panel.h()));

    ZAY_INNER_UI(panel, 0, (enable)? UIName : "",
        ZAY_GESTURE_VNTXY(v, n, t, x, y, this, i, UIRect)
        {
            static Point OldPos;
            if(t == GT_Pressed)
            {
                OldPos = Point(x, y);
                mZaySonAPI.mDraggingComponentID = i;
                mZaySonAPI.mDraggingComponentRect = UIRect;
                clearCapture();
            }
            else if(t == GT_InDragging || t == GT_OutDragging)
            {
                mZaySonAPI.mDraggingComponentRect += Point(x - OldPos.x, y - OldPos.y);
                OldPos = Point(x, y);
                v->invalidate();
            }
            else if(t == GT_InReleased || t == GT_OutReleased)
            {
                mZaySonAPI.mDraggingComponentID = -1;
                mZaySonAPI.mDraggingComponentRect = Rect();
            }
            else if(t == GT_WheelUp || t == GT_WheelDown)
            {
                Platform::Popup::CloseAllTracker();
                const sint32 OldPos = v->scrollpos("apispace-scroll").y;
                const sint32 NewPos = OldPos + ((t == GT_WheelUp)? 300 : -300);
                v->moveScroll("apispace-scroll", 0, OldPos, 0, NewPos, 1.0, true);
                v->invalidate(2);
            }
        })
    {
        if(blank)
        {
            ZAY_INNER(panel, 4)
            {
                ZAY_RGBA(panel, 0, 0, 0, 96)
                    panel.fill();
                ZAY_RGBA(panel, 0, 0, 0, 128)
                    panel.rect(2);
            }
        }
        else
        {
            ZAY_INNER(panel, 3)
            ZAY_RGBA(panel, 0, 0, 0, 96)
                panel.fill();

            ZAY_LTRB(panel, 6, 4, panel.w() - 4, panel.h() - 4)
            {
                const bool IsFocusedOnly = ((panel.state(UIName) & (PS_Focused | PS_Dragging | PS_Dropping)) == PS_Focused);
                ZAY_RGB_IF(panel, 160, 160, 160, IsFocusedOnly)
                    panel.ninepatch(R(CurComponent.mColorRes));

                ZAY_LTRB(panel, 5 + 3, 0, panel.w() - 3, panel.h())
                ZAY_RGB(panel, 255, 255, 255)
                ZAY_FONT(panel, 1.0, gTitleFont)
                    panel.text(CurComponent.mName, UIFA_LeftMiddle, UIFE_Right);
            }
        }
    }
}

void zayproData::RenderDOM(ZayPanel& panel)
{
    auto& DOM = mPipe.dom();
    if(0 < DOM.Count())
    {
        static const sint32 TitleHeight = 22;
        static const sint32 ElementHeight = 22;
        static const sint32 ViewWidth = 500;
        const sint32 ViewHeight = (mPipe.expanddom())? mPipe.domcount() * ElementHeight : 0;
        ZAY_LTRB(panel, 10, 10, 10 + ViewWidth, Math::Min(10 + TitleHeight + ViewHeight, panel.h() - 10))
        {
            // 타이틀
            ZAY_LTRB_UI_SCISSOR(panel, 0, 0, panel.w(), TitleHeight, "dom-title")
            {
                ZAY_RGB(panel, 0, 0, 0)
                    panel.fill();
                ZAY_RGB(panel, 255, 255, 255)
                    panel.text(5, panel.h() / 2 - 2, "dom", UIFA_LeftMiddle);

                // 검색기능
                ZAY_XYWH_UI(panel, panel.w() - 50 - 200, 2, 200, panel.h() - 5, "dom-search",
                    ZAY_GESTURE_VNT(v, n, t, this)
                    {
                        if(t == GT_Pressed)
                        {
                            auto CurRect = v->rect(n);
                            String CurText = mPipe.domsearch();
                            if(Platform::Popup::OpenEditTracker(CurText, UIET_String, CurRect.l, CurRect.t, CurRect.r, CurRect.b))
                            {
                                mPipe.SetDOMSearch(CurText);
                                v->invalidate();
                            }
                        }
                    })
                {
                    const bool IsFocused = !!(panel.state("dom-search") & PS_Focused);
                    ZAY_RGBA(panel, 255, 255, 255, (IsFocused)? 192 : 128)
                        panel.fill();
                    ZAY_LTRB(panel, 3, 0, panel.w(), panel.h())
                    ZAY_RGB(panel, 0, 0, 0)
                        panel.text(mPipe.domsearch(), UIFA_LeftMiddle, UIFE_Right);
                }

                // 검색어제거
                ZAY_XYWH_UI(panel, panel.w() - 50 + 1, 0, 25, panel.h(), "dom-search-x",
                    ZAY_GESTURE_T(t, this)
                    {
                        if(t == GT_InReleased)
                            mPipe.SetDOMSearch("");
                    })
                ZAY_LTRB(panel, 3, 3, panel.w() - 3, panel.h() - 4)
                {
                    const bool IsFocused = !!(panel.state("dom-search-x") & PS_Focused);
                    const bool IsDragging = !!(panel.state("dom-search-x") & PS_Dragging);
                    ZAY_MOVE_IF(panel, 0, 1, IsDragging)
                    {
                        ZAY_RGBA(panel, 255, 255, 255, (IsFocused)? 96 : 64)
                            panel.fill();
                        ZAY_RGBA(panel, 255, 255, 255, (IsFocused)? 192 : 128)
                        {
                            panel.text(panel.w() / 2 + 1, panel.h() / 2 - 1, "×");
                            panel.rect(1);
                        }
                    }
                }

                // 열기/닫기
                ZAY_LTRB_UI(panel, panel.w() - 25, 0, panel.w(), panel.h(), "dom-expand",
                    ZAY_GESTURE_T(t, this)
                    {
                        if(t == GT_InReleased)
                            mPipe.SetExpandDOM(!mPipe.expanddom());
                    })
                ZAY_LTRB(panel, 3, 3, panel.w() - 3, panel.h() - 4)
                {
                    const bool IsFocused = !!(panel.state("dom-expand") & PS_Focused);
                    const bool IsDragging = !!(panel.state("dom-expand") & PS_Dragging);
                    ZAY_MOVE_IF(panel, 0, 1, IsDragging)
                    {
                        ZAY_RGBA(panel, 255, 255, 255, (IsFocused)? 96 : 64)
                            panel.fill();
                        ZAY_RGBA(panel, 255, 255, 255, (IsFocused)? 192 : 128)
                        {
                            ZAY_FONT(panel, 0.75)
                                panel.text((mPipe.expanddom())? "▲" : "▼");
                            panel.rect(1);
                        }
                    }
                }
            }

            // 바디
            if(mPipe.expanddom())
            ZAY_XYWH_SCISSOR(panel, 0, TitleHeight, panel.w(), Math::Max(ElementHeight, panel.h() - TitleHeight))
            {
                ZAY_RGBA(panel, 0, 0, 0, 128)
                    panel.fill();
                ZAY_SCROLL_UI(panel, 0, ViewHeight, "dom-body-scroll",
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
                        else if(t == GT_WheelUp)
                        {
                            auto PosY = v->scrollpos("dom-body-scroll").y;
                            v->moveScroll("dom-body-scroll", 0, PosY, 0, PosY + 1000, 2.0, true);
                        }
                        else if(t == GT_WheelDown)
                        {
                            auto PosY = v->scrollpos("dom-body-scroll").y;
                            v->moveScroll("dom-body-scroll", 0, PosY, 0, PosY - 1000, 2.0, true);
                        }
                    }, {-1, 5}, 20)
                {
                    // 페이로드를 구성
                    struct Payload
                    {
                        zayproData* self {nullptr};
                        ZayPanel* panel {nullptr};
                        sint32 i {0};
                        sint32 count {0};
                        const uint64 msec {Platform::Utility::CurrentTimeMsec()};
                        const String* search {nullptr};
                        bool needupdate {false};
                    } NewPayload;
                    NewPayload.self = this;
                    NewPayload.panel = &panel;
                    if(0 < mPipe.domsearch().Length())
                        NewPayload.search = &mPipe.domsearch();

                    // DOM순회
                    DOM.AccessByCallback(
                        [](const MapPath* path, ZEWidgetPipe::Document* doc, payload data)->void
                        {
                            // 검색어처리
                            auto& Data = *((Payload*) data);
                            chararray Variable;
                            if(Data.search)
                            {
                                Variable = path->GetPath();
                                if(String(&Variable[0]).Find(0, *Data.search) == -1)
                                {
                                    Data.i++;
                                    return;
                                }
                            }

                            // 한줄처리
                            ZAY_XYWH_SCISSOR(*Data.panel, 0, Data.count * ElementHeight, Data.panel->w(), ElementHeight)
                            {
                                if(!Data.search)
                                    Variable = path->GetPath();

                                const String VariableText = &Variable[0];
                                const String FormulaTextFront = String::Format("%s = %s", (chars) VariableText, (chars) doc->mResult);
                                const String FormulaTextRear = (doc->mResult.Compare(doc->mFormula) && doc->mResult.Compare('\'' + doc->mFormula + '\''))?
                                    " ← " + doc->mFormula : "";
                                const sint32 FormulaTextRearSize = Platform::Graphics::GetStringWidth(FormulaTextRear);
                                const String FormulaText = FormulaTextFront + FormulaTextRear;
                                const String FullTextFront = " ● " + FormulaTextFront;
                                const sint32 FullTextFrontSize = Platform::Graphics::GetStringWidth(FullTextFront);
                                const bool NeedToolTop = (Data.panel->w() < FullTextFrontSize + FormulaTextRearSize);

                                const String UIName = String::Format("dom-body-scroll.%d", Data.i);
                                ZAY_INNER_UI(*Data.panel, 0, UIName,
                                    ZAY_GESTURE_VNT(v, n, t, VariableText, FormulaText, NeedToolTop)
                                    {
                                        if(t == GT_ToolTip)
                                        {
                                            if(NeedToolTop)
                                                Platform::Popup::ShowToolTip(FormulaText);
                                        }
                                        else if(t == GT_Pressed)
                                        {
                                            v->stopScroll("dom-body-scroll", true);
                                            Platform::Utility::SendToTextClipboard("d." + VariableText);
                                        }
                                        else if(t == GT_WheelUp)
                                        {
                                            auto PosY = v->scrollpos("dom-body-scroll").y;
                                            v->moveScroll("dom-body-scroll", 0, PosY, 0, PosY + 1000, 2.0, true);
                                        }
                                        else if(t == GT_WheelDown)
                                        {
                                            auto PosY = v->scrollpos("dom-body-scroll").y;
                                            v->moveScroll("dom-body-scroll", 0, PosY, 0, PosY - 1000, 2.0, true);
                                        }
                                    })
                                {
                                    const sint32 CurAni = 255 * Math::Min(1000, Data.msec - doc->mUpdateMsec) / 1000;
                                    Data.needupdate |= (CurAni < 255);
                                    // 배경색
                                    if(Data.i & 1)
                                    ZAY_RGBA(*Data.panel, 0, 0, 0, 8)
                                        Data.panel->fill();
                                    // 포커싱효과
                                    if(Data.panel->state(UIName) & PS_Focused)
                                        ZAY_RGBA(*Data.panel, 0, 0, 0, 32)
                                            Data.panel->fill();
                                    // 스트링
                                    ZAY_MOVE_IF(*Data.panel, 1, 1, Data.panel->state(UIName) & PS_Pressed)
                                    {
                                        // 계산결과
                                        ZAY_RGB(*Data.panel, 255, 255, CurAni)
                                            Data.panel->text(FullTextFront, UIFA_LeftMiddle, UIFE_Right);
                                        // 계산식
                                        if(0 < FormulaTextRearSize && FullTextFrontSize < Data.panel->w())
                                        {
                                            ZAY_LTRB(*Data.panel, FullTextFrontSize, 0, Data.panel->w(), Data.panel->h())
                                            ZAY_RGB(*Data.panel, 255 - CurAni, 255, CurAni)
                                                Data.panel->text(FormulaTextRear, UIFA_LeftMiddle, UIFE_Right);
                                        }
                                    }
                                }
                            }
                            Data.i++;
                            Data.count++;
                        }, &NewPayload);

                    // DOM갱신
                    if(mPipe.domcount() != NewPayload.count)
                    {
                        mPipe.SetDOMCount(NewPayload.count);
                        invalidate(2);
                    }
                    if(NewPayload.needupdate)
                        invalidate(2);
                }
            }

            ZAY_RGB(panel, 0, 0, 0)
                panel.rect(1);
        }
    }
}

void zayproData::RenderMiniMap(ZayPanel& panel)
{
    // 최대최소 검사
    Rect ScreenRect(0, 0, mWorkViewSize.w, mWorkViewSize.h);
    Rect SumRect = ScreenRect;
    for(sint32 i = 0, iend = ZEZayBox::TOP().Count(); i < iend; ++i)
    {
        auto CurBox = ZEZayBox::TOP().AccessByOrder(i);
        auto CurRect = (*CurBox)->GetRect() + mWorkViewScroll;
        SumRect.l = Math::MinF(SumRect.l, CurRect.l);
        SumRect.t = Math::MinF(SumRect.t, CurRect.t);
        SumRect.r = Math::MaxF(SumRect.r, CurRect.r);
        SumRect.b = Math::MaxF(SumRect.b, CurRect.b);
    }

    const bool IsWidthTaller = (SumRect.Width() > SumRect.Height());
    const sint32 TitleHeight = 22;
    const sint32 ViewWidth = (IsWidthTaller)? 200 : 200 * SumRect.Width() / SumRect.Height();
    const sint32 ViewHeight = (IsWidthTaller)? 200 * SumRect.Height() / SumRect.Width() : 200;
    ZAY_XYWH(panel, panel.w() - 10 - ViewWidth, 10, ViewWidth, TitleHeight + ViewHeight)
    {
        // 타이틀
        ZAY_LTRB_UI_SCISSOR(panel, 0, 0, panel.w(), TitleHeight, "map-title")
        {
            ZAY_RGB(panel, 0, 0, 0)
                panel.fill();
            ZAY_RGB(panel, 255, 255, 255)
                panel.text(5, panel.h() / 2 - 2, "map", UIFA_LeftMiddle);
        }

        // 바디
        ZAY_LTRB_UI_SCISSOR(panel, 0, TitleHeight, panel.w(), panel.h(), "map-body",
            ZAY_GESTURE_VNTXY(v, n, t, x, y, this, SumRect)
            {
                if(t == GT_Moving || t == GT_MovingIdle)
                    mShowCommentTagMsec = Platform::Utility::CurrentTimeMsec() + 2000;
                else if(t == GT_Pressed || t == GT_InDragging || t == GT_OutDragging)
                {
                    const Rect MapRect(v->rect(n));
                    const sint32 X = SumRect.l + SumRect.Width() * (x - MapRect.l) / MapRect.Width();
                    const sint32 Y = SumRect.t + SumRect.Height() * (y - MapRect.t) / MapRect.Height();
                    mWorkViewDrag = Point(mWorkViewSize.w / 2 - X, mWorkViewSize.h / 2 - Y);
                }
            })
        {
            ZAY_RGBA(panel, 0, 0, 0, 128)
                panel.fill();

            // 출력
            const Point BeginPos(SumRect.l, SumRect.t);
            const float Rate = panel.w() / SumRect.Width();
            for(sint32 i = 0, iend = ZEZayBox::TOP().Count(); i < iend; ++i)
            {
                auto CurBox = ZEZayBox::TOP().AccessByOrder(i);
                auto CurRect = (*CurBox)->GetRect() + mWorkViewScroll;
                CurRect -= BeginPos;
                CurRect *= Rate;
                ZAY_RECT(panel, CurRect)
                ZAY_COLOR(panel, (*CurBox)->color())
                {
                    // 연결선
                    if((*CurBox)->hooked())
                    {
                        auto CurHookPos = (*CurBox)->hookpos() * Rate;
                        ZAY_RGB(panel, 192, 192, 192)
                            panel.line(CurHookPos, Point(0, 0), 1);
                    }
                    // 영역
                    ZAY_RGB(panel, 128, 128, 128)
                        panel.fill();
                }
            }

            // 화면영역
            ScreenRect -= BeginPos;
            ScreenRect *= Rate;
            ZAY_RECT(panel, ScreenRect)
            for(sint32 i = 0; i < 8; ++i)
            {
                ZAY_INNER(panel, -i)
                ZAY_RGBA(panel, 255, 0, 200, 200 * (8 - i) / 8)
                    panel.rect(1);
            }
        }

        ZAY_RGB(panel, 0, 0, 0)
            panel.rect(1);
    }

    // 코멘트 태그
    if(mShowCommentTagMsec != 0)
    {
        const sint32 Alpha = Math::Clamp(mShowCommentTagMsec - Platform::Utility::CurrentTimeMsec(), 0, 1000) * 128 / 1000;
        const sint32 Gap = 10, InnerGap = 2;
        const sint32 TextHeight = Platform::Graphics::GetStringHeight();
        ZAY_LTRB(panel, 0, Gap + InnerGap, panel.w() - 10 - ViewWidth - Gap + InnerGap, panel.h())
        {
            Array<sint32, datatype_pod_canmemcpy_zeroset> XSums;
            sint32 i = 0, iY = 0, MaxXSum = 100, NextMaxXSum = 100;
            for(bool IsValid = ZEZayBox::CommentTagService::SetFirstFocus(); IsValid; IsValid = ZEZayBox::CommentTagService::SetNextFocus(), ++i)
            {
                auto& CurXSum = XSums.AtWherever(iY);
                const sint32 TextPosY = (TextHeight + Gap) * iY;
                chars Text = ZEZayBox::CommentTagService::GetFocusText();
                const Color BGColor = ZEZayBox::CommentTagService::GetFocusColor();
                const sint32 TextWidth = Platform::Graphics::GetStringWidth(Text);
                const Point TargetPos = ZEZayBox::CommentTagService::GetFocusPos() + mWorkViewScroll;

                const String UIName = String::Format("tag-%d", i);
                ZAY_XYWH(panel, panel.w() - (CurXSum + TextWidth), TextPosY, TextWidth, TextHeight)
                ZAY_INNER_UI(panel, -Gap / 2, UIName,
                    ZAY_GESTURE_T(t, this, TargetPos)
                    {
                        if(t == GT_Moving || t == GT_MovingIdle)
                            mShowCommentTagMsec = Platform::Utility::CurrentTimeMsec() + 2000;
                        else if(t == GT_Pressed)
                            mWorkViewDrag = Point(mWorkViewSize.w / 2 - TargetPos.x, mWorkViewSize.h / 2 - TargetPos.y);
                    })
                ZAY_INNER(panel, Gap / 2 - InnerGap)
                {
                    const bool IsFocused = !!(panel.state(UIName) & PS_Focused);
                    ZAY_RGBA(panel, BGColor.r, BGColor.g, BGColor.b, (IsFocused)? 255 : 192)
                    ZAY_RGBA(panel, 128, 128, 128, Alpha)
                        panel.fill();
                    ZAY_RGBA(panel, 0, 0, 0, (IsFocused)? 255 : 192)
                    ZAY_RGBA(panel, 128, 128, 128, Alpha)
                        panel.text(Text);
                    ZAY_RGB(panel, 0, 0, 0)
                    ZAY_RGBA(panel, 128, 128, 128, Alpha)
                        panel.rect(1);
                }
                NextMaxXSum = Math::Max(NextMaxXSum, CurXSum + TextWidth + TextPosY);
                CurXSum += TextWidth + Gap;
                // 위치재조정
                if(MaxXSum <= TextPosY)
                {iY = 0; MaxXSum = NextMaxXSum;}
                else if(MaxXSum - TextPosY < CurXSum) iY++;
            }
        }
    }
}

void zayproData::RenderLogList(ZayPanel& panel)
{
    auto CurElement = mPipe.GetLogElement();
    if(CurElement)
    {
        const sint32 TitleHeight = 23;
        const sint32 ViewWidth = 300;
        ZAY_XYWH(panel, panel.w() - 10 - ViewWidth, panel.h() - 10 - TitleHeight, ViewWidth, TitleHeight)
        {
            // 타이틀
            ZAY_INNER_UI_SCISSOR(panel, 0, "log-title")
            {
                ZAY_RGB(panel, 0, 0, 0)
                    panel.fill();
                ZAY_RGB(panel, 255, 255, 255)
                    panel.text(5, panel.h() / 2 - 2, "log", UIFA_LeftMiddle);
                // 열기/닫기
                ZAY_LTRB_UI(panel, panel.w() - 25 - 1, 1, panel.w() - 1, panel.h(), "log-expand",
                    ZAY_GESTURE_T(t, this)
                    {
                        if(t == GT_InReleased)
                            mPipe.SetExpandLog(!mPipe.expandlog());
                    })
                ZAY_LTRB(panel, 3, 3, panel.w() - 3, panel.h() - 4)
                {
                    const bool IsFocused = !!(panel.state("log-expand") & PS_Focused);
                    const bool IsDragging = !!(panel.state("log-expand") & PS_Dragging);
                    ZAY_MOVE_IF(panel, 0, 1, IsDragging)
                    {
                        ZAY_RGBA(panel, 255, 255, 255, (IsFocused)? 96 : 64)
                            panel.fill();
                        ZAY_RGBA(panel, 255, 255, 255, (IsFocused)? 192 : 128)
                        {
                            ZAY_FONT(panel, 0.75)
                                panel.text((mPipe.expandlog())? "▼" : "▲");
                            panel.rect(1);
                        }
                    }
                }
            }

            // 로그들
            if(mPipe.expandlog())
            {
                const sint32 InnerGap = 3;
                const sint32 ElementHeight = 22 + InnerGap * 2;
                ZAY_XYWH(panel, 1 - InnerGap, -ElementHeight - InnerGap, panel.w() + InnerGap * 2 - 2, ElementHeight)
                for(sint32 y = 0; CurElement && 0 < panel.toview(0, 0).y + y + panel.h(); y -= ElementHeight)
                {
                    ZAY_MOVE(panel, 0, y)
                    ZAY_INNER(panel, 3)
                        CurElement->Render(panel);
                    CurElement = mPipe.NextLogElement(CurElement);
                }
            }
        }
    }
}

void zayproData::RenderTitleBar(ZayPanel& panel)
{
    ZAY_LTRB_UI(panel, 0, 0, panel.w(), mTitleHeight, "ui_title",
        ZAY_GESTURE_T(t, this)
        {
            static point64 OldCursorPos;
            static rect128 OldWindowRect;
            if(!Platform::Utility::IsFullScreen())
            {
                if(t == GT_Moving || t == GT_MovingIdle)
                    SetCursor(CR_SizeAll);
                else if(t == GT_MovingLosed)
                    SetCursor(CR_Arrow);
                else if(t == GT_Pressed)
                {
                    Platform::Utility::GetCursorPos(OldCursorPos);
                    OldWindowRect = Platform::GetWindowRect();
                    clearCapture();
                }
                else if(t == GT_InDragging || t == GT_OutDragging)
                {
                    point64 CurCursorPos;
                    Platform::Utility::GetCursorPos(CurCursorPos);
                    Platform::SetWindowPos(
                        OldWindowRect.l + CurCursorPos.x - OldCursorPos.x,
                        OldWindowRect.t + CurCursorPos.y - OldCursorPos.y);
                    invalidate();
                }
            }
        })
    {
        // 배경색
        ZAY_RGB(panel, 60, 76, 91)
            panel.fill();

        // 로고
        ZAY_LTRB(panel, 0, 0, panel.h(), panel.h())
            panel.icon(R("logo"), UIA_CenterMiddle);

        // 타이틀
        ZAY_RGB(panel, 129, 165, 200)
        ZAY_FONT(panel, 1.4, gTitleFont)
        ZAY_LTRB(panel, panel.h() + 5, 0, panel.w() - 10 - 30 * 3, panel.h())
            panel.text(mWindowName, UIFA_LeftMiddle, UIFE_Right);

        #if !BOSS_WASM
            // 최소화버튼
            ZAY_XYWH_UI(panel, panel.w() - 10 - 30 * 3, 10, 30, panel.h() - 20, "ui_win_min",
                ZAY_GESTURE_T(t)
                {
                    if(t == GT_InReleased)
                        Platform::Utility::SetMinimize();
                })
            {
                const bool Pressed = ((panel.state("ui_win_min") & (PS_Pressed | PS_Dropping)) == PS_Pressed);
                const bool Hovered = ((panel.state("ui_win_min") & (PS_Focused | PS_Dropping)) == PS_Focused);
                panel.icon(R((Pressed)? "btn_win_min_p" : ((Hovered)? "btn_win_min_o" : "btn_win_min_n")), UIA_CenterMiddle);
            }

            // 최대화버튼
            ZAY_XYWH_UI(panel, panel.w() - 10 - 30 * 2, 10, 30, panel.h() - 20, "ui_win_max",
                ZAY_GESTURE_T(t)
                {
                    if(t == GT_InReleased)
                    {
                        if(Platform::Utility::IsFullScreen())
                            Platform::Utility::SetNormalWindow();
                        else Platform::Utility::SetFullScreen(false, 1);
                    }
                })
            {
                const bool Pressed = ((panel.state("ui_win_max") & (PS_Pressed | PS_Dropping)) == PS_Pressed);
                const bool Hovered = ((panel.state("ui_win_max") & (PS_Focused | PS_Dropping)) == PS_Focused);
                panel.icon(R((Pressed)? "btn_win_max_p" : ((Hovered)? "btn_win_max_o" : "btn_win_max_n")), UIA_CenterMiddle);
            }

            // 종료버튼
            ZAY_XYWH_UI(panel, panel.w() - 10 - 30 * 1, 10, 30, panel.h() - 20, "ui_win_close",
                ZAY_GESTURE_T(t)
                {
                    if(t == GT_InReleased)
                        Platform::Utility::ExitProgram();
                })
            {
                const bool Pressed = ((panel.state("ui_win_close") & (PS_Pressed | PS_Dropping)) == PS_Pressed);
                const bool Hovered = ((panel.state("ui_win_close") & (PS_Focused | PS_Dropping)) == PS_Focused);
                panel.icon(R((Pressed)? "btn_win_close_p" : ((Hovered)? "btn_win_close_o" : "btn_win_close_n")), UIA_CenterMiddle);
            }
        #endif
    }
}

void zayproData::RenderOutline(ZayPanel& panel)
{
    ZAY_INNER(panel, 1)
    ZAY_RGBA(panel, 0, 0, 0, 32)
        panel.rect(1);

    // 리사이징바
    ZAY_RGBA(panel, 31, 198, 253, 64 + 128 * Math::Abs(((frame() * 2) % 100) - 50) / 50)
    {
        if(mNcLeftBorder)
        {
            for(sint32 i = 0; i < 5; ++i)
            ZAY_RGBA(panel, 128, 128, 128, 128 - i * 25)
            ZAY_XYWH(panel, i, 0, 1, panel.h())
                panel.fill();
            invalidate(2);
        }
        if(mNcTopBorder)
        {
            for(sint32 i = 0; i < 5; ++i)
            ZAY_RGBA(panel, 128, 128, 128, 128 - i * 25)
            ZAY_XYWH(panel, 0, i, panel.w(), 1)
                panel.fill();
            invalidate(2);
        }
        if(mNcRightBorder)
        {
            for(sint32 i = 0; i < 5; ++i)
            ZAY_RGBA(panel, 128, 128, 128, 128 - i * 25)
            ZAY_XYWH(panel, panel.w() - 1 - i, 0, 1, panel.h())
                panel.fill();
            invalidate(2);
        }
        if(mNcBottomBorder)
        {
            for(sint32 i = 0; i < 5; ++i)
            ZAY_RGBA(panel, 128, 128, 128, 128 - i * 25)
            ZAY_XYWH(panel, 0, panel.h() - 1 - i, panel.w(), 1)
                panel.fill();
            invalidate(2);
        }
    }

    // 윈도우 리사이징 모듈
    #define RESIZING_MODULE(C, L, T, R, B, BORDER) do {\
        static point64 OldMousePos; \
        static rect128 OldWindowRect; \
        if(!Platform::Utility::IsFullScreen()) \
        { \
            if(t == GT_Moving) \
            { \
                SetCursor(C); \
                mNcLeftBorder = false; \
                mNcTopBorder = false; \
                mNcRightBorder = false; \
                mNcBottomBorder = false; \
                BORDER = true; \
            } \
            else if(t == GT_MovingLosed) \
            { \
                SetCursor(CR_Arrow); \
            } \
            else if(t == GT_Pressed) \
            { \
                Platform::Utility::GetCursorPos(OldMousePos); \
                OldWindowRect = Platform::GetWindowRect(); \
            } \
            else if(t == GT_InDragging || t == GT_OutDragging || t == GT_InReleased || t == GT_OutReleased || t == GT_CancelReleased) \
            { \
                point64 NewMousePos; \
                Platform::Utility::GetCursorPos(NewMousePos); \
                const rect128 NewWindowRect = {L, T, R, B}; \
                Platform::SetWindowRect(NewWindowRect.l, NewWindowRect.t, \
                    NewWindowRect.r - NewWindowRect.l, NewWindowRect.b - NewWindowRect.t); \
            } \
        }} while(false);

    // 윈도우 리사이징 꼭지점
    const sint32 SizeBorder = 10;
    ZAY_LTRB_UI(panel, 0, 0, SizeBorder, SizeBorder, "NcLeftTop",
        ZAY_GESTURE_T(t, this)
        {
            RESIZING_MODULE(CR_SizeFDiag,
                MoveNcLeft(OldWindowRect, NewMousePos.x - OldMousePos.x),
                MoveNcTop(OldWindowRect, NewMousePos.y - OldMousePos.y),
                OldWindowRect.r,
                OldWindowRect.b,
                mNcLeftBorder = mNcTopBorder);
        });
    ZAY_LTRB_UI(panel, panel.w() - SizeBorder, 0, panel.w(), SizeBorder, "NcRightTop",
        ZAY_GESTURE_T(t, this)
        {
            RESIZING_MODULE(CR_SizeBDiag,
                OldWindowRect.l,
                MoveNcTop(OldWindowRect, NewMousePos.y - OldMousePos.y),
                MoveNcRight(OldWindowRect, NewMousePos.x - OldMousePos.x),
                OldWindowRect.b,
                mNcRightBorder = mNcTopBorder);
        });
    ZAY_LTRB_UI(panel, 0, panel.h() - SizeBorder, SizeBorder, panel.h(), "NcLeftBottom",
        ZAY_GESTURE_T(t, this)
        {
            RESIZING_MODULE(CR_SizeBDiag,
                MoveNcLeft(OldWindowRect, NewMousePos.x - OldMousePos.x),
                OldWindowRect.t,
                OldWindowRect.r,
                MoveNcBottom(OldWindowRect, NewMousePos.y - OldMousePos.y),
                mNcLeftBorder = mNcBottomBorder);
        });
    ZAY_LTRB_UI(panel, panel.w() - SizeBorder, panel.h() - SizeBorder, panel.w(), panel.h(), "NcRightBottom",
        ZAY_GESTURE_T(t, this)
        {
            RESIZING_MODULE(CR_SizeFDiag,
                OldWindowRect.l,
                OldWindowRect.t,
                MoveNcRight(OldWindowRect, NewMousePos.x - OldMousePos.x),
                MoveNcBottom(OldWindowRect, NewMousePos.y - OldMousePos.y),
                mNcRightBorder = mNcBottomBorder);
        });

    // 윈도우 리사이징 모서리
    ZAY_LTRB_UI(panel, 0, SizeBorder, SizeBorder, panel.h() - SizeBorder, "NcLeft",
        ZAY_GESTURE_T(t, this)
        {
            RESIZING_MODULE(CR_SizeHor,
                MoveNcLeft(OldWindowRect, NewMousePos.x - OldMousePos.x),
                OldWindowRect.t,
                OldWindowRect.r,
                OldWindowRect.b,
                mNcLeftBorder);
        });
    ZAY_LTRB_UI(panel, SizeBorder, 0, panel.w() - SizeBorder, SizeBorder, "NcTop",
        ZAY_GESTURE_T(t, this)
        {
            RESIZING_MODULE(CR_SizeVer,
                OldWindowRect.l,
                MoveNcTop(OldWindowRect, NewMousePos.y - OldMousePos.y),
                OldWindowRect.r,
                OldWindowRect.b,
                mNcTopBorder);
        });
    ZAY_LTRB_UI(panel, panel.w() - SizeBorder, SizeBorder, panel.w(), panel.h() - SizeBorder, "NcRight",
        ZAY_GESTURE_T(t, this)
        {
            RESIZING_MODULE(CR_SizeHor,
                OldWindowRect.l,
                OldWindowRect.t,
                MoveNcRight(OldWindowRect, NewMousePos.x - OldMousePos.x),
                OldWindowRect.b,
                mNcRightBorder);
        });
    ZAY_LTRB_UI(panel, SizeBorder, panel.h() - SizeBorder, panel.w() - SizeBorder, panel.h(), "NcBottom",
        ZAY_GESTURE_T(t, this)
        {
            RESIZING_MODULE(CR_SizeVer,
                OldWindowRect.l,
                OldWindowRect.t,
                OldWindowRect.r,
                MoveNcBottom(OldWindowRect, NewMousePos.y - OldMousePos.y),
                mNcBottomBorder);
        });
}

void zayproData::SetCursor(CursorRole role)
{
    if(mNowCursor != role)
    {
        mNowCursor = role;
        Platform::Utility::SetCursor(role);
        if(mNowCursor != CR_SizeVer && mNowCursor != CR_SizeHor && mNowCursor != CR_SizeBDiag && mNowCursor != CR_SizeFDiag && mNowCursor != CR_SizeAll)
        {
            mNcLeftBorder = false;
            mNcTopBorder = false;
            mNcRightBorder = false;
            mNcBottomBorder = false;
        }
    }
}

sint32 zayproData::MoveNcLeft(const rect128& rect, sint32 addx)
{
    const sint32 NewLeft = rect.l + addx;
    return rect.r - Math::Max(mMinWindowWidth, rect.r - NewLeft);
}

sint32 zayproData::MoveNcTop(const rect128& rect, sint32 addy)
{
    const sint32 NewTop = rect.t + addy;
    return rect.b - Math::Max(mMinWindowHeight, rect.b - NewTop);
}

sint32 zayproData::MoveNcRight(const rect128& rect, sint32 addx)
{
    const sint32 NewRight = rect.r + addx;
    return rect.l + Math::Max(mMinWindowWidth, NewRight - rect.l);
}

sint32 zayproData::MoveNcBottom(const rect128& rect, sint32 addy)
{
    const sint32 NewBottom = rect.b + addy;
    return rect.t + Math::Max(mMinWindowHeight, NewBottom - rect.t);
}
