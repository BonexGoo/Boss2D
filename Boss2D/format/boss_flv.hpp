#pragma once
#include <boss.hpp>

namespace BOSS
{
    /// @brief 포맷-FLV
    class Flv
    {
    public:
        static id_flash Create(sint32 width, sint32 height);
        static id_flash Create(chars filename);
        static void Remove(id_flash flash);

    public:
        static bytes ReadChunk(id_flash flash, uint08* type = nullptr, sint32* length = nullptr, sint32* timestamp = nullptr);
        static void WriteChunk(id_flash flash, uint08 type, bytes chunk, sint32 length, sint32 timestamp = 0);
        static sint32 TimeStampForReadFocus(id_flash flash);

    public:
        static void Empty(id_flash flash);
        static bytes GetBits(id_flash_read flash, sint32* length = nullptr);
        static String BuildLog(bytes bits, sint32 length);

    public:
        static const void* ToBE2(id_flash flash, sint32 value);
        static const void* ToBE3(id_flash flash, sint32 value);
        static const void* ToBE4(id_flash flash, sint32 value);
        static const void* ToBE8_Double(id_flash flash, double value);

    public:
        static sint32 FromBE2(const void* be2);
        static sint32 FromBE3(const void* be3);
        static sint32 FromBE4(const void* be4);
        static double FromBE8_Double(const void* be8_double);
    };
}
