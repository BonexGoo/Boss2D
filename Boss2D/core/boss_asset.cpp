#include <boss.hpp>
#include "boss_asset.hpp"

#include <platform/boss_platform.hpp>
#if BOSS_NEED_EMBEDDED_ASSET
    #include <boss_gen_assets.cpp>
#endif

class AssetPathClass
{
public:
    AssetPathClass() {}
    ~AssetPathClass() {}

public:
    bool Exist(chars filename, uint64* size, uint64* ctime, uint64* atime, uint64* mtime) const
    {
        for(sint32 i = 0, iend = m_pathes.Count(); i < iend; ++i)
            if(Asset::Exist(m_pathes[i] + filename, nullptr, size, ctime, atime, mtime))
                return true;
        return false;
    }

    id_asset_read OpenForRead(chars filename) const
    {
        for(sint32 i = 0, iend = m_pathes.Count(); i < iend; ++i)
            if(auto NewFile = Asset::OpenForRead(m_pathes[i] + filename))
                return NewFile;
        return nullptr;
    }

    void AddRoot()
    {
        m_pathes.AtAdding();
    }

    void AddPath(chars pathname)
    {
        String& NewString = m_pathes.AtAdding();
        NewString = pathname;
        NewString += '/';
    }

    chars GetPath(sint32 index) const
    {
        return m_pathes[index];
    }

private:
    Strings m_pathes;
};

namespace BOSS
{
    static void _AssetsSearcher(chars itemname, payload data)
    {
        Context& Collector = *((Context*) data);
        if(Platform::File::ExistForDir(itemname)) // 폴더는 재귀처리
            Platform::File::Search(itemname, _AssetsSearcher, data, true);
        else
        {
            // 무시해야하는 파일은 제외
            chars Ignores[] = BOSS_NEED_EMBEDDED_IGNORES;
            const String LowerName = String(itemname).Lower();
            for(sint32 i = 0, iend = sizeof(Ignores) / sizeof(chars); i < iend; ++i)
                if(LowerName.Find(0, Ignores[i]) != -1)
                    return;

            const WString FullName = WString::FromChars(itemname);
            uint64 FileSize = 0, ModifiedTime = 0;
            if(Platform::File::GetAttributes(FullName, &FileSize, nullptr, nullptr, &ModifiedTime) != -1)
            {
                String ID = String(itemname + Platform::File::RootForAssets().Length()).Lower();
                ID.Replace("/", "_DIR_");
                ID.Replace(" ", "_BLANK_");
                ID.Replace(".", "_DOT_");

                auto& NewFile = Collector.At("files").AtAdding();
                NewFile.At("id").Set(ID);
                NewFile.At("path").Set(String::FromWChars(FullName));
                NewFile.At("size").Set(String::FromInteger((sint64) FileSize));
                NewFile.At("time").Set(String::FromInteger((sint64) ModifiedTime));

                // 집계
                const sint64 Count = Collector("data")("count").GetInt();
                const sint64 Size = Collector("data")("size").GetInt();
                const sint64 Time = Collector("data")("time").GetInt();
                Collector.At("data").At("count").Set(String::FromInteger(Count + 1));
                Collector.At("data").At("size").Set(String::FromInteger(sint64(Size + FileSize)));
                if(Time < sint64(ModifiedTime))
                    Collector.At("data").At("time").Set(String::FromInteger(sint64(ModifiedTime)));
            }
        }
    }

    static void _Rebuild(const Context& info)
    {
        if(auto GenFile = Platform::File::OpenForWrite(Platform::File::RootForAssets() + "../source/boss_gen_assets.cpp"))
        {
            String GenText;
            GenText += "#include <boss.hpp>\n";
            GenText += "\n";
            GenText += String::Format("#define BOSS_EMBEDDED_ASSET_COUNT Signed64(%lld)\n", info("data")("count").GetInt());
            GenText += String::Format("#define BOSS_EMBEDDED_ASSET_SIZE  Signed64(%lld)\n", info("data")("size").GetInt());
            GenText += String::Format("#define BOSS_EMBEDDED_ASSET_TIME  Signed64(%lld)\n", info("data")("time").GetInt());
            GenText += "\n";

            // 파일내역
            for(sint32 i = 0, iend = info("files").LengthOfIndexable(); i < iend; ++i)
            {
                auto& CurFile = info("files")[i];
                GenText += String::Format("static bytes ASSETS_%s = (bytes) // %s (%lldbytes, modified at %lld)",
                    CurFile("id").GetString(), CurFile("path").GetString(), CurFile("size").GetInt(), CurFile("time").GetInt());

                if(auto BinFile = Platform::File::OpenForRead(CurFile("path").GetString()))
                {
                    if(Platform::File::Size(BinFile) == 0)
                        GenText += "\n    \"\"";
                    else
                    {
                        uint08 Data[50];
                        while(sint32 DataSize = Platform::File::Read(BinFile, Data, 50))
                        {
                            GenText += "\n    \"";
                            for(sint32 j = 0; j < DataSize; ++j)
                                GenText += String::Format("\\x%02X", Data[j]);
                            GenText += "\"";
                        }
                    }
                    Platform::File::Close(BinFile);
                }
                else GenText += String::Format("\n\"-File Not Found- (%s)\"", CurFile("path").GetString());
                GenText += ";\n";
                GenText += "\n";
            }

            // 파일인스턴스
            GenText += "struct EmbeddedFile {\n";
            GenText += "    chars  mPath;\n";
            GenText += "    bytes  mData;\n";
            GenText += "    uint64 mSize;\n";
            GenText += "    uint64 mCTime;\n";
            GenText += "    uint64 mATime;\n";
            GenText += "    uint64 mMTime;\n";
            GenText += "};\n";
            GenText += "\n";

            GenText += String::Format("static const sint32 gEmbeddedFileCount = %d;\n", info("files").LengthOfIndexable());
            GenText += "static EmbeddedFile gEmbeddedFiles[gEmbeddedFileCount] = {\n";
            const sint32 AssetsPathLength = Platform::File::RootForAssets().Length();
            for(sint32 i = 0, iend = info("files").LengthOfIndexable(); i < iend; ++i)
            {
                auto& CurFile = info("files")[i];
                uint64 CurSize = 0, CurCTime = 0, CurATime = 0, CurMTime = 0;
                Platform::File::GetAttributes(WString::FromChars(CurFile("path").GetString()),
                    &CurSize, &CurCTime, &CurATime, &CurMTime);

                GenText += String::Format("    {\"%s\", ASSETS_%s, Unsigned64(%llu), Unsigned64(%llu), Unsigned64(%llu), Unsigned64(%llu)}%s\n",
                    CurFile("path").GetString() + AssetsPathLength, CurFile("id").GetString(), CurSize, CurCTime, CurATime, CurMTime,
                    (i < iend - 1)? "," : "");
            }
            GenText += "};\n";

            Platform::File::Write(GenFile, (bytes)(chars) GenText, GenText.Length());
            Platform::File::Close(GenFile);
        }
    }

    bool Asset::RebuildForEmbedded()
    {
        #if BOSS_NEED_EMBEDDED_ASSET
        const String& AssetsPath = Platform::File::RootForAssets();
        const String AssetsDir = AssetsPath.Left(AssetsPath.Length() - 1);

        // Assets폴더가 있는 경우에만 리빌드조건 검사를 진행
        if(Platform::File::ExistForDir(AssetsDir))
        {
            // 내장되어야 하는 파일을 리스팅
            Context NewCollector;
            Platform::File::Search(AssetsDir, _AssetsSearcher, &NewCollector, true);

            // 기존 소스와의 비교
            if(NewCollector("data")("count").GetInt() != BOSS_EMBEDDED_ASSET_COUNT ||
                NewCollector("data")("size").GetInt() != BOSS_EMBEDDED_ASSET_SIZE ||
                NewCollector("data")("time").GetInt() != BOSS_EMBEDDED_ASSET_TIME)
            {
                // 리빌드 묻기
                if(Platform::Popup::MessageDialog("리빌드가 필요합니다",
                    "임베디드어셋에 변경이 있습니다.\n\n리빌드후 종료할까요?", DBT_YesNo) == 0)
                {
                    _Rebuild(NewCollector);
                    return true;
                }
            }
        }
        #endif
        return false;
    }

    bool Asset::Exist(chars filename, id_assetpath_read assetpath,
        uint64* size, uint64* ctime, uint64* atime, uint64* mtime)
    {
        if(assetpath)
            return ((const AssetPathClass*) assetpath)->Exist(filename, size, ctime, atime, mtime);
        if(Platform::File::GetAttributes(WString::FromChars(Platform::File::RootForAssetsRem() + filename),
            size, ctime, atime, mtime) != -1)
            return true;
        if(Platform::File::GetAttributes(WString::FromChars(Platform::File::RootForAssets() + filename),
            size, ctime, atime, mtime) != -1)
            return true;
        return false;
    }

    buffer Asset::ToBuffer(chars filename, id_assetpath_read assetpath)
    {
        if(auto Asset = OpenForRead(filename, assetpath))
        {
            auto AssetSize = Asset::Size(Asset);
            buffer Result = Buffer::Alloc(BOSS_DBG AssetSize);
            Asset::Read(Asset, (uint08*) Result, AssetSize);
            Asset::Close(Asset);
            return Result;
        }
        return nullptr;
    }

    id_asset_read Asset::OpenForRead(chars filename, id_assetpath_read assetpath)
    {
        if(assetpath)
            return ((const AssetPathClass*) assetpath)->OpenForRead(filename);
        if(auto Asset = Platform::File::OpenForRead(Platform::File::RootForAssetsRem() + filename))
            return (id_asset_read) Asset;
        if(auto Asset = Platform::File::OpenForRead(Platform::File::RootForAssets() + filename))
            return (id_asset_read) Asset;
        return nullptr;
    }

    id_asset Asset::OpenForWrite(chars filename, bool autocreatedir)
    {
        return (id_asset) Platform::File::OpenForWrite(
            Platform::File::RootForAssetsRem() + filename, autocreatedir);
    }

    void Asset::Close(id_asset_read asset)
    {
        Platform::File::Close((id_file_read) asset);
    }

    const sint32 Asset::Size(id_asset_read asset)
    {
        return Platform::File::Size((id_file_read) asset);
    }

    sint32 Asset::Read(id_asset_read asset, uint08* data, const sint32 size)
    {
        return Platform::File::Read((id_file_read) asset, data, size);
    }

    sint32 Asset::Write(id_asset asset, bytes data, const sint32 size)
    {
        return Platform::File::Write((id_file) asset, data, size);
    }

    sint32 Asset::Skip(id_asset_read asset, const sint32 size)
    {
        const sint32 OldPos = Platform::File::Focus((id_file_read) asset);
        Platform::File::Seek((id_file_read) asset, OldPos + size);
        return Platform::File::Focus((id_file_read) asset);
    }

    id_assetpath AssetPath::Create()
    {
        AssetPathClass* NewAssetPath = (AssetPathClass*) Buffer::Alloc<AssetPathClass>(BOSS_DBG 1);
        return (id_assetpath) NewAssetPath;
    }

    void AssetPath::AddRoot(id_assetpath assetpath)
    {
        BOSS_ASSERT("assetpath인수가 nullptr입니다", assetpath);
        ((AssetPathClass*) assetpath)->AddRoot();
    }

    void AssetPath::AddPath(id_assetpath assetpath, chars pathname)
    {
        BOSS_ASSERT("assetpath인수가 nullptr입니다", assetpath);
        BOSS_ASSERT("pathname인수가 nullptr입니다", pathname);
        ((AssetPathClass*) assetpath)->AddPath(pathname);
    }

    void AssetPath::Release(id_assetpath assetpath)
    {
        Buffer::Free((buffer) assetpath);
    }

    void AssetPath::Find(id_assetpath_read assetpath, FindFileCB filecb, FindPathCB pathcb)
    {
        //////////////////////////////////
        //////////////////////////////////
        //////////////////////////////////
    }

    void AssetPath::Reset(id_assetpath_read assetpath)
    {
        //////////////////////////////////
        //////////////////////////////////
        //////////////////////////////////
    }
}
