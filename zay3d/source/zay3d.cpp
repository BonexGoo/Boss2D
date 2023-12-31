#include <boss.hpp>
#include "zay3d.hpp"

#include <resource.hpp>
#include <service/boss_zaywidget.hpp>
#include <format/boss_bmp.hpp>
#include <format/boss_flv.hpp>

ZAY_DECLARE_VIEW_CLASS("zay3dView", zay3dData)

ZAY_VIEW_API OnCommand(CommandType type, id_share in, id_cloned_share* out)
{
    if(type == CT_Size)
    {
        sint32s WH(in);
        m->mWindowSize.w = WH[0];
        #if BOSS_WINDOWS
            m->mWindowSize.h = WH[1] - 1;
        #else
            m->mWindowSize.h = WH[1];
        #endif
    }
}

ZAY_VIEW_API OnNotify(NotifyType type, chars topic, id_share in, id_cloned_share* out)
{
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
            Platform::SetWindowRect(
                OldWindowRect.l + CurCursorPos.x - OldCursorPos.x,
                OldWindowRect.t + CurCursorPos.y - OldCursorPos.y,
                OldWindowRect.r - OldWindowRect.l, OldWindowRect.b - OldWindowRect.t);
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
        ZAY_RGB(panel, 192, 192, 192)
            panel.fill();

        if(Platform::Graphics::BeginGL())
        {
            AddOn::Abc::Render(m->mXyzABC, 0, 0, panel.w(), panel.h());
            AddOn::Abc::Render(m->mOctopusABC, 0, 0, panel.w(), panel.h());
            Platform::Graphics::EndGL();
        }

        // 윈도우시스템
        #if !BOSS_ANDROID && !BOSS_IPHONE
            m->RenderWindowSystem(panel);
        #endif
    }
}

zay3dData::zay3dData()
{
    mWhisper.AddRecvCB("EnumToy",
        [this](const Context& data)->void
        {
            static const int Count = 5 * Math::Random();
            Context Packet;
            for(sint32 i = 0, iend = 1 + Count; i < iend; ++i)
                Packet.At("toy").AtAdding().Set(String::Format("test%d.zt1", i));
            mWhisper.Send("ToyList", Packet);
        });
    mWhisper.AddRecvCB("LoadToy",
        [this](const Context& data)->void
        {
            /////////////
        });
    mWhisper.OpenForSpot();

    mXyzABC = AddOn::Abc::Create(Platform::File::RootForAssets() + "abc/xyz_01.abc");
    AddOn::Abc::SetColor(mXyzABC, "x.Cylinder_x", 1, 0, 0);
    AddOn::Abc::SetColor(mXyzABC, "y.Cylinder_y", 0, 1, 0);
    AddOn::Abc::SetColor(mXyzABC, "z.Cylinder_z", 0, 0, 1);
    AddOn::Abc::SetColor(mXyzABC, "Cube.Cube_002", 1, 1, 1);

    mOctopusABC = AddOn::Abc::Create(Platform::File::RootForAssets() + "abc/octopus_toy.abc");
    AddOn::Abc::SetColor(mOctopusABC, "Octopus.Sphere", 0.5, 0, 1);
}

zay3dData::~zay3dData()
{
    mWhisper.Close();
    AddOn::Abc::Release(mXyzABC);
    AddOn::Abc::Release(mOctopusABC);
}

void zay3dData::RenderWindowSystem(ZayPanel& panel)
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

bool zay3dData::IsFullScreen()
{
    return mIsFullScreen;
}

void zay3dData::SetFullScreen()
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

void zay3dData::SetNormalWindow()
{
    if(mIsFullScreen)
    {
        mIsFullScreen = false;
        Platform::SetWindowRect(mSavedNormalRect.l, mSavedNormalRect.t,
            mSavedNormalRect.r - mSavedNormalRect.l, mSavedNormalRect.b - mSavedNormalRect.t);
    }
}

void zay3dData::SetCursor(CursorRole role)
{
    if(mNowCursor != role)
    {
        mNowCursor = role;
        Platform::Utility::SetCursor(role);
    }
}
