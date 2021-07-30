#include <boss.h>

#if !defined(BOSS_NEED_ADDON_ZIPA) || (BOSS_NEED_ADDON_ZIPA != 0 && BOSS_NEED_ADDON_ZIPA != 1)
    #error BOSS_NEED_ADDON_ZIPA macro is invalid use
#endif
bool __LINK_ADDON_ZIPA__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_ZIPA

#include "boss_addon_zipa.hpp"

#include <boss.hpp>

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(Zipa, Open, id_zipa, wchars, sint32*, chars)
    BOSS_DECLARE_ADDON_FUNCTION(Zipa, Close, void, id_zipa)
    BOSS_DECLARE_ADDON_FUNCTION(Zipa, ExtractFile, bool, id_zipa, sint32, wchars)
    BOSS_DECLARE_ADDON_FUNCTION(Zipa, ToBuffer, buffer, id_zipa, sint32)
    BOSS_DECLARE_ADDON_FUNCTION(Zipa, GetFileInfo, chars, id_zipa, sint32, uint64*,
        bool*, uint64*, uint64*, uint64*, bool*, bool*, bool*, bool*)

    static autorun Bind_AddOn_Zipa()
    {
        Core_AddOn_Zipa_Open() = Customized_AddOn_Zipa_Open;
        Core_AddOn_Zipa_Close() = Customized_AddOn_Zipa_Close;
        Core_AddOn_Zipa_ExtractFile() = Customized_AddOn_Zipa_ExtractFile;
        Core_AddOn_Zipa_ToBuffer() = Customized_AddOn_Zipa_ToBuffer;
        Core_AddOn_Zipa_GetFileInfo() = Customized_AddOn_Zipa_GetFileInfo;
        return true;
    }
    static autorun _ = Bind_AddOn_Zipa();
}

// 구현부
namespace BOSS
{
    class ZipaClass
    {
    public:
        ZipaClass()
        {
        }
        ZipaClass(wchars zippath, sint32* filecount, chars extension)
        {
            sint32 LastSlash = 0;
            auto PathFocus = zippath;
            for(sint32 i = 0; PathFocus[i] != L'\0'; ++i)
                if(PathFocus[i] == L'/' || PathFocus[i] == L'\\')
                    LastSlash = i;
            mZipDir = WString(zippath, LastSlash);

            try
            {
                if(!String::FromWChars(zippath).Right(4).CompareNoCase(extension))
                    mZip.Open(zippath, CZipArchive::zipOpenReadOnly);
                else mZip.Open(zippath, CZipArchive::zipOpenBinSplit);
            }
            catch(CZipException e)
            {
                auto ErrorW = e.GetErrorDescription();
                String Error = String::FromWChars((LPCTSTR) ErrorW);
                BOSS_ASSERT(Error, false);
            }
            if(filecount) *filecount = mZip.GetCount();
        }
        ~ZipaClass()
        {
            mZip.Close();
        }

    public:
        bool ExtractFile(sint32 fileindex, wchars newzippath)
        {
            bool Result = false;
            if(newzippath)
            {
                sint32 LastSlash = 0;
                auto PathFocus = newzippath;
                for(sint32 i = 0; PathFocus[i] != L'\0'; ++i)
                    if(PathFocus[i] == L'/' || PathFocus[i] == L'\\')
                        LastSlash = i;

                try
                {
                    Result = mZip.ExtractFile(fileindex,
                        WString(newzippath, LastSlash), true, WString(newzippath + LastSlash + 1));
                }
                catch(CZipException e)
                {
                    auto ErrorW = e.GetErrorDescription();
                    String Error = String::FromWChars((LPCTSTR) ErrorW);
                    BOSS_ASSERT(Error, false);
                    return false;
                }
            }
            else
            {
                try
                {
                    Result = mZip.ExtractFile(fileindex, mZipDir);
                }
                catch(CZipException e)
                {
                    auto ErrorW = e.GetErrorDescription();
                    String Error = String::FromWChars((LPCTSTR) ErrorW);
                    BOSS_ASSERT(Error, false);
                    return false;
                }
            }
            return Result;
        }

        buffer ToBuffer(sint32 fileindex)
        {
            buffer Result = nullptr;
            try
            {
                if(const auto FileInfo = mZip.GetFileInfo(fileindex))
                {
                    if(mZip.OpenFile(fileindex))
                    {
                        Result = Buffer::Alloc(BOSS_DBG 0);
                        for(DWORD ReadSize = 1024 * 64; ReadSize == 1024 * 64;)
                        {
                            uint08 Temp[1024 * 64];
                            ReadSize = mZip.ReadFile(Temp, 1024 * 64);
                            if(0 < ReadSize)
                            {
                                auto OldSize = Buffer::CountOf(Result);
                                Result = Buffer::Realloc(BOSS_DBG Result, OldSize + ReadSize);
                                Memory::Copy(&((uint08*) Result)[OldSize], Temp, ReadSize);
                            }
                        }
                        mZip.CloseFile();
                    }
                }
            }
            catch(CZipException e)
            {
                auto ErrorW = e.GetErrorDescription();
                String Error = String::FromWChars((LPCTSTR) ErrorW);
                BOSS_ASSERT(Error, false);
                return Result;
            }
            return Result;
        }

        chars GetFileInfo(sint32 fileindex, uint64* filesize,
            bool* isdir, uint64* ctime, uint64* mtime, uint64* atime,
            bool* archive, bool* hidden, bool* readonly, bool* system)
        {
            try
            {
                if(const auto FileInfo = mZip.GetFileInfo(fileindex))
                {
                    const DWORD SystemAttr = FileInfo->GetSystemAttr();
                    if(filesize) *filesize = FileInfo->m_uUncomprSize;
                    if(isdir) *isdir = ((SystemAttr & FILE_ATTRIBUTE_DIRECTORY) != 0);
                    if(ctime) *ctime = (uint64) FileInfo->GetCreationTime();
                    if(mtime) *mtime = (uint64) FileInfo->GetModificationTime();
                    if(atime) *atime = (uint64) FileInfo->GetLastAccessTime();
                    if(archive) *archive = ((SystemAttr & FILE_ATTRIBUTE_ARCHIVE) != 0);
                    if(hidden) *hidden = ((SystemAttr & FILE_ATTRIBUTE_HIDDEN) != 0);
                    if(readonly) *readonly = ((SystemAttr & FILE_ATTRIBUTE_READONLY) != 0);
                    if(system) *system = ((SystemAttr & FILE_ATTRIBUTE_SYSTEM) != 0);

                    static String LastFileName;
                    LastFileName = String::FromWChars((LPCWSTR)(LPCTSTR) FileInfo->GetFileName());
                    return LastFileName;
                }
            }
            catch(CZipException e)
            {
                auto ErrorW = e.GetErrorDescription();
                String Error = String::FromWChars((LPCTSTR) ErrorW);
                BOSS_ASSERT(Error, false);
                return nullptr;
            }
            return nullptr;
        }

    private:
        WString mZipDir;
        CZipArchive mZip;
    };

    id_zipa Customized_AddOn_Zipa_Open(wchars zippath, sint32* filecount, chars extension)
    {
        buffer NewBuffer = Buffer::AllocNoConstructorOnce<ZipaClass>(BOSS_DBG 1);
        BOSS_CONSTRUCTOR(NewBuffer, 0, ZipaClass, zippath, filecount, extension);
        return (id_zipa) NewBuffer;
    }

    void Customized_AddOn_Zipa_Close(id_zipa zipa)
    {
        Buffer::Free((buffer) zipa);
    }

    bool Customized_AddOn_Zipa_ExtractFile(id_zipa zipa, sint32 fileindex, wchars newzippath)
    {
        if(auto CurZipa = (ZipaClass*) zipa)
            return CurZipa->ExtractFile(fileindex, newzippath);
        return false;
    }

    buffer Customized_AddOn_Zipa_ToBuffer(id_zipa zipa, sint32 fileindex)
    {
        if(auto CurZipa = (ZipaClass*) zipa)
            return CurZipa->ToBuffer(fileindex);
        return nullptr;
    }

    chars Customized_AddOn_Zipa_GetFileInfo(id_zipa zipa, sint32 fileindex, uint64* filesize,
        bool* isdir, uint64* ctime, uint64* mtime, uint64* atime,
        bool* archive, bool* hidden, bool* readonly, bool* system)
    {
        if(auto CurZipa = (ZipaClass*) zipa)
            return CurZipa->GetFileInfo(fileindex, filesize,
                isdir, ctime, mtime, atime, archive, hidden, readonly, system);
        return nullptr;
    }
}

#endif
