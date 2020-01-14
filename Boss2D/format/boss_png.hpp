#pragma once
#include <boss.hpp>

namespace BOSS
{
    /// @brief 포맷-PNG
    class Png
    {
    public:
        Png();
        ~Png();
        id_bitmap ToBmp(bytes pngsource, bool mustbetruecolor);

    protected:
        bool IsFinalBlock; // IDAT의 블럭헤더 디코딩중 현재 블럭이 마지막인지를 나타냄
        char BlockMode; // IDAT의 블럭헤더 디코딩중 현재 블럭이 어떠한 블럭인지를 나타냄
        sint32 LiteralLength; // 비압축블럭의 사이즈
        const uint08* _Buf;
        uint32 _Off;
        uint08 LZWindow[32768];
        uint32 LZPos;
        uint32 LZCopyPos;
        uint32 LZCopyCount;
        sint32 MaxCode[16 * 3]; // [3][15]
        sint32 MinCode[16 * 3]; // [3][15]
        sint32 ValueArray[16 * 3]; // [3][15]
        sint32 HuffValues[512 * 3]; // [3][288]

    private:
        uint32 GetBit();
        uint32 GetBits(const uint32 len);
        uint08 GetByte();
        uint32 GetUInt32();
        uint08 DecodeLiteralByte();
        uint08 DecodeByte();
        sint32 Decode(const uint32 tablenum);
        uint08 DecodeCompressedByte();
        void DecodeLengths(uint32 tablenum, uint32* lengths, uint32 lengthcount);
        void DecodeBlockHeader();
        void BuildFixedTable();
        void BuildTable(uint32 tablenum, uint32 vcount, const uint32* codelengths);
    };
}
