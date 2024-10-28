#include <boss.hpp>
#include "resource.hpp"

#include <format/boss_bmp.hpp>
#include <format/boss_png.hpp>
#include <platform/boss_platform.hpp>
#include <service/boss_boxr.hpp>

namespace BOSS
{
    class BoolClass
    {
    public:
        BoolClass() {mFlag = false;}
        ~BoolClass() {}
    public:
        bool mFlag;
    };
    class AtlasSet
    {
    public:
        AtlasSet& operator=(const AtlasSet& rhs)
        {
            KeyVerCode = rhs.KeyVerCode;
            KeyFileName = rhs.KeyFileName;
            AtlasFileName = rhs.AtlasFileName;
            AtlasFolderName = rhs.AtlasFolderName;
            FileSize = rhs.FileSize;
            ModifyTime = rhs.ModifyTime;
            return *this;
        }
    public:
        sint32 KeyVerCode {0}; // 1 또는 2
        String KeyFileName;
        String AtlasFileName;
        String AtlasFolderName;
        sint32 FileSize {0};
        sint32 ModifyTime {0};
        bool Updated {false};
    };
    static Map<Image> gImageMap;
    static Map<BoolClass> gExistMap;
    static id_assetpath gAssetPath = nullptr;
    static String gAtlasDir;
    static Array<AtlasSet> gAtlasSets;

    R::R(chars name)
    {
        mImage = gImageMap.Access(name);
        bool& Exist = gExistMap(name).mFlag;
        if(!mImage)
        {
            mImage = &gImageMap(name);
            // 파일형 이미지 로딩시도
            String Path = gAtlasDir + name;
            Exist = mImage->SetName(Path.Replace('.', '/')).Load(gAssetPath);
            // 아틀라스형 이미지들 로딩시도
            for(sint32 i = gAtlasSets.Count() - 1; 0 <= i && !Exist; --i)
            {
                Path = gAtlasDir + gAtlasSets[i].AtlasFolderName + '/' + name;
                Exist = mImage->SetName(Path).Load(gAssetPath);
            }
            // 로딩실패시
            if(!Exist)
            {
                if(id_asset_read PngAsset = Asset::OpenForRead(gAtlasDir + "noimg.png"))
                {
                    const sint32 PngSize = Asset::Size(PngAsset);
                    buffer PngBuffer = Buffer::Alloc(BOSS_DBG PngSize);
                    Asset::Read(PngAsset, (uint08*) PngBuffer, PngSize);
                    Asset::Close(PngAsset);
                    mImage->LoadBitmap(Png().ToBmp((bytes) PngBuffer, true));
                    Buffer::Free(PngBuffer);
                }
                else
                {
                    id_bitmap NewBitmap = Bmp::Create(4, 10, 10);
                    Bmp::FillColor(NewBitmap, Color(255, 0, 0).argb);
                    id_bitmap OldBitmap = mImage->ChangeBitmap(NewBitmap);
                    Bmp::Remove(OldBitmap);
                }
            }
        }
        mExist = Exist;
    }

    R::~R()
    {
    }

    void R::SetAssetPath(id_assetpath assetpath)
    {
        AssetPath::Release(gAssetPath);
        gAssetPath = assetpath;
    }

    void R::SetAtlasDir(chars dirname)
    {
        if(*dirname == '\0') gAtlasDir = "";
        else gAtlasDir = String::Format("%s/", dirname);
    }

    void R::ClearAll(bool withcache)
    {
        gImageMap.Reset();
        gExistMap.Reset();
        gAtlasSets.Clear();
        if(withcache)
        {
            const String AtlasPath = (Platform::File::RootForAssetsRem() + gAtlasDir).SubTail(1);
            Platform::File::Search(AtlasPath,
                [](chars itemname, payload data)->void
                {
                    if(Platform::File::Exist(itemname))
                        Platform::File::Remove(WString::FromChars(itemname), false);
                }, nullptr, true);
            Platform::File::RemoveDir(WString::FromChars(AtlasPath), true);
        }
    }

    void R::AddAtlas(chars key_filename, chars map_filename, const Context& ctx, sint32 keyver)
    {
        auto& NewAtlasSet = gAtlasSets.AtAdding();
        NewAtlasSet.KeyVerCode = keyver;
        NewAtlasSet.KeyFileName = key_filename;
        NewAtlasSet.AtlasFileName = map_filename;
        NewAtlasSet.AtlasFolderName = String(map_filename).Replace('.', '_');
        NewAtlasSet.FileSize = ctx(map_filename)("filesize").GetInt(0);
        NewAtlasSet.ModifyTime = ctx(map_filename)("modifytime").GetInt(0);
    }

    void R::SaveAtlas(Context& ctx)
    {
        for(sint32 i = 0, iend = gAtlasSets.Count(); i < iend; ++i)
        {
            ctx.At(gAtlasSets[i].AtlasFileName).At("filesize").Set(String::FromInteger(gAtlasSets[i].FileSize));
            ctx.At(gAtlasSets[i].AtlasFileName).At("modifytime").Set(String::FromInteger(gAtlasSets[i].ModifyTime));
        }
    }

    bool R::IsAtlasUpdated()
    {
        bool AnyUpdated = false;
        for(sint32 i = 0, iend = gAtlasSets.Count(); i < iend; ++i)
        {
            auto& CurAtlasSet = gAtlasSets.At(i);
            String FullPath = gAtlasDir + CurAtlasSet.AtlasFileName;
            uint64 FileSize = 0, ModifyTime = 0;
            if(Asset::Exist(FullPath, nullptr, &FileSize, nullptr, nullptr, &ModifyTime))
            if(CurAtlasSet.FileSize != (FileSize & 0x7FFFFFFF) || CurAtlasSet.ModifyTime != (ModifyTime & 0x7FFFFFFF))
            {
                CurAtlasSet.FileSize = (sint32) (FileSize & 0x7FFFFFFF);
                CurAtlasSet.ModifyTime = (sint32) (ModifyTime & 0x7FFFFFFF);
                CurAtlasSet.Updated = true;
                AnyUpdated = true;
            }
        }

        if(AnyUpdated)
        {
            Platform::BroadcastNotify("AtlasUpdated", nullptr, NT_ZayAtlas, nullptr, true);
            return true;
        }
        return false;
    }

    String R::PrintUpdatedAtlas(bool everything)
    {
        Context UpdatedAtlas;
        for(sint32 i = 0, iend = gAtlasSets.Count(); i < iend; ++i)
        {
            if(gAtlasSets[i].Updated || everything)
            {
                UpdatedAtlas.At(i).At("folder").Set(Platform::File::RootForAssetsRem() + gAtlasDir + gAtlasSets[i].AtlasFolderName);
                UpdatedAtlas.At(i).At("filesize").Set(String::FromInteger(gAtlasSets[i].FileSize));
                UpdatedAtlas.At(i).At("modifytime").Set(String::FromInteger(gAtlasSets[i].ModifyTime));
            }
        }
        return UpdatedAtlas.SaveJson();
    }

    void R::RebuildAll()
    {
        gImageMap.Reset();
        for(sint32 i = 0, iend = gAtlasSets.Count(); i < iend; ++i)
        {
            BoxrBuilder Builder;
            if(gAtlasSets[i].Updated)
            {
                gAtlasSets.At(i).Updated = false;
                Builder.LoadAtlas(gAtlasDir + gAtlasSets[i].KeyFileName,
                    gAtlasDir + gAtlasSets[i].AtlasFileName, 0 < i, gAtlasSets[i].KeyVerCode);
            }
            Builder.SaveSubImages(gAtlasDir + gAtlasSets[i].AtlasFolderName);
        }
    }

    void R::ClearImages(Strings names)
    {
        for(sint32 i = 0, iend = names.Count(); i < iend; ++i)
            gImageMap.Remove(names[i]);
    }
}
