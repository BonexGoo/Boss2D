#include <boss.hpp>
#include <platform/boss_platform.hpp>
#include <service/boss_zay.hpp>

#include <resource.hpp>

sint32 gProgramWidth = 900;
#if BOSS_WINDOWS
    sint32 gProgramHeight = 600 + 1;
#else
    sint32 gProgramHeight = 600;
#endif
String gFirstWidget;
sint32 gFirstPosX = 0;
sint32 gFirstPosY = 0;
sint32 gFirstScale = 0;
h_view gWindowView;

bool PlatformInit()
{
    #if BOSS_WINDOWS
        Platform::InitForGL(true);
        if(Asset::RebuildForEmbedded())
            return false;
    #else
        Platform::InitForMDI(true);
    #endif

    sint32 ArgCount = 0;
    chars WidgetName = Platform::Utility::GetArgument(1, &ArgCount);
    if(WidgetName != nullptr && 3 < ArgCount)
    {
        gFirstWidget = WidgetName;
        gFirstPosX = Parser::GetInt(Platform::Utility::GetArgument(2));
        gFirstPosY = Parser::GetInt(Platform::Utility::GetArgument(3));
        if(4 < ArgCount)
            gFirstScale = Parser::GetInt(Platform::Utility::GetArgument(4));
    }

    Platform::SetViewCreator(ZayView::Creator);
    Platform::SetWindowName("HelloApps");
    gWindowView = Platform::SetWindowView("helloappsView");

    // 윈도우 위치설정
    String WindowInfoString = String::FromAsset("windowinfo.json");
    Context WindowInfo(ST_Json, SO_OnlyReference, WindowInfoString, WindowInfoString.Length());
    const sint32 ScreenID = WindowInfo("screen").GetInt(0);
    rect128 ScreenRect = {};
    Platform::Utility::GetScreenRect(ScreenRect, ScreenID);
    const sint32 ScreenWidth = ScreenRect.r - ScreenRect.l;
    const sint32 ScreenHeight = ScreenRect.b - ScreenRect.t;
    const sint32 WindowWidth = gProgramWidth;
    const sint32 WindowHeight = gProgramHeight;
    const sint32 WindowX = Math::Clamp(WindowInfo("x").GetInt((ScreenWidth - WindowWidth) / 2), 0, ScreenWidth - WindowWidth);
    const sint32 WindowY = Math::Clamp(WindowInfo("y").GetInt((ScreenHeight - WindowHeight) / 2), 30, ScreenHeight - WindowHeight);
    if(gFirstWidget.Length() == 0)
        Platform::SetWindowRect(ScreenRect.l + WindowX, ScreenRect.t + WindowY, WindowWidth, WindowHeight);
    else Platform::SetWindowRect(gFirstPosX, gFirstPosY, WindowWidth, WindowHeight);

    // 아틀라스 동적로딩
    const String AtlasInfoString = String::FromAsset("atlasinfo.json");
    Context AtlasInfo(ST_Json, SO_OnlyReference, AtlasInfoString, AtlasInfoString.Length());
    R::SetAtlasDir("image");
    R::AddAtlas("ui_atlaskey2.png", "main.png", AtlasInfo, 2);
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
    if(gFirstWidget.Length() == 0)
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
    }

    // 아틀라스
    Context AtlasInfo;
    R::SaveAtlas(AtlasInfo);
    AtlasInfo.SaveJson().ToAsset("atlasinfo.json");
}

void PlatformFree()
{
}
