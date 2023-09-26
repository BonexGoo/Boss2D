#pragma once
#include <element/boss_image.hpp>

namespace BOSS
{
    class R
    {
    public:
        R(chars name);
        ~R();

    public:
        static void SetAssetPath(id_assetpath assetpath);
        static void SetAtlasDir(chars dirname);
        static void AddAtlas(chars key_filename, chars map_filename, const Context& ctx);
        static void SaveAtlas(Context& ctx);
        static bool IsAtlasUpdated();
        static void RebuildAll();
        static void ClearImages(Strings names);

    public:
        inline bool exist() const {return mExist;}
        inline sint32 w() const {return (mImage)? mImage->GetWidth() : 0;}
        inline sint32 h() const {return (mImage)? mImage->GetHeight() : 0;}

    public:
        inline operator const Image&() const
        {
            BOSS_ASSERT("이미지가 로드되지 않았습니다", mImage);
            return *mImage;
        }
        inline const Image* operator->() const
        {
            BOSS_ASSERT("이미지가 로드되지 않았습니다", mImage);
            return mImage;
        }

    private:
        Image* mImage;
        bool mExist;
    };
}
