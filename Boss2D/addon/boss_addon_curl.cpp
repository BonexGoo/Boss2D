#include <boss.h>

#if !defined(BOSS_NEED_ADDON_CURL) || (BOSS_NEED_ADDON_CURL != 0 && BOSS_NEED_ADDON_CURL != 1)
    #error BOSS_NEED_ADDON_CURL macro is invalid use
#endif
bool __LINK_ADDON_CURL__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_CURL

#include "boss_addon_curl.hpp"

#include <boss.hpp>
#include <platform/boss_platform.hpp>

extern "C"
{
    typedef void (RTMP_LogCallback)(int level, const char *fmt, va_list);
    void RTMP_LogSetCallback(RTMP_LogCallback *cb);
}

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(Curl, Create, id_curl, sint32)
    BOSS_DECLARE_ADDON_FUNCTION(Curl, CreateForUser, id_curl, chars, chars, sint32)
    BOSS_DECLARE_ADDON_FUNCTION(Curl, Clone, id_curl, id_curl)
    BOSS_DECLARE_ADDON_FUNCTION(Curl, Release, void, id_curl)
    BOSS_DECLARE_ADDON_FUNCTION(Curl, GetString, chars, id_curl, chars, chars, AddOn::Curl::SendType, chars, sint32)
    BOSS_DECLARE_ADDON_FUNCTION(Curl, GetBytes, bytes, id_curl, chars, sint32*, chars, AddOn::Curl::SendType, chars, sint32)
    BOSS_DECLARE_ADDON_FUNCTION(Curl, GetRedirectUrl, chars, id_curl, chars, sint32, chars, AddOn::Curl::SendType, chars, sint32)
    BOSS_DECLARE_ADDON_FUNCTION(Curl, SendStream, void, id_curl, chars, AddOn::Curl::CurlReadCB, payload)
    BOSS_DECLARE_ADDON_FUNCTION(Curl, FtpUpload, bool, id_curl, chars, chars, buffer)
    BOSS_DECLARE_ADDON_FUNCTION(Curl, FtpDownload, buffer, id_curl, chars, chars)
    BOSS_DECLARE_ADDON_FUNCTION(Curl, FtpDelete, bool, id_curl, chars, chars)
    BOSS_DECLARE_ADDON_FUNCTION(Curl, FtpCreateFolder, bool, id_curl, chars, chars)
    BOSS_DECLARE_ADDON_FUNCTION(Curl, FtpDeleteFolder, bool, id_curl, chars, chars)
    BOSS_DECLARE_ADDON_FUNCTION(Curl, FtpSearch, sint32, id_curl, chars, chars, AddOn::Curl::SearchCB, payload)

    static autorun Bind_AddOn_Curl()
    {
        Core_AddOn_Curl_Create() = Customized_AddOn_Curl_Create;
        Core_AddOn_Curl_CreateForUser() = Customized_AddOn_Curl_CreateForUser;
        Core_AddOn_Curl_Clone() = Customized_AddOn_Curl_Clone;
        Core_AddOn_Curl_Release() = Customized_AddOn_Curl_Release;
        Core_AddOn_Curl_GetString() = Customized_AddOn_Curl_GetString;
        Core_AddOn_Curl_GetBytes() = Customized_AddOn_Curl_GetBytes;
        Core_AddOn_Curl_GetRedirectUrl() = Customized_AddOn_Curl_GetRedirectUrl;
        Core_AddOn_Curl_SendStream() = Customized_AddOn_Curl_SendStream;
        Core_AddOn_Curl_FtpUpload() = Customized_AddOn_Curl_FtpUpload;
        Core_AddOn_Curl_FtpDownload() = Customized_AddOn_Curl_FtpDownload;
        Core_AddOn_Curl_FtpDelete() = Customized_AddOn_Curl_FtpDelete;
        Core_AddOn_Curl_FtpCreateFolder() = Customized_AddOn_Curl_FtpCreateFolder;
        Core_AddOn_Curl_FtpDeleteFolder() = Customized_AddOn_Curl_FtpDeleteFolder;
        Core_AddOn_Curl_FtpSearch() = Customized_AddOn_Curl_FtpSearch;

        // 로그연결
        #if !BOSS_NDEBUG
            RTMP_LogSetCallback(
                [](int level, const char* fmt, va_list args)->void
                {
                    char Temp[1024];
                    boss_vsnprintf(Temp, 1024, fmt, args);
                    BOSS_TRACE("[RTMP LV-%d] %s", level, Temp);
                });
        #endif
        return true;
    }
    static autorun _ = Bind_AddOn_Curl();
}

static size_t CurlWriteToResult(char* ptr, size_t size, size_t nitems, void* outstream)
{
    size_t realsize = size * nitems;
    *((String*) outstream) += String((chars) ptr, realsize);
    return realsize;
}

static size_t CurlReadFromAsset(void* ptr, size_t size, size_t nitems, void* stream)
{
    size_t realsize = Platform::File::Read((id_file_read) stream, (uint08*) ptr, size * nitems);
    BOSS_TRACE("CurlReadFromAsset: %d bytes", (sint32) realsize);
    return realsize;
}

static size_t CurlWriteForAssert(char* ptr, size_t size, size_t nitems, void* outstream)
{
    size_t realsize = size * nitems;
    BOSS_ASSERT((chars) ptr, false);
    return realsize;
}

static size_t CurlWriteToUint08s(char* ptr, size_t size, size_t nitems, void* outstream)
{
    size_t realsize = size * nitems;
    uint08s& Result = *((uint08s*) outstream);
    Memory::Copy(Result.AtDumpingAdded(realsize), ptr, realsize);
    return realsize;
}

// 구현부
namespace BOSS
{
    class CurlStruct
    {
    public:
        CurlStruct(sint32 timeout) : mTimeOut(timeout)
        {
            mId = nullptr;
            mRefCount = 0;
        }
        ~CurlStruct()
        {
        }

    public:
        const sint32 mTimeOut;
        CURL* mId;
        sint32 mRefCount;
        uint08s mCoreCacheBytes;
        String mRequestCacheString;
        uint08s mRequestCacheBytes;
        uint08s mFTPCacheBytes;
        chararray mFTPCacheSearch;
    };

    ////////////////////////////////////////////////////////////////////////////////
    id_curl Customized_AddOn_Curl_Create(sint32 timeout)
    {
        CurlStruct* NewStruct = new CurlStruct(timeout);
        NewStruct->mId = curl_easy_init();
        NewStruct->mRefCount = 1;
        return (id_curl) NewStruct;
    }

    ////////////////////////////////////////////////////////////////////////////////
    id_curl Customized_AddOn_Curl_CreateForUser(chars username, chars password, sint32 timeout)
    {
        CurlStruct* NewStruct = new CurlStruct(timeout);
        NewStruct->mId = curl_easy_init();
        NewStruct->mRefCount = 1;
        curl_easy_setopt(NewStruct->mId, CURLOPT_USERPWD,
            (chars) String::Format("%s:%s", username, password));
        return (id_curl) NewStruct;
    }

    ////////////////////////////////////////////////////////////////////////////////
    id_curl Customized_AddOn_Curl_Clone(id_curl curl)
    {
        if(!curl) return nullptr;
        CurlStruct* CurStruct = (CurlStruct*) curl;
        CurStruct->mRefCount++;
        return (id_curl) CurStruct;
    }

    ////////////////////////////////////////////////////////////////////////////////
    void Customized_AddOn_Curl_Release(id_curl curl)
    {
        if(!curl) return;
        CurlStruct* OldStruct = (CurlStruct*) curl;
        if(--OldStruct->mRefCount == 0)
        {
            curl_easy_cleanup(OldStruct->mId);
            delete OldStruct;
        }
    }

    static curl_slist* _MakeCHeader(curl_slist* cheader, chars url)
    {
        String Host;
        for(chars iurl = url + 2, ibegin = nullptr; true; ++iurl)
        {
            if(*iurl == '\0')
            {
                if(ibegin)
                    Host = String(ibegin, iurl - ibegin);
                break;
            }
            else if(*iurl == '/')
            {
                if(ibegin)
                {
                    Host = String(ibegin, iurl - ibegin);
                    break;
                }
                else if(iurl[-2] == ':' && iurl[-1] == '/')
                    ibegin = iurl + 1;
            }
        }
        Host = "Host: " + Host;

        String Referer;
        for(chars iurl = url; true; ++iurl)
        {
            if(*iurl == '\0' || *iurl == '?')
            {
                Referer = String(url, iurl - url);
                break;
            }
        }
        Referer = "Referer: " + Referer;

        cheader = curl_slist_append(cheader, (chars) Host);
        cheader = curl_slist_append(cheader, (chars) Referer);
        return cheader;
    }

    struct UploadData
    {
        const void* mData;
        sint32 mLength;
        sint32 mFocus;
    };

    static size_t OnUpload(void* ptr, size_t size, size_t nitems, payload data)
    {
        auto Data = (BOSS::UploadData*) data;
        const size_t CopyLen = Math::Min(Data->mLength - Data->mFocus, size * nitems);
        if(0 < CopyLen)
        {
            Memory::Copy((uint08*) ptr, Data->mData, CopyLen);
            Data->mFocus += CopyLen;
        }
        return CopyLen;
    }

    static uint08s _Request(id_curl curl, chars url, chars headerdata, AddOn::Curl::SendType sendtype, chars senddata, sint32 datalen, String* redirect_url, sint32 successcode, sint32 timeout)
    {
        if(!curl) return uint08s();
        while(true)
        {
            ((CurlStruct*) curl)->mCoreCacheBytes.SubtractionAll();
            CURL* CurCurl = ((CurlStruct*) curl)->mId;

            curl_easy_setopt(CurCurl, CURLOPT_URL, url);
            curl_easy_setopt(CurCurl, CURLOPT_SSL_VERIFYPEER, 0L);
            BOSS::UploadData PutData;
            if(sendtype == AddOn::Curl::ST_Put)
            {
                PutData.mLength = (datalen == -1)? boss_strlen(senddata) : datalen;
                PutData.mFocus = 0;
                PutData.mData = senddata;
                curl_easy_setopt(CurCurl, CURLOPT_UPLOAD, 1);
                curl_easy_setopt(CurCurl, CURLOPT_READDATA, &PutData);
                curl_easy_setopt(CurCurl, CURLOPT_READFUNCTION, BOSS::OnUpload);
                curl_easy_setopt(CurCurl, CURLOPT_INFILESIZE_LARGE, (curl_off_t) PutData.mLength);
            }
            else curl_easy_setopt(CurCurl, CURLOPT_UPLOAD, 0);
            if(sendtype == AddOn::Curl::ST_Post)
            {
                curl_easy_setopt(CurCurl, CURLOPT_POST, 1);
                curl_easy_setopt(CurCurl, CURLOPT_POSTFIELDS , senddata);
                curl_easy_setopt(CurCurl, CURLOPT_POSTFIELDSIZE, (datalen == -1)? boss_strlen(senddata) : datalen);
            }
            else curl_easy_setopt(CurCurl, CURLOPT_POST, 0);
            curl_easy_setopt(CurCurl, CURLOPT_WRITEDATA, &((CurlStruct*) curl)->mCoreCacheBytes);
            curl_easy_setopt(CurCurl, CURLOPT_WRITEFUNCTION, CurlWriteToUint08s);

            curl_slist* cheader = nullptr;
            cheader = BOSS::_MakeCHeader(cheader, url);
            cheader = curl_slist_append(cheader, "User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:47.0) Gecko/20100101 Firefox/47.0");
            cheader = curl_slist_append(cheader, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
            cheader = curl_slist_append(cheader, "Accept-Language: ko-kr,ko;q=0.8,en-us;q=0.5,en;q=0.3");
            cheader = curl_slist_append(cheader, "Connection: keep-alive");

            // 헤더데이터 추가분
            String HeaderData = (headerdata)? headerdata : "";
            if(HeaderData.Find(0, "Content-Type:") == -1)
                cheader = curl_slist_append(cheader, "Content-Type: application/x-www-form-urlencoded");
            if(0 < HeaderData.Length())
            {
                HeaderData.Replace("\r", "");
                HeaderData.Replace('\n', '\0');
                chars HeaderFocus = (chars) HeaderData;
                for(sint32 i = 0, iend = HeaderData.Length(); i < iend; ++i)
                {
                    cheader = curl_slist_append(cheader, &HeaderFocus[i]);
                    while(HeaderFocus[++i] != '\0');
                }
            }
            curl_easy_setopt(CurCurl, CURLOPT_HTTPHEADER, cheader);
            if(timeout != -1)
                curl_easy_setopt(CurCurl, CURLOPT_TIMEOUT, timeout);

            CURLcode res = curl_easy_perform(CurCurl);
            curl_slist_free_all(cheader);

            if(res == CURLE_OK)
            {
                long statLong = 0;
                if(CURLE_OK == curl_easy_getinfo(CurCurl, CURLINFO_HTTP_CODE, &statLong))
                if(redirect_url && statLong == successcode)
                {
                    char* location = nullptr;
                    curl_easy_getinfo(CurCurl, CURLINFO_REDIRECT_URL, &location);
                    *redirect_url = location;
                }
                break;
            }
            else if(timeout != -1)
                break;
        }
        return ((CurlStruct*) curl)->mCoreCacheBytes;
    }

    ////////////////////////////////////////////////////////////////////////////////
    chars Customized_AddOn_Curl_GetString(id_curl curl, chars url, chars headerdata, AddOn::Curl::SendType sendtype, chars senddata, sint32 datalen)
    {
        if(!curl) return nullptr;
        uint08s RequestResult = BOSS::_Request(curl, url, headerdata, sendtype, senddata, datalen, nullptr, 0, ((CurlStruct*) curl)->mTimeOut);
        if(RequestResult.Count() == 0) return "";

        ((CurlStruct*) curl)->mRequestCacheString = String((chars) RequestResult.AtDumping(0, 1), RequestResult.Count());
        return ((CurlStruct*) curl)->mRequestCacheString;
    }

    ////////////////////////////////////////////////////////////////////////////////
    bytes Customized_AddOn_Curl_GetBytes(id_curl curl, chars url, sint32* getsize, chars headerdata, AddOn::Curl::SendType sendtype, chars senddata, sint32 datalen)
    {
        if(!curl) return nullptr;
        ((CurlStruct*) curl)->mRequestCacheBytes = BOSS::_Request(curl, url, headerdata, sendtype, senddata, datalen, nullptr, 0, ((CurlStruct*) curl)->mTimeOut);
        if(getsize) *getsize = ((CurlStruct*) curl)->mRequestCacheBytes.Count();
        return ((CurlStruct*) curl)->mRequestCacheBytes.AtDumping(0, 1);
    }

    ////////////////////////////////////////////////////////////////////////////////
    chars Customized_AddOn_Curl_GetRedirectUrl(id_curl curl, chars url, sint32 successcode, chars headerdata, AddOn::Curl::SendType sendtype, chars senddata, sint32 datalen)
    {
        if(!curl) return nullptr;
        ((CurlStruct*) curl)->mRequestCacheString.Empty();
        BOSS::_Request(curl, url, headerdata, sendtype, senddata, datalen, &((CurlStruct*) curl)->mRequestCacheString, successcode, ((CurlStruct*) curl)->mTimeOut);
        return ((CurlStruct*) curl)->mRequestCacheString;
    }

    ////////////////////////////////////////////////////////////////////////////////
    void Customized_AddOn_Curl_SendStream(id_curl curl, chars url, AddOn::Curl::CurlReadCB cb, payload data)
    {
        if(!curl) return;
        CURL* CurCurl = ((CurlStruct*) curl)->mId;

        curl_easy_setopt(CurCurl, CURLOPT_URL, url);
        curl_easy_setopt(CurCurl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(CurCurl, CURLOPT_POST, 0);
        curl_easy_setopt(CurCurl, CURLOPT_UPLOAD, 1);
        curl_easy_setopt(CurCurl, CURLOPT_READDATA, data);
        curl_easy_setopt(CurCurl, CURLOPT_READFUNCTION, cb);
        curl_easy_setopt(CurCurl, CURLOPT_WRITEDATA, nullptr);
        curl_easy_setopt(CurCurl, CURLOPT_WRITEFUNCTION, CurlWriteForAssert);
        curl_easy_setopt(CurCurl, CURLOPT_VERBOSE, 1);
        curl_easy_perform(CurCurl);
    }

    ////////////////////////////////////////////////////////////////////////////////
    bool Customized_AddOn_Curl_FtpUpload(id_curl curl, chars url, chars filename, buffer data)
    {
        if(!curl)
        {
            Buffer::Free(data);
            return false;
        }
        CURL* CurCurl = ((CurlStruct*) curl)->mId;

        BOSS::UploadData NewData;
        NewData.mLength = Buffer::SizeOf(data) * Buffer::CountOf(data);
        NewData.mFocus = 0;
        NewData.mData = data;

        const String Url = String::Format("%s/%s.upload", url, filename);
        curl_easy_setopt(CurCurl, CURLOPT_URL, (chars) Url);
        curl_easy_setopt(CurCurl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(CurCurl, CURLOPT_POST, 0);
        curl_easy_setopt(CurCurl, CURLOPT_UPLOAD, 1);
        curl_easy_setopt(CurCurl, CURLOPT_READDATA, &NewData);
        curl_easy_setopt(CurCurl, CURLOPT_READFUNCTION, BOSS::OnUpload);
        curl_easy_setopt(CurCurl, CURLOPT_WRITEDATA, nullptr);
        curl_easy_setopt(CurCurl, CURLOPT_WRITEFUNCTION, nullptr);
        curl_easy_setopt(CurCurl, CURLOPT_INFILESIZE_LARGE, (curl_off_t) NewData.mLength);

        // 파일명을 수정
        curl_slist* cheader = nullptr;
        const String RNFR = String::Format("RNFR /%s.upload", filename);
        const String RNTO = String::Format("RNTO /%s", filename);
        cheader = curl_slist_append(cheader, (chars) RNFR);
        cheader = curl_slist_append(cheader, (chars) RNTO);
        curl_easy_setopt(CurCurl, CURLOPT_POSTQUOTE, cheader);

        CURLcode res = curl_easy_perform(CurCurl);
        curl_slist_free_all(cheader);
        Buffer::Free(data);
        return (res == CURLE_OK && NewData.mFocus == NewData.mLength);
    }

    size_t OnFtpDownload(char* ptr, size_t size, size_t nitems, void* outstream)
    {
        const sint32 CopyLen = (sint32) (size * nitems);
        Memory::Copy(((uint08s*) outstream)->AtDumpingAdded(CopyLen), ptr, CopyLen);
        return CopyLen;
    }

    ////////////////////////////////////////////////////////////////////////////////
    buffer Customized_AddOn_Curl_FtpDownload(id_curl curl, chars url, chars filename)
    {
        if(!curl) return nullptr;
        CURL* CurCurl = ((CurlStruct*) curl)->mId;

        ((CurlStruct*) curl)->mFTPCacheBytes.SubtractionAll();
        const String Url = String::Format("%s/%s", url, filename);
        curl_easy_setopt(CurCurl, CURLOPT_URL, (chars) Url);
        curl_easy_setopt(CurCurl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(CurCurl, CURLOPT_POST, 0);
        curl_easy_setopt(CurCurl, CURLOPT_UPLOAD, 0);
        curl_easy_setopt(CurCurl, CURLOPT_READDATA, nullptr);
        curl_easy_setopt(CurCurl, CURLOPT_READFUNCTION, nullptr);
        curl_easy_setopt(CurCurl, CURLOPT_WRITEDATA, (void*) &((CurlStruct*) curl)->mFTPCacheBytes);
        curl_easy_setopt(CurCurl, CURLOPT_WRITEFUNCTION, BOSS::OnFtpDownload);

        CURLcode res = curl_easy_perform(CurCurl);
        if(res != CURLE_OK) return nullptr;
        return ((Share*)(id_share) ((CurlStruct*) curl)->mFTPCacheBytes)->CopiedBuffer();
    }

    size_t OnFtpResult(char* ptr, size_t size, size_t nitems, void* outstream)
    {
        return size * nitems;
    }

    ////////////////////////////////////////////////////////////////////////////////
    bool Customized_AddOn_Curl_FtpDelete(id_curl curl, chars url, chars filename)
    {
        if(!curl) return false;
        CURL* CurCurl = ((CurlStruct*) curl)->mId;

        curl_easy_setopt(CurCurl, CURLOPT_URL, url);
        curl_easy_setopt(CurCurl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(CurCurl, CURLOPT_POST, 0);
        curl_easy_setopt(CurCurl, CURLOPT_UPLOAD, 0);
        curl_easy_setopt(CurCurl, CURLOPT_READDATA, nullptr);
        curl_easy_setopt(CurCurl, CURLOPT_READFUNCTION, nullptr);
        curl_easy_setopt(CurCurl, CURLOPT_WRITEDATA, nullptr);
        curl_easy_setopt(CurCurl, CURLOPT_WRITEFUNCTION, BOSS::OnFtpResult);

        // 파일을 제거
        curl_slist* cheader = nullptr;
        const String DELE = String::Format("DELE /%s", filename);
        cheader = curl_slist_append(cheader, (chars) DELE);
        curl_easy_setopt(CurCurl, CURLOPT_POSTQUOTE, cheader);

        CURLcode res = curl_easy_perform(CurCurl);
        curl_slist_free_all(cheader);
        return (res == CURLE_OK);
    }

    ////////////////////////////////////////////////////////////////////////////////
    bool Customized_AddOn_Curl_FtpCreateFolder(id_curl curl, chars url, chars dirname)
    {
        if(!curl) return false;
        CURL* CurCurl = ((CurlStruct*) curl)->mId;

        curl_easy_setopt(CurCurl, CURLOPT_URL, url);
        curl_easy_setopt(CurCurl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(CurCurl, CURLOPT_POST, 0);
        curl_easy_setopt(CurCurl, CURLOPT_UPLOAD, 0);
        curl_easy_setopt(CurCurl, CURLOPT_READDATA, nullptr);
        curl_easy_setopt(CurCurl, CURLOPT_READFUNCTION, nullptr);
        curl_easy_setopt(CurCurl, CURLOPT_WRITEDATA, nullptr);
        curl_easy_setopt(CurCurl, CURLOPT_WRITEFUNCTION, OnFtpResult);

        // 디렉터리 만들기
        curl_slist* cheader = nullptr;
        const String DELE = String::Format("MKD /%s", dirname);
        cheader = curl_slist_append(cheader, (chars) DELE);
        curl_easy_setopt(CurCurl, CURLOPT_POSTQUOTE, cheader);

        CURLcode res = curl_easy_perform(CurCurl);
        curl_slist_free_all(cheader);
        return (res == CURLE_OK);
    }

    ////////////////////////////////////////////////////////////////////////////////
    bool Customized_AddOn_Curl_FtpDeleteFolder(id_curl curl, chars url, chars dirname)
    {
        if(!curl) return false;
        CURL* CurCurl = ((CurlStruct*) curl)->mId;

        curl_easy_setopt(CurCurl, CURLOPT_URL, url);
        curl_easy_setopt(CurCurl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(CurCurl, CURLOPT_POST, 0);
        curl_easy_setopt(CurCurl, CURLOPT_UPLOAD, 0);
        curl_easy_setopt(CurCurl, CURLOPT_READDATA, nullptr);
        curl_easy_setopt(CurCurl, CURLOPT_READFUNCTION, nullptr);
        curl_easy_setopt(CurCurl, CURLOPT_WRITEDATA, nullptr);
        curl_easy_setopt(CurCurl, CURLOPT_WRITEFUNCTION, OnFtpResult);

        // 디렉터리 제거
        curl_slist* cheader = nullptr;
        const String DELE = String::Format("RMD /%s", dirname);
        cheader = curl_slist_append(cheader, (chars) DELE);
        curl_easy_setopt(CurCurl, CURLOPT_POSTQUOTE, cheader);

        CURLcode res = curl_easy_perform(CurCurl);
        curl_slist_free_all(cheader);
        return (res == CURLE_OK);
    }

    size_t OnFtpList(char* ptr, size_t size, size_t nitems, void* outstream)
    {
        const sint32 CopyLen = (sint32) (size * nitems);
        Memory::Copy(((chararray*) outstream)->AtDumpingAdded(CopyLen), ptr, CopyLen);
        return CopyLen;
    }

    ////////////////////////////////////////////////////////////////////////////////
    sint32 Customized_AddOn_Curl_FtpSearch(id_curl curl, chars url, chars dirname, AddOn::Curl::SearchCB cb, payload data)
    {
        if(!curl) return -1;
        CURL* CurCurl = ((CurlStruct*) curl)->mId;

        ((CurlStruct*) curl)->mFTPCacheSearch.SubtractionAll();
        const String Url = String::Format("%s/%s/", url, dirname);
        curl_easy_setopt(CurCurl, CURLOPT_URL, (chars) Url);
        curl_easy_setopt(CurCurl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(CurCurl, CURLOPT_POST, 0);
        curl_easy_setopt(CurCurl, CURLOPT_UPLOAD, 0);
        curl_easy_setopt(CurCurl, CURLOPT_READDATA, nullptr);
        curl_easy_setopt(CurCurl, CURLOPT_READFUNCTION, nullptr);
        curl_easy_setopt(CurCurl, CURLOPT_WRITEDATA, (void*) &((CurlStruct*) curl)->mFTPCacheSearch);
        curl_easy_setopt(CurCurl, CURLOPT_WRITEFUNCTION, BOSS::OnFtpList);

        CURLcode res = curl_easy_perform(CurCurl);
        if(res != CURLE_OK) return -1;

        sint32 Result = 0;
        ((CurlStruct*) curl)->mFTPCacheSearch.AtAdding() = '\0';
        chars SearchFocus = &((CurlStruct*) curl)->mFTPCacheSearch[0];
        String CollectorInPart;
        sint32 StepInPart = 0;
        sint32 SavedYear = 0, SavedMonth = 0, SavedDay = 0;
        sint32 SavedHour = 0, SavedMin = 0;
        sint32 SavedFilesize = 0;
        do
        {
            if(StepInPart < 3 && *SearchFocus == ' ')
            {
                if(0 < CollectorInPart.Length())
                {
                    switch(StepInPart++)
                    {
                    case 0: // 날짜(CollectorInPart의 예: "04-24-17")
                        if(CollectorInPart[2] == '-' && CollectorInPart[5] == '-')
                        {
                            SavedYear = 2000 + Parser::GetInt(&((chars) CollectorInPart)[6], 2);
                            SavedMonth = Parser::GetInt(&((chars) CollectorInPart)[0], 2);
                            SavedDay = Parser::GetInt(&((chars) CollectorInPart)[3], 2);
                        }
                        else BOSS_ASSERT("알 수 없는 형식입니다", false);
                        break;
                    case 1: // 시간(CollectorInPart의 예: "06:20PM")
                        if(CollectorInPart[2] == ':' && CollectorInPart[6] == 'M')
                        {
                            SavedHour = Parser::GetInt(&((chars) CollectorInPart)[0], 2)
                                + ((CollectorInPart[5] == 'P')? 12 : 0);
                            SavedMin = Parser::GetInt(&((chars) CollectorInPart)[3], 2);
                        }
                        else BOSS_ASSERT("알 수 없는 형식입니다", false);
                        break;
                    case 2: // 파일사이즈(-1은 폴더, CollectorInPart의 예: "3560" 또는 "<DIR>")
                        if(!CollectorInPart.CompareNoCase("<DIR>"))
                        {
                            SavedFilesize = -1;
                            while(SearchFocus[1] == ' ') ++SearchFocus; // 공백제거
                        }
                        else SavedFilesize = Parser::GetInt(CollectorInPart);
                        break;
                    }
                    CollectorInPart = "";
                }
            }
            else if(*SearchFocus == '\n')
            {
                Result++;
                StepInPart = 0;
                if(cb)
                {
                    id_clock NewClock = Platform::Clock::Create(
                        SavedYear, SavedMonth, SavedDay, SavedHour, SavedMin, 0, 0);
                    cb(data, CollectorInPart, SavedFilesize, NewClock);
                    Platform::Clock::Release(NewClock);
                }
                CollectorInPart = "";
            }
            else if(*SearchFocus != '\r')
                CollectorInPart += *SearchFocus;
        }
        while(*(++SearchFocus));
        return Result;
    }
}

#endif
