#include <boss.hpp>
#include "boss_asset.hpp"

#include <platform/boss_platform.hpp>

#if BOSS_NEED_PLATFORM_FILE
    #if BOSS_WINDOWS
        #include <windows.h>
    #endif
#else
    #if BOSS_WINDOWS
        #include <windows.h>
    #else
        #include <sys/stat.h>
    #endif
    #include <stdio.h>
#endif

class AssetPathClass
{
public:
    AssetPathClass() {}
    ~AssetPathClass() {}

public:
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

    roottype Exist(chars filename) const
    {
        // 순수한 어셋명 추출
        chars FileNameOnly = GetBySearchingRule(filename);
        // 시스템패스 순회
        for(sint32 i = 0, iend = m_pathes.Count(); i < iend; ++i)
            if(roottype RootType = Asset::Exist(m_pathes[i] + FileNameOnly))
                return (roottype) (RootType + 2 * (i + 1));
        return roottype_null;
    }
public:
    static chars GetBySearchingRule(chars filename)
    {
        #ifndef BOSS_RULE_ADMIT_MIDDLE_PATH
            return filename;
        #else
            chars FileNameOnly = filename;
            for(chars NameFocus = filename; *NameFocus; ++NameFocus)
                if(*NameFocus == '/' || *NameFocus == '\\')
                    FileNameOnly = NameFocus + 1;
            return FileNameOnly;
        #endif
    }

private:
    Strings m_pathes;
};

namespace BOSS
{
    roottype Asset::Exist(chars filename, id_assetpath_read assetpath)
    {
        if(assetpath)
        if(roottype RootType = ((const AssetPathClass*) assetpath)->Exist(filename))
            return RootType;

        #if BOSS_NEED_PLATFORM_FILE
            if(Platform::File::Exist(Platform::File::RootForAssetsRem() + filename))
                return roottype_assetsrem;
            if(Platform::File::Exist(Platform::File::RootForAssets() + filename))
                return roottype_assets;
        #else
            if(FILE* CheckFile = fopen(Platform::File::RootForAssetsRem() + filename, "rb"))
            {
                fclose(CheckFile);
                return roottype_assetsrem;
            }
            if(FILE* CheckFile = fopen(Platform::File::RootForAssets() + filename, "rb"))
            {
                fclose(CheckFile);
                return roottype_assets;
            }
        #endif

        return roottype_null;
    }

    chars Asset::GetPathForExist(roottype type, id_assetpath_read assetpath)
    {
        if(type == roottype_null) return "/";
        String DirPath = ((type % 2) != roottype_assets)?
            Platform::File::RootForAssetsRem() : Platform::File::RootForAssets();
        if(assetpath)
        if(const sint32 PathIndex = (type - roottype_assets) / 2)
            DirPath += ((const AssetPathClass*) assetpath)->GetPath(PathIndex - 1);

        static String StaticResult;
        StaticResult = DirPath;
        return StaticResult;
    }

    id_asset_read Asset::OpenForRead(chars filename, id_assetpath_read assetpath)
    {
        const roottype RootType = Exist(filename, assetpath);
        if(RootType != roottype_null)
        {
            String FilePath = GetPathForExist(RootType, assetpath);
            FilePath += AssetPathClass::GetBySearchingRule(filename);
            #if BOSS_NEED_PLATFORM_FILE
                return (id_asset_read) Platform::File::OpenForRead(FilePath);
            #else
                return (id_asset_read) fopen(FilePath, "rb");
            #endif
        }
        return nullptr;
    }

    id_asset Asset::OpenForWrite(chars filename, bool autocreatedir)
    {
        #if BOSS_NEED_PLATFORM_FILE
            return (id_asset) Platform::File::OpenForWrite(Platform::File::RootForAssetsRem() + filename, autocreatedir);
        #else
            FILE* WriteFile = fopen(Platform::File::RootForAssetsRem() + filename, "wb");
            if(autocreatedir && !WriteFile)
            {
                String DirPath = Platform::File::RootForAssetsRem();
                for(chars iChar = filename; *iChar; ++iChar)
                {
                    const char OneChar = *iChar;
                    if(OneChar == '/' || OneChar == '\\')
                        #if BOSS_WINDOWS
                            CreateDirectoryA(DirPath, nullptr);
                        #else
                            mkdir(DirPath, 0777);
                        #endif
                    DirPath += OneChar;
                }
                WriteFile = fopen(Platform::File::RootForAssetsRem() + filename, "wb");
            }
            return (id_asset) WriteFile;
        #endif
    }

    void Asset::Close(id_asset_read asset)
    {
        #if BOSS_NEED_PLATFORM_FILE
            Platform::File::Close((id_file_read) asset);
        #else
            fclose((FILE*) asset);
        #endif
    }

    const sint32 Asset::Size(id_asset_read asset)
    {
        #if BOSS_NEED_PLATFORM_FILE
            return Platform::File::Size((id_file_read) asset);
        #else
            long CurrentPos = ftell((FILE*) asset);
            fseek((FILE*) asset, 0, SEEK_END);
            long Result = ftell((FILE*) asset);
            fseek((FILE*) asset, CurrentPos, SEEK_SET);
            return Result;
        #endif
    }

    void Asset::Read(id_asset_read asset, uint08* data, const sint32 size)
    {
        #if BOSS_NEED_PLATFORM_FILE
            Platform::File::Read((id_file_read) asset, data, size);
        #else
            fread(data, 1, size, (FILE*) asset);
        #endif
    }

    void Asset::Write(id_asset asset, bytes data, const sint32 size)
    {
        #if BOSS_NEED_PLATFORM_FILE
            Platform::File::Write((id_file) asset, data, size);
        #else
            fwrite(data, 1, size, (FILE*) asset);
        #endif
    }

    void Asset::Skip(id_asset_read asset, const sint32 size)
    {
        #if BOSS_NEED_PLATFORM_FILE
            Platform::File::Seek((id_file_read) asset, Platform::File::Focus((id_file_read) asset) + size);
        #else
            fseek((FILE*) asset, size, SEEK_CUR);
        #endif
    }

    bool Asset::ValidCache(chars filename, chars cachename, id_assetpath_read assetpath)
    {
        if(roottype FileRoot = Asset::Exist(filename, assetpath))
        if(roottype CacheRoot = Asset::Exist(cachename, assetpath))
        {
            uint64 FileTime = 0, CacheTime = 0;
            const WString FileName = WString::FromChars(String(GetPathForExist(FileRoot, assetpath)) + filename);
            const WString CacheName = WString::FromChars(String(GetPathForExist(CacheRoot, assetpath)) + cachename);
            Platform::File::GetAttributes(FileName, nullptr, nullptr, nullptr, &FileTime);
            Platform::File::GetAttributes(CacheName, nullptr, nullptr, nullptr, &CacheTime);
            return (FileTime < CacheTime);
        }
        return false;
    }

    id_assetpath Asset::CreatePath(chars pathname)
    {
        AssetPathClass* NewAssetPath = (AssetPathClass*) Buffer::Alloc<AssetPathClass>(BOSS_DBG 1);
        if(pathname) NewAssetPath->AddPath(pathname);
        return (id_assetpath) NewAssetPath;
    }

    void Asset::AddByPath(id_assetpath assetpath, chars pathname)
    {
        BOSS_ASSERT("assetpath인수가 nullptr입니다", assetpath);
        BOSS_ASSERT("pathname인수가 nullptr입니다", pathname);
        ((AssetPathClass*) assetpath)->AddPath(pathname);
    }

    void Asset::ReleasePath(id_assetpath_read assetpath)
    {
        Buffer::Free((buffer) assetpath);
    }
}
