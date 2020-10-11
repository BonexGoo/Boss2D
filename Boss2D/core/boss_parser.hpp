#pragma once
#include <boss_string.hpp>

namespace BOSS
{
    /// @brief 파서지원
    class Parser
    {
    public:
        class Tag
        {
        public:
            Tag(const String key);
            ~Tag();

        public:
            const String m_key_base;
            chars m_key;
            sint32 m_map[256];
        };

    public:
        Parser(const String source);
        ~Parser();

    public:
        const bool SeekOf(const Tag& tag);
        // Int
        const sint32 ReadInt();
        const sint32s ReadInts(const char separator, const char endmark);
        // Float
        const float ReadFloat();
        const floats ReadFloats(const char separator, const char endmark);
        // Bool
        const bool ReadBool();
        // Hex32
        const uint32 ReadHex32();

    public:
        template<typename TYPE = sint32>
        static const TYPE GetInt(chars source, sint32 length = -1, sint32* offset = nullptr)
        {
            if(source == nullptr || length == 0) return 0;
            else if(length == -1) length = boss_strlen(source);
            sint32 Offset = (offset)? *offset : 0;

            TYPE MinusFlag = 1;
            if(source[Offset] == '+') {++Offset;}
            else if(source[Offset] == '-') {++Offset; MinusFlag = -1;}

            TYPE Value = 0;
            char OneChar = source[Offset];
            while(Offset < length && '0' <= OneChar && OneChar <= '9')
            {
                Value = Value * 10 + OneChar - '0';
                OneChar = source[++Offset];
            }

            if(offset) *offset = Offset;
            return Value * MinusFlag;
        }
        template<typename TYPE = float>
        static const TYPE GetFloat(chars source, sint32 length = -1, sint32* offset = nullptr)
        {
            if(source == nullptr || length == 0) return 0;
            else if(length == -1) length = boss_strlen(source);
            sint32 Offset = (offset)? *offset : 0;

            TYPE MinusFlug = 1;
            if(source[Offset] == '+') {++Offset;}
            else if(source[Offset] == '-') {++Offset; MinusFlug = -1;}

            sint64 Value = 0, DecimalLevel = 0;
            float Decimal = 0;
            char OneChar = source[Offset];
            while(Offset < length && '0' <= OneChar && OneChar <= '9')
            {
                if(0 < DecimalLevel)
                {
                    DecimalLevel *= 10;
                    Decimal += (OneChar - '0') / (float) DecimalLevel;
                }
                else Value = Value * 10 + OneChar - '0';

                OneChar = source[++Offset];
                if(DecimalLevel == 0 && OneChar == '.')
                {
                    DecimalLevel = 1;
                    OneChar = source[++Offset];
                }
            }

            TYPE Result = (Value + Decimal) * MinusFlug;
            if(OneChar == 'E' || OneChar == 'e')
            {
                static const void* ECodeEM = (sizeof(TYPE) == 4)?
                    []()->const void* {static float _[38]; float EM = 10; for(sint32 i = 0; i < 38; ++i) _[i] = (EM /= 10); return _;}() :
                    []()->const void* {static double _[308]; double EM = 10; for(sint32 i = 0; i < 308; ++i) _[i] = (EM /= 10); return _;}();
                static const void* ECodeEP = (sizeof(TYPE) == 4)?
                    []()->const void* {static float _[38]; float EP = 1; for(sint32 i = 0; i < 38; ++i) _[i] = (EP *= 10); return _;}() :
                    []()->const void* {static double _[308]; double EP = 1; for(sint32 i = 0; i < 308; ++i) _[i] = (EP *= 10); return _;}();
                const TYPE* ECode[2] = {(const TYPE*) ECodeEM, (const TYPE*) ECodeEP};
                const sint32 ESign = (source[(++Offset)++] == '+');
                const sint32 EValue = GetInt(source, length, offset);
                const bool ESuccess = (0 <= EValue && EValue - ESign < ((sizeof(TYPE) == 4)? 38 : 308));
                if(ESuccess) Result *= ECode[ESign][EValue - ESign];
            }

            if(offset) *offset = Offset;
            return Result;
        }

        static const bool GetBool(chars source, sint32 length = -1, sint32* offset = nullptr)
        {
            if (source == nullptr || length == 0) return 0;
            else if (length == -1) length = boss_strlen(source);
            sint32 Offset = (offset) ? *offset : 0;

            bool Value = false;
            if(!String::CompareNoCase(&source[Offset], "true", 4))
            {
                Offset += 4;
                Value = true;
            }
            else if(!String::CompareNoCase(&source[Offset], "false", 5))
            {
                Offset += 5;
                Value = false;
            }

            if(offset) *offset = Offset;
            return Value;
        }

        template<typename TYPE = uint32>
        static const TYPE GetHex32(chars source, sint32 length = -1, sint32* offset = nullptr)
        {
            if(source == nullptr || length == 0) return 0;
            else if(length == -1) length = boss_strlen(source);
            sint32 Offset = (offset)? *offset : 0;

            if(source[Offset] == '0'
                && (source[Offset + 1] == 'x' || source[Offset + 1] == 'X'))
                Offset += 2;

            TYPE Value = 0;
            bool IsNumeric = false, IsHexLower = false, IsHexUpper = false;
            char OneChar = source[Offset];
            while((IsNumeric = ('0' <= OneChar && OneChar <= '9'))
                || (IsHexLower = ('a' <= OneChar && OneChar <= 'f'))
                || (IsHexUpper = ('A' <= OneChar && OneChar <= 'F')))
            {
                if(IsNumeric) Value = (Value << 4) + OneChar - '0';
                else if(IsHexLower) Value = (Value << 4) + 10 + OneChar - 'a';
                else if(IsHexUpper) Value = (Value << 4) + 10 + OneChar - 'A';
                OneChar = source[++Offset];
            }

            if(offset) *offset = Offset;
            return Value;
        }

    private:
        const String m_source_base;
        chars m_source;
        sint32 m_offset;
    };
}
