#include <boss.hpp>
#include <platform/boss_platform.hpp>
#include <service/boss_zay.hpp>

#include <resource.hpp>

chars gAppName = nullptr;
sint32 gStartPort = 0;
static sint32 gStartX = 0;
static sint32 gStartY = 0;
static sint32 gStartWidth = 0;
static sint32 gStartHeight = 0;
bool gStartSSL = false;

bool PlatformInit()
{
    #if BOSS_WASM
        Platform::InitForMDI(true);
    #else
        Platform::InitForMDI();
        if(Asset::RebuildForEmbedded())
            return false;

        String DataPath = Platform::File::RootForData();
        Platform::File::ResetAssetsRemRoot(DataPath);
    #endif

    gAppName = Platform::Utility::GetArgument(1);
    if(*gAppName)
    {
        gStartPort = Parser::GetInt(Platform::Utility::GetArgument(2));
        gStartX = Parser::GetInt(Platform::Utility::GetArgument(3));
        gStartY = Parser::GetInt(Platform::Utility::GetArgument(4));
        gStartWidth = Math::Max(400, Parser::GetInt(Platform::Utility::GetArgument(5)));
        gStartHeight = Math::Max(300, Parser::GetInt(Platform::Utility::GetArgument(6)));
        gStartSSL = (String::CompareNoCase(Platform::Utility::GetArgument(7), "SSL") == 0);
    }

    Platform::SetViewCreator(ZayView::Creator);
    Platform::SetWindowName("ZayData");
    Platform::SetWindowView("zaydataView");

    // 윈도우 위치설정
    if(*gAppName == '\0')
    {
        String WindowInfoString = String::FromAsset("windowinfo.json");
        Context WindowInfo(ST_Json, SO_OnlyReference, WindowInfoString, WindowInfoString.Length());
        const sint32 ScreenID = WindowInfo("screen").GetInt(0);
        rect128 ScreenRect = {};
        Platform::Utility::GetScreenRect(ScreenRect, ScreenID);
        const sint32 ScreenWidth = ScreenRect.r - ScreenRect.l;
        const sint32 ScreenHeight = ScreenRect.b - ScreenRect.t;
        const sint32 WindowWidth = WindowInfo("w").GetInt(800);
        const sint32 WindowHeight = WindowInfo("h").GetInt(600);
        const sint32 WindowX = Math::Clamp(WindowInfo("x").GetInt((ScreenWidth - WindowWidth) / 2), 0, ScreenWidth - WindowWidth);
        const sint32 WindowY = Math::Clamp(WindowInfo("y").GetInt((ScreenHeight - WindowHeight) / 2), 30, ScreenHeight - WindowHeight);
        Platform::SetWindowRect(ScreenRect.l + WindowX, ScreenRect.t + WindowY, WindowWidth, WindowHeight);
    }
    else Platform::SetWindowRect(gStartX, gStartY, gStartWidth, gStartHeight);

    // 아틀라스 동적로딩
    const String AtlasInfoString = String::FromAsset("atlasinfo.json");
    Context AtlasInfo(ST_Json, SO_OnlyReference, AtlasInfoString, AtlasInfoString.Length());
    R::SetAtlasDir("image");
    R::AddAtlas("ui_atlaskey.png", "atlas.png", AtlasInfo);
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
    if(*gAppName == '\0')
    {
        const rect128 WindowRect = Platform::GetWindowRect(true);
        const sint32 ScreenID = Platform::Utility::GetScreenID({(WindowRect.l + WindowRect.r) / 2, (WindowRect.t + WindowRect.b) / 2});
        rect128 ScreenRect = {};
        Platform::Utility::GetScreenRect(ScreenRect, ScreenID);
        Context WindowInfo;
        WindowInfo.At("screen").Set(String::FromInteger(ScreenID));
        WindowInfo.At("x").Set(String::FromInteger(WindowRect.l - ScreenRect.l));
        WindowInfo.At("y").Set(String::FromInteger(WindowRect.t - ScreenRect.t));
        WindowInfo.At("w").Set(String::FromInteger(WindowRect.r - WindowRect.l));
        WindowInfo.At("h").Set(String::FromInteger(WindowRect.b - WindowRect.t));
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
