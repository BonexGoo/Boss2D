#include <boss.hpp>
#include "boss_boxr.hpp"

#include <format/boss_bmp.hpp>
#include <format/boss_png.hpp>

namespace BOSS
{
    BoxrBuilder::BoxrBuilder()
    {
    }

    BoxrBuilder::~BoxrBuilder()
    {
    }

    bool BoxrBuilder::LoadAtlas(chars key_filename, chars map_filename, bool keep_collection)
    {
        id_bitmap KeyBitmap = MakeBitmap(key_filename);
        id_bitmap MapBitmap = MakeBitmap(map_filename);
        if(!KeyBitmap || !MapBitmap)
        {
            Bmp::Remove(KeyBitmap);
            Bmp::Remove(MapBitmap);
            return false;
        }

        // Make key bitmaps
        Array<id_bitmap> Keys;
        const sint32 KeyWidth = Bmp::GetWidth(KeyBitmap);
        const sint32 KeyHeight = Bmp::GetHeight(KeyBitmap);
        const Bmp::bitmappixel* KeyBits = (const Bmp::bitmappixel*) Bmp::GetBits(KeyBitmap);
        const argb32 SplitColor = KeyBits[0].argb;
        for(sint32 i = 1, oldi = 1; i < KeyWidth; ++i)
        {
            if(KeyBits[i].argb == SplitColor)
            {
                if(oldi < i)
                    Keys.AtAdding() = Bmp::Copy(KeyBitmap, oldi, 0, i, KeyHeight);
                oldi = i + 1;
            }
        }

        // Find key-head in map
        Array<point64> KeyHeads;
        const sint32 KeyHeadWidth = Bmp::GetWidth(Keys[0]);
        const sint32 KeyHeadHeight = Bmp::GetHeight(Keys[0]);
        const Bmp::bitmappixel* KeyHeadBits = (const Bmp::bitmappixel*) Bmp::GetBits(Keys[0]);
        const sint32 MapWidth = Bmp::GetWidth(MapBitmap);
        const sint32 MapHeight = Bmp::GetHeight(MapBitmap);
        const Bmp::bitmappixel* MapBits = (const Bmp::bitmappixel*) Bmp::GetBits(MapBitmap);
        for(sint32 y = 0, yend = MapHeight - KeyHeadHeight; y < yend; ++y)
        for(sint32 x = 0, xend = MapWidth - KeyHeadWidth; x < xend; ++x)
        {
            bool IsFinded = true;
            for(sint32 hy = 0; hy < KeyHeadHeight && IsFinded; ++hy)
            for(sint32 hx = 0; hx < KeyHeadWidth; ++hx)
                if(MapBits[(x + hx) + (y + hy) * MapWidth].argb !=
                    KeyHeadBits[hx + hy * KeyHeadWidth].argb)
                {
                    IsFinded = false;
                    break;
                }
            if(IsFinded)
            {
                KeyHeads.AtAdding();
                KeyHeads.At(-1).x = x;
                KeyHeads.At(-1).y = y;
            }
        }

        // Find filename by key-head
        Array<String> FileNames;
        const char KeyChars[] = {
            'a', 'b', 'c', 'd', 'e', 'f', 'g',
            'h', 'i', 'j', 'k', 'l', 'm', 'n',
            'o', 'p', 'q', 'r', 's', 't', 'u',
            'v', 'w', 'x', 'y', 'z', '_', '.'};
        const sint32 KeyTailWidth = Bmp::GetWidth(Keys[-1]);
        for(sint32 i = 0; i < KeyHeads.Count(); ++i)
        {
            FileNames.AtAdding();
            const sint32 jxbegin = KeyHeads[i].x + KeyHeadWidth;
            const sint32 jybegin = KeyHeads[i].y;
            sint32 jxlast = jxbegin;
            for(sint32 jx = jxbegin; jx < MapWidth; ++jx)
            {
                if(KeyTailWidth < jx - jxlast) break;
                else for(sint32 k = 0, kend = sizeof(KeyChars) / sizeof(KeyChars[0]); k < kend; ++k)
                {
                    const sint32 kWidth = Bmp::GetWidth(Keys[k + 1]);
                    const sint32 kHeight = Bmp::GetHeight(Keys[k + 1]);
                    const Bmp::bitmappixel* kBits = (const Bmp::bitmappixel*) Bmp::GetBits(Keys[k + 1]);
                    if(jx + kWidth <= MapWidth)
                    {
                        bool IsFinded = true;
                        for(sint32 ky = 0; ky < kHeight && IsFinded; ++ky)
                        for(sint32 kx = 0; kx < kWidth; ++kx)
                            if(MapBits[(jx + kx) + (jybegin + ky) * MapWidth].argb !=
                                kBits[kx + ky * kWidth].argb)
                            {
                                IsFinded = false;
                                break;
                            }
                        if(IsFinded)
                        {
                            FileNames.At(-1) += KeyChars[k];
                            jxlast = jx + kWidth;
                            jx = jx + kWidth - 1;
                            break;
                        }
                    }
                }
            }
        }

        // Find subimage by key-head
        Array<rect128> SubImageUVs;
        const argb32 BgColor = MapBits[0].argb;
        for(sint32 i = 0; i < KeyHeads.Count(); ++i)
        {
            SubImageUVs.AtAdding();
            const sint32 jxbegin = KeyHeads[i].x + KeyHeadWidth / 2;
            const sint32 jybegin = KeyHeads[i].y;
            for(sint32 jy = jybegin - 1; 0 <= jy; --jy)
            {
                const Bmp::bitmappixel& CurColor = MapBits[jxbegin + jy * MapWidth];
                if(CurColor.a && CurColor.argb != BgColor)
                {
                    rect128 RectTest = {jxbegin, MapHeight - 1 - jy, jxbegin + 1, MapHeight - jy};
                    Bmp::MaxTestWithBgColor(MapBitmap, RectTest, BgColor);
                    SubImageUVs.At(-1).l = RectTest.l;
                    SubImageUVs.At(-1).t = RectTest.t;
                    SubImageUVs.At(-1).r = RectTest.r;
                    SubImageUVs.At(-1).b = RectTest.b;
                    break;
                }
            }
        }

        if(!keep_collection)
            m_subimages.Clear();

        // Save subimage
        for(sint32 i = 0; i < KeyHeads.Count(); ++i)
        {
            Image& NewImage = m_subimages.AtAdding().SetName(FileNames[i]);
            id_bitmap NewBitmap = Bmp::Copy(MapBitmap, SubImageUVs[i].l, SubImageUVs[i].t, SubImageUVs[i].r, SubImageUVs[i].b);
            const sint32 NewWidth = Bmp::GetWidth(NewBitmap);
            const sint32 NewHeight = Bmp::GetHeight(NewBitmap);
            const Bmp::bitmappixel* NewBits = (const Bmp::bitmappixel*) Bmp::GetBits(NewBitmap);

            if(NewBits[(NewHeight - 1) * NewWidth].argb == 0xFFFF0000)
                NewImage.LoadUIBitmap(NewBitmap);
            else NewImage.LoadBitmap(NewBitmap);
            NewImage.ChangeToMagentaAlpha();
            Bmp::Remove(NewBitmap);
        }

        Bmp::Remove(KeyBitmap);
        Bmp::Remove(MapBitmap);
        for(sint32 i = 0; i < Keys.Count(); ++i)
            Bmp::Remove(Keys[i]);
        return true;
    }

    void BoxrBuilder::SaveSubImages(chars pathname) const
    {
        for(sint32 i = 0; i < m_subimages.Count(); ++i)
            m_subimages[i].Save(pathname);
    }

    id_bitmap BoxrBuilder::MakeBitmap(chars png_filename, id_assetpath_read assetpath)
    {
        id_bitmap Result = nullptr;
        if(id_asset_read PngAsset = Asset::OpenForRead(png_filename, assetpath))
        {
            const sint32 PngBufferSize = Asset::Size(PngAsset);
            uint08* PngBuffer = (uint08*) Memory::Alloc(PngBufferSize);
            Asset::Read(PngAsset, PngBuffer, PngBufferSize);
            Asset::Close(PngAsset);
            Result = Png().ToBmp(PngBuffer, true);
            Memory::Free(PngBuffer);
        }
        return Result;
    }

    id_bitmap BoxrBuilder::MakeTagBitmap(chars key_filename, chars tagname)
    {
        id_bitmap KeyBitmap = MakeBitmap(key_filename);
        if(!KeyBitmap)
        {
            Bmp::Remove(KeyBitmap);
            return nullptr;
        }

        // Make key bitmaps
        Array<id_bitmap> Keys;
        const sint32 KeyWidth = Bmp::GetWidth(KeyBitmap);
        const sint32 KeyHeight = Bmp::GetHeight(KeyBitmap);
        const Bmp::bitmappixel* KeyBits = (const Bmp::bitmappixel*) Bmp::GetBits(KeyBitmap);
        const argb32 SplitColor = KeyBits[0].argb;
        for(sint32 i = 1, oldi = 1; i < KeyWidth; ++i)
        {
            if(KeyBits[i].argb == SplitColor)
            {
                if(oldi < i)
                    Keys.AtAdding() = Bmp::Copy(KeyBitmap, oldi, 0, i, KeyHeight);
                oldi = i + 1;
            }
        }

        // Match key bitmaps
        Array<sint32> MatchedIndex;
        sint32 DstWidth = 0;
        sint32 DstHeight = 0;
        const char KeyChars[29] = {
            '~', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
            'h', 'i', 'j', 'k', 'l', 'm', 'n',
            'o', 'p', 'q', 'r', 's', 't', 'u',
            'v', 'w', 'x', 'y', 'z', '_', '.'};
        char CurTagChar = '~';
        while(CurTagChar != '\0')
        {
            for(sint32 i = 0, iend = sizeof(KeyChars) / sizeof(char); i < iend; ++i)
            {
                if(CurTagChar == KeyChars[i] && i < Keys.Count())
                {
                    MatchedIndex.AtAdding() = i;
                    DstWidth += Bmp::GetWidth(Keys[i]) + 1;
                    DstHeight = Math::Max(DstHeight, Bmp::GetHeight(Keys[i]));
                }
            }
            CurTagChar = *(tagname++);
        }

        // Copy result bitmap
        id_bitmap DstBitmap = Bmp::Create(3, DstWidth, DstHeight);
        uint08* DstBitmapBits = Bmp::GetBits(DstBitmap);
        const sint32 DstBitmapRow = (3 * DstWidth + 3) & ~3;
        Memory::Set(DstBitmapBits, 0xFF, DstBitmapRow * DstHeight);
        sint32 DstPos = 0;
        for(sint32 i = 0, iend = MatchedIndex.Count(); i < iend; ++i)
        {
            id_bitmap_read SrcBitmap = Keys[MatchedIndex[i]];
            Bmp::bitmappixel* SrcBitmapBits = (Bmp::bitmappixel*) Bmp::GetBits(SrcBitmap);
            const sint32 SrcWidth = Bmp::GetWidth(SrcBitmap);
            const sint32 SrcHeight = Bmp::GetHeight(SrcBitmap);
            for(sint32 y = 0; y < SrcHeight; ++y)
            {
                uint08* DstBits = &DstBitmapBits[3 * DstPos + DstBitmapRow * (DstHeight - 1 - y)];
                const Bmp::bitmappixel* SrcBits = &SrcBitmapBits[SrcWidth * (SrcHeight - 1 - y)];
                for(sint32 x = 0; x < SrcWidth; ++x)
                {
                    *(DstBits++) = SrcBits->b;
                    *(DstBits++) = SrcBits->g;
                    *(DstBits++) = SrcBits->r;
                    SrcBits++;
                }
            }
            DstPos += SrcWidth + 1;
        }

        Bmp::Remove(KeyBitmap);
        for(sint32 i = 0; i < Keys.Count(); ++i)
            Bmp::Remove(Keys[i]);
        return DstBitmap;
    }
}
