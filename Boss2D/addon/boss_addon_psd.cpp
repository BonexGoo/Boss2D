#include <boss.h>

#if !defined(BOSS_NEED_ADDON_PSD) || (BOSS_NEED_ADDON_PSD != 0 && BOSS_NEED_ADDON_PSD != 1)
    #error BOSS_NEED_ADDON_PSD macro is invalid use
#endif
bool __LINK_ADDON_PSD__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_PSD

#include "boss_addon_psd.hpp"

#include <boss.hpp>

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(Psd, Create, id_psd, void)
    BOSS_DECLARE_ADDON_FUNCTION(Psd, Release, void, id_psd)
    BOSS_DECLARE_ADDON_FUNCTION(Psd, Insert, void, id_psd, id_bitmap_read, sint32, sint32)
    BOSS_DECLARE_ADDON_FUNCTION(Psd, Build, buffer, id_psd)

    static autorun Bind_AddOn_Psd()
    {
        Core_AddOn_Psd_Create() = Customized_AddOn_Psd_Create;
        Core_AddOn_Psd_Release() = Customized_AddOn_Psd_Release;
        Core_AddOn_Psd_Insert() = Customized_AddOn_Psd_Insert;
        Core_AddOn_Psd_Build() = Customized_AddOn_Psd_Build;
        return true;
    }
    static autorun _ = Bind_AddOn_Psd();
}

// 구현부
namespace BOSS
{
    class PsdClass
    {
    public:
        PsdClass() {}
        ~PsdClass() {}

    public:
    };

    id_psd Customized_AddOn_Psd_Create(void)
    {
        auto NewPsd = (PsdClass*) Buffer::Alloc<PsdClass>(BOSS_DBG 1);
        return (id_psd) NewPsd;
    }

    void Customized_AddOn_Psd_Release(id_psd psd)
    {
        Buffer::Free((buffer) psd);
    }

    void Customized_AddOn_Psd_Insert(id_psd psd, id_bitmap_read bmp, sint32 x, sint32 y)
    {
    }

    buffer Customized_AddOn_Psd_Build(id_psd psd)
    {
        return nullptr;
    }
}

#endif
