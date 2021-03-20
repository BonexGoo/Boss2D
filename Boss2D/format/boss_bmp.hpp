#pragma once
#include <boss.hpp>

namespace BOSS
{
    /// @brief 포맷-BMP
    class Bmp
    {
    public:
        typedef struct {uint32 size; sint16 param1; sint16 param2; uint32 offbits;} bitmapfile;
        typedef struct {uint32 size; sint32 width; sint32 height;
            sint16 planes; sint16 bitcount; uint32 compression; uint32 size_image;
            sint32 xpels_meter; sint32 ypels_meter; uint32 color_used; uint32 color_important;} bitmapinfo;
        typedef union {argb32 argb; struct {uint08 b; uint08 g; uint08 r; uint08 a;};} bitmappixel;

    public:
        static id_bitmap FromFile(chars filename);
        static void ToFile(id_bitmap_read bitmap, chars filename);
        static id_bitmap FromAsset(chars filename);
        static void ToAsset(id_bitmap_read bitmap, chars filename);

    public:
        static id_bitmap Create(sint32 bytesperpixel, sint32 width, sint32 height, sint16 param1 = 0, sint16 param2 = 0);
        static id_bitmap Clone(id_bitmap_read bitmap);
        // 안드로이드 일부폰에는 자주 큰 메모리를 잡고 해제하면 프레임이 현저하게 나빠지니 oldbitmap을 활용할 것!
        static id_bitmap CloneFromBits(bytes bits, sint32 width, sint32 height, sint32 bitcount, orientationtype ori, id_bitmap oldbitmap = nullptr);
        static id_bitmap CloneFromNV21(bytes ys, uv16s uvs, sint32 width, sint32 height);
        static id_bitmap Copy(id_bitmap_read bitmap, sint32 l, sint32 t, sint32 r, sint32 b);
        static void FillColor(id_bitmap bitmap, argb32 color);
        static void FillRect(id_bitmap bitmap, argb32 color, rect128 rect);
        static void ChangeColor(id_bitmap bitmap, argb32 from, argb32 to);
        static void SwapRedBlue(id_bitmap bitmap);
        static void Flatten(id_bitmap bitmap, uint08 r, uint08 g, uint08 b);
        static void Remove(id_bitmap bitmap);

    public:
        static void MinTest(id_bitmap_read bitmap, rect128& recttest);
        static void MaxTest(id_bitmap_read bitmap, rect128& recttest);
        static void MaxTestWithBgColor(id_bitmap_read bitmap, rect128& recttest, argb32 bgcolor);

    public:
        static uint32 GetFileSizeWithoutBM(id_bitmap_read bitmap);
        static sint16 GetParam1(id_bitmap_read bitmap);
        static sint16 GetParam2(id_bitmap_read bitmap);
        static sint32 GetWidth(id_bitmap_read bitmap);
        static sint32 GetHeight(id_bitmap_read bitmap);
        static sint16 GetBitCount(id_bitmap_read bitmap);
        static uint32 GetImageSize(id_bitmap_read bitmap);
        static bytes GetBits(id_bitmap_read bitmap);
        static uint08* GetBits(id_bitmap bitmap);
    };
}
