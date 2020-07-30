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
    BOSS_DECLARE_ADDON_FUNCTION(Zipa, Create, id_zipa, wchars, sint32*)
    BOSS_DECLARE_ADDON_FUNCTION(Zipa, Release, void, id_zipa)
    BOSS_DECLARE_ADDON_FUNCTION(Zipa, Extract, bool, id_zipa, sint32, wchars)

    static autorun Bind_AddOn_Zipa()
    {
        Core_AddOn_Zipa_Create() = Customized_AddOn_Zipa_Create;
        Core_AddOn_Zipa_Release() = Customized_AddOn_Zipa_Release;
        Core_AddOn_Zipa_Extract() = Customized_AddOn_Zipa_Extract;
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
        ZipaClass(wchars zippath, sint32* filecount)
        {
            sint32 LastSlash = 0;
            auto PathFocus = zippath;
            for(sint32 i = 0; PathFocus[i] != L'\0'; ++i)
                if(PathFocus[i] == L'/' || PathFocus[i] == L'\\')
                    LastSlash = i;
            mZipDir = WString(zippath, LastSlash);

            try
            {
                mZip.Open(zippath, CZipArchive::zipOpenReadOnly);
            }
            catch(CZipException e) {}
            if(filecount) *filecount = mZip.GetCount();
        }
        ~ZipaClass()
        {
            mZip.Close();
        }

    public:
        bool Extract(sint32 fileindex, wchars newzippath)
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

    private:
        WString mZipDir;
        CZipArchive mZip;
    };

    id_zipa Customized_AddOn_Zipa_Create(wchars zippath, sint32* filecount)
    {
        buffer NewBuffer = Buffer::AllocNoConstructorOnce<ZipaClass>(BOSS_DBG 1);
        BOSS_CONSTRUCTOR(NewBuffer, 0, ZipaClass, zippath, filecount);
        return (id_zipa) NewBuffer;
    }

    void Customized_AddOn_Zipa_Release(id_zipa zipa)
    {
        Buffer::Free((buffer) zipa);
    }

    bool Customized_AddOn_Zipa_Extract(id_zipa zipa, sint32 fileindex, wchars newzippath)
    {
        if(auto CurZipa = (ZipaClass*) zipa)
            return CurZipa->Extract(fileindex, newzippath);
        return false;
    }
}

#endif
