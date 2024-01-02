#include <boss.hpp>
#include "zaytoy.hpp"

#include <resource.hpp>
#include <service/boss_zaywidget.hpp>
#include <format/boss_bmp.hpp>
#include <format/boss_flv.hpp>

ZAY_DECLARE_VIEW_CLASS("zaytoyView", zaytoyData)

ZAY_VIEW_API OnCommand(CommandType type, id_share in, id_cloned_share* out)
{
    if(type == CT_Create)
    {
        // DOM로드
        const uint64 CurMsec = Platform::Utility::CurrentTimeMsec();
        m->CheckDOM(CurMsec);
    }
    else if(type == CT_Activate)
    {
        if(ZayWidgetDOM::GetValue("whisper.select").ToText().Length() == 0)
            m->UpdateWhisper();
        m->invalidate();
    }
    else if(type == CT_Size)
    {
        sint32s WH(in);
        m->mWindowSize.w = WH[0];
        #if BOSS_WINDOWS
            m->mWindowSize.h = WH[1] - 1;
        #else
            m->mWindowSize.h = WH[1];
        #endif
        ZayWidgetDOM::SetValue("program.width", String::FromInteger(m->mWindowSize.w));
        ZayWidgetDOM::SetValue("program.height", String::FromInteger(m->mWindowSize.h));
    }
    else if(type == CT_Tick)
    {
        // 예약된 위젯호출 또는 DOM확인
        const uint64 CurMsec = Platform::Utility::CurrentTimeMsec();
        if(m->CheckDOM(CurMsec))
            m->invalidate();

        // Update처리
        if(m->mUpdateMsec != 0)
        {
            if(m->mUpdateMsec < CurMsec)
                m->mUpdateMsec = 0;
            m->invalidate(2);
        }

        // 위젯 틱실행
        if(m->mWidgetMain && m->mWidgetMain->TickOnce())
            m->invalidate();
    }
}

ZAY_VIEW_API OnNotify(NotifyType type, chars topic, id_share in, id_cloned_share* out)
{
    if(type == NT_Normal)
    {
        if(!String::Compare(topic, "Update"))
        {
            const uint64 Msec = (in)? Platform::Utility::CurrentTimeMsec() + sint32o(in).ConstValue() : 1;
            if(m->mUpdateMsec < Msec)
                m->mUpdateMsec = Msec;
            return;
        }

        // 제이모델 제거
        if(!String::Compare(topic, "zaymodel_remove"))
        {
            const sint32 OrderIdx = Parser::GetInt(String(in));
            if(0 <= OrderIdx && OrderIdx < m->mZayModelOrder.Count())
            {
                const String ClassID = m->mZayModelOrder[OrderIdx];
                if(auto CurBox = m->mZayModels(ClassID).Value())
                {
                    // 해당 박스삭제
                    CurBox->RemoveBox();
                    m->mZayModels.Remove(ClassID);
                    m->mZayModelOrder.SubtractionSection(OrderIdx);
                    m->UpdateZayModelOrderDOM();
                    // 해당 박스를 부모로 연결하는 와이어들 제거
                    for(sint32 i = 0, iend = m->mZayModelOrder.Count(); i < iend; ++i)
                    {
                        const String CurClassID = m->mZayModelOrder[i];
                        if(auto CurBox = m->mZayModels(CurClassID).Value())
                        if(!CurBox->ParentID().Compare(ClassID))
                        {
                            CurBox->SetParent(nullptr);
                            m->UpdateZayModelMatrix(*CurBox);
                        }
                    }
                    // 포커싱이 해당 박스인 경우 제거
                    const String FocusedClassID = ZayWidgetDOM::GetValue("zaymodel.focus").ToText();
                    if(!FocusedClassID.Compare(ClassID))
                        ZayWidgetDOM::SetValue("zaymodel.focus", "'x'");
                }
            }
            return;
        }

        // 제이모델 상태변경
        if(!String::Compare(topic, "zaymodel_set_status"))
        {
            const Strings Params(in);
            const sint32 OrderIdx = Parser::GetInt(Params[0]);
            const String Status(Params[1]);
            if(0 <= OrderIdx && OrderIdx < m->mZayModelOrder.Count())
            {
                const String ClassID = m->mZayModelOrder[OrderIdx];
                if(auto CurBox = m->mZayModels(ClassID).Value())
                {
                    CurBox->SetStatus(Status);
                    // 송신
                    Strings ChildCollector;
                    for(sint32 i = 0, iend = CurBox->ChildCount(); i < iend; ++i)
                        ChildCollector.AtAdding() = CurBox->Child(i);
                    m->Send_SetChildStatus(CurBox->ObjectID(), ChildCollector, Status);
                }
            }
            return;
        }

        // 제이모델 기본값 세이브
        if(!String::Compare(topic, "zaymodel_save_default"))
        {
            const sint32 OrderIdx = Parser::GetInt(String(in));
            if(0 <= OrderIdx && OrderIdx < m->mZayModelOrder.Count())
            {
                const String ClassID = m->mZayModelOrder[OrderIdx];
                if(auto CurBox = m->mZayModels(ClassID).Value())
                    CurBox->SaveDefault();
            }
            return;
        }

        // 제이모델 액티비티 포커싱
        if(!String::Compare(topic, "zaymodel_focus_activity"))
        {
            const sint32s Params(in);
            const sint32 OrderIdx = Params[0];
            const sint32 AnimateIdx = Params[1];
            if(0 <= OrderIdx && OrderIdx < m->mZayModelOrder.Count())
            {
                const String ClassID = m->mZayModelOrder[OrderIdx];
                if(auto CurBox = m->mZayModels(ClassID).Value())
                if(CurBox->AnimateFocus() != AnimateIdx)
                {
                    CurBox->DoneActivity();
                    CurBox->SetAnimateFocus(AnimateIdx);
                    m->UpdateZayModelMatrix(*CurBox);
                }
            }
            return;
        }

        // 제이모델 액티비티 마무리
        if(!String::Compare(topic, "zaymodel_done_activity"))
        {
            const String FocusedClassID = ZayWidgetDOM::GetValue("zaymodel.focus").ToText();
            for(sint32 i = 0, iend = m->mZayModelOrder.Count(); i < iend; ++i)
            {
                const String CurClassID = m->mZayModelOrder[i];
                if(!CurClassID.Compare(FocusedClassID))
                if(auto CurBox = m->mZayModels(CurClassID).Value())
                {
                    CurBox->DoneActivity();
                    break;
                }
            }
            return;
        }

        // 제이모델 액티비티 전체클리어
        if(!String::Compare(topic, "zaymodel_clear_activity"))
        {
            const sint32 OrderIdx = Parser::GetInt(String(in));
            if(0 <= OrderIdx && OrderIdx < m->mZayModelOrder.Count())
            {
                const String ClassID = m->mZayModelOrder[OrderIdx];
                if(auto CurBox = m->mZayModels(ClassID).Value())
                {
                    CurBox->ClearActivity();
                    m->UpdateZayModelMatrix(*CurBox);
                }
            }
            return;
        }

        // 제이모델 액티비티 개별삭제
        if(!String::Compare(topic, "zaymodel_remove_activity"))
        {
            const Strings Params(in);
            const sint32 OrderIdx = Parser::GetInt(Params[0]);
            const sint32 ActivityIdx = Parser::GetInt(Params[1]);
            if(0 <= OrderIdx && OrderIdx < m->mZayModelOrder.Count())
            {
                const String ClassID = m->mZayModelOrder[OrderIdx];
                if(auto CurBox = m->mZayModels(ClassID).Value())
                {
                    CurBox->RemoveActivity(ActivityIdx);
                    m->UpdateZayModelMatrix(*CurBox);
                }
            }
            return;
        }

        // 제이모델 액티비티 행렬추가
        if(!String::Compare(topic, "zaymodel_add_matrix_activity"))
        {
            const sint32 OrderIdx = Parser::GetInt(String(in));
            if(0 <= OrderIdx && OrderIdx < m->mZayModelOrder.Count())
            {
                const String ClassID = m->mZayModelOrder[OrderIdx];
                if(auto CurBox = m->mZayModels(ClassID).Value())
                    CurBox->AddActivity("matrix", nullptr, 0);
            }
            return;
        }

        // 제이모델 액티비티 속도추가
        if(!String::Compare(topic, "zaymodel_add_speed_activity"))
        {
            const sint32 OrderIdx = Parser::GetInt(String(in));
            if(0 <= OrderIdx && OrderIdx < m->mZayModelOrder.Count())
            {
                const String ClassID = m->mZayModelOrder[OrderIdx];
                if(auto CurBox = m->mZayModels(ClassID).Value())
                    CurBox->AddActivity("speed", nullptr, 0);
            }
            return;
        }

        // 제이모델 애니메이션 추가
        if(!String::Compare(topic, "zaymodel_create_animate"))
        {
            const sint32 OrderIdx = Parser::GetInt(String(in));
            if(0 <= OrderIdx && OrderIdx < m->mZayModelOrder.Count())
            {
                const String ClassID = m->mZayModelOrder[OrderIdx];
                if(auto CurBox = m->mZayModels(ClassID).Value())
                    CurBox->CreateAnimate();
            }
            return;
        }

        // 제이모델 애니메이션 삭제
        if(!String::Compare(topic, "zaymodel_remove_animate"))
        {
            const Strings Params(in);
            const sint32 OrderIdx = Parser::GetInt(Params[0]);
            const sint32 AnimateIdx = Parser::GetInt(Params[1]);
            if(0 <= OrderIdx && OrderIdx < m->mZayModelOrder.Count())
            {
                const String ClassID = m->mZayModelOrder[OrderIdx];
                if(auto CurBox = m->mZayModels(ClassID).Value())
                    CurBox->RemoveAnimate(AnimateIdx);
            }
            return;
        }

        // 제이모델 애니메이션 실행
        if(!String::Compare(topic, "zaymodel_play_animate"))
        {
            const Strings Params(in);
            const sint32 OrderIdx = Parser::GetInt(Params[0]);
            const sint32 AnimateIdx = Parser::GetInt(Params[1]);
            if(0 <= OrderIdx && OrderIdx < m->mZayModelOrder.Count())
            {
                const String ClassID = m->mZayModelOrder[OrderIdx];
                if(auto CurBox = m->mZayModels(ClassID).Value())
                    CurBox->PlayAnimate(AnimateIdx);
            }
            return;
        }

        if(!String::Compare(topic, "zaymodel_set_left_wire"))
        {
            const Strings Params(in);
            const String SrcClassID(Params[0]);
            const sint32 DstOrderIdx = Parser::GetInt(Params[1]);
            if(0 <= DstOrderIdx && DstOrderIdx < m->mZayModelOrder.Count())
            {
                const String DstClassID = m->mZayModelOrder[DstOrderIdx];
                if(auto SrcBox = m->mZayModels(SrcClassID).Value())
                {
                    SrcBox->SetParent(DstClassID);
                    m->UpdateZayModelMatrix(*SrcBox);
                }
            }
            return;
        }

        if(!String::Compare(topic, "zaymodel_add_right_wire"))
        {
            const Strings Params(in);
            const String SrcClassID(Params[0]);
            const sint32 DstOrderIdx = Parser::GetInt(Params[1]);
            if(0 <= DstOrderIdx && DstOrderIdx < m->mZayModelOrder.Count())
            {
                const String DstClassID = m->mZayModelOrder[DstOrderIdx];
                if(auto SrcBox = m->mZayModels(DstClassID).Value())
                {
                    SrcBox->SetParent(SrcClassID);
                    m->UpdateZayModelMatrix(*SrcBox);
                }
            }
            return;
        }

        if(!String::Compare(topic, "zaymodel_cut_left_wire"))
        {
            const sint32 OrderIdx = Parser::GetInt(String(in));
            if(0 <= OrderIdx && OrderIdx < m->mZayModelOrder.Count())
            {
                const String ClassID = m->mZayModelOrder[OrderIdx];
                if(auto CurBox = m->mZayModels(ClassID).Value())
                {
                    CurBox->SetParent(nullptr);
                    m->UpdateZayModelMatrix(*CurBox);
                }
            }
            return;
        }
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
    static point64 OldCursorPos;
    static rect128 OldWindowRect;
    static uint64 ReleaseMsec = 0;

    if(type == GT_Moving || type == GT_MovingIdle)
        m->SetCursor(CR_Arrow);
    else if(type == GT_Pressed)
    {
        Platform::Utility::GetCursorPos(OldCursorPos);
        OldWindowRect = Platform::GetWindowRect();
        m->clearCapture();
    }
    else if(type == GT_InDragging || type == GT_OutDragging)
    {
        if(!m->IsFullScreen())
        {
            point64 CurCursorPos;
            Platform::Utility::GetCursorPos(CurCursorPos);
            rect128 CurWindowRect;
            CurWindowRect.l = OldWindowRect.l + CurCursorPos.x - OldCursorPos.x;
            CurWindowRect.t = OldWindowRect.t + CurCursorPos.y - OldCursorPos.y;
            CurWindowRect.r = OldWindowRect.r + CurCursorPos.x - OldCursorPos.x;
            CurWindowRect.b = OldWindowRect.b + CurCursorPos.y - OldCursorPos.y;
            Platform::SetWindowRect(CurWindowRect.l, CurWindowRect.t,
                CurWindowRect.r - CurWindowRect.l, CurWindowRect.b - CurWindowRect.t);
            if(ZayWidgetDOM::GetValue("whisper.select").ToText().Length() == 0)
                m->UpdateWhisper(CurWindowRect);
        }
        ReleaseMsec = 0;
        m->invalidate();
    }
    else if(type == GT_InReleased || type == GT_OutReleased || type == GT_CancelReleased)
    {
        const uint64 CurReleaseMsec = Platform::Utility::CurrentTimeMsec();
        const bool HasDoubleClick = (CurReleaseMsec < ReleaseMsec + 300);
        if(HasDoubleClick)
        {
            if(m->IsFullScreen())
                m->SetNormalWindow();
            else m->SetFullScreen();
        }
        else ReleaseMsec = CurReleaseMsec;
    }
}

ZAY_VIEW_API OnRender(ZayPanel& panel)
{
    ZAY_XYWH(panel, 0, 0, m->mWindowSize.w, m->mWindowSize.h)
    {
        ZAY_RGBA(panel, 0, 0, 0, 192)
            panel.fill();

        if(m->mWidgetMain)
            m->mWidgetMain->Render(panel);

        // 윈도우시스템
        #if !BOSS_ANDROID && !BOSS_IPHONE
            m->RenderWindowSystem(panel);
        #endif
    }
}

zaytoyData::zaytoyData()
{
    String DateText = __DATE__;
    DateText.Replace("Jan", "01"); DateText.Replace("Feb", "02"); DateText.Replace("Mar", "03");
    DateText.Replace("Apr", "04"); DateText.Replace("May", "05"); DateText.Replace("Jun", "06");
    DateText.Replace("Jul", "07"); DateText.Replace("Aug", "08"); DateText.Replace("Sep", "09");
    DateText.Replace("Oct", "10"); DateText.Replace("Nov", "11"); DateText.Replace("Dec", "12");
    const String Day = String::Format("%02d", Parser::GetInt(DateText.Middle(2, DateText.Length() - 6).Trim()));
    DateText = DateText.Right(4) + "/" + DateText.Left(2) + "/" + Day;
    ZayWidgetDOM::SetValue("program.update", "'" + DateText + "'");
    ZayWidgetDOM::SetValue("program.width", "0");
    ZayWidgetDOM::SetValue("program.height", "0");
    ZayWidgetDOM::SetValue("whisper.count", "0");
    ZayWidgetDOM::SetValue("whisper.select", "''");
    ZayWidgetDOM::SetValue("whispertoy.count", "0");
    ZayWidgetDOM::SetValue("whispertoy.opened", "''");

    mWidgetMain = new ZayWidget();
    InitWidget(*mWidgetMain, "Main");
    mWidgetMain->Reload("widget/main.zwidget");

    mWhisper.AddRecvCB("ToyList",
        [this](const Context& data)->void
        {
            ZayWidgetDOM::SetJson(data("toy"), "whispertoy.");
        });
}

zaytoyData::~zaytoyData()
{
    delete mWidgetMain;
}

void zaytoyData::RenderWindowSystem(ZayPanel& panel)
{
    // 최소화버튼
    ZAY_XYWH_UI(panel, panel.w() - 8 - (24 + 18) * 3, 26, 24, 24, "ui_win_min",
        ZAY_GESTURE_T(t)
        {
            if(t == GT_InReleased)
                Platform::Utility::SetMinimize();
        })
    ZAY_ZOOM(panel, 0.8)
    {
        const bool Hovered = ((panel.state("ui_win_min") & (PS_Focused | PS_Dropping)) == PS_Focused);
        panel.icon(R((Hovered)? "btn_minimize_h" : "btn_minimize_n"), UIA_CenterMiddle);
    }

    // 최대화버튼
    ZAY_XYWH_UI(panel, panel.w() - 8 - (24 + 18) * 2, 26, 24, 24, "ui_win_max",
        ZAY_GESTURE_T(t)
        {
            if(t == GT_InReleased)
            {
                if(m->IsFullScreen())
                    m->SetNormalWindow();
                else m->SetFullScreen();
            }
        })
    ZAY_ZOOM(panel, 0.8)
    {
        const bool Hovered = ((panel.state("ui_win_max") & (PS_Focused | PS_Dropping)) == PS_Focused);
        panel.icon(R((Hovered)? "btn_downsizing_h" : "btn_downsizing_n"), UIA_CenterMiddle);
    }

    // 종료버튼
    ZAY_XYWH_UI(panel, panel.w() - 8 - (24 + 18) * 1, 26, 24, 24, "ui_win_close",
        ZAY_GESTURE_T(t)
        {
            if(t == GT_InReleased)
                Platform::Utility::ExitProgram();
        })
    ZAY_ZOOM(panel, 0.8)
    {
        const bool Hovered = ((panel.state("ui_win_close") & (PS_Focused | PS_Dropping)) == PS_Focused);
        panel.icon(R((Hovered)? "btn_close_h" : "btn_close_n"), UIA_CenterMiddle);
    }

    // 아웃라인
    if(!m->IsFullScreen())
    ZAY_INNER(panel, 1)
    ZAY_RGB(panel, 0, 0, 0)
        panel.rect(1);
}

bool zaytoyData::IsFullScreen()
{
    return mIsFullScreen;
}

void zaytoyData::SetFullScreen()
{
    if(!mIsFullScreen)
    {
        mIsFullScreen = true;
        mSavedNormalRect = Platform::GetWindowRect();

        point64 CursorPos;
        Platform::Utility::GetCursorPos(CursorPos);
        sint32 ScreenID = Platform::Utility::GetScreenID(CursorPos);
        rect128 FullScreenRect;
        Platform::Utility::GetScreenRect(FullScreenRect, ScreenID, false);
        Platform::SetWindowRect(FullScreenRect.l, FullScreenRect.t,
            FullScreenRect.r - FullScreenRect.l, FullScreenRect.b - FullScreenRect.t + 1);
    }
}

void zaytoyData::SetNormalWindow()
{
    if(mIsFullScreen)
    {
        mIsFullScreen = false;
        Platform::SetWindowRect(mSavedNormalRect.l, mSavedNormalRect.t,
            mSavedNormalRect.r - mSavedNormalRect.l, mSavedNormalRect.b - mSavedNormalRect.t);
    }
}

void zaytoyData::SetCursor(CursorRole role)
{
    if(mNowCursor != role)
    {
        mNowCursor = role;
        Platform::Utility::SetCursor(role);
    }
}

bool zaytoyData::CheckDOM(uint64 msec)
{
    static uint64 LastUpdateCheckMsec = 0;
    if(LastUpdateCheckMsec + 100 < msec)
    {
        LastUpdateCheckMsec = msec;
        uint64 DataModifyMsec = 0;
        if(Asset::Exist("widget/data.json", nullptr, nullptr, nullptr, nullptr, &DataModifyMsec))
        {
            if(mLastModifyMsec != DataModifyMsec)
            {
                mLastModifyMsec = DataModifyMsec;
                ReloadDOM();
                return true;
            }
        }
    }
    return false;
}

void zaytoyData::ReloadDOM()
{
    const String DataString = String::FromAsset("widget/data.json");
    Context Data(ST_Json, SO_OnlyReference, DataString, DataString.Length());
    ZayWidgetDOM::RemoveVariables("data.");
    ZayWidgetDOM::SetJson(Data, "data.");

    // 폰트확보
    const sint32 FontCount = ZayWidgetDOM::GetValue("data.fonts.count").ToInteger();
    for(sint32 i = 0; i < FontCount; ++i)
    {
        const String CurHeader = String::Format("data.fonts.%d.", i);
        const String CurName = ZayWidgetDOM::GetValue(CurHeader + "name").ToText();
        const String CurPath = ZayWidgetDOM::GetValue(CurHeader + "path").ToText();
        if(CurName.Length() == 0 && 0 < CurPath.Length())
        {
            buffer NewFontData = Asset::ToBuffer(CurPath);
            auto NewFontName = Platform::Utility::CreateSystemFont((bytes) NewFontData, Buffer::CountOf(NewFontData));
            Buffer::Free(NewFontData);
            ZayWidgetDOM::SetValue(CurHeader + "name", "'" + NewFontName + "'");
        }
    }
}

void zaytoyData::InitWidget(ZayWidget& widget, chars name)
{
    widget.Init(name, nullptr,
        [](chars name)->const Image* {return &((const Image&) R(name));})

        // 특정시간동안 지속적인 화면업데이트(60fps)
        .AddGlue("update", ZAY_DECLARE_GLUE(params, this)
        {
            if(params.ParamCount() == 1)
            {
                const uint64 UpdateMsec = Platform::Utility::CurrentTimeMsec() + params.Param(0).ToFloat() * 1000;
                if(mUpdateMsec < UpdateMsec)
                    mUpdateMsec = UpdateMsec;
            }
        })

        // 변수값을 확인하기 위한 개발용 팝업
        .AddGlue("popup", ZAY_DECLARE_GLUE(params, this)
        {
            if(params.ParamCount() == 1)
            {
                const String Message = params.Param(0).ToText();
                Platform::Popup::MessageDialog("DevMessage", Message);
            }
        })

        // 이벤트
        .AddGlue("event", ZAY_DECLARE_GLUE(params, this)
        {
            if(params.ParamCount() == 1)
            {
                const String Topic = params.Param(0).ToText();
                Platform::BroadcastNotify(Topic, nullptr);
            }
        })

        // 초기화
        .AddGlue("reload", ZAY_DECLARE_GLUE(params, this)
        {
            ReloadDOM();
            invalidate();
        })

        // 위스퍼 선택
        .AddGlue("whisper_select", ZAY_DECLARE_GLUE(params, this)
        {
            if(params.ParamCount() == 1)
            {
                const String Spot = params.Param(0).ToText();
                if(mWhisper.OpenForConnector(Spot))
                {
                    mWhisper.Send("EnumToy");
                    ZayWidgetDOM::SetValue("whisper.select", "'" + Spot + "'");
                }
                else
                {
                    ZayWidgetDOM::SetValue("whisper.select", "''");
                    ZayWidgetDOM::RemoveVariables("whispertoy.");
                    ZayWidgetDOM::SetValue("whispertoy.count", "0");
                    ZayWidgetDOM::SetValue("whispertoy.opened", "''");
                    UpdateWhisper();
                }
                invalidate();
            }
        })

        // 위스퍼 선택해제
        .AddGlue("whisper_deselect", ZAY_DECLARE_GLUE(params, this)
        {
            mWhisper.Close();
            ZayWidgetDOM::SetValue("whisper.select", "''");
            ZayWidgetDOM::RemoveVariables("whispertoy.");
            ZayWidgetDOM::SetValue("whispertoy.count", "0");
            ZayWidgetDOM::SetValue("whispertoy.opened", "''");
            UpdateWhisper();
            invalidate();
        })

        // 위스퍼토이 열기
        .AddGlue("whispertoy_open", ZAY_DECLARE_GLUE(params, this)
        {
            if(params.ParamCount() == 1)
            {
                const String Toy = params.Param(0).ToText();
                mWhisper.Send("LoadToy");
                ZayWidgetDOM::SetValue("whispertoy.opened", "'" + Toy + "'");
            }
            invalidate();
        })

        // 위스퍼토이 닫기
        .AddGlue("whispertoy_close", ZAY_DECLARE_GLUE(params, this)
        {
            ZayWidgetDOM::SetValue("whispertoy.opened", "''");
            invalidate();
        })

        // 제이모델 제거
        .AddGlue("zaymodel_remove", ZAY_DECLARE_GLUE(params, this)
        {
            if(params.ParamCount() == 1)
            {
                const String OrderIdx = params.Param(0).ToText();
                Platform::BroadcastNotify("zaymodel_remove", OrderIdx);
            }
        })

        // 제이모델 상태변경
        .AddGlue("zaymodel_set_status", ZAY_DECLARE_GLUE(params, this)
        {
            if(params.ParamCount() == 2)
            {
                Strings Params;
                Params.AtAdding() = params.Param(0).ToText(); // OrderIdx
                Params.AtAdding() = params.Param(1).ToText(); // Status
                Platform::BroadcastNotify("zaymodel_set_status", Params);
            }
        })

        // 제이모델 기본값 세이브
        .AddGlue("zaymodel_save_default", ZAY_DECLARE_GLUE(params, this)
        {
            if(params.ParamCount() == 1)
            {
                const String OrderIdx = params.Param(0).ToText();
                Platform::BroadcastNotify("zaymodel_save_default", OrderIdx);
            }
        })

        // 제이모델 액티비티 포커싱
        .AddGlue("zaymodel_focus_activity", ZAY_DECLARE_GLUE(params, this)
        {
            if(params.ParamCount() == 2)
            {
                sint32s Params;
                Params.AtAdding() = params.Param(0).ToInteger(); // OrderIdx
                Params.AtAdding() = params.Param(1).ToInteger(); // AnimateIdx
                Platform::BroadcastNotify("zaymodel_focus_activity", Params);
            }
        })

        // 제이모델 액티비티 마무리
        .AddGlue("zaymodel_done_activity", ZAY_DECLARE_GLUE(params, this)
        {
            Platform::BroadcastNotify("zaymodel_done_activity", nullptr);
        })

        // 제이모델 액티비티 전체클리어
        .AddGlue("zaymodel_clear_activity", ZAY_DECLARE_GLUE(params, this)
        {
            if(params.ParamCount() == 1)
            {
                const String OrderIdx = params.Param(0).ToText();
                Platform::BroadcastNotify("zaymodel_clear_activity", OrderIdx);
            }
        })

        // 제이모델 액티비티 개별삭제
        .AddGlue("zaymodel_remove_activity", ZAY_DECLARE_GLUE(params, this)
        {
            if(params.ParamCount() == 2)
            {
                Strings Params;
                Params.AtAdding() = params.Param(0).ToText(); // OrderIdx
                Params.AtAdding() = params.Param(1).ToText(); // ActivityIdx
                Platform::BroadcastNotify("zaymodel_remove_activity", Params);
            }
        })

        // 제이모델 액티비티 행렬추가
        .AddGlue("zaymodel_add_matrix_activity", ZAY_DECLARE_GLUE(params, this)
        {
            if(params.ParamCount() == 1)
            {
                const String OrderIdx = params.Param(0).ToText();
                Platform::BroadcastNotify("zaymodel_add_matrix_activity", OrderIdx);
            }
        })

        // 제이모델 액티비티 속도추가
        .AddGlue("zaymodel_add_speed_activity", ZAY_DECLARE_GLUE(params, this)
        {
            if(params.ParamCount() == 1)
            {
                const String OrderIdx = params.Param(0).ToText();
                Platform::BroadcastNotify("zaymodel_add_speed_activity", OrderIdx);
            }
        })

        // 제이모델 애니메이션 추가
        .AddGlue("zaymodel_create_animate", ZAY_DECLARE_GLUE(params, this)
        {
            if(params.ParamCount() == 1)
            {
                const String OrderIdx = params.Param(0).ToText();
                Platform::BroadcastNotify("zaymodel_create_animate", OrderIdx);
            }
        })

        // 제이모델 애니메이션 삭제
        .AddGlue("zaymodel_remove_animate", ZAY_DECLARE_GLUE(params, this)
        {
            if(params.ParamCount() == 2)
            {
                Strings Params;
                Params.AtAdding() = params.Param(0).ToText(); // OrderIdx
                Params.AtAdding() = params.Param(1).ToText(); // AnimateIdx
                Platform::BroadcastNotify("zaymodel_remove_animate", Params);
            }
        })

        // 제이모델 애니메이션 실행
        .AddGlue("zaymodel_play_animate", ZAY_DECLARE_GLUE(params, this)
        {
            if(params.ParamCount() == 2)
            {
                Strings Params;
                Params.AtAdding() = params.Param(0).ToText(); // OrderIdx
                Params.AtAdding() = params.Param(1).ToText(); // AnimateIdx
                Platform::BroadcastNotify("zaymodel_play_animate", Params);
            }
        })

        // 제이모델 좌측-박스줄 연결
        .AddGlue("zaymodel_set_left_wire", ZAY_DECLARE_GLUE(params, this)
        {
            if(params.ParamCount() == 2)
            {
                Strings Params;
                Params.AtAdding() = params.Param(0).ToText(); // SrcClassID
                Params.AtAdding() = params.Param(1).ToText(); // DstOrderIdx
                Platform::BroadcastNotify("zaymodel_set_left_wire", Params);
            }
        })

        // 제이모델 우측-박스줄 연결
        .AddGlue("zaymodel_add_right_wire", ZAY_DECLARE_GLUE(params, this)
        {
            if(params.ParamCount() == 2)
            {
                Strings Params;
                Params.AtAdding() = params.Param(0).ToText(); // SrcClassID
                Params.AtAdding() = params.Param(1).ToText(); // DstOrderIdx
                Platform::BroadcastNotify("zaymodel_add_right_wire", Params);
            }
        })

        // 제이모델 좌측-박스줄 제거
        .AddGlue("zaymodel_cut_left_wire", ZAY_DECLARE_GLUE(params, this)
        {
            if(params.ParamCount() == 1)
            {
                const String OrderIdx = params.Param(0).ToText();
                Platform::BroadcastNotify("zaymodel_cut_left_wire", OrderIdx);
            }
        })

        // user_content
        .AddComponent(ZayExtend::ComponentType::ContentWithParameter, "user_content", ZAY_DECLARE_COMPONENT(panel, params, this)
        {
            if(params.ParamCount() < 1)
                return panel._push_pass();
            const String Type = params.Param(0).ToText();
            bool HasRender = false;

            branch;
            jump(!String::Compare(Type, "zaymodel_trigger_button") && params.ParamCount() == 4)
            {
                const String Title = params.Param(1).ToText();
                const String Model = params.Param(2).ToText();
                const String Atlas = params.Param(3).ToText();
                HasRender = RenderUC_ZayModelIconButton(panel,
                    "trigger", Title, Model, Atlas, params.ElementID(), params.UIName());
            }
            jump(!String::Compare(Type, "zaymodel_joint_button") && params.ParamCount() == 4)
            {
                const String Title = params.Param(1).ToText();
                const String Model = params.Param(2).ToText();
                const String Atlas = params.Param(3).ToText();
                HasRender = RenderUC_ZayModelIconButton(panel,
                    "joint", Title, Model, Atlas, params.ElementID(), params.UIName());
            }
            jump(!String::Compare(Type, "zaymodel_bg") && params.ParamCount() == 2)
            {
                const String ClassID = params.Param(1).ToText();
                HasRender = RenderUC_ZayModelBg(panel, ClassID);
            }
            jump(!String::Compare(Type, "zaymodel_wire") && params.ParamCount() == 6)
            {
                const sint32 X1 = params.Param(1).ToInteger();
                const sint32 Y1 = params.Param(2).ToInteger();
                const sint32 X2 = params.Param(3).ToInteger();
                const sint32 Y2 = params.Param(4).ToInteger();
                const float Thick = params.Param(5).ToFloat();
                HasRender = RenderUC_ZayModelWire(panel, X1, Y1, X2, Y2, Thick);
            }
            jump(!String::Compare(Type, "zaymodel_spin") && params.ParamCount() == 4)
            {
                const String ClassID = params.Param(1).ToText();
                const String Group = params.Param(2).ToText();
                const String Key = params.Param(3).ToText();
                HasRender = RenderUC_ZayModelSpin(panel, ClassID, Group, Key);
            }

            // 그외 처리
            if(!HasRender)
            ZAY_INNER_SCISSOR(panel, 0)
            {
                ZAY_RGBA(panel, 255, 0, 0, 128)
                    panel.fill();
                for(sint32 i = 0; i < 5; ++i)
                {
                    ZAY_INNER(panel, 1 + i)
                    ZAY_RGBA(panel, 255, 0, 0, 128 - 16 * i)
                        panel.rect(1);
                }
                ZAY_FONT(panel, 1.2 * Math::MinF(Math::MinF(panel.w(), panel.h()) / 40, 1))
                ZAY_RGB(panel, 255, 0, 0)
                    panel.text(Type, UIFA_CenterMiddle, UIFE_Right);
            }
            return panel._push_pass();
        });
}

void zaytoyData::UpdateWhisper(rect128 window)
{
    const Strings Spots = mWhisper.EnumSpots(window);
    sint32 ConnectedCount = 0;
    for(sint32 i = 0, iend = Spots.Count(); i < iend; ++i)
    {
        if(Whisper::ConnectTest(Spots[i]))
        {
            ZayWidgetDOM::SetValue(String::Format("whisper.%d", ConnectedCount), "'" + Spots[i] + "'");
            ConnectedCount++;
        }
    }
    const sint32 OldCount = ZayWidgetDOM::GetValue("whisper.count").ToInteger();
    for(sint32 i = ConnectedCount; i < OldCount; ++i)
        ZayWidgetDOM::RemoveVariables(String::Format("whisper.%d", i));
    ZayWidgetDOM::SetValue("whisper.count", String::FromInteger(ConnectedCount));
}

void zaytoyData::ValidZayModelRoot()
{
    if(mZayModelOrder.Count() == 0)
    if(ZayModel* NewBox = ZayModel::CreateBox("root", ZayModel::CreateID()))
    {
        Context NewJson;
        NewJson.At("text").Set("DECK");
        NewJson.At("x").Set("100");
        NewJson.At("y").Set("100");
        NewBox->Import(NewJson);
        NewBox->UpdateDOM();
        mZayModels(NewBox->ID()).CreateValue(NewBox);
        mZayModelOrder.AtAdding() = NewBox->ID();
    }
}

void zaytoyData::UpdateZayModelOrderDOM()
{
    ZayWidgetDOM::RemoveVariables("zaymodel.order.");
    for(sint32 i = 0, iend = mZayModelOrder.Count(); i < iend; ++i)
        ZayWidgetDOM::SetValue(String::Format("zaymodel.order.%d", i), "'" + mZayModelOrder[i] + "'");
    ZayWidgetDOM::SetValue("zaymodel.order.count", String::FromInteger(mZayModelOrder.Count()));
}

void zaytoyData::UpdateZayModelMatrix(ZayModel& box)
{
    // 행렬 재계산 및 송신
    auto Parent = mZayModels(box.ParentID()).Value();
    auto& CalcedMatrix = box.CalcMatrix((Parent)? Parent->Matrix(false) : ZMMatrix());
    Strings ChildCollector;
    for(sint32 i = 0, iend = box.ChildCount(); i < iend; ++i)
        ChildCollector.AtAdding() = box.Child(i);
    Send_SetChildMatrix(box.ObjectID(), ChildCollector, CalcedMatrix);
    // 서포터 동시처리
    if(auto CurMatrix = box.SupportMatrix())
    {
        Strings SupportCollector;
        for(sint32 i = 0, iend = ChildCollector.Count(); i < iend; ++i)
            SupportCollector.AtAdding() = ChildCollector[i] + "s";
        Send_SetChildMatrix(box.ObjectID(), SupportCollector, *CurMatrix);
    }

    // 해당 박스가 부모인 자식들로 재귀
    const String BoxID = box.ID();
    for(sint32 i = 0, iend = mZayModelOrder.Count(); i < iend; ++i)
    {
        const String CurClassID = mZayModelOrder[i];
        if(auto CurBox = mZayModels(CurClassID).Value())
        if(!CurBox->ParentID().Compare(BoxID))
            UpdateZayModelMatrix(*CurBox);
    }
}

void zaytoyData::AnimateZayModelMatrix(ZayModel& box)
{
    // 행렬 재계산 및 송신
    auto Parent = mZayModels(box.ParentID()).Value();
    auto& CalcedMatrix = box.CalcMatrix((Parent)? Parent->Matrix(false) : ZMMatrix());
    Strings ChildCollector;
    for(sint32 i = 0, iend = box.ChildCount(); i < iend; ++i)
        ChildCollector.AtAdding() = box.Child(i);
    Send_SetChildMatrix(box.ObjectID(), ChildCollector, CalcedMatrix);
    // 서포터 동시처리
    if(auto CurMatrix = box.SupportMatrix())
    {
        Strings SupportCollector;
        for(sint32 i = 0, iend = ChildCollector.Count(); i < iend; ++i)
            SupportCollector.AtAdding() = ChildCollector[i] + "s";
        Send_SetChildMatrix(box.ObjectID(), SupportCollector, *CurMatrix);
    }

    // 해당 박스가 부모인 애니메이션중이 아닌 자식들로 재귀
    const String BoxID = box.ID();
    for(sint32 i = 0, iend = mZayModelOrder.Count(); i < iend; ++i)
    {
        const String CurClassID = mZayModelOrder[i];
        if(auto CurBox = mZayModels(CurClassID).Value())
        if(!CurBox->ParentID().Compare(BoxID))
        if(!CurBox->IsAnimating())
            AnimateZayModelMatrix(*CurBox);
    }
}

MessageCache& zaytoyData::NewMessageCache(String& msgid, chars type, chars mission)
{
    static sint32 LastMsgID = 10000 + (Platform::Utility::Random() % 10000);
    if(mission) msgid = String::Format("%d/%s", ++LastMsgID, mission);
    else msgid = String::FromInteger(++LastMsgID);
    MessageCache& NewMessage = mZayModelMsgCaches(msgid);
    NewMessage.mType = type;
    return NewMessage;
}

sint32 zaytoyData::MessageCacheCount() const
{
    return mZayModelMsgCaches.Count();
}

String zaytoyData::MessageCacheInfo(sint32 i, String& type, String& error) const
{
    chararray MsgID;
    if(auto OneMessage = mZayModelMsgCaches.AccessByOrder(i, &MsgID))
    {
        type = OneMessage->mType;
        error = OneMessage->mErrorCode;
        return String(&MsgID[0]);
    }
    return String();
}

void zaytoyData::MessageCacheFlush(MessageCache& msg, const Context& json) const
{
    msg.mJson = json.SaveJson();
    SendJson(msg.mJson);
}

void zaytoyData::SendJson(const String& json) const
{
    //gSendSocket("VAS", mSocket, (bytes)(chars) json, json.Length() + 1);
}

void zaytoyData::Send_CreateObject(chars objectid, chars filepath)
{
    String MsgID;
    auto& NewMessage = NewMessageCache(MsgID, "CreateObject");
    NewMessage.mSaveValues("ObjectID") = objectid;
    NewMessage.mSaveValues("FilePath") = filepath;

    Context NewPacket;
    NewPacket.At("type").Set(NewMessage.mType);
    NewPacket.At("msg").Set(MsgID);
    NewPacket.At("collect").Set(String::Format("object_%s", objectid));
    NewPacket.At("id").Set(objectid);
    NewPacket.At("file").Set(filepath);
    NewPacket.At("thema").Set("");
    NewPacket.At("spawnx").Set("0");
    NewPacket.At("spawny").Set("0");
    MessageCacheFlush(NewMessage, NewPacket);
}

void zaytoyData::Send_SetChildStatus(chars objectid, const Strings& children, chars status)
{
    String MsgID;
    auto& NewMessage = NewMessageCache(MsgID, "SetChildStatus");

    Context NewPacket;
    NewPacket.At("type").Set(NewMessage.mType);
    NewPacket.At("msg").Set(MsgID);
    NewPacket.At("objectid").Set(objectid);
    hook(NewPacket.At("child"))
    for(sint32 i = 0, iend = children.Count(); i < iend; ++i)
        fish.AtAdding().Set(children[i]);
    NewPacket.At("status").Set(status);
    SendJson(NewPacket.SaveJson());
}

void zaytoyData::Send_SetChildMatrix(chars objectid, const Strings& children, const ZMMatrix& matrix)
{
    String MsgID;
    auto& NewMessage = NewMessageCache(MsgID, "SetChildMatrix");

    Context NewPacket;
    NewPacket.At("type").Set(NewMessage.mType);
    NewPacket.At("msg").Set(MsgID);
    NewPacket.At("objectid").Set(objectid);
    hook(NewPacket.At("child"))
    for(sint32 i = 0, iend = children.Count(); i < iend; ++i)
        fish.AtAdding().Set(children[i]);
    hook(NewPacket.At("matrix"))
    {
        fish.AtAdding().Set(String::FromFloat(matrix.m00));
        fish.AtAdding().Set(String::FromFloat(matrix.m01));
        fish.AtAdding().Set(String::FromFloat(matrix.m02));
        fish.AtAdding().Set(String::FromFloat(matrix.m03));
        fish.AtAdding().Set(String::FromFloat(matrix.m10));
        fish.AtAdding().Set(String::FromFloat(matrix.m11));
        fish.AtAdding().Set(String::FromFloat(matrix.m12));
        fish.AtAdding().Set(String::FromFloat(matrix.m13));
        fish.AtAdding().Set(String::FromFloat(matrix.m20));
        fish.AtAdding().Set(String::FromFloat(matrix.m21));
        fish.AtAdding().Set(String::FromFloat(matrix.m22));
        fish.AtAdding().Set(String::FromFloat(matrix.m23));
        fish.AtAdding().Set(String::FromFloat(matrix.m30));
        fish.AtAdding().Set(String::FromFloat(matrix.m31));
        fish.AtAdding().Set(String::FromFloat(matrix.m32));
        fish.AtAdding().Set(String::FromFloat(matrix.m33));
    }
    SendJson(NewPacket.SaveJson());
}

void zaytoyData::Send_CreateToolChild(chars child, chars model)
{
    String MsgID;
    auto& NewMessage = NewMessageCache(MsgID, "CreateToolChild");

    Context NewPacket;
    NewPacket.At("type").Set(NewMessage.mType);
    NewPacket.At("msg").Set(MsgID);
    NewPacket.At("objectid").Set("tool");
    NewPacket.At("child").Set(child);
    NewPacket.At("model").Set(model);
    SendJson(NewPacket.SaveJson());
}

void zaytoyData::Send_RemoveToolChild(chars child)
{
    String MsgID;
    auto& NewMessage = NewMessageCache(MsgID, "RemoveToolChild");

    Context NewPacket;
    NewPacket.At("type").Set(NewMessage.mType);
    NewPacket.At("msg").Set(MsgID);
    NewPacket.At("objectid").Set("tool");
    NewPacket.At("child").Set(child);
    SendJson(NewPacket.SaveJson());
}

bool zaytoyData::RenderUC_ZayModelIconButton(ZayPanel& panel, chars type, chars title, chars model, chars atlas, sint32 elementid, String uiname)
{
    static point64 GrabPosAdd {0, 0};
    const String UIName = String::Format("ui_zaymodel_icon_%s", title);
    const String Type = type;
    const String Title = title;
    const String Model = model;
    ZAY_INNER_UI(panel, 0, UIName,
        ZAY_GESTURE_VNTXY(v, n, t, x, y, this, Type, Title, Model, elementid, uiname)
        {
            static point64 GrabPosBegin;
            if(t == GT_Pressed)
            {
                GrabPosBegin.x = x;
                GrabPosBegin.y = y;
                GrabPosAdd = {0, 0};
            }
            else if(t == GT_InDragging || t == GT_OutDragging)
            {
                GrabPosAdd.x = x - GrabPosBegin.x;
                GrabPosAdd.y = y - GrabPosBegin.y;
                v->invalidate();
            }
            else if(t == GT_InReleased || t == GT_OutReleased || t == GT_CancelReleased)
            {
                ValidZayModelRoot();
                if(ZayModel* NewBox = ZayModel::CreateBox(Type, ZayModel::CreateID()))
                {
                    // 송신
                    static sint32 LastChildIndex = -1;
                    const String ChildName = String::Format("child%03d", ++LastChildIndex);
                    Send_CreateToolChild(ChildName, Model);

                    Context NewJson;
                    NewJson.At("title").Set(Title);
                    NewJson.At("x").Set(String::FromInteger(x));
                    NewJson.At("y").Set(String::FromInteger(y));
                    NewJson.At("child").Set(ChildName);
                    NewBox->Import(NewJson);
                    NewBox->LoadDefault();
                    NewBox->UpdateDOM();
                    mZayModels(NewBox->ID()).CreateValue(NewBox);
                    mZayModelOrder.AtAdding() = NewBox->ID();
                    UpdateZayModelOrderDOM();
                    UpdateZayModelMatrix(*NewBox);
                }
            }
        })
    {
        const bool Grabbed = ((panel.state(UIName) & (PS_Pressed | PS_Dragging)) != 0);
        if(Grabbed)
        ZAY_INNER(panel, 5)
        ZAY_MOVE(panel, GrabPosAdd.x, GrabPosAdd.y)
            panel.stretch(R(atlas), Image::Build::Force);
    }
    return true;
}

bool zaytoyData::RenderUC_ZayModelBg(ZayPanel& panel, chars classid)
{
    const String UIName = String::Format("ui_zaymodel_bg_%s", classid);
    const String ClassID = classid;
    ZAY_INNER_UI(panel, 0, UIName,
        ZAY_GESTURE_VNTXY(v, n, t, x, y, this, ClassID)
        {
            static bool HasDrag;
            static point64 GrabPosBegin;
            static point64 BoxPos;
            if(t == GT_Pressed)
            {
                HasDrag = false;
                GrabPosBegin.x = x;
                GrabPosBegin.y = y;
                BoxPos = mZayModels(ClassID)->BoxPos();
                // 순서를 맨앞으로
                for(sint32 i = 0, iend = mZayModelOrder.Count(); i < iend; ++i)
                    if(!mZayModelOrder[i].Compare(ClassID))
                    {
                        mZayModelOrder.SubtractionSection(i);
                        mZayModelOrder.AtAdding() = ClassID;
                        UpdateZayModelOrderDOM();
                        break;
                    }
            }
            else if(t == GT_InDragging || t == GT_OutDragging)
            {
                HasDrag = true;
                mZayModels(ClassID)->ChangeBoxPos(
                    BoxPos.x + x - GrabPosBegin.x,
                    BoxPos.y + y - GrabPosBegin.y);
                v->invalidate();
            }
            else if(t == GT_InReleased || t == GT_OutReleased || t == GT_CancelReleased)
            {
                if(!HasDrag)
                {
                    const String FocusedClassID = ZayWidgetDOM::GetValue("zaymodel.focus").ToText();
                    if(!FocusedClassID.Compare(ClassID))
                        ZayWidgetDOM::SetValue("zaymodel.focus", "'x'");
                    else ZayWidgetDOM::SetValue("zaymodel.focus", "'" + ClassID + "'");
                }
            }
        });
    return true;
}

bool zaytoyData::RenderUC_ZayModelWire(ZayPanel& panel, sint32 x1, sint32 y1, sint32 x2, sint32 y2, float thick)
{
    const sint32 Tail = Math::Min(20 + Math::Clamp((x1 - x2) / 2 + 20, 0, 60), Math::Abs((y1 - y2) / 2));
    Points Dots;
    Dots.AtAdding() = Point(x1 - Tail, y1);
    Dots.AtAdding() = Point(x1, y1);
    Dots.AtAdding() = Point(x2, y2);
    Dots.AtAdding() = Point(x2 + Tail, y2);
    panel.polybezier(Dots, thick, false, false);
    return true;
}

bool zaytoyData::RenderUC_ZayModelSpin(ZayPanel& panel, chars classid, chars group, chars key)
{
    const String UIName = String::Format("ui_zaymodel_spin_%s_%s", group, key);
    const String DomName = String::Format("zaymodel.spin.%s.%s", group, key);
    const String ClassID = classid, Group = group, Key = key;
    ZAY_INNER_UI(panel, 0, UIName,
        ZAY_GESTURE_VNTXY(v, n, t, x, y, this, DomName, ClassID, Group, Key)
        {
            if(t == GT_Pressed || t == GT_InDragging || t == GT_OutDragging)
            {
                auto CurRect = v->rect(n);
                const float Rate = 2.0 * (x - CurRect.l) / (CurRect.r - CurRect.l) - 1.0;
                ZayWidgetDOM::SetValue(DomName, String::FromFloat(Rate));
                v->invalidate();
                // 스핀적용
                if(auto CurBox = mZayModels(ClassID).Value())
                {
                    CurBox->AddActivity(Group, Key, Rate);
                    UpdateZayModelMatrix(*CurBox);
                }
            }
            else if(t == GT_InReleased || t == GT_OutReleased || t == GT_CancelReleased)
            {
                ZayWidgetDOM::SetValue(DomName, DomName + " ~ 0 : 0.25");
                mUpdateMsec = Platform::Utility::CurrentTimeMsec() + 250;
            }
        })
    {
        panel.ninepatch(R("gauge_slider_e_n"));
        auto BarPos = ZayWidgetDOM::GetValue(DomName).ToFloat();
        // 그래프
        if(0 < BarPos)
        {
            ZAY_LTRB(panel, panel.w() / 2, 0, Math::Min(panel.w() * (BarPos + 1) / 2, panel.w()), panel.h())
                panel.ninepatch(R("gauge_slider_e_h"));
        }
        else if(BarPos < 0)
        {
            ZAY_LTRB(panel, Math::Max(0, panel.w() * (BarPos + 1) / 2), 0, panel.w() / 2, panel.h())
                panel.ninepatch(R("gauge_slider_e_h"));
        }
        // 조그바
        ZAY_XYRR(panel, Math::Clamp(panel.w() * (BarPos + 1) / 2, 5, panel.w() - 5), panel.h() / 2, 20, 20)
        {
            const bool Hovered = ((panel.state(UIName) & (PS_Focused | PS_Dropping)) == PS_Focused);
            const bool Grabbed = ((panel.state(UIName) & (PS_Pressed | PS_Dragging)) != 0);
            if(Grabbed) panel.icon(R("btn_jog_c_p"), UIA_CenterMiddle);
            else if(Hovered) panel.icon(R("btn_jog_c_h"), UIA_CenterMiddle);
            else panel.icon(R("btn_jog_c_h"), UIA_CenterMiddle);
        }
    }
    return true;
}
