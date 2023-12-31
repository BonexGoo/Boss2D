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
        AtlasSet() {FileSize = 0; ModifyTime = 0;}
        ~AtlasSet() {}
        AtlasSet& operator=(const AtlasSet& rhs)
        {
            KeyFileName = rhs.KeyFileName;
            MapFileName = rhs.MapFileName;
            FileSize = rhs.FileSize;
            ModifyTime = rhs.ModifyTime;
            return *this;
        }
    public:
        String KeyFileName;
        String MapFileName;
        sint32 FileSize;
        sint32 ModifyTime;
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
            Exist = true;
            String Path = gAtlasDir + name;
            if(!mImage->SetName(Path).Load(gAssetPath))
            if(!mImage->SetName(Path.Replace('.', '/')).Load(gAssetPath))
            {
                Exist = false;
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

    void R::AddAtlas(chars key_filename, chars map_filename, const Context& ctx)
    {
        gAtlasSets.AtAdding();
        gAtlasSets.At(-1).KeyFileName = key_filename;
        gAtlasSets.At(-1).MapFileName = map_filename;
        gAtlasSets.At(-1).FileSize = ctx(map_filename)("filesize").GetInt(0);
        gAtlasSets.At(-1).ModifyTime = ctx(map_filename)("modifytime").GetInt(0);
    }

    void R::SaveAtlas(Context& ctx)
    {
        for(sint32 i = 0, iend = gAtlasSets.Count(); i < iend; ++i)
        {
            ctx.At(gAtlasSets[i].MapFileName).At("filesize").Set(String::FromInteger(gAtlasSets[i].FileSize));
            ctx.At(gAtlasSets[i].MapFileName).At("modifytime").Set(String::FromInteger(gAtlasSets[i].ModifyTime));
        }
    }

    bool R::IsAtlasUpdated()
    {
        for(sint32 i = 0, iend = gAtlasSets.Count(); i < iend; ++i)
        {
            String FullPath = gAtlasDir + gAtlasSets[i].MapFileName;
            uint64 FileSize = 0, ModifyTime = 0;
            if(Asset::Exist(FullPath, nullptr, &FileSize, nullptr, nullptr, &ModifyTime))
            if(gAtlasSets[i].FileSize != (FileSize & 0x7FFFFFFF) || gAtlasSets[i].ModifyTime != (ModifyTime & 0x7FFFFFFF))
                return true;
        }
        return false;
    }

    void R::RebuildAll()
    {
        gImageMap.Reset();
        BoxrBuilder Builder;
        for(sint32 i = 0, iend = gAtlasSets.Count(); i < iend; ++i)
        {
            String FullPath = gAtlasDir + gAtlasSets[i].MapFileName;
            uint64 FileSize = 0, ModifyTime = 0;
            if(Asset::Exist(FullPath, nullptr, &FileSize, nullptr, nullptr, &ModifyTime))
            if(Builder.LoadAtlas(gAtlasDir + gAtlasSets[i].KeyFileName, gAtlasDir + gAtlasSets[i].MapFileName, 0 < i))
            {
                gAtlasSets.At(i).FileSize = (sint32) (FileSize & 0x7FFFFFFF);
                gAtlasSets.At(i).ModifyTime = (sint32) (ModifyTime & 0x7FFFFFFF);
            }
        }
        Builder.SaveSubImages(gAtlasDir);
    }

    void R::ClearImages(Strings names)
    {
        for(sint32 i = 0, iend = names.Count(); i < iend; ++i)
            gImageMap.Remove(names[i]);
    }
}
