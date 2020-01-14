#include <boss.hpp>
#include "boss_png.hpp"

#include <format/boss_bmp.hpp>

namespace BOSS
{
    const sint32 LengthBase[29] = {
        3,   4,   5,   6,   7,   8,   9,  10,  11,  13,
        15,  17,  19,  23,  27,  31,  35,  43,  51,  59,
        67,  83,  99, 115, 131, 163, 195, 227, 258};
    const sint32 LengthExtra[29] = {
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
        1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
        4, 4, 4, 4, 5, 5, 5, 5, 0};
    const sint32 DistanceBase[30] = {
        1,     2,     3,     4,     5,     7,     9,    13,    17,    25,
        33,    49,    65,    97,   129,   193,   257,   385,   513,   769,
        1025,  1537,  2049,  3073,  4097,  6145,  8193, 12289, 16385, 24577};
    const sint32 DistanceExtra[30] = {
        0,  0,  0,  0,  1,  1,  2,  2,  3,  3,
        4,  4,  5,  5,  6,  6,  7,  7,  8,  8,
        9,  9, 10, 10, 11, 11, 12, 12, 13, 13};
    const sint32 PngLengthOrder[19] = {
        16, 17, 18,  0,  8,  7,  9,  6, 10,  5,
        11,  4, 12,  3, 13,  2, 14,  1, 15};

    Png::Png()
    {
    }

    Png::~Png()
    {
    }

    // PNG메모리를 BMP메모리로 변환함
    id_bitmap Png::ToBmp(bytes pngsource, bool mustbetruecolor)
    {
        id_bitmap Bitmap = nullptr;

        // 작업변수 초기화
        IsFinalBlock = false;
        BlockMode = (char) 0xFF;
        LiteralLength = 0;
        _Buf = pngsource;
        _Off = 0;
        LZPos = 0;
        LZCopyPos = 0;
        LZCopyCount = 0;

        // 청크정보 수집
        uint32 ImageWidth = 0;
        uint32 ImageHeight = 0;
        uint32 ImageDepth = 0;
        uint32 ImageColorType = 0;
        uint08 BMPPalette[3 * 256];
        uint08 BMPPaletteAlpha[256];
        sint32 BytesPerPixel = 0;
        uint32 BitmapRow = 0;
        uint08* BitmapBits = nullptr;
        uint08* DataPtr = nullptr;
        {
            bytes PNGNext = pngsource + 4;
            uint32 ChunkName = 0;

            // IHDR청크
            while(ChunkName != 0x49484452) // IHDR
            {
                _Buf = PNGNext + 4;
                PNGNext += GetUInt32() + 12;
                ChunkName = GetUInt32();
                if(ChunkName == 0x49454E44) // IEND
                {
                    BOSS_ASSERT("PNG에 IHDR청크가 없습니다", false);
                    return nullptr;
                }
            }

            ImageWidth = (uint32) GetUInt32();
            ImageHeight = (uint32) GetUInt32();
            ImageDepth = (uint32) GetByte();
            ImageColorType = (uint32) GetByte();
            GetByte(); GetByte(); // Compression Method, Filter Method 생략

            // 인터레이스를 지원하지 않는다
            if(0 < GetByte())
            {
                BOSS_ASSERT("인터레이스가 들어간 PNG는 지원하지 않습니다", false);
                return nullptr;
            }

            // PLTE청크
            uint32 PaletteSize = 0;
            while(ChunkName != 0x504C5445) // PLTE
            {
                _Buf = PNGNext + 4;
                PNGNext += (PaletteSize = GetUInt32()) + 12;
                ChunkName = GetUInt32();
                if(ChunkName == 0x49454E44) // IEND
                {
                    PNGNext = (uint08*) pngsource + 4;
                    break;
                }
            }
            if(0 < PaletteSize)
            for(uint32 p = 0; p < PaletteSize; p++)
                BMPPalette[p] = GetByte();

            // tRNS청크
            uint32 TransparencySize = 0;
            while(ChunkName != 0x74524E53) // tRNS
            {
                _Buf = PNGNext + 4;
                PNGNext += (TransparencySize = GetUInt32()) + 12;
                ChunkName = GetUInt32();
                if(ChunkName == 0x49454E44) // IEND
                {
                    PNGNext = (uint08*) pngsource + 4;
                    break;
                }
            }
            if(0 < TransparencySize)
            for(uint32 a = 0; a < TransparencySize; a++)
                BMPPaletteAlpha[a] = GetByte();

            // 비트맵구성
            BytesPerPixel = (mustbetruecolor ||
                ImageColorType == 4 || ImageColorType == 6 ||
                (ImageColorType == 3 && 0 < TransparencySize))? 4 : 3;
            BitmapRow = (ImageWidth * BytesPerPixel + 3) & ~3;
            Bitmap = Bmp::Create(BytesPerPixel, ImageWidth, ImageHeight);
            BitmapBits = Bmp::GetBits(Bitmap);

            // IDAT청크 조사
            uint32 DataSize = 0;
            bool IsMultiData = false;
            do
            {
                _Buf = PNGNext + 4;
                const uint32 TempSize = GetUInt32();
                PNGNext += TempSize + 12;
                ChunkName = GetUInt32();
                if(ChunkName == 0x49444154) // IDAT
                {
                    if(!DataPtr) DataPtr = (uint08*) _Buf;
                    else IsMultiData = true;
                    DataSize += TempSize;
                }
            }
            while(ChunkName != 0x49454E44); // IEND
            if(!DataPtr)
            {
                BOSS_ASSERT("PNG에 IDAT청크가 없습니다", false);
                return nullptr;
            }

            // IDAT청크 병합(IDAT가 복수개일 경우만)
            if(IsMultiData)
            {
                DataPtr = (uint08*) Memory::Alloc(DataSize);
                uint08* DataFocus = DataPtr;
                PNGNext = (uint08*) pngsource + 4;
                do
                {
                    _Buf = PNGNext + 4;
                    const uint32 TempSize = GetUInt32();
                    PNGNext += TempSize + 12;
                    ChunkName = GetUInt32();
                    if(ChunkName == 0x49444154) // IDAT
                    {
                        Memory::Copy(DataFocus, _Buf, TempSize);
                        DataFocus += TempSize;
                    }
                }
                while(ChunkName != 0x49454E44); // IEND
            }
            _Buf = DataPtr;
            if(!IsMultiData)
                DataPtr = nullptr;
        }

        // 디코딩된 픽셀정보 임시저장소
        uint32 RowBufferWidth = 0, FilterOffset = 0;
        switch(ImageColorType)
        {
        case 0: // 흑백
        case 3: // 팔레트
            RowBufferWidth = ((ImageWidth * ImageDepth) + 7) / 8;
            FilterOffset = 1;
            break;
        case 4: // 투명흑백
            RowBufferWidth = 2 * ImageWidth * ImageDepth / 8;
            FilterOffset = 2 * ImageDepth / 8;
            break;
        case 2: // RGB
            RowBufferWidth = 3 * ImageWidth * ImageDepth / 8;
            FilterOffset = 3 * ImageDepth / 8;
            break;
        case 6: // 투명RGB
            RowBufferWidth = 4 * ImageWidth * ImageDepth / 8;
            FilterOffset = 4 * ImageDepth / 8;
            break;
        }

        // 열버퍼생성
        uint08* RowBuffers = (uint08*) Memory::Alloc(RowBufferWidth * 2);
        uint08* RowBufferA = RowBuffers;
        uint08* RowBufferB = RowBuffers + RowBufferWidth;
        Memory::Set(RowBufferA, 0x00, RowBufferWidth);
        Memory::Set(RowBufferB, 0x00, RowBufferWidth);

        GetByte(); // Compression Method 생략
        GetByte(); // Compression Level, Preset Dictionary 생략
        // 첫 블럭헤더 디코딩
        DecodeBlockHeader();

        // 행단위 디코딩
        for(uint32 y = 0; y < ImageHeight; ++y)
        {
            const uint08 Filter = DecodeByte();
            for(uint32 x = 0; x < RowBufferWidth; ++x)
                RowBufferA[x] = DecodeByte();

            // 필터링
            switch(Filter)
            {
            case 1: // Sub필터
                for(uint32 Col = FilterOffset; Col < RowBufferWidth; ++Col)
                    RowBufferA[Col] += RowBufferA[Col - FilterOffset];
                break;
            case 2: // Up필터
                for(uint32 Col = 0; Col < RowBufferWidth; ++Col)
                    RowBufferA[Col] += RowBufferB[Col];
                break;
            case 3: // Average필터
                for(uint32 Col = 0; Col < FilterOffset; ++Col)
                    RowBufferA[Col] += RowBufferB[Col] / 2;
                for(uint32 Col = FilterOffset; Col < RowBufferWidth; ++Col)
                    RowBufferA[Col] += (RowBufferA[Col - FilterOffset] + RowBufferB[Col]) / 2;
                break;
            case 4: // Paeth필터
                for(uint32 Col = 0; Col < FilterOffset; ++Col)
                    RowBufferA[Col] += RowBufferB[Col];
                for(uint32 Col = FilterOffset; Col < RowBufferWidth; ++Col)
                {
                    // 죄측, 윗열, 윗좌측의 값들의 평균값에 가까운 필터를 사용
                    // 전체압축율에 손실을 입힐 우려가 있는 필터를 배제하기 위함
                    const uint08 Left = RowBufferA[Col - FilterOffset];
                    const uint08 Above = RowBufferB[Col];
                    const uint08 Aboveleft = RowBufferB[Col - FilterOffset];
                    int PaethA = Above - Aboveleft;
                    int PaethB = Left - Aboveleft;
                    int PaethC = PaethA + PaethB;
                    PaethA *= PaethA;
                    PaethB *= PaethB;
                    PaethC *= PaethC;
                    RowBufferA[Col] += (PaethA <= PaethB && PaethA <= PaethC)? Left
                        : ((PaethB <= PaethC)? Above : Aboveleft);
                }
                break;
            }

            // 이미지쓰기
            uint32 ImageOffset = (ImageHeight - y - 1) * BitmapRow;
            switch(ImageColorType)
            {
            case 0: // 흑백
                for(uint32 x = 0, xend = ImageDepth * ImageWidth; x < xend; x += ImageDepth, ImageOffset += BytesPerPixel)
                {
                    bytes Bytes = &RowBufferA[x >> 3];
                    const uint64 Union
                        = ((Bytes[0] & ox00000000000000FF) << 0)
                        | ((Bytes[1] & ox00000000000000FF) << 8)
                        | ((Bytes[2] & ox00000000000000FF) << 16)
                        | ((Bytes[3] & ox00000000000000FF) << 24)
                        | ((Bytes[4] & ox00000000000000FF) << 32);
                    const uint32 GrayLevel = (uint32) (Union >> (x & 7)) & ~(0xFFFFFFFF << ImageDepth);
                    const uint08 ColorValue = (uint08) (GrayLevel * 255 / ((1 << ImageDepth) - 1));
                    BitmapBits[ImageOffset] = ColorValue;
                    BitmapBits[ImageOffset + 1] = ColorValue;
                    BitmapBits[ImageOffset + 2] = ColorValue;
                }
                break;
            case 3: // 팔레트
                for(uint32 x = 0, xend = ImageDepth * ImageWidth; x < xend; x += ImageDepth, ImageOffset += BytesPerPixel)
                {
                    bytes Bytes = &RowBufferA[x >> 3];
                    const uint64 Union
                        = ((Bytes[0] & ox00000000000000FF) << 0)
                        | ((Bytes[1] & ox00000000000000FF) << 8)
                        | ((Bytes[2] & ox00000000000000FF) << 16)
                        | ((Bytes[3] & ox00000000000000FF) << 24)
                        | ((Bytes[4] & ox00000000000000FF) << 32);
                    const uint32 AlphaOffset = (uint32) (Union >> (x & 7)) & ~(0xFFFFFFFF << ImageDepth);
                    const uint32 PaletteOffset = AlphaOffset * 3;
                    BitmapBits[ImageOffset] = BMPPalette[PaletteOffset + 2];
                    BitmapBits[ImageOffset + 1] = BMPPalette[PaletteOffset + 1];
                    BitmapBits[ImageOffset + 2] = BMPPalette[PaletteOffset];
                    if(BytesPerPixel == 4)
                    {
                        if(BMPPaletteAlpha)
                            BitmapBits[ImageOffset + 3] = BMPPaletteAlpha[AlphaOffset];
                        else BitmapBits[ImageOffset + 3] = 0xFF;
                    }
                }
                break;
            case 4: // 투명흑백
                for(uint32 x = 0, xend = ImageWidth * 2; x < xend; x += 2)
                {
                    BitmapBits[ImageOffset++] = RowBufferA[x + 0];
                    BitmapBits[ImageOffset++] = RowBufferA[x + 0];
                    BitmapBits[ImageOffset++] = RowBufferA[x + 0];
                    if(BytesPerPixel == 4)
                        BitmapBits[ImageOffset++] = RowBufferA[x + 1];
                }
                break;
            case 2: // RGB
                for(uint32 x = 0, xend = ImageWidth * 3; x < xend; x += 3)
                {
                    BitmapBits[ImageOffset++] = RowBufferA[x + 2];
                    BitmapBits[ImageOffset++] = RowBufferA[x + 1];
                    BitmapBits[ImageOffset++] = RowBufferA[x + 0];
                    if(BytesPerPixel == 4)
                        BitmapBits[ImageOffset++] = 0xFF;
                }
                break;
            case 6: // 투명RGB
                for(uint32 x = 0, xend = ImageWidth * 4; x < xend; x += 4)
                {
                    BitmapBits[ImageOffset++] = RowBufferA[x + 2];
                    BitmapBits[ImageOffset++] = RowBufferA[x + 1];
                    BitmapBits[ImageOffset++] = RowBufferA[x + 0];
                    if(BytesPerPixel == 4)
                        BitmapBits[ImageOffset++] = RowBufferA[x + 3];
                }
                break;
            }

            // 열버퍼교환
            uint08* RowBufferTemp = RowBufferA;
            RowBufferA = RowBufferB;
            RowBufferB = RowBufferTemp;
        }

        Memory::Free(RowBuffers);
        RowBuffers = nullptr;
        Memory::Free(DataPtr);
        DataPtr = nullptr;
        return Bitmap;
    }

    // 1비트를 읽음
    uint32 Png::GetBit()
    {
        const uint32 RValue = (*_Buf >> _Off) & 1;
        const int Value = _Off + 1;
        _Buf += Value >> 3;
        _Off = Value & 7;
        return RValue;
    }

    // len비트를 읽음, len허용범위 : 0-32
    uint32 Png::GetBits(const uint32 len)
    {
        const uint64 Union
            = ((_Buf[0] & ox00000000000000FF) << 0)
            | ((_Buf[1] & ox00000000000000FF) << 8)
            | ((_Buf[2] & ox00000000000000FF) << 16)
            | ((_Buf[3] & ox00000000000000FF) << 24)
            | ((_Buf[4] & ox00000000000000FF) << 32);
        const uint32 RValue = (uint32) (Union >> _Off) & ~(0xFFFFFFFF << len);
        const int Value = _Off + len;
        _Buf += Value >> 3;
        _Off = Value & 7;
        return RValue;
    }

    // 1바이트를 읽음, GetBits()로 읽다 남은것이 있다면 해당 바이트 건너뜀
    uint08 Png::GetByte()
    {
        // 비트단위로 읽던것은 넘어감
        _Buf += (_Off != 0);
        _Off = 0;
        return *(_Buf++);
    }

    // 4바이트를 읽음
    uint32 Png::GetUInt32()
    {
        // 비트단위로 읽던것은 넘어감
        _Buf += (_Off != 0);
        _Off = 0;
        const uint32 RValue =
            ((_Buf[0] & 0xFF) << 24) |
            ((_Buf[1] & 0xFF) << 16) |
            ((_Buf[2] & 0xFF) << 8) |
            ((_Buf[3] & 0xFF) << 0);
        _Buf += 4;
        return RValue;
    }

    uint08 Png::DecodeLiteralByte()
    {
        if(LiteralLength == 0)
        {
            if(!IsFinalBlock)
                DecodeBlockHeader();
            return DecodeByte();
        }
        --LiteralLength;
        return GetByte();
    }

    // 허프만방식으로 읽음
    uint08 Png::DecodeByte()
    {
        if(BlockMode == 0)
            return DecodeLiteralByte();
        return DecodeCompressedByte();
    }

    // 허프만테이블에서 값을 찾음
    sint32 Png::Decode(const uint32 tablenum)
    {
        int CodeLength = 0, Code = GetBit();
        const int* MaxCodeFocus = &MaxCode[tablenum << 4];
        for(CodeLength = 0; MaxCodeFocus[CodeLength] < Code && CodeLength < 15; ++CodeLength)
            Code = (Code << 1) | GetBit(); // 1비트씩 거꾸로 읽음
        return HuffValues[(tablenum << 9) | (ValueArray[(tablenum << 4) | CodeLength] + (Code - MinCode[(tablenum << 4) | CodeLength]))];
    }

    uint08 Png::DecodeCompressedByte()
    {
        // LZ윈도우의 복사거리동안 디코딩은 쉼
        if(0 < LZCopyCount)
        {
            uint08 Value8 = LZWindow[LZCopyPos];
            LZWindow[LZPos] = Value8;
            --LZCopyCount;
            LZCopyPos = (LZCopyPos + 1) & 0x7FFF;
            LZPos = (LZPos + 1) & 0x7FFF;
            return Value8;
        }

        // 허프만트리에서 하나의 값을 얻음
        const sint32 ValueA = Decode(0);
        if(ValueA == 256)
        {
            if(!IsFinalBlock)
                DecodeBlockHeader();
            return DecodeByte();
        }
        else if(ValueA < 256) // LZ윈도우상의 기호토큰인 경우
        {
            LZWindow[LZPos] = (uint08) ValueA;
            LZPos = (LZPos + 1) & 0x7FFF;
            return (uint08) ValueA;
        }

        // LZ윈도우상의 구를 이루는 토큰인 경우
        const uint32 Length = LengthBase[ValueA - 257] + GetBits(LengthExtra[ValueA - 257]);
        const sint32 ValueB = Decode(1);
        const uint32 Distance = DistanceBase[ValueB] + GetBits(DistanceExtra[ValueB]);
        LZCopyPos = (32768 + LZPos - Distance) & 0x7FFF;
        LZCopyCount = Length;
        return DecodeCompressedByte();
    }

    // 블럭헤더의 Length를 읽음
    void Png::DecodeLengths(uint32 tablenum, uint32* lengths, uint32 lengthcount)
    {
        // 동적허프만테이블인 경우 이미지정보값과 LZ윈도우거리값의 배열이
        // 따로 기록되어 있으니 디코딩해야 하며 RLE압축방식을 쓴다
        uint32 Index = 0;
        while(Index < lengthcount)
        {
            uint32 Command = Decode(tablenum);
            if(Command < 16) lengths[Index++] = Command;
            else
            {
                uint32 Count = 0;
                switch(Command)
                {
                case 16: Count = GetBits(2) + 3; break; // 카운트만큼 이전과 같음
                case 17: Count = GetBits(3) + 3; break; // 카운트만큼 0값
                case 18: Count = GetBits(7) + 11; break; // 카운트만큼 0값
                }
                for(uint32 i = 0; i < Count; ++i)
                {
                    if(Command == 16)
                    {
                        const uint32 Index2 = Index - 1;
                        lengths[Index++] = lengths[Index2];
                    }
                    else lengths[Index++] = 0;
                }
            }
        }
    }

    // IDAT의 블럭헤더를 디코딩함
    void Png::DecodeBlockHeader()
    {
        // 압축블럭은 이미지 행단위가 아니라 행속에서의 효율단위이다
        IsFinalBlock = (GetBit() != 0);
        switch(GetBits(2))
        {
        case 0: // 비압축블럭
            {
                BlockMode = 0;
                LiteralLength = GetByte();
                LiteralLength |= ((int) GetByte()) << 8;
                GetByte(); GetByte(); // Not Length 생략
            }
            break;
        case 1: // 고정허프만테이블
            if(BlockMode != 1)
            {
                BlockMode = 1;
                BuildFixedTable();
            }
            break;
        case 2: // 동적허프만테이블
            {
                BlockMode = 2;
                const uint32 LiteralsNum = GetBits(5) + 257;
                const uint32 DistancesNum = GetBits(5) + 1;
                const uint32 LengthsNum = GetBits(4) + 4;
                // Lengths 빌드
                uint32 Lengths[19];
                Memory::Set(Lengths, 0, sizeof(uint32) * 19);
                for(uint32 i = 0; i < LengthsNum; ++i)
                    Lengths[PngLengthOrder[i]] = GetBits(3);
                BuildTable(2, 19, Lengths);
                // Literals/Distances 빌드
                uint32 Literals[288], Distances[32];
                DecodeLengths(2, Literals, LiteralsNum);
                DecodeLengths(2, Distances, DistancesNum);
                BuildTable(0, LiteralsNum, Literals);
                BuildTable(1, DistancesNum, Distances);
            }
            break;
        }
    }

    // 고정허프만테이블 구성
    void Png::BuildFixedTable()
    {
        for(int j = 0; j < 2; j++)
        for(int i = 0; i < 15; ++i)
        {
            MaxCode[(j << 4) | i] = -1;
            MinCode[(j << 4) | i] = 0x0FFFFFFF;
            ValueArray[(j << 4) | i] = 0;
        }

        MaxCode[(0 << 4) | 6] = 23, MaxCode[(0 << 4) | 7] = 199, MaxCode[(0 << 4) | 8] = 511;
        MinCode[(0 << 4) | 6] = 0, MinCode[(0 << 4) | 7] = 48, MinCode[(0 << 4) | 8] = 400;
        ValueArray[(0 << 4) | 6] = 0, ValueArray[(0 << 4) | 7] = 24, ValueArray[(0 << 4) | 8] = 176;
        int valuesfocus = 0;
        for(uint16 f = 256; f <= 279; ++f) HuffValues[(0 << 9) | valuesfocus++] = f;
        for(uint16 f = 0; f <= 143; ++f) HuffValues[(0 << 9) | valuesfocus++] = f;
        for(uint16 f = 280; f <= 287; ++f) HuffValues[(0 << 9) | valuesfocus++] = f;
        for(uint16 f = 144; f <= 255; ++f) HuffValues[(0 << 9) | valuesfocus++] = f;

        MaxCode[(1 << 4) | 4] = 31, MinCode[(1 << 4) | 4] = 0, ValueArray[(1 << 4) | 4] = 0;
        valuesfocus = 0;
        for(uint16 f = 0; f <= 31; ++f) HuffValues[(1 << 9) | valuesfocus++] = f;
    }

    // 동적허프만테이블 구성
    void Png::BuildTable(uint32 tablenum, uint32 vcount, const uint32* codelengths)
    {
        uint32 LengthArray[288];
        // 배열번호는 그 자체로 최종적으로 디코딩시 얻는 이미지정보등의 값이며
        // Length는 그 값을 표현하기 위한 비트수이다
        for(uint32 i = 0; i < vcount; ++i)
        {
            HuffValues[(tablenum << 9) | i] = i;
            LengthArray[i] = codelengths[i];
        }

        // 배열의 뒤로 갈수록 비트수가 많아지게 정렬한다
        for(uint32 i = 0; i < vcount - 1; ++i)
        {
            uint32 MinIndex = i;
            for(uint32 j = i + 1; j < vcount; ++j)
                if(LengthArray[j] < LengthArray[MinIndex]
                    || (LengthArray[j] == LengthArray[MinIndex] && HuffValues[(tablenum << 9) | j] < HuffValues[(tablenum << 9) | MinIndex]))
                        MinIndex = j;
            if(MinIndex != i)
            {
                const int Temp1 = HuffValues[(tablenum << 9) | MinIndex];
                HuffValues[(tablenum << 9) | MinIndex] = HuffValues[(tablenum << 9) | i];
                HuffValues[(tablenum << 9) | i] = Temp1;
                const uint32 Temp2 = LengthArray[MinIndex];
                LengthArray[MinIndex] = LengthArray[i];
                LengthArray[i] = Temp2;
            }
        }

        uint16 HuffCodes[288];
        // 해당비트수로 표현할 수 있는 최소수부터 1씩 올라가며 차례차례 기록한다
        uint32 LastLen = 0, Code = 0;
        for(uint32 i = 0; i < vcount; ++i)
        {
            while(LastLen < LengthArray[i])
            {
                ++LastLen;
                Code <<= 1;
            }
            if(LastLen != 0)
                HuffCodes[i] = (uint16) Code++;
        }

        // 비트수는 보통 5, 7, 8, 9만을 쓰기 때문에 초기화함
        for(uint32 i = 0; i < 15; ++i)
        {
            MaxCode[(tablenum << 4) | i] = -1;
            MinCode[(tablenum << 4) | i] = 0x0FFFFFFF;
            ValueArray[(tablenum << 4) | i] = 0;
        }

        // 비트수에 따른 최소값과 최대값, 이미지정보기본값을 저장한다
        uint32 LastLength = 0;
        for(uint32 i = 0; i < vcount; ++i)
        {
            if(LastLength < LengthArray[i])
            {
                LastLength = LengthArray[i];
                ValueArray[(tablenum << 4) | (LastLength - 1)] = (uint16) i;
                MinCode[(tablenum << 4) | (LastLength - 1)] = HuffCodes[i];
            }
            if(0 < LastLength)
                MaxCode[(tablenum << 4) | (LastLength - 1)] = HuffCodes[i];
        }
    }
}
