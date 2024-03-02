#pragma once
#include <element/boss_image.hpp>

namespace BOSS
{
    /// @brief 복서빌더
    class BoxrBuilder
    {
    public:
        BoxrBuilder();
        ~BoxrBuilder();
        BoxrBuilder& operator=(const BoxrBuilder&)
        {
            BOSS_ASSERT("해당 객체는 복사할 수 없습니다", false);
            return *this;
        }

    public:
        bool LoadAtlas(chars key_filename, chars map_filename, bool keep_collection = false, sint32 vercode = 1);
        Strings LoadAtlasBitmap(id_bitmap_read key_bitmap, id_bitmap_read map_bitmap, bool keep_collection = false, sint32 vercode = 1);
        void SaveSubImages(chars pathname = "") const;

    public:
        static id_bitmap MakeBitmap(chars png_filename, id_assetpath_read assetpath = nullptr);
        static id_bitmap MakeTagBitmap(chars key_filename, chars tagname, sint32 vercode = 1);

    private:
        Array<Image> m_subimages;
    };
}
