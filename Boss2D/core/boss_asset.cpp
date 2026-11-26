#include <boss.hpp>
#include "boss_asset.hpp"

#include <platform/boss_platform.hpp>
#ifndef BOSS_NEED_WEBFILES_ASSET
    #define BOSS_NEED_WEBFILES_ASSET 0
#endif

#if BOSS_NEED_EMBEDDED_ASSET
    #include <boss_assets.hpp>
#elif BOSS_NEED_EMBEDDED_CACHE
    struct EmbeddedFile
    {
        String mPath;
        buffer mBuffer = nullptr;
        bytes* mPages = nullptr;
        uint64 mSize = 0;
        uint64 mCTime = 0;
        uint64 mATime = 0;
        uint64 mMTime = 0;
        ~EmbeddedFile()
        {
            Buffer::Free(mBuffer);
            delete[] mPages;
        }
    };
    static Map<EmbeddedFile> gEmbeddedCaches;
#endif

#if BOSS_NEED_WEBFILES_ASSET || BOSS_STATIC_ASSET || BOSS_GENERATE_ASSET
    #include <cerrno>
    #include <cstdlib>
    #include <map>
    #include <mutex>
    #include <string>
    #include <vector>

    #if BOSS_WASM && !defined(__EMSCRIPTEN_PTHREADS__)
        struct AssetIndexMutex { void lock() {} void unlock() {} };
    #else
        typedef std::mutex AssetIndexMutex;
    #endif

    struct AssetIndexFileInfo
    {
        uint64 mSize = 0;
        uint64 mCTime = 0;
        uint64 mATime = 0;
        uint64 mMTime = 0;
    };
    typedef std::map<std::string, AssetIndexFileInfo> AssetIndexFiles;
    static std::string gIndexFilename = "assetsindex.json";
    static std::string gIndexVersion = "Boss2D-AssetsIndex-1";
    static std::string gDeckFilename;
    static bool LegacyIndex(const std::string& version) {return version == "Boss2D-AssetsIndex-1";}

    // 파일명은 assets 기준 상대경로이며, 웹에서 사용할 때는 URL 인코딩한다.
    static bool AssetIndexValidPath(const std::string& path)
    {
        if(path.empty() || path[0] == '/' || path.find('\\') != std::string::npos ||
            path.find(':') != std::string::npos || path == "assetsindex.json" || path == gIndexFilename) return false;
        for(size_t i = 0; i < path.length();)
        {
            const size_t End = path.find('/', i);
            const std::string Part = path.substr(i, End == std::string::npos? End : End - i);
            if(Part.empty() || Part == "." || Part == "..") return false;
            if(End == std::string::npos) break;
            i = End + 1;
            if(i == path.length()) return false;
        }
        return true;
    }

    // assets:/, 하위 폴더, '..' 처리를 공통의 상대경로 규칙으로 변환한다.
    static std::string AssetIndexNormalizePath(chars pathname, bool directory = false, bool* valid = nullptr)
    {
        if(valid) *valid = true;
        std::string Path = pathname ? pathname : "";
        for(auto& Ch : Path) if(Ch == '\\') Ch = '/';
        if(Path.compare(0, 8, "assets:/") == 0) Path.erase(0, 8);
        if((!Path.empty() && Path[0] == '/') || Path.find(':') != std::string::npos)
        {
            if(valid) *valid = false;
            return "";
        }
        std::vector<std::string> Parts;
        for(size_t Pos = 0; Pos < Path.size();)
        {
            const auto End = Path.find('/', Pos);
            const auto Part = Path.substr(Pos, (End == std::string::npos)? End : End - Pos);
            if(Part == "..")
            {
                if(Parts.empty())
                {
                    if(valid) *valid = false;
                    return "";
                }
                Parts.pop_back();
            }
            else if(!Part.empty() && Part != ".") Parts.push_back(Part);
            if(End == std::string::npos) break;
            Pos = End + 1;
        }
        Path.clear();
        for(const auto& Part : Parts)
        {
            if(!Path.empty()) Path += '/';
            Path += Part;
        }
        if(directory && !Path.empty()) Path += '/';
        return Path;
    }

    static bool AssetIndexToUInt64(chars text, uint64& value)
    {
        if(!text || !*text || *text < '0' || '9' < *text) return false;
        errno = 0;
        char* End = nullptr;
        const unsigned long long Parsed = std::strtoull(text, &End, 10);
        if(errno == ERANGE || !End || *End) return false;
        value = (uint64) Parsed;
        return true;
    }

    static bool AssetIndexParse(const char* data, size_t size, AssetIndexFiles& result, const std::string& version = gIndexVersion)
    {
        result.clear();
        if(!data || !size || size > 16 * 1024 * 1024) return false;
        const size_t Bom = (size >= 3 && (uint08)data[0] == 0xEF &&
            (uint08)data[1] == 0xBB && (uint08)data[2] == 0xBF)? 3 : 0;
        std::string Text(data + Bom, size - Bom);
        Text.push_back('\0');
        Context Index;
        Index.LoadJson(SO_OnlyReference, Text.c_str());
        const bool Legacy = LegacyIndex(version);
        if(std::string((chars) Index(Legacy? "format" : "version").GetText()) != version) return false;
        const auto& Files = Index(Legacy? "files" : "list");
        if(Files.LengthOfIndexable() <= 0) return false;
        for(sint32 i = 0, iend = Files.LengthOfIndexable(); i < iend; ++i)
        {
            const auto& Item = Files[i];
            const std::string Path = (chars) Item("path").GetText();
            AssetIndexFileInfo Info;
            if(!AssetIndexValidPath(Path) ||
                !AssetIndexToUInt64(Item("size").GetText(), Info.mSize) ||
                !AssetIndexToUInt64(Item("ctime").GetText(), Info.mCTime) ||
                !AssetIndexToUInt64(Item("atime").GetText(), Info.mATime) ||
                !AssetIndexToUInt64(Item("mtime").GetText(), Info.mMTime) ||
                !result.emplace(Path, Info).second)
            {
                result.clear();
                return false;
            }
        }
        return true;
    }

    static String AssetIndexBuild(const AssetIndexFiles& files, const std::string& version)
    {
        Context Index;
        const bool Legacy = LegacyIndex(version);
        Index.At(Legacy? "format" : "version").Set(version.c_str());
        for(const auto& Pair : files)
        {
            auto& File = Index.At(Legacy? "files" : "list").AtAdding();
            File.At("path").Set(Pair.first.c_str());
            File.At("size").Set(std::to_string(Pair.second.mSize).c_str());
            File.At("ctime").Set(std::to_string(Pair.second.mCTime).c_str());
            File.At("atime").Set(std::to_string(Pair.second.mATime).c_str());
            File.At("mtime").Set(std::to_string(Pair.second.mMTime).c_str());
        }
        return Index.SaveJson();
    }
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

#if BOSS_NEED_WEBFILES_ASSET
    #include <chrono>
    #include <memory>

    #if BOSS_WASM
        #include <emscripten.h>

        EM_JS(char*, BossWebFiles_ResolveUrl, (const char* base, const char* path), {
            try {
                const page = typeof window !== 'undefined' ? window.location.href : undefined;
                const value = UTF8ToString(base);
                if(!value && !page) return 0;
                const root = new URL(value || './', page);
                if(root.protocol !== 'http:' && root.protocol !== 'https:') return 0;
                root.search = '';
                root.hash = '';
                if(!root.pathname.endsWith('/')) root.pathname += '/';
                const url = new URL(UTF8ToString(path), root).href;
                const length = lengthBytesUTF8(url) + 1;
                const result = _malloc(length);
                if(result) stringToUTF8(url, result, length);
                return result;
            } catch(error) {
                console.warn('BOSS web asset URL:', error);
                return 0;
            }
        });

        // 브라우저 메인스레드에서도 호출 스택을 중단하지 않는 동기식 다운로드.
        // 텍스트는 UTF-8+BOM을 유지하고 그 외 파일은 x-user-defined로 바이너리를 복원한다.
        // 동기 XHR은 다운로드 중 브라우저 UI를 차단한다.
        EM_JS(void*, BossWebFiles_FetchSync, (const char* url, int* size, int* status), {
            try {
                const path = UTF8ToString(url);
                const isUtf8Text = /\.(zay|json)(?:[?#]|$)/i.test(path);
                const request = new XMLHttpRequest();
                request.open('GET', path, false);
                request.overrideMimeType(isUtf8Text ?
                    'text/plain; charset=utf-8' : 'text/plain; charset=x-user-defined');
                request.send(null);
                HEAP32[status >> 2] = request.status;
                if(request.status < 200 || request.status >= 300 || request.status === 206)
                    return 0;

                const content = request.responseText;
                let data;
                if(isUtf8Text) {
                    // UTF-8 텍스트는 반드시 BOM을 포함한 바이트로 반환한다.
                    // XHR은 BOM을 제거할 수 있으므로 재인코딩 후 복원한다.
                    const encoded = new TextEncoder().encode(content);
                    const hasBom = encoded.length >= 3 && encoded[0] === 0xEF &&
                        encoded[1] === 0xBB && encoded[2] === 0xBF;
                    if(hasBom) data = encoded;
                    else {
                        data = new Uint8Array(encoded.length + 3);
                        data.set([0xEF, 0xBB, 0xBF]);
                        data.set(encoded, 3);
                    }
                }
                else {
                    // 이미지/음원/폰트 등의 임의 바이트는 문자 디코딩하지 않는다.
                    data = new Uint8Array(content.length);
                    for(let i = 0, len = content.length; i < len; ++i)
                        data[i] = content.charCodeAt(i) & 0xFF;
                }
                if(data.length > 0x7fffffff) return 0;
                const result = _malloc(Math.max(1, data.length));
                if(!result) return 0;
                HEAPU8.set(data, result);
                HEAP32[size >> 2] = data.length;
                return result;
            } catch(error) {
                console.warn('BOSS sync web asset download:', error);
                return 0;
            }
        });
    #elif defined(BOSS_PLATFORM_QT5) || defined(BOSS_PLATFORM_QT6)
        #include <QByteArray>
        #include <QEventLoop>
        #include <QNetworkAccessManager>
        #include <QNetworkReply>
        #include <QNetworkRequest>
        #include <QCryptographicHash>
        #include <QDir>
        #include <QFile>
        #include <QFileInfo>
        #include <QSaveFile>
        #include <QJsonDocument>
        #include <QJsonObject>
        #include <QJsonArray>
        #include <QSet>
        #include <QThread>
        #include <QTimer>
        #include <QUrl>
    #else
        #error BOSS_NEED_WEBFILES_ASSET requires Emscripten or Qt5/Qt6
    #endif

    struct AssetWebFileData
    {
        AssetWebFileData() : mData(nullptr), mSize(0) {}
        ~AssetWebFileData() {std::free(mData);}
        std::string mPath;
        uint08* mData;
        sint32 mSize;
    };
    typedef std::shared_ptr<const AssetWebFileData> AssetWebFilePtr;

    typedef AssetIndexMutex AssetWebMutex;

    struct AssetWebFilesState
    {
        AssetWebFilesState() : mGeneration(0) {}
        AssetWebMutex mMutex;
        std::string mUrl;
        uint64 mGeneration;
        AssetIndexFiles mIndex;
        bool mIndexReady = false;
        std::string mDeck;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // AssetWebReadFileClass
    class AssetWebReadFileClass : public AssetClass
    {
    public:
        AssetWebReadFileClass(AssetWebFilePtr file) : mFile(file), mPos(0) {}
        ~AssetWebReadFileClass() override {}

    public:
        static void SetUrl(chars url)
        {
            std::string NewUrl = url ? url : "";
            const auto First = NewUrl.find_first_not_of(" \t\r\n");
            if(First == std::string::npos) NewUrl.clear();
            else NewUrl = NewUrl.substr(First, NewUrl.find_last_not_of(" \t\r\n") - First + 1);
            const auto Suffix = NewUrl.find_first_of("?#");
            if(Suffix != std::string::npos) NewUrl.erase(Suffix);
            if(!NewUrl.empty() && NewUrl.back() != '/') NewUrl += '/';

            auto& State = ST();
            std::lock_guard<AssetWebMutex> Lock(State.mMutex);
            State.mUrl = NewUrl;
            ++State.mGeneration;
            State.mIndex.clear();
            State.mIndexReady = false;
            State.mDeck.clear();
        }
        static bool Enabled()
        {
            #if BOSS_WASM
                return true;
            #else
                auto& State = ST();
                std::lock_guard<AssetWebMutex> Lock(State.mMutex);
                return Scoped() || !State.mUrl.empty();
            #endif
        }
        static std::string Url()
        {
            auto& State = ST();
            std::lock_guard<AssetWebMutex> Lock(State.mMutex);
            return State.mUrl;
        }
        static bool Scoped() {return !gDeckFilename.empty();}
        static bool GeneratedPath(const std::string& path)
        {
            if(path.size() < 65 || path[64] != '/') return false;
            for(size_t i = 0; i < 64; ++i)
                if(!('0' <= path[i] && path[i] <= '9') && !('a' <= path[i] && path[i] <= 'f')) return false;
            return true;
        }
        static bool Routed(chars pathname)
        {
            if(!Enabled()) return false;
            #if BOSS_WASM
                return true; // Existing WASM runtime serves all assets from its page.
            #else
                if(!Scoped()) return true;
                bool Valid = false;
                const std::string Path = AssetIndexNormalizePath(pathname, true, &Valid);
                if(!Valid) return true;
                // Main and generated cache are never controlled by a remote deck.
                if(Path.compare(0,5,"main/") == 0 || GeneratedPath(Path)) return false;
                if(Path == gDeckFilename + "/" || Path == gIndexFilename + "/") return true;
                return true; // No fallback to another site's app resources, including custom directories.
            #endif
        }
        static bool LocalOriginal(chars pathname)
        {
            #if !BOSS_WASM
                if(Scoped())
                {
                    bool Valid = false;
                    const std::string Path = AssetIndexNormalizePath(pathname, true, &Valid);
                    return Valid && Path.compare(0, 5, "main/") == 0;
                }
            #endif
            return true;
        }
        static String CachePrefix()
        {
            #if !BOSS_WASM
                const auto Base = Url();
                if(Scoped() && !Base.empty())
                    return String((Hash(Base) + "/").c_str());
            #endif
            return String();
        }
        static String UserPath(chars pathname)
        {
            #if !BOSS_WASM
                if(Scoped() && Routed(pathname))
                {
                    bool Valid = false;
                    const std::string Path = AssetIndexNormalizePath(pathname, false, &Valid);
                    if(!Valid || Url().empty()) return String();
                    return CachePrefix() + ".user/" + Path.c_str();
                }
            #endif
            return String(pathname);
        }
        static bool Listed(chars pathname)
        {
            if(Scoped() && (AssetIndexNormalizePath(pathname)==gDeckFilename || AssetIndexNormalizePath(pathname)==gIndexFilename)) return true;
            AssetIndexFileInfo Info; return FindInfo(pathname, Info);
        }
        static bool TryUrl(chars url, String* deckjson)
        {
            #if BOSS_WASM
                // The browser-hosted shell has no native URL switch. Keep its own origin.
                (void) url; (void) deckjson; return false;
            #else
                if(!Scoped()) return false;
                QUrl Root(QString::fromUtf8(url?url:"").trimmed(), QUrl::StrictMode);
                if(!Root.isValid() || Root.host().isEmpty() || Root.isRelative() || !Root.userInfo().isEmpty() ||
                    (Root.scheme() != "http" && Root.scheme() != "https")) return false;
                Root = Root.adjusted(QUrl::NormalizePathSegments | QUrl::RemoveQuery | QUrl::RemoveFragment);
                if((Root.scheme()=="https" && Root.port()==443) || (Root.scheme()=="http" && Root.port()==80)) Root.setPort(-1);
                if(!Root.path(QUrl::FullyEncoded).endsWith('/')) Root.setPath(Root.path(QUrl::FullyEncoded)+'/',QUrl::StrictMode);
                const std::string Base = Root.toEncoded().toStdString();
                AssetIndexFiles Index; AssetWebFileData Manifest, Deck; sint32 Status = 0;
                if(!FetchFresh(Base,gIndexFilename,Manifest,Status) || Manifest.mSize>16*1024*1024) return false;
                QByteArray ManifestBytes((const char*)Manifest.mData,Manifest.mSize);
                if(ManifestBytes.startsWith("\xEF\xBB\xBF")) ManifestBytes.remove(0,3);
                QJsonParseError IndexError;
                const QJsonDocument IndexDoc=QJsonDocument::fromJson(ManifestBytes,&IndexError);
                if(IndexError.error!=QJsonParseError::NoError || !IndexDoc.isObject() ||
                    !AssetIndexParse((const char*)Manifest.mData,Manifest.mSize,Index)) return false;
                if(!FetchFresh(Base,gDeckFilename,Deck,Status) || Deck.mSize > 1024*1024) return false;
                QSet<QString> FileNames;
                for(const auto& Pair : Index)
                {
                    if(!SafeDiskPath(Pair.first)) return false;
                    const QString Key=QString::fromStdString(Pair.first).toCaseFolded();
                    if(FileNames.contains(Key)) return false;
                    FileNames.insert(Key);
                }
                QByteArray DeckBytes((const char*)Deck.mData,Deck.mSize);
                QByteArray JsonBytes = DeckBytes;
                if(JsonBytes.startsWith("\xEF\xBB\xBF")) JsonBytes.remove(0,3);
                QJsonParseError Error;
                const QJsonDocument Doc = QJsonDocument::fromJson(JsonBytes,&Error);
                if(Error.error != QJsonParseError::NoError || !Doc.isObject()) return false;
                const auto Obj = Doc.object();
                if(Obj.value("version").toString() != "zayapp_deck_v1" || !Obj.value("list").isArray()) return false;
                QSet<QString> Seen;
                for(const auto Entry : Obj.value("list").toArray())
                {
                    if(!Entry.isObject()) return false;
                    const auto App = Entry.toObject();
                    const QString Id = App.value("dom").toString(), Script = App.value("script").toString();
                    if(Id.isEmpty() || Id == "main" || Id == "home" || Id == "settings" || Id == "data" ||
                        Id == "deck" || Id == "program" || Id == "main_browser" || Id == "main_settings_save" || Seen.contains(Id) ||
                        !SafeAppName(Id) || !Script.endsWith(".zay") || !SafeAppName(Script.left(Script.size()-4)) || Script == "main.zay" ||
                        App.value("title").toString().isEmpty()) return false;
                    Seen.insert(Id);
                    const std::string ScriptPath = "widget/" + Script.toStdString();
                    const std::string JsonPath = ScriptPath.substr(0,ScriptPath.size()-4)+".json";
                    if(!Index.count(ScriptPath) || !Index.count(JsonPath)) return false;
                }
                // Commit only after both manifests are valid. Never reuse a previous site's deck on failure.
                if(!SaveManifestPair(CacheFolder(Base), Manifest, DeckBytes)) return false;
                auto& State = ST();
                {std::lock_guard<AssetWebMutex> Lock(State.mMutex);
                    State.mUrl = Base; ++State.mGeneration; State.mIndex.swap(Index);
                    State.mIndexReady = true; State.mDeck.assign(JsonBytes.constData(),JsonBytes.size());}
                if(deckjson) *deckjson = String(JsonBytes.constData(),JsonBytes.size());
                return true;
            #endif
        }
        static String NativePath(chars pathname)
        {
            #if !BOSS_WASM
                bool Valid=false;
                const std::string Path=AssetIndexNormalizePath(pathname,false,&Valid);
                if(!Valid || Url().empty() || !EnsureIndex()) return String();
                if(Scoped() && (Path==gDeckFilename || Path==gIndexFilename))
                {
                    const QString Name=CacheFolder(Url())+QString::fromStdString(Path);
                    return QFileInfo(Name).isFile()? String(Name.toUtf8().constData()) : String();
                }
                AssetIndexFileInfo Info; std::string Base;
                {auto& State=ST(); std::lock_guard<AssetWebMutex> Lock(State.mMutex);
                    auto It=State.mIndex.find(Path); if(It==State.mIndex.end()) return String();
                    Info=It->second; Base=State.mUrl;}
                return String(CacheFile(Base,Path,Info).toUtf8().constData());
            #else
                (void) pathname; return String();
            #endif
        }
        class DiskRead : public AssetClass
        {
        public:
            explicit DiskRead(decltype(Platform::File::OpenForRead("")) f) : mFile(f) {}
            ~DiskRead() override {Platform::File::Close(mFile);}
            sint32 Size() override {return Platform::File::Size(mFile);}
            sint32 Read(uint08* data, sint32 size) override {return Platform::File::Read(mFile,data,size);}
            sint32 Write(bytes, sint32) override {return 0;}
            sint32 Skip(sint32 size) override {Platform::File::Seek(mFile,Platform::File::Focus(mFile)+size);return Platform::File::Focus(mFile);}
        private: decltype(Platform::File::OpenForRead("")) mFile;
        };
        static AssetClass* Create(chars pathname)
        {
            #if !BOSS_WASM
                if(Scoped())
                {
                    const String Path = NativePath(pathname);
                    if(Path.Length()) if(auto File = Platform::File::OpenForRead(Path)) return new DiskRead(File);
                    return nullptr;
                }
            #endif
            if(auto File = Match(pathname))
                return new AssetWebReadFileClass(File);
            return nullptr;
        }
        static bool Exist(chars pathname, uint64* size, uint64* ctime, uint64* atime, uint64* mtime)
        {
            #if !BOSS_WASM
                if(Scoped() && (AssetIndexNormalizePath(pathname)==gDeckFilename || AssetIndexNormalizePath(pathname)==gIndexFilename))
                {
                    const String Path=NativePath(pathname);
                    if(!Path.Length()) return false;
                    const sint32 Attr=Platform::File::GetAttributes(WString::FromChars(Path),size,ctime,atime,mtime);
                    return Attr!=-1 && (Attr&0x10)==0;
                }
            #endif
            if(!gDeckFilename.empty() && AssetIndexNormalizePath(pathname)==gDeckFilename)
            {
                if(auto File = Match(pathname)) {if(size) *size=File->mSize; if(ctime) *ctime=0; if(atime) *atime=0; if(mtime) *mtime=0; return true;}
                return false;
            }
            AssetIndexFileInfo Info;
            if(!FindInfo(pathname, Info)) return false;
            if(size) *size = Info.mSize;
            if(ctime) *ctime = Info.mCTime;
            if(atime) *atime = Info.mATime;
            if(mtime) *mtime = Info.mMTime;
            return true;
        }
        static void Search(chars pathname, Platform::File::SearchCB cb, payload data)
        {
            if(!cb) return;
            bool Valid = false;
            const std::string Path = AssetIndexNormalizePath(pathname, true, &Valid);
            if(!Valid) return;
            if(!EnsureIndex()) return;
            std::vector<std::string> Names;
            {
                auto& State = ST();
                std::lock_guard<AssetWebMutex> Lock(State.mMutex);
                for(const auto& Item : State.mIndex)
                {
                    const std::string& Name = Item.first;
                    if(Name.compare(0, Path.size(), Path) == 0 &&
                        Name.find('/', Path.size()) == std::string::npos)
                        Names.push_back(Name.substr(Path.size()));
                }
            }
            for(const auto& Name : Names)
                cb(Name.c_str(), data);
        }

    public:
        sint32 Size() override {return mFile->mSize;}
        sint32 Read(uint08* data, const sint32 size) override
        {
            if(!data || size <= 0) return 0;
            const sint32 ReadSize = Math::Min(size, mFile->mSize - mPos);
            if(0 < ReadSize)
            {
                Memory::Copy(data, mFile->mData + mPos, ReadSize);
                mPos += ReadSize;
            }
            return ReadSize;
        }
        sint32 Write(bytes data, const sint32 size) override {return 0;}
        sint32 Skip(const sint32 size) override
        {
            const sint64 NewPos = (sint64) mPos + size;
            mPos = (NewPos < 0)? 0 : ((mFile->mSize < NewPos)? mFile->mSize : (sint32) NewPos);
            return mPos;
        }

    private:
        static AssetWebFilesState& ST()
        {
            static AssetWebFilesState State;
            return State;
        }
        // 웹 모드에서 인덱스만 1회 다운로드한다. 존재검사는 통신하지 않는다.
        static bool FetchFresh(const std::string& base,const std::string& path,AssetWebFileData& file,sint32& status)
        {
            const auto Url = ResolveUrl(base,path);
            if(Url.empty()) return false;
            const auto Nonce = std::chrono::system_clock::now().time_since_epoch().count();
            return Download(Url + "?v=" + std::to_string(Nonce),file,status);
        }
        #if !BOSS_WASM
        static std::string Hash(const std::string& s)
        {return QCryptographicHash::hash(QByteArray(s.data(),int(s.size())),QCryptographicHash::Sha256).toHex().toStdString();}
        static QString CacheFolder(const std::string& base)
        {return QString::fromUtf8((chars)Platform::File::RootForAssetsRem()) + QString::fromStdString(Hash(base)) + "/";}
        static bool SafeAppName(const QString& s)
        {
            if(s.isEmpty() || s.size()>128) return false;
            for(const auto c : s) if(!((c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='_')) return false;
            return true;
        }
        static bool SafeDiskPath(const std::string& path)
        {
            if(!AssetIndexValidPath(path)) return false;
            for(const auto& Part : QString::fromStdString(path).split('/'))
            {
                if(Part.endsWith('.') || Part.endsWith(' ') || Part.contains(QChar(0)) ||
                    Part.contains('"') || Part.contains('<') || Part.contains('>') || Part.contains('|') ||
                    Part.contains('?') || Part.contains('*')) return false;
                for(auto c : Part) if(c.unicode() < 32) return false;
                const auto Stem = Part.section('.',0,0).toUpper();
                if(Stem=="CON"||Stem=="PRN"||Stem=="AUX"||Stem=="NUL"||
                    (Stem.size()==4 && (Stem.startsWith("COM")||Stem.startsWith("LPT")) && Stem[3].isDigit())) return false;
            }
            return true;
        }
        static bool SaveAtomic(const QString& name,const char* data,qint64 size)
        {
            if(!QDir().mkpath(QFileInfo(name).absolutePath())) return false;
            QSaveFile File(name);
            if(!File.open(QIODevice::WriteOnly) || File.write(data,size)!=size) return false;
            return File.commit();
        }
        static bool SaveManifestPair(const QString& root, const AssetWebFileData& index, const QByteArray& deck)
        {
            const QString IndexName = root + QString::fromStdString(gIndexFilename);
            const QString DeckName = root + QString::fromStdString(gDeckFilename);
            QFile PreviousIndex(IndexName);
            const bool HadIndex = PreviousIndex.exists();
            QByteArray PreviousBytes;
            if(HadIndex)
            {
                if(!PreviousIndex.open(QIODevice::ReadOnly)) return false;
                PreviousBytes = PreviousIndex.readAll();
                if(PreviousIndex.error() != QFileDevice::NoError) return false;
                PreviousIndex.close();
            }
            if(!SaveAtomic(IndexName, (const char*) index.mData, index.mSize)) return false;
            if(!SaveAtomic(DeckName, deck.constData(), deck.size()))
            {
                // 각각의 파일은 원자적으로 저장하며, 두 번째 저장 실패시 이전 인덱스를 복원한다.
                const bool Restored = HadIndex? SaveAtomic(IndexName, PreviousBytes.constData(), PreviousBytes.size()) : QFile::remove(IndexName);
                if(!Restored) BOSS_TRACE("Assets index rollback failed: %s", IndexName.toUtf8().constData());
                return false;
            }
            return true;
        }
        static QString CacheFile(const std::string& base,const std::string& path,const AssetIndexFileInfo& info)
        {
            if(base.empty() || !SafeDiskPath(path)) return QString();
            static AssetWebMutex CacheMutex;
            std::lock_guard<AssetWebMutex> Guard(CacheMutex);
            const QString Root = CacheFolder(base);
            const QString Name = Root + QString::fromStdString(path);
            const QString Meta = Root + ".cache/" + QString::fromStdString(Hash(path)) + ".stamp";
            const QByteArray Stamp = QByteArray::number(qulonglong(info.mSize)) + ':' +
                QByteArray::number(qulonglong(info.mMTime)) + ':' + QByteArray::number(qulonglong(info.mCTime));
            const QFileInfo CachedFile(Name);
            QFile StampFile(Meta);
            if(CachedFile.isFile() && StampFile.open(QIODevice::ReadOnly))
            {
                const QByteArray SavedStamp = StampFile.readAll();
                // 인덱스 서명과 실제 저장길이를 함께 확인하고, 기존 3항목 스탬프도 호환한다.
                if(SavedStamp == Stamp + ':' + QByteArray::number(CachedFile.size()) ||
                    (SavedStamp == Stamp && uint64(CachedFile.size()) == info.mSize)) return Name;
            }
            StampFile.close();
            AssetWebFileData File; sint32 Status=0;
            const auto Url = ResolveUrl(base,path);
            if(Url.empty() || !Download(Url + "?v=" + std::to_string(info.mMTime) + "_" + std::to_string(info.mCTime),File,Status)) return QString();
            if(uint64(File.mSize)!=info.mSize)
            {
                BOSS_TRACE("[AssetWeb] Index size differs: %s (index=%llu, downloaded=%d). Using downloaded bytes.",
                    path.c_str(), (unsigned long long) info.mSize, File.mSize);
            }
            // 줄바꿈 변환 등으로 크기가 달라도 받은 파일을 사용하며, 다음에는 실제 길이로 캐시를 검증한다.
            const QByteArray SavedStamp = Stamp + ':' + QByteArray::number(File.mSize);
            if(!SaveAtomic(Name,(const char*)File.mData,File.mSize) || !SaveAtomic(Meta,SavedStamp.constData(),SavedStamp.size())) return QString();
            return Name;
        }
        #endif
        static bool EnsureIndex()
        {
            static AssetWebMutex LoadMutex;
            std::lock_guard<AssetWebMutex> OneLoad(LoadMutex);
            auto& State = ST();
            std::string Base;
            uint64 Generation = 0;
            {
                std::lock_guard<AssetWebMutex> Lock(State.mMutex);
                if(State.mIndexReady) return true;
                Base = State.mUrl;
                Generation = State.mGeneration;
            }

            std::string Url = ResolveUrl(Base, gIndexFilename);
            AssetIndexFiles NewIndex;
            bool Success = false;
            sint32 Status = 0;
            if(!Url.empty())
            {
                // Github Pages의 기존 600초 캐시와 관계없이 새 배포 목록을 확인한다.
                const auto Nonce = std::chrono::system_clock::now().time_since_epoch().count();
                Url += "?v=" + std::to_string(Nonce);
                AssetWebFileData Manifest;
                if(Download(Url, Manifest, Status))
                    Success = AssetIndexParse((const char*) Manifest.mData, Manifest.mSize, NewIndex);
            }
            {
                std::lock_guard<AssetWebMutex> Lock(State.mMutex);
                if(Generation != State.mGeneration) return false;
                if(Success)
                {
                    State.mIndex.swap(NewIndex);
                    State.mIndexReady = true;
                }
            }
            if(!Success)
                BOSS_TRACE("Assets index load failed (%d): %s", Status, Url.c_str());
            return Success;
        }
        static bool FindInfo(chars pathname, AssetIndexFileInfo& info)
        {
            bool Valid = false;
            const std::string Path = AssetIndexNormalizePath(pathname, false, &Valid);
            if(!Valid || !AssetIndexValidPath(Path) || !EnsureIndex()) return false;
            auto& State = ST();
            std::lock_guard<AssetWebMutex> Lock(State.mMutex);
            const auto Found = State.mIndex.find(Path);
            if(Found == State.mIndex.end()) return false;
            info = Found->second;
            return true;
        }
        static std::string EncodePath(const std::string& path)
        {
            static const char Hex[] = "0123456789ABCDEF";
            std::string Result;
            for(const unsigned char Ch : path)
            {
                if(('a' <= Ch && Ch <= 'z') || ('A' <= Ch && Ch <= 'Z') ||
                    ('0' <= Ch && Ch <= '9') || Ch == '/' || Ch == '-' || Ch == '_' || Ch == '.' || Ch == '~')
                    Result += (char) Ch;
                else
                {
                    Result += '%';
                    Result += Hex[Ch >> 4];
                    Result += Hex[Ch & 15];
                }
            }
            return Result;
        }
        static std::string ResolveUrl(const std::string& base, const std::string& path)
        {
            const std::string Encoded = EncodePath(path);
            #if BOSS_WASM
                if(char* Url = BossWebFiles_ResolveUrl(base.c_str(), Encoded.c_str()))
                {
                    const std::string Result = Url;
                    std::free(Url);
                    return Result;
                }
                return "";
            #else
                const QUrl Root(QString::fromUtf8(base.c_str()));
                if(!Root.isValid() || Root.isRelative() || Root.host().isEmpty() ||
                    (Root.scheme() != "http" && Root.scheme() != "https")) return "";
                return Root.resolved(QUrl::fromEncoded(QByteArray(Encoded.c_str()))).toEncoded().toStdString();
            #endif
        }
        static bool Download(const std::string& url, AssetWebFileData& file, sint32& status)
        {
            #if BOSS_WASM
                file.mData = (uint08*) BossWebFiles_FetchSync(url.c_str(), &file.mSize, &status);
            #else
                auto Worker = QThread::create([&]()
                {
                    QNetworkAccessManager Manager;
                    QNetworkRequest Request(QUrl::fromEncoded(QByteArray(url.c_str())));
                    Request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
                    Request.setMaximumRedirectsAllowed(5);
                    QEventLoop Loop;
                    QTimer Timer;
                    Timer.setSingleShot(true);
                    QNetworkReply* Reply = Manager.get(Request);
                    QObject::connect(Reply, &QNetworkReply::finished, &Loop, &QEventLoop::quit);
                    QObject::connect(&Timer, &QTimer::timeout, Reply, &QNetworkReply::abort);
                    Timer.start(30000);
                    if(!Reply->isFinished()) Loop.exec();
                    Timer.stop();
                    status = Reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                    if(Reply->error() == QNetworkReply::NoError && 200 <= status && status < 300 && status != 206)
                    {
                        const QByteArray Data = Reply->readAll();
                        if(Data.size() <= 0x7fffffff)
                        {
                            file.mSize = (sint32) Data.size();
                            file.mData = (uint08*) std::malloc(file.mSize ? file.mSize : 1);
                            if(file.mData && 0 < file.mSize)
                                Memory::Copy(file.mData, Data.constData(), file.mSize);
                        }
                    }
                    if(!file.mData)
                        BOSS_TRACE("[AssetWeb] Download failed: HTTP=%d, Qt=%d (%s), URL=%s",
                            status, sint32(Reply->error()), Reply->errorString().toUtf8().constData(), url.c_str());
                    delete Reply;
                });
                Worker->start();
                Worker->wait();
                delete Worker;
            #endif
            return file.mData != nullptr;
        }
        static AssetWebFilePtr Match(chars pathname)
        {
            if(!gDeckFilename.empty() && AssetIndexNormalizePath(pathname)==gDeckFilename)
            {
                auto File = std::make_shared<AssetWebFileData>();
                const auto Base = Url();
                #if !BOSS_WASM
                    {auto& State=ST(); std::lock_guard<AssetWebMutex> Lock(State.mMutex);
                     if(!State.mDeck.empty()) {File->mSize=sint32(State.mDeck.size()); File->mData=(uint08*)std::malloc(File->mSize);
                         if(!File->mData) return nullptr; Memory::Copy(File->mData,State.mDeck.data(),File->mSize); return File;}}
                #endif
                sint32 Status=0;
                if(FetchFresh(Base,gDeckFilename,*File,Status)) return File;
                return nullptr;
            }
            AssetIndexFileInfo Info;
            if(!FindInfo(pathname, Info)) return nullptr;
            const std::string Path = AssetIndexNormalizePath(pathname);
            auto& State = ST();
            std::string Base;
            {
                std::lock_guard<AssetWebMutex> Lock(State.mMutex);
                Base = State.mUrl;
            }
            const std::string Url = ResolveUrl(Base, Path);
            if(Url.empty()) return nullptr;

            // 존재여부는 인덱스로 판단한다. 다운로드 성공/실패 결과는 재사용하지 않는다.
            auto File = std::make_shared<AssetWebFileData>();
            File->mPath = Path;
            sint32 Status = 0;
            if(!Download(Url, *File, Status))
            {
                BOSS_TRACE("Web asset listed in index but load failed (%d): %s", Status, Url.c_str());
                return nullptr;
            }
            return File;
        }

    private:
        AssetWebFilePtr mFile;
        sint32 mPos;
    };
#endif


#if BOSS_STATIC_ASSET
    ////////////////////////////////////////////////////////////////////////////////
    // AssetLocalIndex: BOSS_STATIC_ASSET의 로컬 원본 파일 목록.
    // 다운로드나 파일 존재 결과를 캐싱하지 않고, 유효한 목록만 메모리에 유지한다.
    class AssetLocalIndex
    {
    public:
        static bool Exist(chars pathname, uint64* size, uint64* ctime, uint64* atime, uint64* mtime)
        {
            AssetIndexFileInfo Info;
            if(!Find(pathname, Info)) return false;
            if(size) *size = Info.mSize;
            if(ctime) *ctime = Info.mCTime;
            if(atime) *atime = Info.mATime;
            if(mtime) *mtime = Info.mMTime;
            return true;
        }
        static bool Has(chars pathname)
        {
            AssetIndexFileInfo Info;
            return Find(pathname, Info);
        }
        static void Search(chars pathname, Platform::File::SearchCB cb, payload data)
        {
            if(!cb) return;
            bool Valid = false;
            const std::string Path = AssetIndexNormalizePath(pathname, true, &Valid);
            if(!Valid || !Ensure()) return;
            std::vector<std::string> Names;
            {
                auto& State = ST();
                std::lock_guard<AssetIndexMutex> Lock(State.mMutex);
                for(const auto& Item : State.mFiles)
                {
                    const std::string& Name = Item.first;
                    if(Name.compare(0, Path.size(), Path) == 0 &&
                        Name.find('/', Path.size()) == std::string::npos)
                        Names.push_back(Name.substr(Path.size()));
                }
            }
            for(const auto& Name : Names) cb(Name.c_str(), data);
        }
    private:
        struct State
        {
            AssetIndexMutex mMutex;
            AssetIndexFiles mFiles;
            bool mReady = false;
        };
        static State& ST()
        {
            static State OneState;
            return OneState;
        }
        static bool Ensure()
        {
            static AssetIndexMutex LoadMutex;
            std::lock_guard<AssetIndexMutex> OneLoad(LoadMutex);
            auto& State = ST();
            {
                std::lock_guard<AssetIndexMutex> Lock(State.mMutex);
                if(State.mReady) return true;
            }
            const String IndexPath = Platform::File::RootForAssets() + gIndexFilename.c_str();
            auto File = Platform::File::OpenForRead(IndexPath);
            if(!File) return false;
            const sint32 Size = Platform::File::Size(File);
            if(Size <= 0 || 16 * 1024 * 1024 < Size)
            {
                Platform::File::Close(File);
                return false;
            }
            std::vector<char> Data(Size);
            sint32 Done = 0;
            while(Done < Size)
            {
                const sint32 ReadSize = Platform::File::Read(File, (uint08*) Data.data() + Done, Size - Done);
                if(ReadSize <= 0) break;
                Done += ReadSize;
            }
            Platform::File::Close(File);
            AssetIndexFiles NewFiles;
            if(Done != Size || !AssetIndexParse(Data.data(), Data.size(), NewFiles)) return false;
            {
                std::lock_guard<AssetIndexMutex> Lock(State.mMutex);
                State.mFiles.swap(NewFiles);
                State.mReady = true;
            }
            return true;
        }
        static bool Find(chars pathname, AssetIndexFileInfo& info)
        {
            bool Valid = false;
            const std::string Path = AssetIndexNormalizePath(pathname, false, &Valid);
            if(!Valid || !AssetIndexValidPath(Path) || !Ensure()) return false;
            auto& State = ST();
            std::lock_guard<AssetIndexMutex> Lock(State.mMutex);
            const auto Found = State.mFiles.find(Path);
            if(Found == State.mFiles.end()) return false;
            info = Found->second;
            return true;
        }
    };
#endif

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
        const sint32 Result = Platform::File::GetAttributes(WString::FromChars(pathname), size, ctime, atime, mtime);
        return (Result != -1 && (Result & 0x10) == 0);
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
    static bool Remove(chars pathname, bool autoremovedir)
    {
        return Platform::File::Remove(WString::FromChars(pathname), autoremovedir);
    }
    static bool Rename(chars old_pathname, chars new_pathname)
    {
        return Platform::File::Rename(
            WString::FromChars(old_pathname),
            WString::FromChars(new_pathname));
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

#define BOSS_COMPILER_DATA_LENGTH_MAX 40000
#if BOSS_NEED_EMBEDDED_ASSET || BOSS_NEED_EMBEDDED_CACHE
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
            const sint64 NewPos = (sint64) m_pos + size;
            m_pos = (NewPos < 0)? 0 : ((m_embedded_file->mSize < (uint64) NewPos)?
                (sint32) m_embedded_file->mSize : (sint32) NewPos);
            return m_pos;
        }
        static void SearchAsset(chars pathname, Platform::File::SearchCB cb, payload data)
        {
            #if BOSS_NEED_EMBEDDED_ASSET
            const sint32 PathLen = boss_strlen(pathname);
            for(sint32 i = 0; i < BOSS_EMBEDDED_ASSET_COUNT; ++i)
            {
                const String& CurPath = gSortedEmbeddedFiles[i].mPath;
                if(!String::CompareNoCase(CurPath, pathname, PathLen))
                {
                    const String& FileName = CurPath.Offset(PathLen);
                    if(FileName.Find(0, "/") == -1)
                        cb(FileName, data);
                }
            }
            #endif
        }
        static void SearchCache(chars pathname, Platform::File::SearchCB cb, payload data)
        {
            struct Payload
            {
                chars mPathName;
                const boss_size_t mPathLen;
                Platform::File::SearchCB mCB;
                payload mData;
            };
            Payload OnePayload {pathname, boss_strlen(pathname), cb, data};

            gEmbeddedCaches.AccessByCallback(
                [](const MapPath* path, EmbeddedFile* file, payload data)->void
                {
                    auto& CurPayload = *((Payload*) data);
                    const String& CurPath = file->mPath;
                    if(!String::CompareNoCase(CurPath, CurPayload.mPathName, CurPayload.mPathLen))
                    {
                        const String& FileName = CurPath.Offset(CurPayload.mPathLen);
                        if(FileName.Find(0, "/") == -1)
                            CurPayload.mCB(FileName, CurPayload.mData);
                    }
                }, &OnePayload);
        }

    protected:
        static EmbeddedFile* MatchAsset(chars pathname)
        {
            #if BOSS_NEED_EMBEDDED_ASSET
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
            #endif
            return nullptr;
        }
        static EmbeddedFile* MatchCache(chars pathname, bool forced)
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

    protected:
        EmbeddedFile* m_embedded_file;
        sint32 m_pos;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // AssetEmbeddedReadFileClass
    class AssetEmbeddedReadFileClass : public AssetEmbeddedFileClass
    {
    public:
        static AssetClass* CreateForAsset(chars pathname)
        {
            if(auto OneFile = MatchAsset(pathname))
            {
                auto Result = new AssetEmbeddedReadFileClass();
                Result->m_embedded_file = OneFile;
                return Result;
            }
            return nullptr;
        }
        static AssetClass* CreateForCache(chars pathname)
        {
            if(auto OneFile = MatchCache(pathname, false))
            {
                auto Result = new AssetEmbeddedReadFileClass();
                Result->m_embedded_file = OneFile;
                return Result;
            }
            return nullptr;
        }
        static bool ExistForAsset(chars pathname, uint64* size, uint64* ctime, uint64* atime, uint64* mtime)
        {
            if(auto OneFile = MatchAsset(pathname))
            {
                if(size) *size = OneFile->mSize;
                if(ctime) *ctime = OneFile->mCTime;
                if(atime) *atime = OneFile->mATime;
                if(mtime) *mtime = OneFile->mMTime;
                return true;
            }
            return false;
        }
        static bool ExistForCache(chars pathname, uint64* size, uint64* ctime, uint64* atime, uint64* mtime)
        {
            if(auto OneFile = MatchCache(pathname, false))
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
            if(!data || size <= 0 || m_embedded_file->mSize <= (uint64) m_pos) return 0;
            // 포커스정보 갱신
            sint32 ReadSize = Math::Min(size, m_embedded_file->mSize - m_pos);
            sint32 PageIndex = m_pos / BOSS_COMPILER_DATA_LENGTH_MAX;
            sint32 PageOffset = m_pos % BOSS_COMPILER_DATA_LENGTH_MAX;
            const sint32 Result = ReadSize;

            // 페이지를 돌며 버퍼읽기
            while(0 < ReadSize)
            {
                const sint32 CopySize = Math::Min(ReadSize, BOSS_COMPILER_DATA_LENGTH_MAX - PageOffset);
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
            if(auto OneFile = MatchCache(pathname, true))
            {
                OneFile->mSize = 0;
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
                const sint32 NumPages = (CurBufferSize + BOSS_COMPILER_DATA_LENGTH_MAX - 1) / BOSS_COMPILER_DATA_LENGTH_MAX;
                m_embedded_file->mPages = new bytes[NumPages];
                for(sint32 i = 0; i < NumPages; ++i)
                    m_embedded_file->mPages[i] = ((bytes) m_embedded_file->mBuffer) + BOSS_COMPILER_DATA_LENGTH_MAX * i;
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
    id_asset_read OpenForRead(chars filename, bool originalonly) const
    {
        for(sint32 i = 0, iend = m_pathes.Count(); i < iend; ++i)
            if(auto NewFile = Asset::OpenForRead(m_pathes[i] + filename, nullptr, originalonly))
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
    sint32 Search(AssetPath::SearchCB cb, payload data, bool originalonly)
    {
        sint32 FindedCount = 0;
        for(sint32 i = 0, iend = m_pathes.Count(); i < iend; ++i)
        {
            Map<String> Collector;
            auto SearchCB = [](chars itemname, payload data)->void
            {
                Map<String>& Collector = *((Map<String>*) data);
                #if BOSS_NEED_WEBFILES_ASSET
                    const String Key = AssetWebReadFileClass::Enabled()? String(itemname) : String(itemname).Lower();
                #else
                    const String Key = String(itemname).Lower();
                #endif
                if(!Collector.Access(Key))
                    Collector(Key) = itemname;
            };

            if(!originalonly)
            {
                #if BOSS_NEED_WEBFILES_ASSET && !BOSS_WASM
                    if(AssetWebReadFileClass::Scoped() && AssetWebReadFileClass::Routed(m_pathes[i]))
                    {
                        const String UserDir = AssetWebReadFileClass::UserPath(m_pathes[i]);
                        if(UserDir.Length())
                            Platform::File::Search(Platform::File::RootForAssetsRem() + UserDir + "/*",
                                SearchCB, (payload) &Collector, false, true);
                    }
                    else
                #endif
                {
                    #if BOSS_NEED_EMBEDDED_CACHE
                        AssetEmbeddedFileClass::SearchCache(m_pathes[i], SearchCB, (payload) &Collector);
                    #else
                        Platform::File::Search(Platform::File::RootForAssetsRem() + m_pathes[i] + "*",
                            SearchCB, (payload) &Collector, false, true);
                    #endif
                }
            }

            #if BOSS_NEED_WEBFILES_ASSET
                if(AssetWebReadFileClass::Routed(m_pathes[i]))
                    AssetWebReadFileClass::Search(m_pathes[i], SearchCB, (payload) &Collector);
                else if(AssetWebReadFileClass::LocalOriginal(m_pathes[i]))
            #endif
            {
                #if BOSS_STATIC_ASSET
                    #if BOSS_NEED_WEBFILES_ASSET && !BOSS_WASM
                        if(AssetWebReadFileClass::Scoped())
                            Platform::File::Search(Platform::File::RootForAssets() + m_pathes[i] + "*",
                                SearchCB, (payload) &Collector, false, true);
                        else
                    #endif
                        AssetLocalIndex::Search(m_pathes[i], SearchCB, (payload) &Collector);
                #elif BOSS_NEED_EMBEDDED_ASSET
                    AssetEmbeddedFileClass::SearchAsset(m_pathes[i], SearchCB, (payload) &Collector);
                #else
                    Platform::File::Search(Platform::File::RootForAssets() + m_pathes[i] + "*",
                        SearchCB, (payload) &Collector, false, true);
                #endif
            }

            for(sint32 j = 0, jend = Collector.Count(); j < jend; ++j)
            {
                if(const String* CurItemName = Collector.AccessByOrder(j))
                {
                    if(cb) cb(m_pathes[i], *CurItemName, data);
                    FindedCount++;
                }
            }
        }
        return FindedCount;
    }

private:
    Strings m_pathes;
};

namespace BOSS
{
    #if BOSS_GENERATE_ASSET && BOSS_NEED_EMBEDDED_ASSET
    static void _AssetsSearcher(chars itemname, payload data)
    {
        Context& Collector = *((Context*) data);
        if(Platform::File::ExistForDir(itemname)) // 폴더는 재귀처리
            Platform::File::Search(itemname, _AssetsSearcher, data, true);
        else
        {
            // 목록 자체는 임베디드어셋에 내장하지 않는다.
            if(!String(itemname).Right(16).CompareNoCase("assetsindex.json") ||
                String(itemname).Right(gIndexFilename.size()) == gIndexFilename.c_str()) return;
            // 무시해야하는 파일은 제외
            chars Ignores[] = BOSS_ASSET_IGNORES;
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
                ID.Replace(",", "_COMMA_");
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
        if(auto GenFile = Platform::File::OpenForWrite(Platform::File::RootForAssets() + "../source-gen/boss_assets.hpp"))
        {
            String GenText;
            GenText += "\xEF\xBB\xBF";
            GenText += "#include <boss.hpp>\n";
            GenText += "\n";
            GenText += "#if BOSS_NEED_EMBEDDED_ASSET\n";
            GenText += String::Format("    #define BOSS_EMBEDDED_ASSET_COUNT Signed64(%lld)\n", info("data")("count").GetInt());
            GenText += String::Format("    #define BOSS_EMBEDDED_ASSET_SIZE  Signed64(%lld)\n", info("data")("size").GetInt());
            GenText += String::Format("    #define BOSS_EMBEDDED_ASSET_TIME  Signed64(%lld)\n", info("data")("time").GetInt());
            GenText += "\n";
            // 스트링비우기
            Platform::File::Write(GenFile, (bytes)(chars) GenText, GenText.Length());
            GenText.Empty(true);

            // 파일내역
            sint32s PartCounts; // 파트정보
            for(sint32 i = 0, iend = info("files").LengthOfIndexable(); i < iend; ++i)
            {
                auto& CurFile = info("files")[i];
                GenText += String::Format("    static bytes ASSETS_%s = (bytes) // %s (%lldbytes, modified at %lld)",
                    (chars) CurFile("id").GetText(), (chars) CurFile("path").GetText(),
                    CurFile("size").GetInt(), CurFile("time").GetInt());

                auto& CurPartCount = PartCounts.AtAdding();
                CurPartCount = 0;
                if(auto BinFile = Platform::File::OpenForRead(CurFile("path").GetText()))
                {
                    if(Platform::File::Size(BinFile) == 0)
                        GenText += "\n        \"\"";
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
                            GenText += "\n        \"";
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
                            if(BOSS_COMPILER_DATA_LENGTH_MAX < (WrittenSize += DataSize) + LineSize)
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
                                    (chars) CurFile("id").GetText(), CurPartCount);
                            }
                        }
                    }
                    Platform::File::Close(BinFile);
                }
                else GenText += String::Format("\n\"-File Not Found- (%s)\"", (chars) CurFile("path").GetText());
                GenText += ";\n";
                GenText += "\n";
                // 스트링비우기
                Platform::File::Write(GenFile, (bytes)(chars) GenText, GenText.Length());
                GenText.Empty(true);
            }

            // 파일인스턴스
            GenText += "    struct EmbeddedFile {\n";
            GenText += "        String mPath;\n";
            GenText += "        buffer mBuffer;\n";
            GenText += "        bytes* mPages;\n";
            GenText += "        uint64 mSize;\n";
            GenText += "        uint64 mCTime;\n";
            GenText += "        uint64 mATime;\n";
            GenText += "        uint64 mMTime;\n";
            GenText += "        ~EmbeddedFile() {\n";
            GenText += "            Buffer::Free(mBuffer);\n";
            GenText += "            delete[] mPages;\n";
            GenText += "        }\n";
            GenText += "    };\n";
            GenText += "\n";

            GenText += "    static Map<EmbeddedFile> gEmbeddedCaches;\n";
            GenText += "    static EmbeddedFile gSortedEmbeddedFiles[] = {\n";
            if(info("files").LengthOfIndexable() == 0)
                GenText += "        {\"\", nullptr, nullptr, 0, 0, 0, 0}\n";
            else
            {
                // 파일정렬
                Map<sint32> Sortor;
                const sint32 FileCount = info("files").LengthOfIndexable();
                const sint32 AssetsPathLength = Platform::File::RootForAssets().Length();
                for(sint32 i = 0; i < FileCount; ++i)
                {
                    auto& CurFile = info("files")[i];
                    const String CurPath = CurFile("path").GetText().Offset(AssetsPathLength).Lower();
                    Sortor(CurPath) = i;
                }

                // 페이로드 구성
                struct Payload
                {
                    const Context& mFiles;
                    const sint32s& mPartCounts;
                    String* mGenText;
                    sint32 mFocus;
                    sint32 mCount;
                };
                Payload OnePayload {info("files"), PartCounts, &GenText, 0, FileCount};

                // 정렬된 순서대로 기록
                Sortor.AccessByCallback([](const MapPath* path, sint32* data, payload param)->void
                    {
                        const String CurPath = &path->GetPath()[0];
                        const sint32 CurIndex = *data;
                        auto& CurPayload = *((Payload*) param);
                        // 파일정보
                        auto& CurFile = CurPayload.mFiles[CurIndex];
                        uint64 CurSize = 0, CurCTime = 0, CurATime = 0, CurMTime = 0;
                        Platform::File::GetAttributes(WString::FromChars(CurFile("path").GetText()),
                            &CurSize, &CurCTime, &CurATime, &CurMTime);
                        // 파트정보
                        auto CurPartCount = CurPayload.mPartCounts[CurIndex];
                        String PageList = String::Format("ASSETS_%s", (chars) CurFile("id").GetText());
                        for(sint32 i = 0; i < CurPartCount; ++i)
                            PageList += String::Format(", ASSETS_%s_PART%d", (chars) CurFile("id").GetText(), i + 1);
                        // 구조체구성
                        *CurPayload.mGenText += String::Format("        {\"%s\", nullptr, new bytes[%d] {%s},\n", (chars) CurPath, CurPartCount + 1, (chars) PageList);
                        *CurPayload.mGenText += String::Format("            Unsigned64(%llu), Unsigned64(%llu), Unsigned64(%llu), Unsigned64(%llu)}%s\n",
                            CurSize, CurCTime, CurATime, CurMTime, (++CurPayload.mFocus < CurPayload.mCount)? "," : "");
                    }, (payload) &OnePayload);
            }
            GenText += "    };\n";
            GenText += "#endif\n";

            // 남은 스트링쓰기
            Platform::File::Write(GenFile, (bytes)(chars) GenText, GenText.Length());
            Platform::File::Close(GenFile);
        }
    }

    #endif

    void Asset::SetWebFilesIndex(chars filename, chars version, chars deckname)
    {
        #if BOSS_NEED_WEBFILES_ASSET || BOSS_STATIC_ASSET || BOSS_GENERATE_ASSET
            gIndexFilename=filename?filename:"assetsindex.json";
            gIndexVersion=version?version:"Boss2D-AssetsIndex-1";
            gDeckFilename=deckname?deckname:"";
        #else
            (void)filename; (void)version; (void)deckname;
        #endif
    }
    chars Asset::TrySetWebFilesUrl(chars url)
    {
        #if BOSS_NEED_WEBFILES_ASSET
            static String DeckJson;
            String NewDeckJson;
            if(!AssetWebReadFileClass::TryUrl(url, &NewDeckJson)) return nullptr;
            DeckJson = NewDeckJson;
            return DeckJson;
        #else
            (void) url;
            BOSS_TRACE("[AssetWeb] Web loading is disabled. Set BOSS_NEED_WEBFILES_ASSET=1 in the shared boss_config.h and rebuild Boss2D.");
            return nullptr;
        #endif
    }
    chars Asset::GetWebFilesUrl()
    {
        static String WebFilesUrl;
        #if BOSS_NEED_WEBFILES_ASSET
            WebFilesUrl = AssetWebReadFileClass::Url().c_str();
        #endif
        return WebFilesUrl;
    }
    chars Asset::GetWebCachePrefix()
    {
        static String WebCachePrefix;
        #if BOSS_NEED_WEBFILES_ASSET
            WebCachePrefix = AssetWebReadFileClass::CachePrefix();
        #endif
        return WebCachePrefix;
    }
    chars Asset::GetFilePath(chars filename)
    {
        const String Filename = filename;
        static String FilePath;
        FilePath.Empty();
        if(!Filename.Length()) return FilePath;
        #if BOSS_NEED_WEBFILES_ASSET && !BOSS_WASM
            if(AssetWebReadFileClass::Routed(Filename))
            {
                if(AssetWebReadFileClass::Url().empty()) return FilePath;
                const String UserName = AssetWebReadFileClass::UserPath(Filename);
                if(!UserName.Length()) return FilePath;
                const String UserFile = Platform::File::RootForAssetsRem() + UserName;
                if(AssetReadFileClass::Exist(UserFile, nullptr, nullptr, nullptr, nullptr))
                    FilePath = UserFile;
                else if(AssetWebReadFileClass::Listed(Filename))
                    FilePath = AssetWebReadFileClass::NativePath(Filename);
                return FilePath;
            }
        #endif
        const String Rem = Platform::File::RootForAssetsRem() + Filename;
        if(AssetReadFileClass::Exist(Rem, nullptr, nullptr, nullptr, nullptr)) FilePath = Rem;
        else
        {
            #if BOSS_NEED_WEBFILES_ASSET && !BOSS_WASM
                if(!AssetWebReadFileClass::LocalOriginal(Filename)) return FilePath;
            #endif
            const String Local = Platform::File::RootForAssets() + Filename;
            if(AssetReadFileClass::Exist(Local, nullptr, nullptr, nullptr, nullptr)) FilePath = Local;
        }
        return FilePath;
    }

    void Asset::SetWebFilesUrl(chars url)
    {
        #if BOSS_NEED_WEBFILES_ASSET
            AssetWebReadFileClass::SetUrl(url);
        #else
            (void) url;
        #endif
    }

    #if BOSS_GENERATE_ASSET
        struct AssetsIndexCollector
        {
            std::string mRoot;
            std::string mOutput;
            AssetIndexFiles mFiles;
            bool mValid = true;
        };

        static std::string _AssetsIndexFullPath(chars pathname)
        {
            std::string Path = pathname? pathname : "";
            for(auto& Ch : Path) if(Ch == '\\') Ch = '/';
            const std::string Prefix = (Path.compare(0, 2, "//") == 0)? "//" : ((!Path.empty() && Path[0] == '/')? "/" : "");
            const bool Rooted = !Prefix.empty() || (3 <= Path.size() && Path[1] == ':' && Path[2] == '/');
            std::vector<std::string> Parts;
            for(size_t Pos = 0; Pos < Path.size();)
            {
                const auto End = Path.find('/', Pos);
                const auto Part = Path.substr(Pos, End == std::string::npos? End : End - Pos);
                if(Part == "..")
                {
                    if(!Parts.empty() && Parts.back() != ".." && Parts.back().find(':') == std::string::npos) Parts.pop_back();
                    else if(!Rooted) Parts.push_back(Part);
                }
                else if(!Part.empty() && Part != ".") Parts.push_back(Part);
                if(End == std::string::npos) break;
                Pos = End + 1;
            }
            Path = Prefix;
            for(const auto& Part : Parts)
            {
                if(!Path.empty() && Path.back() != '/') Path += '/';
                Path += Part;
            }
            return Path;
        }

        static void _AssetsIndexSearcher(chars itemname, payload data)
        {
            auto& Collector = *((AssetsIndexCollector*) data);
            if(Platform::File::ExistForDir(itemname))
            {
                if(Platform::File::Search(itemname, _AssetsIndexSearcher, data, true) == -1)
                    Collector.mValid = false;
                return;
            }
            const std::string FullPath = _AssetsIndexFullPath(itemname);
            if(FullPath.compare(0, Collector.mRoot.size(), Collector.mRoot)) return;
            const std::string Relative = FullPath.substr(Collector.mRoot.size());
            if(Relative == "assetsindex.json" || Relative == Collector.mOutput || Relative == gIndexFilename) return;
            if(!AssetIndexValidPath(Relative))
            {
                Collector.mValid = false;
                BOSS_TRACE("Invalid assets index path: %s", Relative.c_str());
                return;
            }
            AssetIndexFileInfo Info;
            if(Platform::File::GetAttributes(WString::FromChars(itemname),
                &Info.mSize, &Info.mCTime, &Info.mATime, &Info.mMTime) == -1)
            {
                Collector.mValid = false;
                return;
            }
            Collector.mFiles[Relative] = Info;
        }

        static bool _ReadAssetsIndex(chars pathname, AssetIndexFiles& index, const std::string& version)
        {
            auto File = Platform::File::OpenForRead(pathname);
            if(!File) return false;
            const sint32 Size = Platform::File::Size(File);
            if(Size <= 0 || 16 * 1024 * 1024 < Size)
            {
                Platform::File::Close(File);
                return false;
            }
            std::vector<char> Data(Size);
            sint32 Done = 0;
            while(Done < Size)
            {
                const sint32 ReadSize = Platform::File::Read(File, (uint08*) Data.data() + Done, Size - Done);
                if(ReadSize <= 0) break;
                Done += ReadSize;
            }
            Platform::File::Close(File);
            return Done == Size && AssetIndexParse(Data.data(), Data.size(), index, version);
        }

        static bool _WriteAssetsIndex(chars pathname, const AssetIndexFiles& index, const std::string& version)
        {
            if(index.empty()) return false;
            const String Text = AssetIndexBuild(index, version);
            const uint08 BOM[3] = {0xEF, 0xBB, 0xBF};
            if(auto File = Platform::File::OpenForWrite(pathname, true))
            {
                const bool Result = (Platform::File::Write(File, BOM, 3) == 3 &&
                    Platform::File::Write(File, (bytes)(chars) Text, Text.Length()) == Text.Length());
                Platform::File::Close(File);
                return Result;
            }
            return false;
        }
    #endif

    bool Asset::RebuildForAssetsIndex(chars filename, chars version, chars rootpath)
    {
        #if BOSS_GENERATE_ASSET
            const String SourceRoot = rootpath? String(rootpath) : Platform::File::RootForAssets();
            std::string NormalRoot = _AssetsIndexFullPath(SourceRoot);
            if(NormalRoot.empty()) return false;
            if(NormalRoot.back() != '/') NormalRoot += '/';
            const String AssetsPath = NormalRoot.c_str();
            const String AssetsDir = AssetsPath;
            if(!Platform::File::ExistForDir(AssetsDir)) return false;

            if(!filename || !*filename || !version || !*version) return false;
            const std::string Name=filename;
            if(Name.find('/')!=std::string::npos || Name.find('\\')!=std::string::npos || Name.find(':')!=std::string::npos || Name=="." || Name=="..") return false;
            AssetsIndexCollector Collector;
            Collector.mOutput=Name;
            Collector.mRoot = (chars) AssetsPath;
            for(auto& Ch : Collector.mRoot) if(Ch == '\\') Ch = '/';
            if(Platform::File::Search(AssetsDir, _AssetsIndexSearcher, &Collector, true) == -1 ||
                !Collector.mValid) return false;

            AssetIndexFiles OldIndex;
            const String IndexPath = AssetsPath + filename;
            bool Changed = !_ReadAssetsIndex(IndexPath, OldIndex, version) ||
                OldIndex.size() != Collector.mFiles.size();
            if(!Changed)
                for(const auto& Pair : Collector.mFiles)
                {
                    const auto Found = OldIndex.find(Pair.first);
                    if(Found == OldIndex.end() || Found->second.mSize != Pair.second.mSize ||
                        Found->second.mCTime != Pair.second.mCTime ||
                        Found->second.mMTime != Pair.second.mMTime)
                    {
                        Changed = true;
                        break;
                    }
                }
            // 접근시간(atime)은 일반적인 파일 읽기로도 변하므로 변경판정에는 제외한다.
            if(Changed && Platform::Popup::MessageDialog("리빌드가 필요합니다",
                String::Format("에셋 파일 목록에 변경이 있습니다.\n\n%s을 다시 생성하고 종료할까요?", filename), DBT_YesNo) == 0)
            {
                if(_WriteAssetsIndex(IndexPath, Collector.mFiles, version)) return true;
                BOSS_TRACE("Assets index write failed: %s", (chars) IndexPath);
            }
        #endif
        return false;
    }

    bool Asset::RebuildForEmbedded()
    {
        #if BOSS_GENERATE_ASSET && BOSS_NEED_EMBEDDED_ASSET
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

        #if BOSS_NEED_WEBFILES_ASSET && !BOSS_WASM
            if(AssetWebReadFileClass::Scoped() && AssetWebReadFileClass::Routed(filename))
            {
                if(AssetWebReadFileClass::Url().empty()) return false;
                const String UserName = AssetWebReadFileClass::UserPath(filename);
                if(!UserName.Length()) return false;
                if(AssetReadFileClass::Exist(Platform::File::RootForAssetsRem() + UserName, size, ctime, atime, mtime)) return true;
                return AssetWebReadFileClass::Exist(filename, size, ctime, atime, mtime);
            }
        #endif
        #if BOSS_NEED_EMBEDDED_CACHE
            if(AssetEmbeddedReadFileClass::ExistForCache(filename, size, ctime, atime, mtime))
                return true;
        #else
            if(AssetReadFileClass::Exist(Platform::File::RootForAssetsRem() + filename, size, ctime, atime, mtime))
                return true;
        #endif

        #if BOSS_NEED_WEBFILES_ASSET
            if(AssetWebReadFileClass::Routed(filename))
                return AssetWebReadFileClass::Exist(filename, size, ctime, atime, mtime);
            if(!AssetWebReadFileClass::LocalOriginal(filename)) return false;
        #endif

        #if BOSS_STATIC_ASSET
            #if BOSS_NEED_WEBFILES_ASSET && !BOSS_WASM
                if(AssetWebReadFileClass::Scoped())
                    return AssetReadFileClass::Exist(Platform::File::RootForAssets() + filename, size, ctime, atime, mtime);
            #endif
            if(std::string(filename)==gDeckFilename || std::string(filename)==gIndexFilename)
                return AssetReadFileClass::Exist(Platform::File::RootForAssets()+filename,size,ctime,atime,mtime);
            return AssetLocalIndex::Exist(filename, size, ctime, atime, mtime);
        #elif BOSS_NEED_EMBEDDED_ASSET
            if(AssetEmbeddedReadFileClass::ExistForAsset(filename, size, ctime, atime, mtime))
                return true;
        #else
            if(AssetReadFileClass::Exist(Platform::File::RootForAssets() + filename, size, ctime, atime, mtime))
                return true;
        #endif
        return false;
    }

    id_asset_read Asset::OpenForRead(chars filename, id_assetpath_read assetpath, bool originalonly)
    {
        if(assetpath)
            return ((const AssetPathClass*) assetpath)->OpenForRead(filename, originalonly);

        #if BOSS_NEED_WEBFILES_ASSET && !BOSS_WASM
            if(AssetWebReadFileClass::Scoped() && AssetWebReadFileClass::Routed(filename))
            {
                if(AssetWebReadFileClass::Url().empty()) return nullptr;
                const String UserName = AssetWebReadFileClass::UserPath(filename);
                if(!UserName.Length()) return nullptr;
                if(!originalonly)
                    if(auto Asset = AssetReadFileClass::Create(Platform::File::RootForAssetsRem() + UserName))
                        return (id_asset_read) Asset;
                return (id_asset_read) AssetWebReadFileClass::Create(filename);
            }
        #endif
        if(!originalonly)
        {
            #if BOSS_NEED_EMBEDDED_CACHE
                if(auto Asset = AssetEmbeddedReadFileClass::CreateForCache(filename))
                    return (id_asset_read) Asset;
            #else
                if(auto Asset = AssetReadFileClass::Create(Platform::File::RootForAssetsRem() + filename))
                    return (id_asset_read) Asset;
            #endif
        }

        #if BOSS_NEED_WEBFILES_ASSET
            if(AssetWebReadFileClass::Routed(filename))
                return (id_asset_read) AssetWebReadFileClass::Create(filename);
            if(!AssetWebReadFileClass::LocalOriginal(filename)) return nullptr;
        #endif

        #if BOSS_STATIC_ASSET
            #if BOSS_NEED_WEBFILES_ASSET && !BOSS_WASM
                if(!AssetWebReadFileClass::Scoped())
            #endif
            {
                if(std::string(filename)!=gDeckFilename && std::string(filename)!=gIndexFilename && !AssetLocalIndex::Has(filename)) return nullptr;
            }
        #endif

        #if BOSS_NEED_EMBEDDED_ASSET
            if(auto Asset = AssetEmbeddedReadFileClass::CreateForAsset(filename))
                return (id_asset_read) Asset;
        #else
            if(auto Asset = AssetReadFileClass::Create(Platform::File::RootForAssets() + filename))
                return (id_asset_read) Asset;
        #endif
        return nullptr;
    }

    static String WritableAssetName(chars filename)
    {
        #if BOSS_NEED_WEBFILES_ASSET && !BOSS_WASM
            return AssetWebReadFileClass::UserPath(filename);
        #else
            return String(filename);
        #endif
    }
    id_asset Asset::OpenForWrite(chars filename, bool autocreatedir)
    {
        #if BOSS_NEED_WEBFILES_ASSET && !BOSS_WASM
            if(AssetWebReadFileClass::Scoped() && AssetWebReadFileClass::Routed(filename))
            {
                const String UserName = AssetWebReadFileClass::UserPath(filename);
                if(!UserName.Length()) return nullptr;
                return (id_asset) AssetWriteFileClass::Create(
                    Platform::File::RootForAssetsRem() + UserName, autocreatedir);
            }
        #endif
        #if BOSS_NEED_EMBEDDED_CACHE
            return (id_asset) AssetEmbeddedWriteFileClass::Create(filename);
        #else
            return (id_asset) AssetWriteFileClass::Create(
                Platform::File::RootForAssetsRem() + WritableAssetName(filename), autocreatedir);
        #endif
    }

    bool Asset::RemoveForWrite(chars filename, bool autoremovedir)
    {
        #if BOSS_NEED_WEBFILES_ASSET && !BOSS_WASM
            if(AssetWebReadFileClass::Scoped() && AssetWebReadFileClass::Routed(filename))
            {
                const String UserName = AssetWebReadFileClass::UserPath(filename);
                return UserName.Length() && AssetWriteFileClass::Remove(
                    Platform::File::RootForAssetsRem() + UserName, autoremovedir);
            }
        #endif
        #if BOSS_NEED_EMBEDDED_CACHE
            return false;
        #else
            return AssetWriteFileClass::Remove(
                Platform::File::RootForAssetsRem() + WritableAssetName(filename), autoremovedir);
        #endif
    }

    bool Asset::RenameForWrite(chars old_filename, chars new_filename)
    {
        #if BOSS_NEED_WEBFILES_ASSET && !BOSS_WASM
            if(AssetWebReadFileClass::Scoped() &&
                (AssetWebReadFileClass::Routed(old_filename) || AssetWebReadFileClass::Routed(new_filename)))
            {
                const String OldName = AssetWebReadFileClass::UserPath(old_filename);
                const String NewName = AssetWebReadFileClass::UserPath(new_filename);
                return OldName.Length() && NewName.Length() && AssetWriteFileClass::Rename(
                    Platform::File::RootForAssetsRem() + OldName, Platform::File::RootForAssetsRem() + NewName);
            }
        #endif
        #if BOSS_NEED_EMBEDDED_CACHE
            return false;
        #else
            return AssetWriteFileClass::Rename(
                Platform::File::RootForAssetsRem() + WritableAssetName(old_filename),
                Platform::File::RootForAssetsRem() + WritableAssetName(new_filename));
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

    sint32 AssetPath::Search(id_assetpath_read assetpath, SearchCB cb, payload data, bool originalonly)
    {
        if(auto CurAssetPath = (AssetPathClass*) assetpath)
            return CurAssetPath->Search(cb, data, originalonly);
        return 0;
    }
}
