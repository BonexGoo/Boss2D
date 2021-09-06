#include <boss.hpp>
#include "zayeditor.hpp"

#include <resource.hpp>

ZAY_DECLARE_VIEW_CLASS("zayeditorView", zayeditorData)

ZAY_VIEW_API OnCommand(CommandType type, chars topic, id_share in, id_cloned_share* out)
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
        if(1 <= Math::Distance(0, 0, m->mWorkViewDrag.x, m->mWorkViewDrag.y))
        {
            Point Drag = Point(m->mWorkViewDrag.x / 20, m->mWorkViewDrag.y / 20);
            for(sint32 i = 0, iend = m->mBoxMap.Count(); i < iend; ++i)
            {
                auto CurBox = m->mBoxMap.AccessByOrder(i);
                (*CurBox)->Move(Drag);
            }
            m->mWorkViewDrag -= Drag;
            m->invalidate();
        }
    }
}

ZAY_VIEW_API OnNotify(NotifyType type, chars topic, id_share in, id_cloned_share* out)
{
    if(type == NT_KeyPress)
    {
        const sint32 KeyCode = sint32o(in).ConstValue();
        branch;
        jump(KeyCode == 0x01000012) // Left
            m->mWorkViewDrag = Point(300, 0);
        jump(KeyCode == 0x01000013) // Up
            m->mWorkViewDrag = Point(0, 300);
        jump(KeyCode == 0x01000014) // Right
            m->mWorkViewDrag = Point(-300, 0);
        jump(KeyCode == 0x01000015) // Down
            m->mWorkViewDrag = Point(0, -300);
        jump(KeyCode == 0x01000034) // F5: 간단세이브
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
        m->ResetBoxes();
        ZEZayBox::ResetLastID(); // ID발급기 초기화

        const String JsonText = String::FromAsset(ContentName);
        const Context Json(ST_Json, SO_OnlyReference, (chars) JsonText);
        m->mBoxMap[0]->ReadJson(Json);
        m->mBoxMap[0]->LoadChildren(Json("ui"), m->mBoxMap, m->mZaySonAPI.GetCreator());
    }
    else if(type == NT_Normal)
    {
        branch;
        jump(!String::Compare(topic, "Update"))
        {
            m->invalidate(sint32o(in).ConstValue());
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
                const sint32 CurZayBoxID = sint32o(in).ConstValue();
                m->mBoxMap[CurZayBoxID]->AddChild(m->mBoxMap[m->mDraggingHook].Value());
                m->mBoxMap[CurZayBoxID]->Sort(m->mBoxMap);
            }
        }
        jump(!String::Compare(topic, "HookClear")) // 부모가 자식 전부와 연결해제
        {
            const sint32 CurZayBoxID = sint32o(in).ConstValue();
            m->mBoxMap[CurZayBoxID]->ClearChildrenHook(m->mBoxMap);
        }
        jump(!String::Compare(topic, "ZayBoxMove"))
        {
            const sint32 CurZayBoxID = sint32o(in).ConstValue();
            m->mBoxMap[CurZayBoxID]->FlushTitleDrag(m->mBoxMap);
            m->mWorkViewDrag = Point(); // 드래그하던 것이 있다면 중지
        }
        jump(!String::Compare(topic, "ZayBoxMoveWith"))
        {
            const sint32 CurZayBoxID = sint32o(in).ConstValue();
            m->mBoxMap[CurZayBoxID]->FlushTitleDragWith(m->mBoxMap, true);
            m->mWorkViewDrag = Point(); // 드래그하던 것이 있다면 중지
        }
        jump(!String::Compare(topic, "ZayBoxResize"))
        {
            const sint32s Values(in);
            const sint32 CurZayBoxID = Values[0];
            const sint32 AddValue = Values[1];
            m->mBoxMap[CurZayBoxID]->Resize(m->mBoxMap, AddValue);
            m->mWorkViewDrag = Point(); // 드래그하던 것이 있다면 중지
        }
        jump(!String::Compare(topic, "ZayBoxCopy"))
        {
            const sint32 CurZayBoxID = sint32o(in).ConstValue();
            const sint32 NewZayBoxID = m->mBoxMap[CurZayBoxID]->Copy(m->mBoxMap, m->mZaySonAPI.GetCreator());
            const sint32 ParentZayBoxID = m->mBoxMap[CurZayBoxID]->parent();
            if(NewZayBoxID != -1 && ParentZayBoxID != -1)
                m->mBoxMap[ParentZayBoxID]->ChangeChild(
                    m->mBoxMap[CurZayBoxID].Value(), m->mBoxMap[NewZayBoxID].Value());
            m->mBoxMap[CurZayBoxID]->ClearMyHook();
        }
        jump(!String::Compare(topic, "ZayBoxSort"))
        {
            const sint32 CurZayBoxID = sint32o(in).ConstValue();
            m->mBoxMap[CurZayBoxID]->Sort(m->mBoxMap);
        }
        jump(!String::Compare(topic, "ZayBoxRemove")) // 제이박스 삭제(0-remove)
        {
            const sint32 CurZayBoxID = sint32o(in).ConstValue();
            m->mBoxMap[CurZayBoxID]->ClearChildrenHook(m->mBoxMap);
            const sint32 ParentID = m->mBoxMap[CurZayBoxID]->parent();
            if(ParentID != -1)
                m->mBoxMap[ParentID]->SubChild(m->mBoxMap[CurZayBoxID].Value());
            m->mBoxMap.Remove(CurZayBoxID);
        }
        jump(!String::Compare(topic, "ZayBoxHookRemove"))
        {
            const sint32 CurZayBoxID = sint32o(in).ConstValue();
            m->mBoxMap[CurZayBoxID]->ClearParentHook(m->mBoxMap);
        }
        jump(!String::Compare(topic, "ZayBoxRemoveGroup")) // 제이박스 및 자식들 삭제(0-remove)
        {
            const sint32 CurZayBoxID = sint32o(in).ConstValue();
            m->mBoxMap[CurZayBoxID]->RemoveChildren(m->mBoxMap);
            const sint32 ParentID = m->mBoxMap[CurZayBoxID]->parent();
            if(ParentID != -1)
                m->mBoxMap[ParentID]->SubChild(m->mBoxMap[CurZayBoxID].Value());
            m->mBoxMap.Remove(CurZayBoxID);
        }
        jump(!String::Compare(topic, "ZayBoxParamRemove")) // 제이박스 파라미터 삭제(0-param-0-remove)
        {
            sint32s Args(in);
            const sint32 CurZayBoxID = Args[0];
            const sint32 CurZayBoxParamID = Args[1];
            m->mBoxMap[CurZayBoxID]->SubParam(CurZayBoxParamID);
        }
        jump(!String::Compare(topic, "ZayBoxValueRemove")) // 제이박스 밸류 삭제(0-value-0-remove)
        {
            sint32s Args(in);
            const sint32 CurZayBoxID = Args[0];
            const sint32 CurZayBoxValueID = Args[1];
            m->mBoxMap[CurZayBoxID]->SubInput(CurZayBoxValueID);
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
}

ZAY_VIEW_API OnGesture(GestureType type, sint32 x, sint32 y)
{
}

ZAY_VIEW_API OnRender(ZayPanel& panel)
{
    static const sint32 TitleHeight = 37;

    // 타이틀바
    ZAY_LTRB_UI(panel, 0, 0, panel.w(), TitleHeight, "title")
    {
        ZAY_RGB(panel, 60, 76, 91)
            panel.fill();

        // 로고
        ZAY_LTRB(panel, 0, 0, TitleHeight, TitleHeight)
            panel.icon(R("logo"), UIA_CenterMiddle);
    }

    // 뷰
    ZAY_LTRB_SCISSOR(panel, 0, TitleHeight, panel.w(), panel.h())
    {
        ZAY_RGB(panel, 51, 61, 73)
            panel.fill();

        // 작업뷰
        ZAY_XYWH_UI(panel, 0, 0, panel.w() - 240, panel.h(), "workspace",
            ZAY_GESTURE_VNTXY(v, n, t, x, y)
            {
                if(t == GT_InDragging || t == GT_OutDragging)
                {
                    auto& OldPos = v->oldxy(n);
                    const Point Drag(x - OldPos.x, y - OldPos.y);
                    for(sint32 i = 0, iend = m->mBoxMap.Count(); i < iend; ++i)
                    {
                        auto CurBox = m->mBoxMap.AccessByOrder(i);
                        (*CurBox)->Move(Drag);
                    }
                    v->invalidate();
                    m->mWorkViewDrag = Point(); // 드래그하던 것이 있다면 중지
                }
                else if(t == GT_Dropped && m->mZaySonAPI.mDraggingComponentID != -1)
                {
                    // 타이틀바만큼 올림
                    m->mZaySonAPI.mDraggingComponentRect.t -= TitleHeight;
                    m->mZaySonAPI.mDraggingComponentRect.b -= TitleHeight;

                    // 컴포넌트 추가
                    auto& CurComponent = m->mZaySonAPI.GetComponent(m->mZaySonAPI.mDraggingComponentID);
                    ZEZayBoxObject NewZayBox = m->mZaySonAPI.GetCreator()(CurComponent.mName);
                    NewZayBox->InitCompID();
                    m->mBoxMap[NewZayBox->id()] = NewZayBox;
                }
            })
        {
            m->mWorkViewSize = Size(panel.w(), panel.h());
            if(m->mZaySonAPI.mDraggingComponentID != -1 && (panel.state("workspace") & PS_Dropping))
            {
                ZAY_INNER(panel, 4)
                ZAY_RGBA(panel, 255, 255, 255, 20)
                    panel.rect(4);
            }

            // 박스
            for(sint32 i = 0, iend = m->mBoxMap.Count(); i < iend; ++i)
            {
                auto CurBox = m->mBoxMap.AccessByOrder(i);
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
        ZAY_XYWH_UI(panel, panel.w() - 240, 0, 240, panel.h(), "apispace")
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
                        if(t == GT_InReleased)
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
                        if(t == GT_InReleased)
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
                            if(t == GT_InReleased)
                                Platform::Popup::FileContentDialog(L"All File(*.*)\0*.*\0Json File\0*.json\0");
                        };
                    #else
                        ZAY_GESTURE_T(t)
                        {
                            if(t == GT_InReleased)
                            {
                                String JsonPath;
                                if(Platform::Popup::FileDialog(DST_FileOpen, JsonPath, nullptr, "Load json"))
                                {
                                    m->mPipe.ResetJsonPath(JsonPath);
                                    m->ResetBoxes();
                                    ZEZayBox::ResetLastID(); // ID발급기 초기화

                                    const String JsonText = String::FromFile(JsonPath);
                                    const Context Json(ST_Json, SO_OnlyReference, (chars) JsonText);
                                    m->mBoxMap[0]->ReadJson(Json);
                                    m->mBoxMap[0]->LoadChildren(Json("ui"), m->mBoxMap, m->mZaySonAPI.GetCreator());
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
                        if(t == GT_InReleased)
                        {
                            String JsonPath;
                            if(Platform::Popup::FileDialog(DST_FileSave, JsonPath, nullptr, "Save json"))
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
                                Context Json;
                                m->mBoxMap[0]->WriteJson(Json);
                                m->mBoxMap[0]->SaveChildren(Json.At("ui"), m->mBoxMap);
                                Json.SaveJson().ToFile(JsonPath, true);
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
                            const sint32 NewPos = OldPos + ((t == GT_WheelUp)? 200 : -200);
                            v->moveScroll("apispace-scroll", 0, OldPos, 0, NewPos, 1.0, true);
                            v->invalidate(2);
                        }
                    }, 5, 20)
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
}

ZEFakeZaySon::ZEFakeZaySon()
{
    mCreator = [this](chars compname)->ZEZayBoxObject
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
                NewZayBox = ZEZayBoxContent::Create(false, false, CurComponent->mParamComment);
                break;
            case ZayExtend::ComponentType::ContentWithParameter:
                NewZayBox = ZEZayBoxContent::Create(false, true, CurComponent->mParamComment);
                break;
            case ZayExtend::ComponentType::Option:
                NewZayBox = ZEZayBoxContent::Create(true, true, CurComponent->mParamComment);
                break;
            case ZayExtend::ComponentType::Layout:
                NewZayBox = ZEZayBoxLayout::Create(CurComponent->mParamComment);
                break;
            case ZayExtend::ComponentType::Code:
                NewZayBox = ZEZayBoxCode::Create();
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
        colorres = "box_title_d";
        return Color(0, 181, 240, 192);
    case ZayExtend::ComponentType::Option:
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

ZaySonInterface& ZEFakeZaySon::AddComponent(ZayExtend::ComponentType type, chars name, ZayExtend::ComponentCB cb, chars paramcomment)
{
    auto& NewComponent = mComponents.AtAdding();
    NewComponent.mType = type;
    NewComponent.mName = name;
    NewComponent.mColor = GetComponentColor(type, NewComponent.mColorRes);
    NewComponent.mParamComment = (paramcomment)? paramcomment : "";
    return *this;
}

ZaySonInterface& ZEFakeZaySon::AddGlue(chars name, ZayExtend::GlueCB cb)
{
    return *this;
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

ZEZayBox::CreatorCB ZEFakeZaySon::GetCreator() const
{
    return mCreator;
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
                PipeName = String::Format("zayeditor:%08X", RandomValue);
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
            Platform::SetWindowName("ZAYPRO");
        else
        {
            #if BOSS_WASM
                Platform::SetWindowName("[" + mLastJsonPath + ((NewConnected)? "] + ZAYPRO" : "] - HUCLOUD ZAYPRO"));
            #else
                Platform::SetWindowName("[" + String::FromWChars(Platform::File::GetShortName(WString::FromChars(mLastJsonPath))) +
                    ((NewConnected)? "] + ZAYPRO" : "] - ZAYPRO"));
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
        chars Type = (*NewJson)("type").GetText();
        if(!String::Compare(Type, "dom_updated"))
        {
            auto& CurDOM = mDOM((*NewJson)("variable").GetText());
            CurDOM.mResult = (*NewJson)("result").GetText();
            CurDOM.mFormula = (*NewJson)("formula").GetText();
            CurDOM.mUpdateMsec = Platform::Utility::CurrentTimeMsec();
        }
        else if(!String::Compare(Type, "dom_removed"))
            mDOM.Remove((*NewJson)("variable").GetText());
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
        const Color FontColors[3] = {Color(255, 0, 0), Color(0, 0, 255), Color(224, 224, 0)};
        const Color& CurFontColor = FontColors[mGroupID & 0xF];
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
        const Color FontColors[3] = {Color(255, 0, 0), Color(0, 0, 255), Color(224, 224, 0)};
        const Color& CurFontColor = FontColors[mGroupID & 0xF];

        ZAY_RGBA(panel, 0, 0, 0, 32)
            panel.fill();
        // 텍스트
        ZAY_COLOR(panel, CurFontColor)
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

zayeditorData::zayeditorData() : mEasySaveEffect(updater())
{
    mResourceCB = [](chars name)->const Image* {return &((const Image&) R(name));};
    mEasySaveEffect.Reset(0);

    // 위젯 컴포넌트 추가
    ZayWidget::BuildComponents(mZaySonAPI.ViewName(), mZaySonAPI, &mResourceCB);
    ZayWidget::BuildGlues(mZaySonAPI.ViewName(), mZaySonAPI, &mResourceCB);

    // 분기문 컴포넌트 추가
    mZaySonAPI.AddComponent(ZayExtend::ComponentType::ConditionWithOperation, "if → Conditional", nullptr);
    mZaySonAPI.AddComponent(ZayExtend::ComponentType::ConditionWithEvent, "iffocused → MouseFocus", nullptr);
    mZaySonAPI.AddComponent(ZayExtend::ComponentType::ConditionWithEvent, "ifhovered → MouseHover", nullptr);
    mZaySonAPI.AddComponent(ZayExtend::ComponentType::ConditionWithEvent, "ifpressed → Click(Touch)", nullptr);
    mZaySonAPI.AddComponent(ZayExtend::ComponentType::Condition, "else → ConditionalElse", nullptr);
    mZaySonAPI.AddComponent(ZayExtend::ComponentType::Condition, "endif → ConditionalExit", nullptr);

    // 초기화
    ResetBoxes();
}

zayeditorData::~zayeditorData()
{
}

void zayeditorData::ResetBoxes()
{
    mZaySonAPI.ResetBoxInfo();
    mBoxMap.Reset();

    // 스타터 입력
    ZEZayBoxObject NewZayBox;
    NewZayBox = ZEZayBoxStarter::Create();
    NewZayBox->Init(0, "widget", Color(255, 255, 255, 192), "box_title_first", true, 10, 10);
    mBoxMap[0] = NewZayBox;
    mDraggingHook = -1;
    mShowCommentTagMsec = 0;
    mWorkViewDrag = Point(); // 드래그하던 것이 있다면 중지
}

void zayeditorData::NewProject()
{
    mPipe.ResetJsonPath("");
    ResetBoxes();
    ZEZayBox::ResetLastID(); // ID발급기 초기화
}

void zayeditorData::FastSave()
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
        Context Json;
        mBoxMap[0]->WriteJson(Json);
        mBoxMap[0]->SaveChildren(Json.At("ui"), mBoxMap);
        Json.SaveJson().ToFile(mPipe.jsonpath(), true);
        // 애니효과
        mEasySaveEffect.Reset(1);
        mEasySaveEffect.MoveTo(0, 1.0);
    }
}

void zayeditorData::RenderButton(ZayPanel& panel, chars name, Color color, ZayPanel::SubGestureCB cb)
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

void zayeditorData::RenderComponent(ZayPanel& panel, sint32 i, bool enable, bool blank)
{
    auto& CurComponent = mZaySonAPI.GetComponent(i);
    const String UIName = String::Format("%d-component", i);
    const Rect UIRect = Rect(panel.toview(0, 0), Size(panel.w(), panel.h()));

    ZAY_INNER_UI(panel, 0, (enable)? UIName : "",
        ZAY_GESTURE_VNTXY(v, n, t, x, y, this, i, UIRect)
        {
            if(t == GT_Pressed)
            {
                mZaySonAPI.mDraggingComponentID = i;
                mZaySonAPI.mDraggingComponentRect = UIRect;
            }
            else if(t == GT_InDragging || t == GT_OutDragging)
            {
                auto& OldPos = v->oldxy(n);
                mZaySonAPI.mDraggingComponentRect += Point(x - OldPos.x, y - OldPos.y);
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
                const sint32 NewPos = OldPos + ((t == GT_WheelUp)? 200 : -200);
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
                    panel.text(CurComponent.mName, UIFA_LeftMiddle, UIFE_Right);
            }
        }
    }
}

void zayeditorData::RenderDOM(ZayPanel& panel)
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
                    }, 5, 20)
                {
                    // 페이로드를 구성
                    struct Payload
                    {
                        zayeditorData* self {nullptr};
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
                                        ZAY_RGB(*Data.panel, 255, CurAni, CurAni)
                                            Data.panel->text(FullTextFront, UIFA_LeftMiddle, UIFE_Right);
                                        // 계산식
                                        if(0 < FormulaTextRearSize && FullTextFrontSize < Data.panel->w())
                                        {
                                            ZAY_LTRB(*Data.panel, FullTextFrontSize, 0, Data.panel->w(), Data.panel->h())
                                            ZAY_RGB(*Data.panel, 128, CurAni, CurAni)
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

void zayeditorData::RenderMiniMap(ZayPanel& panel)
{
    // 최대최소 검사
    Rect ScreenRect(0, 0, panel.w(), panel.h());
    Rect SumRect = ScreenRect;
    for(sint32 i = 0, iend = mBoxMap.Count(); i < iend; ++i)
    {
        auto CurBox = mBoxMap.AccessByOrder(i);
        auto CurRect = (*CurBox)->GetRect();
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
                    mWorkViewDrag = Point(mWorkViewSize.w / 2 - X,mWorkViewSize.h / 2 - Y);
                }
            })
        {
            ZAY_RGBA(panel, 0, 0, 0, 128)
                panel.fill();

            // 출력
            const Point BeginPos(SumRect.l, SumRect.t);
            const float Rate = panel.w() / SumRect.Width();
            for(sint32 i = 0, iend = mBoxMap.Count(); i < iend; ++i)
            {
                auto CurBox = mBoxMap.AccessByOrder(i);
                auto CurRect = (*CurBox)->GetRect();
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
            {
                ZAY_RGBA(panel, 255, 255, 255, 64)
                    panel.fill();
                ZAY_RGB(panel, 0, 0, 0)
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
                const Point TargetPos = ZEZayBox::CommentTagService::GetFocusPos();

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

void zayeditorData::RenderLogList(ZayPanel& panel)
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
