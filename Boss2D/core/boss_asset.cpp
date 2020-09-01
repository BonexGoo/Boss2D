#include <boss.hpp>
#include "boss_asset.hpp"

#include <platform/boss_platform.hpp>
#if BOSS_NEED_EMBEDDED_ASSET
    #include <boss_gen_assets.hpp>
#endif

////////////////////////////////////////////////////////////////////////////////
// AssetClass
class AssetClass
{
public:
    AssetClass() {}
    virtual ~AssetClass() {}

public:
    virtual sint32 Size() = 0;
    virtual sint32 Read(uint08* data, const sint32 size) = 0;
    virtual sint32 Write(bytes data, const sint32 size) = 0;
    virtual sint32 Skip(const sint32 size) = 0;
};

////////////////////////////////////////////////////////////////////////////////
// AssetFileClass
class AssetFileClass : public AssetClass
{
public:
    AssetFileClass() {m_file = nullptr;}
    ~AssetFileClass() override {Platform::File::Close(m_file);}

public:
    sint32 Size() override
    {
        return Platform::File::Size(m_file);
    }
    sint32 Skip(const sint32 size) override
    {
        const sint32 OldPos = Platform::File::Focus(m_file);
        Platform::File::Seek(m_file, OldPos + size);
        return Platform::File::Focus(m_file);
    }

protected:
    id_file m_file;
};

////////////////////////////////////////////////////////////////////////////////
// AssetReadFileClass
class AssetReadFileClass : public AssetFileClass
{
public:
    static AssetClass* Create(chars pathname)
    {
        if(auto Asset = Platform::File::OpenForRead(pathname))
        {
            auto Result = new AssetReadFileClass();
            Result->m_file = (id_file) Asset;
            return Result;
        }
        return nullptr;
    }
    static bool Exist(chars pathname, uint64* size, uint64* ctime, uint64* atime, uint64* mtime)
    {
        return (Platform::File::GetAttributes(WString::FromChars(pathname),
            size, ctime, atime, mtime) != -1);
    }

public:
    sint32 Read(uint08* data, const sint32 size) override
    {
        return Platform::File::Read(m_file, data, size);
    }
    sint32 Write(bytes data, const sint32 size) override
    {
        return 0;
    }
};

////////////////////////////////////////////////////////////////////////////////
// AssetWriteFileClass
class AssetWriteFileClass : public AssetFileClass
{
public:
    static AssetClass* Create(chars pathname, bool autocreatedir)
    {
        if(auto Asset = Platform::File::OpenForWrite(pathname, autocreatedir))
        {
            auto Result = new AssetWriteFileClass();
            Result->m_file = Asset;
            return Result;
        }
        return nullptr;
    }

public:
    sint32 Read(uint08* data, const sint32 size) override
    {
        return 0;
    }
    sint32 Write(bytes data, const sint32 size) override
    {
        return Platform::File::Write(m_file, data, size);
    }
};

#if BOSS_NEED_EMBEDDED_ASSET
    ////////////////////////////////////////////////////////////////////////////////
    // AssetEmbeddedFileClass
    class AssetEmbeddedFileClass : public AssetClass
    {
    public:
        AssetEmbeddedFileClass() {m_embedded_file = nullptr; m_pos = 0;}
        ~AssetEmbeddedFileClass() override {}

    public:
        sint32 Size() override
        {
            return m_embedded_file->mSize;
        }
        sint32 Skip(const sint32 size) override
        {
            m_pos = Math::Clamp(m_pos + size, 0, m_embedded_file->mSize);
            return m_pos;
        }

    protected:
        static EmbeddedFile* FindCache(chars pathname, bool forced)
        {
            const String PathName = String(pathname).Lower();
            if(auto FindedCache = gEmbeddedCaches.Access(PathName))
                return FindedCache;

            // 캐시생성
            if(forced)
            {
                auto& NewCache = gEmbeddedCaches(PathName);
                NewCache.mPath = (chars) PathName;
                NewCache.mBuffer = Buffer::Alloc(BOSS_DBG 256);
                NewCache.mPages = new bytes[1] {(bytes) NewCache.mBuffer};
                NewCache.mSize = 0;
                NewCache.mCTime = 0;
                NewCache.mATime = 0;
                NewCache.mMTime = 0;
                return &NewCache;
            }
            return nullptr;
        }
        static EmbeddedFile* FindFile(chars pathname)
        {
            // 이진탐색
            const String PathName = String(pathname).Lower();
            sint32 iLow = 0, iHigh = BOSS_EMBEDDED_ASSET_COUNT - 1;
            while(iLow <= iHigh)
            {
                const sint32 i = (iLow + iHigh) / 2;
                const sint32 Result = PathName.Compare(gSortedEmbeddedFiles[i].mPath);
                if(Result == 0)
                    return &gSortedEmbeddedFiles[i];
                else if(Result < 0)
                    iHigh = i - 1;
                else iLow = i + 1;
            }
            return nullptr;
        }

    protected:
        EmbeddedFile* m_embedded_file;
        sint32 m_pos;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // AssetEmbeddedReadFileClass
    class AssetEmbeddedReadFileClass : public AssetEmbeddedFileClass
    {
    public:
        static AssetClass* Create(bool cache, chars pathname)
        {
            if(auto OneFile = (cache)? FindCache(pathname, false) : FindFile(pathname))
            {
                auto Result = new AssetEmbeddedReadFileClass();
                Result->m_embedded_file = OneFile;
                return Result;
            }
            return nullptr;
        }
        static bool Exist(bool cache, chars pathname, uint64* size, uint64* ctime, uint64* atime, uint64* mtime)
        {
            if(auto OneFile = (cache)? FindCache(pathname, false) : FindFile(pathname))
            {
                if(size) *size = OneFile->mSize;
                if(ctime) *ctime = OneFile->mCTime;
                if(atime) *atime = OneFile->mATime;
                if(mtime) *mtime = OneFile->mMTime;
                return true;
            }
            return false;
        }

    public:
        sint32 Read(uint08* data, const sint32 size) override
        {
            // 포커스정보 갱신
            sint32 ReadSize = Math::Min(size, m_embedded_file->mSize - m_pos);
            sint32 PageIndex = m_pos / BOSS_EMBEDDED_ASSET_PAGE;
            sint32 PageOffset = m_pos % BOSS_EMBEDDED_ASSET_PAGE;
            const sint32 Result = ReadSize;

            // 페이지를 돌며 버퍼읽기
            while(0 < ReadSize)
            {
                const sint32 CopySize = Math::Min(ReadSize, BOSS_EMBEDDED_ASSET_PAGE - PageOffset);
                Memory::Copy(data, m_embedded_file->mPages[PageIndex] + PageOffset, CopySize);
                data += CopySize;
                ReadSize -= CopySize;
                PageIndex++;
                PageOffset = 0;
            }
            m_pos += Result;
            return Result;
        }
        sint32 Write(bytes data, const sint32 size) override
        {
            return 0;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    // AssetEmbeddedWriteFileClass
    class AssetEmbeddedWriteFileClass : public AssetEmbeddedFileClass
    {
    public:
        static AssetClass* Create(chars pathname)
        {
            if(auto OneFile = FindCache(pathname, true))
            {
                auto Result = new AssetEmbeddedWriteFileClass();
                Result->m_embedded_file = OneFile;
                return Result;
            }
            return nullptr;
        }

    public:
        sint32 Read(uint08* data, const sint32 size) override
        {
            return 0;
        }
        sint32 Write(bytes data, const sint32 size) override
        {
            // 버퍼확장
            const sint32 NewBufferSize = m_embedded_file->mSize + size;
            sint32 CurBufferSize = Buffer::CountOf(m_embedded_file->mBuffer);
            if(CurBufferSize < NewBufferSize)
            {
                // 버퍼 재구성
                do {CurBufferSize *= 2;}
                while(CurBufferSize < NewBufferSize);
                m_embedded_file->mBuffer =
                    Buffer::Realloc(BOSS_DBG m_embedded_file->mBuffer, CurBufferSize);
                // 페이지 재구성
                delete[] m_embedded_file->mPages;
                const sint32 NumPages = (CurBufferSize + BOSS_EMBEDDED_ASSET_PAGE - 1) / BOSS_EMBEDDED_ASSET_PAGE;
                m_embedded_file->mPages = new bytes[NumPages];
                for(sint32 i = 0; i < NumPages; ++i)
                    m_embedded_file->mPages[i] = ((bytes) m_embedded_file->mBuffer) + BOSS_EMBEDDED_ASSET_PAGE * i;
            }
            // 버퍼쓰기
            Memory::Copy(((uint08*) m_embedded_file->mBuffer)
                + m_embedded_file->mSize, data, size);
            m_embedded_file->mSize += size;
            return size;
        }
    };
#endif

////////////////////////////////////////////////////////////////////////////////
// AssetPathClass
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
    void AddPath(chars pathname)
    {
        String& NewString = m_pathes.AtAdding();
        if(pathname)
        {
            NewString = pathname;
            NewString += '/';
        }
    }
    chars GetPath(sint32 index) const
    {
        return m_pathes[index];
    }
    void Find(AssetPath::FindFileCB filecb, AssetPath::FindPathCB pathcb)
    {
        //////////////////////////////////////////
        //////////////////////////////////////////
        //////////////////////////////////////////
        for(sint32 i = 0, iend = m_pathes.Count(); i < iend; ++i)
        {
        }
    }
    void Reset()
    {
        //////////////////////////////////////////
        //////////////////////////////////////////
        //////////////////////////////////////////
        for(sint32 i = 0, iend = m_pathes.Count(); i < iend; ++i)
        {
        }
    }

private:
    Strings m_pathes;
};

namespace BOSS
{
    #if BOSS_NEED_EMBEDDED_ASSET
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
                    ID.Replace("-", "_MINUS_");
                    ID.Replace("+", "_PLUS_");

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
            if(auto GenFile = Platform::File::OpenForWrite(Platform::File::RootForAssets() + "../source/boss_gen_assets.hpp"))
            {
                String GenText;
                GenText += "#include <boss.hpp>\n";
                GenText += "\n";
                GenText += String::Format("#define BOSS_EMBEDDED_ASSET_COUNT Signed64(%lld)\n", info("data")("count").GetInt());
                GenText += String::Format("#define BOSS_EMBEDDED_ASSET_SIZE  Signed64(%lld)\n", info("data")("size").GetInt());
                GenText += String::Format("#define BOSS_EMBEDDED_ASSET_TIME  Signed64(%lld)\n", info("data")("time").GetInt());
                GenText += "#define BOSS_EMBEDDED_ASSET_PAGE  40000\n";
                GenText += "\n";
                // 스트링비우기
                Platform::File::Write(GenFile, (bytes)(chars) GenText, GenText.Length());
                GenText.Empty(true);

                // 파일내역
                sint32s PartCounts; // 파트정보
                for(sint32 i = 0, iend = info("files").LengthOfIndexable(); i < iend; ++i)
                {
                    auto& CurFile = info("files")[i];
                    GenText += String::Format("static bytes ASSETS_%s = (bytes) // %s (%lldbytes, modified at %lld)",
                        CurFile("id").GetString(), CurFile("path").GetString(), CurFile("size").GetInt(), CurFile("time").GetInt());

                    auto& CurPartCount = PartCounts.AtAdding();
                    CurPartCount = 0;
                    if(auto BinFile = Platform::File::OpenForRead(CurFile("path").GetString()))
                    {
                        if(Platform::File::Size(BinFile) == 0)
                            GenText += "\n    \"\"";
                        else
                        {
                            // 데이터 쓰기도구
                            sint32 WrittenSize = 0;
                            const sint32 LineSize = 40;
                            uint08 Data[LineSize];
                            char HexData[4 * LineSize];
                            for(sint32 j = 0; j < LineSize; ++j)
                            {
                                char* CurHexData = &HexData[4 * j];
                                CurHexData[0] = '\\';
                                CurHexData[1] = 'x';
                            }

                            // 데이터 쓰기
                            while(sint32 DataSize = Platform::File::Read(BinFile, Data, LineSize))
                            {
                                // 빠른 16진수 쓰기
                                GenText += "\n    \"";
                                for(sint32 j = 0; j < DataSize; ++j)
                                {
                                    const uint08& CurData = Data[j];
                                    const sint32 HiWord = (CurData & 0xF0) >> 4;
                                    const sint32 LoWord = (CurData & 0x0F);
                                    char* CurHexData = &HexData[4 * j];
                                    CurHexData[2] = (HiWord < 10)? '0' + HiWord : 'A' + (HiWord - 10);
                                    CurHexData[3] = (LoWord < 10)? '0' + LoWord : 'A' + (LoWord - 10);
                                }
                                GenText.AddTail(HexData, 4 * DataSize);
                                GenText += "\"";

                                // 컴파일러의 스트링길이 한계에 따른 스트링상수의 파트분리
                                if(BOSS_EMBEDDED_ASSET_PAGE < (WrittenSize += DataSize) + LineSize)
                                {
                                    CurPartCount++;
                                    WrittenSize = 0;
                                    GenText += ";\n";
                                    GenText += "\n";
                                    // 스트링비우기
                                    Platform::File::Write(GenFile, (bytes)(chars) GenText, GenText.Length());
                                    GenText.Empty(true);
                                    // 다음 파트의 시작
                                    GenText += String::Format("static bytes ASSETS_%s_PART%d = (bytes)",
                                        CurFile("id").GetString(), CurPartCount);
                                }
                            }
                        }
                        Platform::File::Close(BinFile);
                    }
                    else GenText += String::Format("\n\"-File Not Found- (%s)\"", CurFile("path").GetString());
                    GenText += ";\n";
                    GenText += "\n";
                    // 스트링비우기
                    Platform::File::Write(GenFile, (bytes)(chars) GenText, GenText.Length());
                    GenText.Empty(true);
                }

                // 파일인스턴스
                GenText += "struct EmbeddedFile {\n";
                GenText += "    String mPath;\n";
                GenText += "    buffer mBuffer;\n";
                GenText += "    bytes* mPages;\n";
                GenText += "    uint64 mSize;\n";
                GenText += "    uint64 mCTime;\n";
                GenText += "    uint64 mATime;\n";
                GenText += "    uint64 mMTime;\n";
                GenText += "    ~EmbeddedFile() {\n";
                GenText += "        Buffer::Free(mBuffer);\n";
                GenText += "        delete[] mPages;\n";
                GenText += "    }\n";
                GenText += "};\n";
                GenText += "\n";

                GenText += "static Map<EmbeddedFile> gEmbeddedCaches;\n";
                GenText += "static EmbeddedFile gSortedEmbeddedFiles[] = {\n";
                if(info("files").LengthOfIndexable() == 0)
                    GenText += "    {\"\", nullptr, nullptr, 0, 0, 0, 0}\n";
                else
                {
                    // 파일정렬
                    Map<sint32> Sortor;
                    const sint32 FileCount = info("files").LengthOfIndexable();
                    const sint32 AssetsPathLength = Platform::File::RootForAssets().Length();
                    for(sint32 i = 0; i < FileCount; ++i)
                    {
                        auto& CurFile = info("files")[i];
                        const String CurPath = String(CurFile("path").GetString() + AssetsPathLength).Lower();
                        Sortor(CurPath) = i;
                    }

                    // 페이로드 구성
                    struct Payload
                    {
                        const Context* mFiles;
                        const sint32s* mPartCounts;
                        String* mGenText;
                        sint32 mIndex;
                        sint32 mCount;
                    };
                    Payload OnePayload = {&info("files"), &PartCounts, &GenText, 0, FileCount};

                    // 정렬된 순서대로 기록
                    Sortor.AccessByCallback([](const MapPath* path, sint32* data, payload param)->void
                        {
                            // 파일정보
                            auto& CurPayload = *((Payload*) param);
                            auto& CurFile = (*CurPayload.mFiles)[*data];
                            uint64 CurSize = 0, CurCTime = 0, CurATime = 0, CurMTime = 0;
                            Platform::File::GetAttributes(WString::FromChars(CurFile("path").GetString()),
                                &CurSize, &CurCTime, &CurATime, &CurMTime);

                            // 파트정보
                            auto CurPartCount = (*CurPayload.mPartCounts)[CurPayload.mIndex];
                            String PageList = String::Format("ASSETS_%s", CurFile("id").GetString());
                            for(sint32 i = 0; i < CurPartCount; ++i)
                                PageList += String::Format(", ASSETS_%s_PART%d", CurFile("id").GetString(), i + 1);

                            // 구조체구성
                            *CurPayload.mGenText += String::Format("    {\"%s\", nullptr, new bytes[%d] {%s},\n",
                                &path->GetPath()[0], CurPartCount + 1, (chars) PageList);
                            *CurPayload.mGenText += String::Format("        Unsigned64(%llu), Unsigned64(%llu), Unsigned64(%llu), Unsigned64(%llu)}%s\n",
                                CurSize, CurCTime, CurATime, CurMTime, (CurPayload.mIndex < CurPayload.mCount - 1)? "," : "");
                            CurPayload.mIndex++;
                        }, (payload) &OnePayload);
                }
                GenText += "};\n";

                // 남은 스트링쓰기
                Platform::File::Write(GenFile, (bytes)(chars) GenText, GenText.Length());
                Platform::File::Close(GenFile);
            }
        }
    #endif

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

    bool Asset::Exist(chars filename, id_assetpath_read assetpath,
        uint64* size, uint64* ctime, uint64* atime, uint64* mtime)
    {
        if(assetpath)
            return ((const AssetPathClass*) assetpath)->Exist(filename, size, ctime, atime, mtime);
        #if BOSS_NEED_EMBEDDED_ASSET
            if(AssetEmbeddedReadFileClass::Exist(true, filename, size, ctime, atime, mtime))
                return true;
            if(AssetEmbeddedReadFileClass::Exist(false, filename, size, ctime, atime, mtime))
                return true;
        #else
            if(AssetReadFileClass::Exist(Platform::File::RootForAssetsRem() + filename, size, ctime, atime, mtime))
                return true;
            if(AssetReadFileClass::Exist(Platform::File::RootForAssets() + filename, size, ctime, atime, mtime))
                return true;
        #endif
        return false;
    }

    id_asset_read Asset::OpenForRead(chars filename, id_assetpath_read assetpath)
    {
        if(assetpath)
            return ((const AssetPathClass*) assetpath)->OpenForRead(filename);
        #if BOSS_NEED_EMBEDDED_ASSET
            if(auto Asset = AssetEmbeddedReadFileClass::Create(true, filename))
                return (id_asset_read) Asset;
            if(auto Asset = AssetEmbeddedReadFileClass::Create(false, filename))
                return (id_asset_read) Asset;
        #else
            if(auto Asset = AssetReadFileClass::Create(Platform::File::RootForAssetsRem() + filename))
                return (id_asset_read) Asset;
            if(auto Asset = AssetReadFileClass::Create(Platform::File::RootForAssets() + filename))
                return (id_asset_read) Asset;
        #endif
        return nullptr;
    }

    id_asset Asset::OpenForWrite(chars filename, bool autocreatedir)
    {
        #if BOSS_NEED_EMBEDDED_ASSET
            return (id_asset) AssetEmbeddedWriteFileClass::Create(filename);
        #else
            return (id_asset) AssetWriteFileClass::Create(
                Platform::File::RootForAssetsRem() + filename, autocreatedir);
        #endif
    }

    void Asset::Close(id_asset_read asset)
    {
        delete (AssetClass*) asset;
    }

    sint32 Asset::Size(id_asset_read asset)
    {
        if(auto CurAsset = (AssetClass*) asset)
            return CurAsset->Size();
        return 0;
    }

    sint32 Asset::Read(id_asset_read asset, uint08* data, const sint32 size)
    {
        if(auto CurAsset = (AssetClass*) asset)
            return CurAsset->Read(data, size);
        return 0;
    }

    sint32 Asset::Write(id_asset asset, bytes data, const sint32 size)
    {
        if(auto CurAsset = (AssetClass*) asset)
            return CurAsset->Write(data, size);
        return 0;
    }

    sint32 Asset::Skip(id_asset_read asset, const sint32 size)
    {
        if(auto CurAsset = (AssetClass*) asset)
            return CurAsset->Skip(size);
        return 0;
    }

    id_assetpath AssetPath::Create()
    {
        auto NewAssetPath = (AssetPathClass*) Buffer::Alloc<AssetPathClass>(BOSS_DBG 1);
        return (id_assetpath) NewAssetPath;
    }

    void AssetPath::AddRoot(id_assetpath assetpath)
    {
        if(auto CurAssetPath = (AssetPathClass*) assetpath)
            CurAssetPath->AddPath(nullptr);
    }

    void AssetPath::AddPath(id_assetpath assetpath, chars pathname)
    {
        if(auto CurAssetPath = (AssetPathClass*) assetpath)
            CurAssetPath->AddPath(pathname);
    }

    void AssetPath::Release(id_assetpath assetpath)
    {
        Buffer::Free((buffer) assetpath);
    }

    void AssetPath::Find(id_assetpath_read assetpath, FindFileCB filecb, FindPathCB pathcb)
    {
        if(auto CurAssetPath = (AssetPathClass*) assetpath)
            CurAssetPath->Find(filecb, pathcb);
    }

    void AssetPath::Reset(id_assetpath_read assetpath)
    {
        if(auto CurAssetPath = (AssetPathClass*) assetpath)
            CurAssetPath->Reset();
    }
}
