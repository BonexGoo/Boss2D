#include <boss.h>

#if !defined(BOSS_NEED_ADDON_DLIB) || (BOSS_NEED_ADDON_DLIB != 0 && BOSS_NEED_ADDON_DLIB != 1)
    #error BOSS_NEED_ADDON_DLIB macro is invalid use
#endif
bool __LINK_ADDON_DLIB__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_DLIB

#include "boss_addon_dlib.hpp"

#include <boss.hpp>
#include <platform/boss_platform.hpp>

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(Dlib, Create, id_dlib, void)
    BOSS_DECLARE_ADDON_FUNCTION(Dlib, Release, void, id_dlib)
    BOSS_DECLARE_ADDON_FUNCTION(Dlib, Update, void, id_dlib, id_bitmap_read)
    BOSS_DECLARE_ADDON_FUNCTION(Dlib, GetFaceLandmark, const point64*, id_dlib, AddOn::Dlib::FaceLandmark68Type)

    static autorun Bind_AddOn_Dlib()
    {
        Core_AddOn_Dlib_Create() = Customized_AddOn_Dlib_Create;
        Core_AddOn_Dlib_Release() = Customized_AddOn_Dlib_Release;
        Core_AddOn_Dlib_Update() = Customized_AddOn_Dlib_Update;
        Core_AddOn_Dlib_GetFaceLandmark() = Customized_AddOn_Dlib_GetFaceLandmark;
        return true;
    }
    static autorun _ = Bind_AddOn_Dlib();
}

// 구현부
namespace BOSS
{
    id_dlib Customized_AddOn_Dlib_Create(void)
    {
        return (id_dlib) nullptr;
    }

    void Customized_AddOn_Dlib_Release(id_dlib dlib)
    {
    }

    void Customized_AddOn_Dlib_Update(id_dlib dlib, id_bitmap_read bmp)
    {
    }

    const point64* Customized_AddOn_Dlib_GetFaceLandmark(id_dlib dlib, AddOn::Dlib::FaceLandmark68Type type)
    {
        return nullptr;
    }
}

#endif
