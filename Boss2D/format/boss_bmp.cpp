#include <boss.hpp>
#include "boss_bmp.hpp"

#define CASE256(A) \
	case 0x00: A case 0x01: A case 0x02: A case 0x03: A \
	case 0x04: A case 0x05: A case 0x06: A case 0x07: A \
	case 0x08: A case 0x09: A case 0x0A: A case 0x0B: A \
	case 0x0C: A case 0x0D: A case 0x0E: A case 0x0F: A \
	case 0x10: A case 0x11: A case 0x12: A case 0x13: A \
	case 0x14: A case 0x15: A case 0x16: A case 0x17: A \
	case 0x18: A case 0x19: A case 0x1A: A case 0x1B: A \
	case 0x1C: A case 0x1D: A case 0x1E: A case 0x1F: A \
	case 0x20: A case 0x21: A case 0x22: A case 0x23: A \
	case 0x24: A case 0x25: A case 0x26: A case 0x27: A \
	case 0x28: A case 0x29: A case 0x2A: A case 0x2B: A \
	case 0x2C: A case 0x2D: A case 0x2E: A case 0x2F: A \
	case 0x30: A case 0x31: A case 0x32: A case 0x33: A \
	case 0x34: A case 0x35: A case 0x36: A case 0x37: A \
	case 0x38: A case 0x39: A case 0x3A: A case 0x3B: A \
	case 0x3C: A case 0x3D: A case 0x3E: A case 0x3F: A \
	case 0x40: A case 0x41: A case 0x42: A case 0x43: A \
	case 0x44: A case 0x45: A case 0x46: A case 0x47: A \
	case 0x48: A case 0x49: A case 0x4A: A case 0x4B: A \
	case 0x4C: A case 0x4D: A case 0x4E: A case 0x4F: A \
	case 0x50: A case 0x51: A case 0x52: A case 0x53: A \
	case 0x54: A case 0x55: A case 0x56: A case 0x57: A \
	case 0x58: A case 0x59: A case 0x5A: A case 0x5B: A \
	case 0x5C: A case 0x5D: A case 0x5E: A case 0x5F: A \
	case 0x60: A case 0x61: A case 0x62: A case 0x63: A \
	case 0x64: A case 0x65: A case 0x66: A case 0x67: A \
	case 0x68: A case 0x69: A case 0x6A: A case 0x6B: A \
	case 0x6C: A case 0x6D: A case 0x6E: A case 0x6F: A \
	case 0x70: A case 0x71: A case 0x72: A case 0x73: A \
	case 0x74: A case 0x75: A case 0x76: A case 0x77: A \
	case 0x78: A case 0x79: A case 0x7A: A case 0x7B: A \
	case 0x7C: A case 0x7D: A case 0x7E: A case 0x7F: A \
	case 0x80: A case 0x81: A case 0x82: A case 0x83: A \
	case 0x84: A case 0x85: A case 0x86: A case 0x87: A \
	case 0x88: A case 0x89: A case 0x8A: A case 0x8B: A \
	case 0x8C: A case 0x8D: A case 0x8E: A case 0x8F: A \
	case 0x90: A case 0x91: A case 0x92: A case 0x93: A \
	case 0x94: A case 0x95: A case 0x96: A case 0x97: A \
	case 0x98: A case 0x99: A case 0x9A: A case 0x9B: A \
	case 0x9C: A case 0x9D: A case 0x9E: A case 0x9F: A \
	case 0xA0: A case 0xA1: A case 0xA2: A case 0xA3: A \
	case 0xA4: A case 0xA5: A case 0xA6: A case 0xA7: A \
	case 0xA8: A case 0xA9: A case 0xAA: A case 0xAB: A \
	case 0xAC: A case 0xAD: A case 0xAE: A case 0xAF: A \
	case 0xB0: A case 0xB1: A case 0xB2: A case 0xB3: A \
	case 0xB4: A case 0xB5: A case 0xB6: A case 0xB7: A \
	case 0xB8: A case 0xB9: A case 0xBA: A case 0xBB: A \
	case 0xBC: A case 0xBD: A case 0xBE: A case 0xBF: A \
	case 0xC0: A case 0xC1: A case 0xC2: A case 0xC3: A \
	case 0xC4: A case 0xC5: A case 0xC6: A case 0xC7: A \
	case 0xC8: A case 0xC9: A case 0xCA: A case 0xCB: A \
	case 0xCC: A case 0xCD: A case 0xCE: A case 0xCF: A \
	case 0xD0: A case 0xD1: A case 0xD2: A case 0xD3: A \
	case 0xD4: A case 0xD5: A case 0xD6: A case 0xD7: A \
	case 0xD8: A case 0xD9: A case 0xDA: A case 0xDB: A \
	case 0xDC: A case 0xDD: A case 0xDE: A case 0xDF: A \
	case 0xE0: A case 0xE1: A case 0xE2: A case 0xE3: A \
	case 0xE4: A case 0xE5: A case 0xE6: A case 0xE7: A \
	case 0xE8: A case 0xE9: A case 0xEA: A case 0xEB: A \
	case 0xEC: A case 0xED: A case 0xEE: A case 0xEF: A \
	case 0xF0: A case 0xF1: A case 0xF2: A case 0xF3: A \
	case 0xF4: A case 0xF5: A case 0xF6: A case 0xF7: A \
	case 0xF8: A case 0xF9: A case 0xFA: A case 0xFB: A \
	case 0xFC: A case 0xFD: A case 0xFE: A case 0xFF: A

namespace BOSS
{
    id_bitmap Bmp::FromAsset(chars filename)
    {
        id_asset_read BmpAsset = Asset::OpenForRead(filename);
        const sint32 FileSize = Asset::Size(BmpAsset);
        buffer BmpBuffer = Buffer::Alloc(BOSS_DBG FileSize - 2);
        Asset::Skip(BmpAsset, 2);
        Asset::Read(BmpAsset, (uint08*) BmpBuffer, FileSize - 2);
        Asset::Close(BmpAsset);
        id_bitmap NewBitmap = Bmp::Clone((id_bitmap) BmpBuffer);
        Buffer::Free(BmpBuffer);
        return NewBitmap;
    }

    void Bmp::ToAsset(id_bitmap_read bitmap, chars filename)
    {
        id_asset BmpAsset = Asset::OpenForWrite(filename, true);
        const uint32 BmpSize = Bmp::GetFileSizeWithoutBM(bitmap);
        Asset::Write(BmpAsset, (bytes) "BM", 2);
        Asset::Write(BmpAsset, (bytes) bitmap, BmpSize);
        Asset::Close(BmpAsset);
    }

    id_bitmap Bmp::Create(sint32 bytesperpixel, sint32 width, sint32 height, sint16 param1, sint16 param2)
    {
        const sint32 BmpRow = (bytesperpixel * width + 3) & ~3;
        id_bitmap NewBitmap = (id_bitmap) Memory::Alloc(sizeof(bitmapfile) + sizeof(bitmapinfo) + BmpRow * height);
        bitmapfile* BmpFileHeader = (bitmapfile*) NewBitmap;
        BmpFileHeader->size = sizeof(bitmapfile) + sizeof(bitmapinfo) + BmpRow * height;
        BmpFileHeader->param1 = param1;
        BmpFileHeader->param2 = param2;
        BmpFileHeader->offbits = sizeof(bitmapfile) + sizeof(bitmapinfo);
        bitmapinfo* BmpInfoHeader = (bitmapinfo*) (((uint08*) NewBitmap) + sizeof(bitmapfile));
        BmpInfoHeader->size = sizeof(bitmapinfo);
        BmpInfoHeader->width = width;
        BmpInfoHeader->height = height;
        BmpInfoHeader->planes = 1;
        BmpInfoHeader->bitcount = 8 * bytesperpixel;
        BmpInfoHeader->compression = 0;
        BmpInfoHeader->size_image = BmpRow * height;
        BmpInfoHeader->xpels_meter = 3780;
        BmpInfoHeader->ypels_meter = 3780;
        BmpInfoHeader->color_used = 0;
        BmpInfoHeader->color_important = 0;
        return NewBitmap;
    }

    id_bitmap Bmp::Clone(id_bitmap_read bitmap)
    {
        const sint32 BitCount = GetBitCount(bitmap);
        if(BitCount == 24)
        {
            const sint32 Width = GetWidth(bitmap);
            const sint32 Height = GetHeight(bitmap);
            id_bitmap NewBitmap = Create(4, Width, Height, GetParam1(bitmap), GetParam2(bitmap));
            auto DstBits = (Bmp::bitmappixel*) GetBits(NewBitmap);
            bytes SrcBits = GetBits(bitmap);
            const sint32 SrcRow = (BitCount / 8 * Width + 3) & ~3;
            for(sint32 y = 0; y < Height; ++y)
            {
                bytes CurBits = &SrcBits[y * SrcRow];
                for(sint32 x = 0; x < Width; ++x)
                {
                    DstBits->b = *(CurBits++);
                    DstBits->g = *(CurBits++);
                    DstBits->r = *(CurBits++);
                    DstBits->a = 0xFF;
                    DstBits++;
                }
            }
            return NewBitmap;
        }
        else if(BitCount == 32)
        {
            const uint32 BitmapSize = GetFileSizeWithoutBM(bitmap);
            id_bitmap NewBitmap = (id_bitmap) Memory::Alloc(BitmapSize);
            Memory::Copy((void*) NewBitmap, (const void*) bitmap, BitmapSize);
            return NewBitmap;
        }
        BOSS_ASSERT("본 함수가 지원하지 못하는 비트맵입니다", false);
        return nullptr;
    }

    id_bitmap Bmp::CloneFromBits(bytes bits, sint32 width, sint32 height, sint32 bitcount, orientationtype ori, id_bitmap oldbitmap)
    {
        BOSS_ASSERT("oldbitmap와 복제할 bits는 같을 수 없습니다", !oldbitmap || GetBits(oldbitmap) != bits);

        id_bitmap NewBitmap;
        sint32 NewBitmapWidth;
        sint32 NewBitmapHeight;
        if(ori == orientationtype_normal90 || ori == orientationtype_normal270 ||
            ori == orientationtype_fliped90 || ori == orientationtype_fliped270)
        {
            NewBitmapWidth = height;
            NewBitmapHeight = width;
        }
        else
        {
            NewBitmapWidth = width;
            NewBitmapHeight = height;
        }

        if(!oldbitmap || Bmp::GetBitCount(oldbitmap) != 32 ||
            Bmp::GetWidth(oldbitmap) != NewBitmapWidth || Bmp::GetHeight(oldbitmap) != NewBitmapHeight)
        {
            Remove(oldbitmap);
            NewBitmap = Create(4, NewBitmapWidth, NewBitmapHeight);
        }
        else NewBitmap = oldbitmap;

        const sint32 SrcXRow = (bitcount / 8);
        const sint32 SrcYRow = (bitcount / 8 * width + 3) & ~3;
        const sint32 SrcX___ = 0, SrcXEnd = SrcXRow * (width - 1);
        const sint32 SrcY___ = 0, SrcYEnd = SrcYRow * (height - 1);
        bytes SrcBits;
        sint32 SrcXStep, SrcYStep;
        switch(ori)
        {
        case orientationtype_normal0:
            SrcBits = bits + SrcX___ + SrcY___;
            SrcXStep = +SrcXRow;
            SrcYStep = +SrcYRow;
            break;
        case orientationtype_normal90:
            SrcBits = bits + SrcX___ + SrcYEnd;
            SrcXStep = -SrcYRow;
            SrcYStep = +SrcXRow;
            break;
        case orientationtype_normal180:
            SrcBits = bits + SrcXEnd + SrcYEnd;
            SrcXStep = -SrcXRow;
            SrcYStep = -SrcYRow;
            break;
        case orientationtype_normal270:
            SrcBits = bits + SrcXEnd + SrcY___;
            SrcXStep = +SrcYRow;
            SrcYStep = -SrcXRow;
            break;
        case orientationtype_fliped0:
            SrcBits = bits + SrcX___ + SrcYEnd;
            SrcXStep = +SrcXRow;
            SrcYStep = -SrcYRow;
            break;
        case orientationtype_fliped90:
            SrcBits = bits + SrcXEnd + SrcYEnd;
            SrcXStep = -SrcYRow;
            SrcYStep = -SrcXRow;
            break;
        case orientationtype_fliped180:
            SrcBits = bits + SrcXEnd + SrcY___;
            SrcXStep = -SrcXRow;
            SrcYStep = +SrcYRow;
            break;
        case orientationtype_fliped270:
            SrcBits = bits + SrcX___ + SrcY___;
            SrcXStep = +SrcYRow;
            SrcYStep = +SrcXRow;
            break;
        }

        auto DstBits = (Bmp::bitmappixel*) GetBits(NewBitmap);
        const sint32 DstWidth = GetWidth(NewBitmap);
        const sint32 DstHeight = GetHeight(NewBitmap);
        if(bitcount == 32)
        {
            if(ori == orientationtype_normal0)
                Memory::Copy((void*) DstBits, (const void*) SrcBits, 4 * DstWidth * DstHeight);
            else
            {
                auto CurDstBits = DstBits;
                for(sint32 y = 0; y < DstHeight; ++y)
                {
                    auto CurSrcBits = SrcBits + SrcYStep * y;
                    auto CurDstBitsEnd = CurDstBits + DstWidth;
                    switch(~(DstWidth - 1) & 0xFF)
                    while(CurDstBits < CurDstBitsEnd)
                    {
                        CASE256(
                            *CurDstBits = *((const Bmp::bitmappixel*) CurSrcBits);
                            CurSrcBits += SrcXStep;
                            CurDstBits++;)
                    }
                }
            }
        }
        else
        {
            auto CurDstBits = DstBits;
            for(sint32 y = 0; y < DstHeight; ++y)
            {
                auto CurSrcBits = SrcBits + SrcYStep * y;
                auto CurDstBitsEnd = CurDstBits + DstWidth;
                switch(~(DstWidth - 1) & 0xFF)
                while(CurDstBits < CurDstBitsEnd)
                {
                    CASE256(
                        CurDstBits->r = CurSrcBits[2];
                        CurDstBits->g = CurSrcBits[1];
                        CurDstBits->b = CurSrcBits[0];
                        CurDstBits->a = 0xFF;
                        CurSrcBits += SrcXStep;
                        CurDstBits++;)
                }
            }
        }
        return NewBitmap;
    }

    id_bitmap Bmp::CloneFromNV21(bytes ys, uv16s uvs, sint32 width, sint32 height)
    {
        id_bitmap NewBitmap = Create(4, width, height);
        auto DstBits = (Bmp::bitmappixel*) GetBits(NewBitmap);
        const sint32 UVWidth = width / 2;
        const sint32 UVHeight = height / 2;
        for(sint32 y = 0; y < UVHeight; ++y)
        for(sint32 x = 0; x < UVWidth; ++x)
        {
            bitmappixel* CurBit = &DstBits[x * 2 + (height - 1 - y * 2) * width];
            auto& CurBit0 = CurBit[0];
            auto& CurBit1 = CurBit[1];
            auto& CurBit2 = CurBit[0 - width];
            auto& CurBit3 = CurBit[1 - width];

            bytes CurY = &ys[x * 2 + y * 2 * width];
            const sint32 CurY0 = (sint32) (CurY[0] & 0xFF);
            const sint32 CurY1 = (sint32) (CurY[1] & 0xFF);
            const sint32 CurY2 = (sint32) (CurY[0 + width] & 0xFF);
            const sint32 CurY3 = (sint32) (CurY[1 + width] & 0xFF);

            uv16 CurUV = uvs[x + y * UVWidth];
            const sint32 CurU = ((sint32) (CurUV.u & 0xFF)) - 128;
            const sint32 CurV = ((sint32) (CurUV.v & 0xFF)) - 128;
            const sint32 RAdd = (sint32) (1.403f * CurV);
            const sint32 GSub = (sint32) (0.344f * CurU + 0.714f * CurV);
            const sint32 BAdd = (sint32) (1.770f * CurU);

            CurBit0.a = 0xFF;
            CurBit0.r = Math::Clamp(CurY0 + RAdd, 0, 255);
            CurBit0.g = Math::Clamp(CurY0 - GSub, 0, 255);
            CurBit0.b = Math::Clamp(CurY0 + BAdd, 0, 255);
            CurBit1.a = 0xFF;
            CurBit1.r = Math::Clamp(CurY1 + RAdd, 0, 255);
            CurBit1.g = Math::Clamp(CurY1 - GSub, 0, 255);
            CurBit1.b = Math::Clamp(CurY1 + BAdd, 0, 255);
            CurBit2.a = 0xFF;
            CurBit2.r = Math::Clamp(CurY2 + RAdd, 0, 255);
            CurBit2.g = Math::Clamp(CurY2 - GSub, 0, 255);
            CurBit2.b = Math::Clamp(CurY2 + BAdd, 0, 255);
            CurBit3.a = 0xFF;
            CurBit3.r = Math::Clamp(CurY3 + RAdd, 0, 255);
            CurBit3.g = Math::Clamp(CurY3 - GSub, 0, 255);
            CurBit3.b = Math::Clamp(CurY3 + BAdd, 0, 255);
        }
        return NewBitmap;
    }

    id_bitmap Bmp::Copy(id_bitmap_read bitmap, sint32 l, sint32 t, sint32 r, sint32 b)
    {
        BOSS_ASSERT("본 함수는 32비트 비트맵만 지원합니다", GetBitCount(bitmap) == 32);
        const sint32 Width = GetWidth(bitmap);
        const sint32 Height = GetHeight(bitmap);
        const bitmappixel* Bits = (const bitmappixel*) GetBits(bitmap);

        const sint32 NewWidth = r - l;
        const sint32 NewHeight = b - t;
        id_bitmap NewBitmap = Create(4, NewWidth, NewHeight);
        bitmappixel* NewBits = (bitmappixel*) GetBits(NewBitmap);
        for(sint32 y = 0; y < NewHeight; ++y)
            Memory::Copy(&NewBits[(NewHeight - 1 - y) * NewWidth], &Bits[l + (Height - 1 - t - y) * Width], sizeof(bitmappixel) * NewWidth);
        return NewBitmap;
    }

    void Bmp::FillColor(id_bitmap bitmap, argb32 color)
    {
        BOSS_ASSERT("본 함수는 32비트 비트맵만 지원합니다", GetBitCount(bitmap) == 32);
        const sint32 CurWidth = GetWidth(bitmap);
        const sint32 CurHeight = GetHeight(bitmap);
        bitmappixel* CurBits = (bitmappixel*) GetBits(bitmap);
        Memory::Fill(CurBits, 4 * CurWidth * CurHeight, &color, 4);
    }

    void Bmp::ChangeColor(id_bitmap bitmap, argb32 from, argb32 to)
    {
        BOSS_ASSERT("본 함수는 32비트 비트맵만 지원합니다", GetBitCount(bitmap) == 32);
        const sint32 CurWidth = GetWidth(bitmap);
        const sint32 CurHeight = GetHeight(bitmap);
        bitmappixel* CurBits = (bitmappixel*) GetBits(bitmap);
        for(sint32 y = 0; y < CurHeight; ++y)
        for(sint32 x = 0; x < CurWidth; ++x)
        {
            bitmappixel& CurBit = CurBits[x + (CurHeight - 1 - y) * CurWidth];
            if(CurBit.argb == from) CurBit.argb = to;
        }
    }

    void Bmp::SwapRedBlue(id_bitmap bitmap)
    {
        BOSS_ASSERT("본 함수는 32비트 비트맵만 지원합니다", GetBitCount(bitmap) == 32);
        const sint32 CurWidthHeight = GetWidth(bitmap) * GetHeight(bitmap);
        bitmappixel* CurBits = (bitmappixel*) GetBits(bitmap);
        const bitmappixel* CurBitsEnd = CurBits + CurWidthHeight;
        bitmappixel Temp;
        switch(~(CurWidthHeight - 1) & 0xFF)
        while(CurBits < CurBitsEnd)
        {
            CASE256(
                Temp = *CurBits;
                CurBits->r = Temp.b;
                CurBits->b = Temp.r;
                CurBits++;)
        }
    }

    void Bmp::Flatten(id_bitmap bitmap, uint08 r, uint08 g, uint08 b)
    {
        BOSS_ASSERT("본 함수는 32비트 비트맵만 지원합니다", GetBitCount(bitmap) == 32);
        const sint32 CurWidth = GetWidth(bitmap);
        const sint32 CurHeight = GetHeight(bitmap);
        bitmappixel* CurBits = (bitmappixel*) GetBits(bitmap);
        for(sint32 y = 0; y < CurHeight; ++y)
        for(sint32 x = 0; x < CurWidth; ++x)
        {
            bitmappixel& CurBit = CurBits[x + (CurHeight - 1 - y) * CurWidth];
            const sint32 Alpha = CurBit.a;
            CurBit.r = (CurBit.r * Alpha + r * (255 - Alpha)) / 255;
            CurBit.g = (CurBit.g * Alpha + g * (255 - Alpha)) / 255;
            CurBit.b = (CurBit.b * Alpha + b * (255 - Alpha)) / 255;
            CurBit.a = 0xFF;
        }
    }

    void Bmp::Remove(id_bitmap bitmap)
    {
        Memory::Free(bitmap);
    }

    void Bmp::MinTest(id_bitmap_read bitmap, rect128& recttest)
    {
        BOSS_ASSERT("본 함수는 32비트 비트맵만 지원합니다", GetBitCount(bitmap) == 32);
        const sint32 Width = GetWidth(bitmap);
        const sint32 Height = GetHeight(bitmap);
        const bitmappixel* Bits = (const bitmappixel*) GetBits(bitmap);

        bool AnyUpdated = true;
        while(AnyUpdated)
        {
            AnyUpdated = false;
            if(recttest.l + 1 < recttest.r)
            {
                bool EverybodyBg = true;
                const sint32 TestL = recttest.l;
                for(sint32 ly = Height - recttest.b, lyend = Height - recttest.t; ly < lyend; ++ly)
                    if(Bits[TestL + ly * Width].a)
                    {EverybodyBg = false; break;}
                if(EverybodyBg) {recttest.l++; AnyUpdated = true;}
            }
            if(recttest.t + 1 < recttest.b)
            {
                bool EverybodyBg = true;
                const sint32 TestT = Height - 1 - recttest.t;
                for(sint32 tx = recttest.l; tx < recttest.r; ++tx)
                    if(Bits[tx + TestT * Width].a)
                    {EverybodyBg = false; break;}
                if(EverybodyBg) {recttest.t++; AnyUpdated = true;}
            }
            if(recttest.l < recttest.r - 1)
            {
                bool EverybodyBg = true;
                const sint32 TestR = recttest.r - 1;
                for(sint32 ry = Height - recttest.b, ryend = Height - recttest.t; ry < ryend; ++ry)
                    if(Bits[TestR + ry * Width].a)
                    {EverybodyBg = false; break;}
                if(EverybodyBg) {recttest.r--; AnyUpdated = true;}
            }
            if(recttest.t < recttest.b - 1)
            {
                bool EverybodyBg = true;
                const sint32 TestB = Height - recttest.b;
                for(sint32 bx = recttest.l; bx < recttest.r; ++bx)
                    if(Bits[bx + TestB * Width].a)
                    {EverybodyBg = false; break;}
                if(EverybodyBg) {recttest.b--; AnyUpdated = true;}
            }
        }
    }

    void Bmp::MaxTest(id_bitmap_read bitmap, rect128& recttest)
    {
        BOSS_ASSERT("본 함수는 32비트 비트맵만 지원합니다", GetBitCount(bitmap) == 32);
        const sint32 Width = GetWidth(bitmap);
        const sint32 Height = GetHeight(bitmap);
        const bitmappixel* Bits = (const bitmappixel*) GetBits(bitmap);

        bool AnyUpdated = true;
        while(AnyUpdated)
        {
            AnyUpdated = false;
            if(0 < recttest.l)
            {
                const sint32 TestL = recttest.l - 1;
                for(sint32 ly = Height - recttest.b, lyend = Height - recttest.t; ly < lyend; ++ly)
                    if(Bits[TestL + ly * Width].a)
                    {recttest.l--; AnyUpdated = true; break;}
            }
            if(0 < recttest.t)
            {
                const sint32 TestT = Height - recttest.t;
                for(sint32 tx = recttest.l; tx < recttest.r; ++tx)
                    if(Bits[tx + TestT * Width].a)
                    {recttest.t--; AnyUpdated = true; break;}
            }
            if(recttest.r < Width)
            {
                const sint32 TestR = recttest.r;
                for(sint32 ry = Height - recttest.b, ryend = Height - recttest.t; ry < ryend; ++ry)
                    if(Bits[TestR + ry * Width].a)
                    {recttest.r++; AnyUpdated = true; break;}
            }
            if(recttest.b < Height)
            {
                const sint32 TestB = Height - 1 - recttest.b;
                for(sint32 bx = recttest.l; bx < recttest.r; ++bx)
                    if(Bits[bx + TestB * Width].a)
                    {recttest.b++; AnyUpdated = true; break;}
            }
        }
    }

    void Bmp::MaxTestWithBgColor(id_bitmap_read bitmap, rect128& recttest, argb32 bgcolor)
    {
        BOSS_ASSERT("본 함수는 32비트 비트맵만 지원합니다", GetBitCount(bitmap) == 32);
        const sint32 Width = GetWidth(bitmap);
        const sint32 Height = GetHeight(bitmap);
        const bitmappixel* Bits = (const bitmappixel*) GetBits(bitmap);

        bool AnyUpdated = true;
        while(AnyUpdated)
        {
            AnyUpdated = false;
            if(0 < recttest.l)
            {
                const sint32 TestL = recttest.l - 1;
                for(sint32 ly = Height - recttest.b, lyend = Height - recttest.t; ly < lyend; ++ly)
                {
                    const Bmp::bitmappixel& CurColor = Bits[TestL + ly * Width];
                    if(CurColor.a && CurColor.argb != bgcolor)
                    {recttest.l--; AnyUpdated = true; break;}
                }
            }
            if(0 < recttest.t)
            {
                const sint32 TestT = Height - recttest.t;
                for(sint32 tx = recttest.l; tx < recttest.r; ++tx)
                {
                    const Bmp::bitmappixel& CurColor = Bits[tx + TestT * Width];
                    if(CurColor.a && CurColor.argb != bgcolor)
                    {recttest.t--; AnyUpdated = true; break;}
                }
            }
            if(recttest.r < Width)
            {
                const sint32 TestR = recttest.r;
                for(sint32 ry = Height - recttest.b, ryend = Height - recttest.t; ry < ryend; ++ry)
                {
                    const Bmp::bitmappixel& CurColor = Bits[TestR + ry * Width];
                    if(CurColor.a && CurColor.argb != bgcolor)
                    {recttest.r++; AnyUpdated = true; break;}
                }
            }
            if(recttest.b < Height)
            {
                const sint32 TestB = Height - 1 - recttest.b;
                for(sint32 bx = recttest.l; bx < recttest.r; ++bx)
                {
                    const Bmp::bitmappixel& CurColor = Bits[bx + TestB * Width];
                    if(CurColor.a && CurColor.argb != bgcolor)
                    {recttest.b++; AnyUpdated = true; break;}
                }
            }
        }
    }

    uint32 Bmp::GetFileSizeWithoutBM(id_bitmap_read bitmap)
    {
        return ((const bitmapfile*) bitmap)->size;
    }

    sint16 Bmp::GetParam1(id_bitmap_read bitmap)
    {
        return ((const bitmapfile*) bitmap)->param1;
    }

    sint16 Bmp::GetParam2(id_bitmap_read bitmap)
    {
        return ((const bitmapfile*) bitmap)->param2;
    }

    sint32 Bmp::GetWidth(id_bitmap_read bitmap)
    {
        const bitmapinfo* Info = (const bitmapinfo*) (((bytes) bitmap) + sizeof(bitmapfile));
        return Info->width;
    }

    sint32 Bmp::GetHeight(id_bitmap_read bitmap)
    {
        const bitmapinfo* Info = (const bitmapinfo*) (((bytes) bitmap) + sizeof(bitmapfile));
        return Info->height;
    }

    sint16 Bmp::GetBitCount(id_bitmap_read bitmap)
    {
        const bitmapinfo* Info = (const bitmapinfo*) (((bytes) bitmap) + sizeof(bitmapfile));
        return Info->bitcount;
    }

    uint32 Bmp::GetImageSize(id_bitmap_read bitmap)
    {
        const bitmapinfo* Info = (const bitmapinfo*) (((bytes) bitmap) + sizeof(bitmapfile));
        return Info->size_image;
    }

    bytes Bmp::GetBits(id_bitmap_read bitmap)
    {
        return ((bytes) bitmap) + sizeof(bitmapfile) + sizeof(bitmapinfo);
    }

    uint08* Bmp::GetBits(id_bitmap bitmap)
    {
        return ((uint08*) bitmap) + sizeof(bitmapfile) + sizeof(bitmapinfo);
    }
}
