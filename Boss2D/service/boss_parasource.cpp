#include <platform/boss_platform.hpp>
#include "boss_parasource.hpp"

#include <functional>

namespace BOSS
{
    class ContactPool
    {
    private:
        typedef Tree<ContactPool> ContactPoolTree;
        class STClass
        {
        public:
            STClass() {}
            ~STClass() {}
        public:
            ContactPoolTree m_tree;
        };
        static inline STClass& ST() {return *BOSS_STORAGE_SYS(STClass);}

    public:
        static ContactPool& GetContact(chars domain, uint16 port)
        {
            ContactPool* Ptr = ST().m_tree(domain)[port].Value();
            return (Ptr)? *Ptr : *ST().m_tree(domain)[port].CreateValue();
        }
        static bool RemoveContact(chars domain, uint16 port)
        {
            if(ST().m_tree(domain)[port].Value())
            {
                ST().m_tree(domain).Remove(port);
                return true;
            }
            return false;
        }

    public:
        ContactPool() {mConnected = false; mSocket = nullptr; mPort = 0; mTimeOut = 0;}
        ~ContactPool() {Platform::Socket::Close(mSocket);}

    public:
        void InitForFirstTime(chars domain, uint16 port, sint32 timeout)
        {
            if(!mSocket)
            {
                mConnected = false;
                mSocket = Platform::Socket::OpenForTcp();
                mDomain = domain;
                mPort = port;
            }
            mTimeOut = timeout;
        }
        bool Connect()
        {
            if(!mConnected)
            {
                mConnected = Platform::Socket::Connect(mSocket, mDomain, mPort, mTimeOut);
                if(!mConnected) return false;
            }
            return true;
        }
        void Disconnect()
        {
            mConnected = false;
            Platform::Socket::Disconnect(mSocket, mTimeOut);
        }
        bool Send(chars data, sint32 size)
        {
            if(!Connect()) return false;
            sint32 SendSize = size;
            while(0 < SendSize)
            {
                const sint32 SendResult = Platform::Socket::Send(mSocket, (bytes) data, SendSize, mTimeOut);
                if(SendResult < 0) return (mConnected = false);
                else
                {
                    data += SendResult;
                    SendSize -= SendResult;
                }
            }
            return true;
        }
        bool Recv(chararray& data)
        {
            if(!Connect()) return false;
            uint08 RecvBuffer[4096];
            const sint32 RecvSize = Platform::Socket::Recv(mSocket, RecvBuffer, 4096, mTimeOut);
            if(RecvSize < 0) return (mConnected = (RecvSize == -10035));
            else if(0 < RecvSize)
                Memory::Copy(data.AtDumpingAdded(RecvSize), RecvBuffer, RecvSize);
            return true;
        }
        typedef std::function<bool()> RecvLoopCB;
        bool RecvLoop(chararray& data, RecvLoopCB cb)
        {
            sint32 LastResponseCount = data.Count();
            uint64 LastResponseTimeMsec = Platform::Utility::CurrentTimeMsec();
            while(Recv(data))
            {
                if(cb()) return true;
                if(LastResponseCount < data.Count())
                {
                    LastResponseCount = data.Count();
                    LastResponseTimeMsec = Platform::Utility::CurrentTimeMsec();
                }
                else if(isTimeOut(LastResponseTimeMsec))
                {
                    Disconnect();
                    return false;
                }
            }
            return false;
        }

    public:
        inline chars domain() const {return mDomain;}
        inline bool isTimeOut(uint64 msec) const
        {return (mTimeOut < Platform::Utility::CurrentTimeMsec() - msec);}

    private:
        bool mConnected;
        id_socket mSocket;
        String mDomain;
        uint16 mPort;
        sint32 mTimeOut;

    public:
        class Cookie
        {
        public:
            String mName;
            String mValue;
        };
        Array<Cookie> mCookies;
        Map<Cookie*> mCookieMap;
    };

    class CachePool
    {
    private:
        typedef Array<CachePool> CachePoolArray;
        class STClass
        {
        public:
            STClass() {m_mutex = Mutex::Open();}
            ~STClass() {Mutex::Close(m_mutex);}
        public:
            CachePoolArray m_array;
            id_mutex m_mutex;
        };
        static inline STClass& ST() {static STClass _; return _;}

    public:
        static void Lock() {Mutex::Lock(ST().m_mutex);}
        static void Unlock() {Mutex::Unlock(ST().m_mutex);}
        static CachePool& Add() {return ST().m_array.AtAdding();}
        static CachePool& Get(sint32 i) {return ST().m_array.AtWherever(i);}
        static void Remove(sint32 i) {ST().m_array.SubtractionSection(i);}
        static sint32 Count() {return ST().m_array.Count();}

    public:
        CachePool() {mSize = 0; mDate = 0;}
        ~CachePool() {}

    public:
        String mPath;
        uint64 mSize;
        uint64 mDate;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // ParaSource
    ////////////////////////////////////////////////////////////////////////////////
    static const String gCommentText = "/CommentView.nhn?";
    static const String gJsonBeginText = "#json begin";
    static const String gJsonEndText = "#json end";
    static const String gArticleText = "//cafe.naver.com/ArticleRead.nhn?";
    static const String gEndCodeText = "\r\n\r\n";
    static const String gContentLengthText = "Content-Length:";
    static const String gConnectionText = "Connection: keep-alive";
    static const String gTransferEncodingText = "Transfer-Encoding:";
    static const String gSetCookieText = "Set-Cookie:";

    ParaSource::ParaSource(Type type) : mType(type)
    {
        mLastContact = nullptr;
    }

    ParaSource::ParaSource(chars domain)
    {
        if(!String::CompareNoCase(domain, "cafe.naver.com"))
            mType = NaverCafe;
        else mType = IIS;
        mLastContact = nullptr;
    }

    ParaSource::~ParaSource()
    {
    }

    void ParaSource::SetContact(chars domain, uint16 port, sint32 timeout)
    {
        mLastContact = &ContactPool::GetContact(domain, port);
        mLastContact->InitForFirstTime(domain, port, timeout);
    }

    bool ParaSource::GetFile(uint08s& file, chars path, chars args)
    {
        if(!HTTPQuery(path, args)) return false;
        const sint32 ResponseCount = mResponseData.Count();
        Memory::Copy(file.AtDumpingAdded(ResponseCount), &mResponseData[0], ResponseCount);
        return true;
    }

    bool ParaSource::GetJson(Context& json, chars path, chars args)
    {
        return LoadJsons([](payload data, chars content, sint32 length)->bool
            {
                Context* Json = (Context*) data;
                Json->LoadJson(SO_NeedCopy, content, length);
                return false;
            }, &json, path, args);
    }

    bool ParaSource::GetJsons(Contexts& jsons, chars path, chars args)
    {
        return LoadJsons([](payload data, chars content, sint32 length)->bool
            {
                Contexts* Jsons = (Contexts*) data;
                auto& NewJson = Jsons->AtAdding();
                NewJson.LoadJson(SO_NeedCopy, content, length);
                return true;
            }, &jsons, path, args);
    }

    bool ParaSource::GetLastSpecialJson(Context& json)
    {
        if(mType == NaverCafe)
        {
            if(0 < mResponseData.Count())
            {
                const String SourcePage(&mResponseData[0], mResponseData.Count());
                // CommentText의 검색
                sint32 Pos = SourcePage.Find(0, gCommentText);
                if(Pos != -1)
                {
                    sint32 PosBegin = Pos + gCommentText.Length();
                    sint32 PosEnd = PosBegin;
                    while(SourcePage[PosEnd] != '\'') PosEnd++;
                    // Args의 구성
                    String Args(&mResponseData[PosBegin], PosEnd - PosBegin);
                    while(true)
                    {
                        const sint32 ArgBegin = Args.Find(0, '{');
                        const sint32 ArgEnd = Args.Find(0, '}');
                        if(ArgBegin != -1 && ArgBegin < ArgEnd)
                        {
                            const String ArgText(((chars) Args) + ArgBegin + 1, ArgEnd - ArgBegin - 1);
                            const String FindText = String::Format("name=\"%s\" value=\"", (chars) ArgText);
                            sint32 FindPos = SourcePage.Find(0, FindText);
                            if(FindPos != -1)
                            {
                                sint32 FindPosBegin = FindPos + FindText.Length();
                                sint32 FindPosEnd = FindPosBegin;
                                while(SourcePage[FindPosEnd] != '\"') FindPosEnd++;
                                String FindResult(&mResponseData[FindPosBegin], FindPosEnd - FindPosBegin);
                                Args.Replace(String(((chars) Args) + ArgBegin, ArgEnd - ArgBegin + 1), FindResult);
                            }
                            else return SetLastError("Comment-Text is broken.");
                        }
                        else break;
                    }
                    // 쿼리
                    if(HTTPQuery("CommentView.nhn", Args))
                    {
                        json.LoadJson(SO_NeedCopy, &mResponseData[0], mResponseData.Count());
                        return true;
                    }
                }
            }
            return SetLastError("Comment-Text not found.");
        }
        return false;
    }

    bool ParaSource::LoadJsons(JsonLoaderCB cb, payload data, chars path, chars args)
    {
        if(!HTTPQuery(path, args)) return false;
        if(mType == NaverCafe)
        {
            const String SourcePage(&mResponseData[0], mResponseData.Count());
            // ArticleText의 검색
            sint32 Pos = SourcePage.Find(0, gArticleText);
            if(Pos != -1)
            {
                sint32 PosBegin = Pos + gArticleText.Length();
                sint32 PosEnd = PosBegin;
                while(SourcePage[PosEnd] != '\"') PosEnd++;
                // 쿼리
                String Args(((chars) SourcePage) + PosBegin, PosEnd - PosBegin);
                Args += "&page=1&boardtype=L&referrerAllArticles=true";
                if(!HTTPQuery("ArticleRead.nhn", Args))
                    return false;
            }
            else return SetLastError("Article-Text not found.");
        }

        bool Result = false;
        const String TextData(&mResponseData[0], mResponseData.Count());
        sint32 OldPos = 0, BeginPos = 0;
        while((BeginPos = TextData.Find(OldPos, gJsonBeginText)) != -1)
        {
            BeginPos += 11; // "#json begin"
            const sint32 EndPos = TextData.Find(BeginPos, gJsonEndText);
            if(BeginPos < EndPos)
            {
                String FileName;
                while(mResponseData[++BeginPos] != ')') // "("
                    FileName += mResponseData[BeginPos];
                BOSS_TRACE("ParaSource.GetJson(%s)", (chars) FileName);

                chars ContentPtr = &mResponseData[BeginPos + 1];
                sint32 ContentLength = EndPos - (BeginPos + 1);
                chararray ContentBuffer;
                if(mType == NaverCafe)
                {
                    for(sint32 i = 0; i < ContentLength; ++i)
                    {
                        if(ContentPtr[i] != '<')
                            ContentBuffer.AtAdding() = ContentPtr[i];
                        else while(ContentPtr[++i] != '>');
                    }
                    ContentPtr = &ContentBuffer[0];
                    ContentLength = ContentBuffer.Count();
                }

                if(!cb(data, ContentPtr, ContentLength))
                    return true;
                OldPos = EndPos + 9; // "#json end"
                Result = true;
            }
        }
        return Result;
    }

    bool ParaSource::HTTPQuery(chars path, chars args)
    {
        mResponseData.SubtractionAll();
        if(!mLastContact)
            return SetLastError("You need to call by SetContact() first.");

        // 쿠키모음
        String AllCookies;
        for(sint32 i = 0, iend = mLastContact->mCookies.Count(); i < iend; ++i)
        {
            AllCookies += "Cookie: ";
            AllCookies += mLastContact->mCookies[i].mName;
            AllCookies += mLastContact->mCookies[i].mValue;
            AllCookies += "\r\n";
        }
        AllCookies += "\r\n";

        // 쿼리전송
        const bool HasArgs = (args && *args);
        const String RequestText = String::Format(
            "GET /%s%s%s HTTP/1.1\r\n"
            "Accept: text/html, application/xhtml+xml, */*\r\n"
            "Accept-Language: ko-KR\r\n"
            "Accept-Encoding: deflate\r\n"
            "User-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\n"
            "Host: %s\r\n"
            "Connection: Keep-Alive\r\n"
            "%s", path, (HasArgs)? "?" : "", (HasArgs)? args : "", mLastContact->domain(), (chars) AllCookies);
        if(!mLastContact->Send(RequestText, RequestText.Length()))
            return SetLastError("Failed sending for HTTP request.");

        sint32 EndCodePos = -1;
        sint32 ContentLength = -1;
        bool KeepAlive = false;
        bool Chunked = false;

        // HTTP헤더 수신
        chararray HttpData;
        String HttpText;
        if(mLastContact->RecvLoop(HttpData,
            [&]()->bool
            {
                if(0 < HttpData.Count())
                {
                    HttpText = String(&HttpData[0], HttpData.Count());
                    EndCodePos = HttpText.Find(0, gEndCodeText);
                    if(EndCodePos != -1)
                    {
                        EndCodePos += 4; // "\r\n\r\n"
                        return true;
                    }
                }
                return false;
            }))
        {
            // ContentLength체크
            auto ContentLengthPos = HttpText.Find(0, gContentLengthText);
            if(ContentLengthPos != -1)
            {
                ContentLengthPos += 15; // "Content-Length:"
                while(HttpText[ContentLengthPos] == ' ') ContentLengthPos++;
                ContentLength = Parser::GetInt(((chars) HttpText) + ContentLengthPos);
            }
            // Connection체크
            KeepAlive = (HttpText.Find(0, gConnectionText) != -1);
            // TransferEncoding체크
            auto TransferEncodingPos = HttpText.Find(0, gTransferEncodingText);
            if(TransferEncodingPos != -1)
            {
                TransferEncodingPos += 18; // "Transfer-Encoding:"
                String EncodingCode;
                while(HttpText[TransferEncodingPos] == ' ') TransferEncodingPos++;
                while(boss_isalnum(HttpText[TransferEncodingPos])) EncodingCode += HttpText[TransferEncodingPos++];
                if(!EncodingCode.Compare("chunked")) Chunked = true;
                else BOSS_ASSERT("알 수 없는 TransferEncoding입니다", false);
            }
            // SetCookie체크
            sint32 CurCookiePos = 0, NextCookiePos = 0;
            while((NextCookiePos = HttpText.Find(CurCookiePos, gSetCookieText)) != -1)
            {
                CurCookiePos = NextCookiePos + 11; // "Set-Cookie:"
                String NewName, NewValue;
                while(HttpText[CurCookiePos] == ' ') CurCookiePos++;
                while(HttpText[CurCookiePos] != '=') NewName += HttpText[CurCookiePos++];
                while(HttpText[CurCookiePos] != '\r') NewValue += HttpText[CurCookiePos++];
                if(auto OldCookie = mLastContact->mCookieMap.Access(NewName))
                    (*OldCookie)->mValue = NewValue;
                else
                {
                    auto& NewCookie = mLastContact->mCookies.AtAdding();
                    NewCookie.mName = NewName;
                    NewCookie.mValue = NewValue;
                    mLastContact->mCookieMap(NewName) = &NewCookie;
                }
            }
        }
        else return SetLastError("Failed receive HTTP header.");

        // HTTP데이터 수신(기존)
        const sint32 CopyLen = HttpData.Count() - EndCodePos;
        if(0 < CopyLen) Memory::Copy(mResponseData.AtDumpingAdded(CopyLen), &HttpData.At(EndCodePos), CopyLen);
        const bool NeedMoreRecv = (Chunked)? (mResponseData.Count() < 5 || !!Memory::Compare(&mResponseData[-5], "0\r\n\r\n", 5))
            : (mResponseData.Count() < ContentLength);
        // HTTP데이터 수신(추가분)
        if(NeedMoreRecv)
        {
            if(Chunked)
            {
                if(!mLastContact->RecvLoop(mResponseData,
                    [&]()->bool {return (5 <= mResponseData.Count() && !Memory::Compare(&mResponseData[-5], "0\r\n\r\n", 5));}))
                    return SetLastError("Failed receive HTTP data.");
            }
            else
            {
                if(!mLastContact->RecvLoop(mResponseData,
                    [&]()->bool {return (ContentLength <= mResponseData.Count());}))
                    return SetLastError("Failed receive HTTP data.");
            }
        }

        // HTTP데이터 후가공
        if(Chunked)
        {
            chararray NewResponseData;
            chars ChunkPtr = &mResponseData[0];
            sint32 LastChunkSize = 0;
            for(sint32 i = 0, iend = mResponseData.Count(); i < iend; ++i)
            {
                const char CurChar = ChunkPtr[i];
                if('0' <= CurChar && CurChar <= '9') LastChunkSize = LastChunkSize * 16 + (CurChar - '0');
                else if('a' <= CurChar && CurChar <= 'f') LastChunkSize = LastChunkSize * 16 + 10 + (CurChar - 'a');
                else if('A' <= CurChar && CurChar <= 'F') LastChunkSize = LastChunkSize * 16 + 10 + (CurChar - 'A');
                else
                {
                    if(0 < LastChunkSize)
                        Memory::Copy(NewResponseData.AtDumpingAdded(LastChunkSize), &ChunkPtr[i + 2], LastChunkSize);
                    i += LastChunkSize + 4 - 1;
                    LastChunkSize = 0;
                }
            }
            mResponseData = ToReference(NewResponseData);
        }
        else if(ContentLength < mResponseData.Count())
            mResponseData.SubtractionSection(ContentLength, mResponseData.Count() - ContentLength);

        if(!KeepAlive) mLastContact->Disconnect();
        if(mResponseData.Count() == 0)
            return SetLastError("HTTP data is too small.");
        return true;
    }

    bool ParaSource::SetLastError(chars message)
    {
        BOSS_TRACE("ParaSource(error:%s)", message);
        mLastError = message;
        mLastContact->Disconnect();
        #if !BOSS_NDEBUG
            if(0 < mResponseData.Count())
                String(&mResponseData[0], mResponseData.Count()).ToAsset("parasource_error.txt");
        #endif
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ParaAsset
    ////////////////////////////////////////////////////////////////////////////////
    ParaAsset::ParaAsset()
    {
        mTasking = nullptr;
    }

    ParaAsset::~ParaAsset()
    {
        Tasking::Release(mTasking);
    }

    void ParaAsset::Init(chars script)
    {
        BOSS_ASSERT("Init는 한번만 가능합니다", !mTasking);
        if(mTasking) return;

        String Script = script;
        // <a href...> 코드의 제거
        sint32 APos = 0;
        while((APos = Script.Find(0, "<")) != -1)
        {
            const sint32 APosEnd = Script.Find(APos + 1, ">");
            if(APosEnd != -1)
                Script = Script.Left(APos) + Script.Right(Script.Length() - 1 - APosEnd);
        }
        chars ScriptPtr = Script;

        // 파싱
        sint32 NamePos = Script.Find(0, "[name]");
        String Name;
        if(NamePos != -1)
        {
            NamePos += 6; // [name]
            while(ScriptPtr[NamePos] == ' ') NamePos++;
            while(ScriptPtr[NamePos] != ' ' && ScriptPtr[NamePos] != '\0' && ScriptPtr[NamePos] != '[')
                Name += ScriptPtr[NamePos++];
        }
        sint32 UrlPos = Script.Find(0, "[url]");
        String Url;
        if(UrlPos != -1)
        {
            UrlPos += 5; // [url]
            while(ScriptPtr[UrlPos] == ' ') UrlPos++;
            while(ScriptPtr[UrlPos] != ' ' && ScriptPtr[UrlPos] != '\0' && ScriptPtr[UrlPos] != '[')
                Url += ScriptPtr[UrlPos++];
        }
        // 로딩
        if(0 < Name.Length() && 0 < Url.Length())
        {
            const String AssetName = "paraasset/" + Name;
            // 태스킹을 통한 캐시화
            if(Asset::Exist(AssetName) == roottype_null)
            {
                Strings* NewStrings = (Strings*) Buffer::Alloc<Strings>(BOSS_DBG 1);
                NewStrings->AtAdding() = AssetName;
                NewStrings->AtAdding() = Url;
                mTasking = Tasking::Create(
                    [](buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)->sint32
                    {
                        const Strings& CurStrings = *((Strings*) self);
                        if(!String::CompareNoCase(CurStrings[1], "http://", 7))
                        {
                            const String Url = CurStrings[1].Right(CurStrings[1].Length() - 7);
                            sint32 SlashPos = Url.Find(0, '/');
                            if(SlashPos != -1)
                            {
                                ParaSource Source(ParaSource::IIS);
                                Source.SetContact(Url.Left(SlashPos), 80);
                                uint08s Data;
                                if(Source.GetFile(Data, Url.Right(Url.Length() - 1 - SlashPos)))
                                {
                                    id_asset NewAsset = Asset::OpenForWrite(CurStrings[0], true);
                                    Asset::Write(NewAsset, &Data[0], Data.Count());
                                    Asset::Close(NewAsset);
                                    answer.Enqueue(((Share*)(id_share) CurStrings[0])->CopiedBuffer());
                                }
                            }
                        }
                        return -1;
                    }, (buffer) NewStrings);
            }
            // 캐시에서 로딩
            else InitForCache(AssetName);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ParaJson
    ////////////////////////////////////////////////////////////////////////////////
    ParaJson::ParaJson()
    {
        mContext = nullptr;
    }

    ParaJson::~ParaJson()
    {
        delete mContext;
    }

    Context* ParaJson::GetContext()
    {
        if(!mContext && mTasking)
        {
            // 태스킹후 캐시에서 로딩
            if(buffer Buffer = Tasking::GetAnswer(mTasking))
            {
                const String AssetName((id_cloned_share) Share::Create(Buffer));
                if(Asset::Exist(AssetName) != roottype_null)
                {
                    const String JsonText = String::FromAsset(AssetName);
                    mContext = new Context();
                    mContext->LoadJson(SO_NeedCopy, JsonText, JsonText.Length());
                }
            }
        }
        return mContext;
    }

    void ParaJson::InitForCache(chars assetname)
    {
        const String JsonText = String::FromAsset(assetname);
        mContext = new Context();
        mContext->LoadJson(SO_NeedCopy, JsonText, JsonText.Length());
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ParaView
    ////////////////////////////////////////////////////////////////////////////////
    ParaView::ParaView()
    {
        mLoadingMsec = 0;
    }

    ParaView::~ParaView()
    {

    }

    ZayPanel::SubRenderCB ParaView::GetRenderer()
    {
        return ZAY_RENDER_PN(p, n, this)
        {
            if(mImage.HasBitmap())
            {
                uint64 CurMsec = Platform::Utility::CurrentTimeMsec();
                if(CurMsec < mLoadingMsec + 1000)
                {
                    const sint32 Opacity = 128 - 128 * Math::Clamp(mLoadingMsec + 1000 - CurMsec, 0, 1000) / 1000;
                    ZAY_RGBA(p, 128, 128, 128, Opacity)
                        p.stretch(mImage, Image::Build::AsyncNotNull);
                }
                else p.stretch(mImage, Image::Build::AsyncNotNull);
            }
            else if(mTasking)
            {
                // 태스킹후 캐시에서 로딩
                if(buffer Buffer = Tasking::GetAnswer(mTasking))
                {
                    const String AssetName((id_cloned_share) Share::Create(Buffer));
                    if(Asset::Exist(AssetName) != roottype_null)
                    {
                        if(mImage.SetName(AssetName).Load())
                            mLoadingMsec = Platform::Utility::CurrentTimeMsec();
                    }
                }
            }
        };
    }

    void ParaView::InitForCache(chars assetname)
    {
        if(mImage.SetName(assetname).Load())
            mLoadingMsec = Platform::Utility::CurrentTimeMsec() - 1000;
    }
}
