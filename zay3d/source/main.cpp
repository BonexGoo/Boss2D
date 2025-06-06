﻿#include <boss.hpp>
#include <platform/boss_platform.hpp>
#include <service/boss_zay.hpp>

#include <resource.hpp>

bool PlatformInit()
{
    #if BOSS_WINDOWS
        Platform::InitForGL(true);
        if(Asset::RebuildForEmbedded())
            return false;
    #else
        Platform::InitForMDI(true);
    #endif

    Platform::SetViewCreator(ZayView::Creator);
    Platform::SetWindowName("ZAY3D");
    Platform::SetWindowView("zay3dView");

    // 윈도우 위치설정
    String WindowInfoString = String::FromAsset("windowinfo.json");
    Context WindowInfo(ST_Json, SO_OnlyReference, WindowInfoString, WindowInfoString.Length());
    const sint32 ScreenID = WindowInfo("screen").GetInt(0);
    rect128 ScreenRect = {};
    Platform::Utility::GetScreenRect(ScreenRect, ScreenID);
    const sint32 ScreenWidth = ScreenRect.r - ScreenRect.l;
    const sint32 ScreenHeight = ScreenRect.b - ScreenRect.t;
    const sint32 WindowWidth = 1600;
    const sint32 WindowHeight = 1000 + 1;
    const sint32 WindowX = Math::Clamp(WindowInfo("x").GetInt((ScreenWidth - WindowWidth) / 2), 0, ScreenWidth - WindowWidth);
    const sint32 WindowY = Math::Clamp(WindowInfo("y").GetInt((ScreenHeight - WindowHeight) / 2), 30, ScreenHeight - WindowHeight);
    Platform::SetWindowRect(ScreenRect.l + WindowX, ScreenRect.t + WindowY, WindowWidth, WindowHeight);

    // 아틀라스 동적로딩
    String AtlasInfoString = String::FromAsset("atlasinfo.json");
    Context AtlasInfo(ST_Json, SO_OnlyReference, AtlasInfoString, AtlasInfoString.Length());
    R::SetAtlasDir("image");
    R::AddAtlas("ui_atlaskey.png", "atlas_window.png", AtlasInfo);
    if(R::IsAtlasUpdated())
        R::RebuildAll();

    Platform::AddProcedure(PE_100MSEC,
        [](payload data)->void
        {
            if(R::IsAtlasUpdated())
            {
                R::RebuildAll();
                Platform::UpdateAllViews();
            }
        });
    return true;
}

void PlatformQuit()
{
    // 윈도우
    const rect128 WindowRect = Platform::GetWindowRect(true);
    const sint32 ScreenID = Platform::Utility::GetScreenID(
        {(WindowRect.l + WindowRect.r) / 2, (WindowRect.t + WindowRect.b) / 2});
    rect128 ScreenRect = {};
    Platform::Utility::GetScreenRect(ScreenRect, ScreenID);
    Context WindowInfo;
    WindowInfo.At("screen").Set(String::FromInteger(ScreenID));
    WindowInfo.At("x").Set(String::FromInteger(WindowRect.l - ScreenRect.l));
    WindowInfo.At("y").Set(String::FromInteger(WindowRect.t - ScreenRect.t));
    WindowInfo.SaveJson().ToAsset("windowinfo.json");

    // 아틀라스
    Context AtlasInfo;
    R::SaveAtlas(AtlasInfo);
    AtlasInfo.SaveJson().ToAsset("atlasinfo.json");
}

void PlatformFree()
{
}
