#include <boss.hpp>
#include "boss_string.hpp"

#include <platform/boss_platform.hpp>
#include <stdarg.h>

namespace BOSS
{
    String::String() : m_words(NullString()), m_findmap(sint32s::Null())
    {
    }

    String::String(const String& rhs) : m_words(rhs.m_words), m_findmap(rhs.m_findmap)
    {
    }

    String::String(String&& rhs) : m_words(ToReference(rhs.m_words)), m_findmap(ToReference(rhs.m_findmap))
    {
        rhs.m_words = NullString();
        rhs.m_findmap = sint32s::Null();
    }

    String::String(const chararray& rhs) : m_words(rhs), m_findmap(sint32s::Null())
    {
    }

    String::String(chararray&& rhs) : m_words(ToReference(rhs)), m_findmap(sint32s::Null())
    {
        rhs = NullString();
    }

    String::String(id_share_read rhs) : m_words(rhs), m_findmap(sint32s::Null())
    {
    }

    String::String(id_cloned_share_read rhs) : m_words(rhs), m_findmap(sint32s::Null())
    {
    }

    String::String(chars rhs, sint32 length) : m_words(NullString()), m_findmap(sint32s::Null())
    {
        BOSS_ASSERT(String::Format("정상적인 파라미터가 아닙니다(\"%s\", %d)", rhs, length), -1 <= length);
        if(rhs && length != 0)
        {
            if(length == -1)
                operator=(rhs);
            else
            {
                m_words.AtWherever(length) = '\0';
                Memory::Copy(m_words.AtDumping(0, length + 1), rhs,
                    sizeof(char) * length);
            }
        }
    }

    String::String(char rhs) : m_words(NullString()), m_findmap(sint32s::Null())
    {
        operator=(rhs);
    }

    String::~String()
    {
    }

    String& String::operator=(const String& rhs)
    {
        m_words = rhs.m_words;
        m_findmap = rhs.m_findmap;
        return *this;
    }

    String& String::operator=(String&& rhs)
    {
        m_words = rhs.m_words;
        m_findmap = rhs.m_findmap;
        rhs.m_words = NullString();
        rhs.m_findmap = sint32s::Null();
        return *this;
    }

    String& String::operator=(chars rhs)
    {
        if(&m_words[0] != rhs)
        {
            chararray* RhsTemp = GetSafedRhs(rhs);
            m_words.Clear();
            if(rhs)
            {
                do {m_words.AtAdding() = *rhs;}
                while(*(rhs++));
            }
            else m_words.AtAdding() = '\0';
            m_findmap.Clear();
            delete RhsTemp;
        }
        return *this;
    }

    String& String::operator=(char rhs)
    {
        m_words.Clear();
        m_words.AtAdding() = rhs;
        m_words.AtAdding() = '\0';
        m_findmap.Clear();
        return *this;
    }

    String& String::operator+=(const String& rhs)
    {
        return operator+=((chars) rhs);
    }

    String& String::operator+=(chars rhs)
    {
        chararray* RhsTemp = GetSafedRhs(rhs);
        if(rhs)
        {
            m_words.SubtractionOne();
            do {m_words.AtAdding() = *rhs;}
            while(*(rhs++));
            m_findmap.Clear();
        }
        delete RhsTemp;
        return *this;
    }

    String& String::operator+=(char rhs)
    {
        m_words.At(-1) = rhs;
        m_words.AtAdding() = '\0';
        m_findmap.Clear();
        return *this;
    }

    String String::operator+(const String& rhs) const
    {
        return String(*this).operator+=(rhs);
    }

    String String::operator+(chars rhs) const
    {
        return String(*this).operator+=(rhs);
    }

    String operator+(chars lhs, const String& rhs)
    {
        return String(lhs).operator+=(rhs);
    }

    String String::operator+(char rhs) const
    {
        return String(*this).operator+=(rhs);
    }

    String operator+(char lhs, const String& rhs)
    {
        return String(lhs).operator+=(rhs);
    }

    bool String::operator==(const String& rhs) const
    {
        return (boss_strcmp(&m_words[0], &rhs.m_words[0]) == 0);
    }

    bool String::operator==(chars rhs) const
    {
        return (boss_strcmp(&m_words[0], rhs) == 0);
    }

    bool operator==(chars lhs, const String& rhs)
    {
        return (boss_strcmp(lhs, &rhs.m_words[0]) == 0);
    }

    bool String::operator!=(const String& rhs) const
    {
        return !operator==(rhs);
    }

    bool String::operator!=(chars rhs) const
    {
        return !operator==(rhs);
    }

    bool operator!=(chars lhs, const String& rhs)
    {
        return !operator==(lhs, rhs);
    }

    String::operator chars() const
    {
        return &m_words[0];
    }

    String::operator id_share() const
    {
        return m_words;
    }

    String::operator id_cloned_share() const
    {
        return m_words;
    }

    void String::Empty(bool keepbuffer)
    {
        if(keepbuffer)
        {
            m_words.SubtractionAll();
            m_words.AtAdding() = '\0';
        }
        else m_words = NullString();
        m_findmap = sint32s::Null();
    }

    const char String::operator[](sint32 index) const
    {
        return m_words[index];
    }

    sint32 String::Length() const
    {
        return m_words.Count() - 1;
    }

    sint32 String::Compare(const String& other, sint32 offset) const
    {
        BOSS_ASSERT("other의 범위를 초과한 접근입니다", 0 <= offset && offset < other.m_words.Count());
        return boss_strncmp(&m_words[0], &other.m_words[offset], Math::Min(m_words.Count(), other.m_words.Count() - offset));
    }

    sint32 String::CompareNoCase(const String& other, sint32 offset) const
    {
        BOSS_ASSERT("other의 범위를 초과한 접근입니다", 0 <= offset && offset < other.m_words.Count());
        return boss_strnicmp(&m_words[0], &other.m_words[offset], Math::Min(m_words.Count(), other.m_words.Count() - offset));
    }

    sint32 String::Find(sint32 index, const String& key) const
    {
        if(const sint32 KeyLength = key.Length())
        {
            chars KeyString = &key.m_words[0];
            const char KeyTail = key.m_words[-2];
            const sint32* KeyMap = key.GetFindMap();
            chars FindBegin = &m_words[0];
            chars FindFocus = FindBegin + index + KeyLength - 1;
            chars FindEnd = FindBegin + m_words.Count();
            while(FindFocus < FindEnd)
            {
                if(*FindFocus == KeyTail && !Memory::Compare(&FindFocus[1 - KeyLength], KeyString, KeyLength))
                    return &FindFocus[1 - KeyLength] - FindBegin;
                FindFocus += KeyMap[*FindFocus & 0xFF];
            }
        }
        return -1;
    }

    String& String::Replace(char oldword, char newword)
    {
        for(sint32 i = 0, iend = m_words.Count() - 1; i < iend; ++i)
            if(m_words[i] == oldword) m_words.At(i) = newword;
        m_findmap.Clear();
        return *this;
    }

    String& String::Replace(chars oldtext, chars newtext)
    {
        const String OldKey = oldtext;
        const String NewKey = newtext;
        const sint32 OldKeyLength = OldKey.Length();
        const sint32 NewKeyLength = NewKey.Length();
        sint32 Focus = 0;
        while(0 <= (Focus = Find(Focus, OldKey)))
        {
            String NewString = Left(Focus);
            NewString += newtext;
            NewString += Right(Length() - (Focus + OldKeyLength));
            operator=(ToReference(NewString));
            Focus += NewKeyLength;
        }
        return *this;
    }

    String& String::AddTail(chars other, sint32 length)
    {
        if(length == -1)
            operator+=(other);
        else if(other)
        {
            m_words.SubtractionOne();
            const sint32 OldLength = m_words.Count();
            m_words.AtWherever(OldLength + length) = '\0';
            Memory::Copy(m_words.AtDumping(OldLength, length + 1), other,
                sizeof(char) * length);
            m_findmap.Clear();
        }
        return *this;
    }

    String& String::SubTail(sint32 length)
    {
        if(0 < length)
        {
            m_words.SubtractionOne();
            while(0 < length--)
                m_words.SubtractionOne();
            m_words.AtAdding() = '\0';
            m_findmap.Clear();
        }
        return *this;
    }

    String String::Left(sint32 length) const
    {
        const sint32 CalcedLength = Math::Min(length, Length());
        return String(&m_words[0], CalcedLength);
    }

    String String::Right(sint32 length) const
    {
        const sint32 CalcedLength = Math::Min(length, Length());
        return String(&m_words[Length() - CalcedLength], CalcedLength);
    }

    String String::Middle(sint32 index, sint32 length) const
    {
        const sint32 CalcedIndex = Math::Max(0, index);
        const sint32 CalcedLength = Math::Min(index + length, Length()) - CalcedIndex;
        return String(&m_words[CalcedIndex], CalcedLength);
    }

    String String::Offset(sint32 offset) const
    {
        const sint32 CalcedOffset = Math::Min(offset, Length());
        return String(&m_words[CalcedOffset], Length() - CalcedOffset);
    }

    String String::Trim() const
    {
        sint32 BeginPos = 0;
        sint32 EndPos = Length();
        while(BeginPos < EndPos)
        {
            const char OneChar = m_words[BeginPos];
            if(OneChar == ' ' || OneChar == '\t' || OneChar == '\r' || OneChar == '\n')
                BeginPos++;
            else break;
        }
        while(BeginPos < EndPos)
        {
            const char OneChar = m_words[EndPos - 1];
            if(OneChar == ' ' || OneChar == '\t' || OneChar == '\r' || OneChar == '\n')
                EndPos--;
            else break;
        }
        return String(&m_words[BeginPos], EndPos - BeginPos);
    }

    String String::Lower() const
    {
        chararray NewWords;
        NewWords.AtDumpingAdded(m_words.Count());
        for(sint32 i = 0, iend = m_words.Count() - 1; i < iend; ++i)
        {
            const char OneWord = m_words[i];
            if('A' <= OneWord && OneWord <= 'Z')
                NewWords.At(i) = OneWord + ('a' - 'A');
            else NewWords.At(i) = OneWord;
        }
        NewWords.At(m_words.Count() - 1) = '\0';
        return String(NewWords);
    }

    String String::Upper() const
    {
        chararray NewWords;
        NewWords.AtDumpingAdded(m_words.Count());
        for(sint32 i = 0, iend = m_words.Count() - 1; i < iend; ++i)
        {
            const char OneWord = m_words[i];
            if('a' <= OneWord && OneWord <= 'z')
                NewWords.At(i) = OneWord - ('a' - 'A');
            else NewWords.At(i) = OneWord;
        }
        NewWords.At(m_words.Count() - 1) = '\0';
        return String(NewWords);
    }

    bool String::ToFile(chars filename, bool bom) const
    {
        if(id_file TextAsset = Platform::File::OpenForWrite(filename, true))
        {
            if(bom) Platform::File::Write(TextAsset, (bytes) "\xEF\xBB\xBF", 3);
            Platform::File::Write(TextAsset, (bytes) &m_words[0], Length());
            Platform::File::Close(TextAsset);
            return true;
        }
        return false;
    }

    bool String::ToAsset(chars filename, bool bom) const
    {
        if(id_asset TextAsset = Asset::OpenForWrite(filename, true))
        {
            if(bom) Asset::Write(TextAsset, (bytes) "\xEF\xBB\xBF", 3);
            Asset::Write(TextAsset, (bytes) &m_words[0], Length());
            Asset::Close(TextAsset);
            return true;
        }
        return false;
    }

    String String::ToUrlString(chars safeword) const
    {
        auto AddUrlCode = [](chararray& dst, const char word)->void
        {
            const sint32 HiWord = ((word >> 4) & 0xF);
            const sint32 LoWord = (word & 0xF);
            dst.AtAdding() = '%';
            dst.AtAdding() = (HiWord < 10)? '0' + HiWord : 'A' + HiWord - 10;
            dst.AtAdding() = (LoWord < 10)? '0' + LoWord : 'A' + LoWord - 10;
        };

        chararray NewWords;
        for(sint32 i = 0, iend = m_words.Count() - 1; i < iend; ++i)
        {
            const char OneWord = m_words[i];
            if(OneWord & 0x80)
            {
                AddUrlCode(NewWords, OneWord);
                while(m_words[i + 1] & 0x80)
                    AddUrlCode(NewWords, m_words[++i]);
            }
            else
            {
                bool SafeMatched = false;
                branch;
                jump('A' <= OneWord && OneWord <= 'Z') SafeMatched = true;
                jump('a' <= OneWord && OneWord <= 'z') SafeMatched = true;
                jump('0' <= OneWord && OneWord <= '9') SafeMatched = true;
                else
                {
                    chars SafeWordFocus = safeword;
                    while(*SafeWordFocus)
                    {
                        if(*(SafeWordFocus++) == OneWord)
                        {
                            SafeMatched = true;
                            break;
                        }
                    }
                }
                if(SafeMatched)
                    NewWords.AtAdding() = OneWord;
                else AddUrlCode(NewWords, OneWord);
            }
        }
        NewWords.AtAdding() = '\0';
        return String(NewWords);
    }

    String String::ToHtmlString() const
    {
        auto AddHtmlCode = [](chararray& dst, uint32 code)->void
        {
            dst.AtAdding() = '&';
            dst.AtAdding() = '#';
            char Temp[10];
            BOSS_TRACE("=====> [%u]", code);
            for(sint32 i = 0; i < 10; ++i)
            {
                const sint32 CurCode = code % 10;
                Temp[i] = '0' + CurCode;
                if((code /= 10) == 0)
                {
                    do {dst.AtAdding() = Temp[i];} while(i--);
                    break;
                }
            }
            dst.AtAdding() = ';';
        };

        chararray NewWords;
        for(sint32 i = 0, iend = m_words.Count() - 1; i < iend; ++i)
        {
            const char OneWord = m_words[i];
            branch;
            jump((OneWord & 0xF0) == 0xF0) // 4Bytes
            {
                const char NewValue = (((m_words[i + 0] & 0x07) << 2) | ((m_words[i + 1] & 0x30) >> 4)) - 1;
                uint32 NewCode = 0xD800 | (NewValue << 6) | ((m_words[i + 1] & 0x0F) << 2) | ((m_words[i + 2] & 0x30) >> 4);
                NewCode |= (0xDC00 | ((m_words[i + 2] & 0x0F) << 6) | (m_words[i + 3] & 0x3F)) << 16;
                AddHtmlCode(NewWords, NewCode);
                i += 3;
            }
            jump((OneWord & 0xF0) == 0xE0) // 3Bytes
            {
                const uint32 NewCode = ((m_words[i + 0] & 0x0F) << 12) | ((m_words[i + 1] & 0x3F) << 6) | (m_words[i + 2] & 0x3F);
                AddHtmlCode(NewWords, NewCode);
                i += 2;
            }
            jump((OneWord & 0xE0) == 0xC0) // 2Bytes
            {
                const uint32 NewCode = ((m_words[i + 0] & 0x1F) << 6) | (m_words[i + 1] & 0x3F);
                AddHtmlCode(NewWords, NewCode);
                i += 1;
            }
            else
            {
                bool SafeMatched = false;
                branch;
                jump('A' <= OneWord && OneWord <= 'Z') SafeMatched = true;
                jump('a' <= OneWord && OneWord <= 'z') SafeMatched = true;
                jump('0' <= OneWord && OneWord <= '9') SafeMatched = true;
                if(SafeMatched)
                    NewWords.AtAdding() = OneWord;
                else AddHtmlCode(NewWords, OneWord & 0xFF);
            }
        }
        NewWords.AtAdding() = '\0';
        return String(NewWords);
    }

    String String::Format(chars text, ...)
    {
        va_list Args;
        va_start(Args, text);
        const sint32 Size = boss_vsnprintf(nullptr, 0, text, Args);
        va_end(Args);

        String Result;
        if(0 <= Size)
        {
            Result.m_words.AtWherever(Size) = '\0';
            va_start(Args, text);
            boss_vsnprintf(Result.m_words.AtDumping(0, Size + 1), Size + 1, text, Args);
            va_end(Args);
        }
        else
        {
            BOSS_ASSERT("vsnprintf에서 text의 길이를 추산하지 못함", false);
            Result = "-format error-";
        }
        return Result;
    }

    String String::FromFile(chars filename)
    {
        id_file_read TextFile = Platform::File::OpenForRead(filename);
        if(!TextFile) return String();
        // UTF-8                "EF BB BF"
        // UTF-16 Big Endian    "FE FF"
        // UTF-16 Little Endian "FF FE"
        // UTF-32 Big Endian    "00 00 FE FF"
        // UTF-32 Little Endian "FF FE 00 00"

        String Result;
        sint32 TextSize = Platform::File::Size(TextFile);
        if(3 <= TextSize)
        {
            char BOMTest[3];
            Platform::File::Read(TextFile, (uint08*) BOMTest, 3);
            if(BOMTest[0] == (char) 0xEF && BOMTest[1] == (char) 0xBB && BOMTest[2] == (char) 0xBF)
            {
                TextSize -= 3;
                Result.m_words.AtWherever(TextSize) = '\0';
                Platform::File::Read(TextFile, (uint08*) Result.m_words.AtDumping(0, TextSize), TextSize);
            }
            else
            {
                Result.m_words.AtWherever(TextSize) = '\0';
                Result.m_words.At(0) = BOMTest[0];
                Result.m_words.At(1) = BOMTest[1];
                Result.m_words.At(2) = BOMTest[2];
                Platform::File::Read(TextFile, (uint08*) Result.m_words.AtDumping(3, TextSize - 3), TextSize - 3);
            }
        }
        Platform::File::Close(TextFile);
        return Result;
    }

    String String::FromAsset(chars filename, id_assetpath_read assetpath, bool originalonly)
    {
        id_asset_read TextAsset = Asset::OpenForRead(filename, assetpath, originalonly);
        if(!TextAsset) return String();
        // UTF-8                "EF BB BF"
        // UTF-16 Big Endian    "FE FF"
        // UTF-16 Little Endian "FF FE"
        // UTF-32 Big Endian    "00 00 FE FF"
        // UTF-32 Little Endian "FF FE 00 00"

        String Result;
        sint32 TextSize = Asset::Size(TextAsset);
        if(3 <= TextSize)
        {
            char BOMTest[3];
            Asset::Read(TextAsset, (uint08*) BOMTest, 3);
            if(BOMTest[0] == (char) 0xEF && BOMTest[1] == (char) 0xBB && BOMTest[2] == (char) 0xBF)
            {
                TextSize -= 3;
                Result.m_words.AtWherever(TextSize) = '\0';
                Asset::Read(TextAsset, (uint08*) Result.m_words.AtDumping(0, TextSize), TextSize);
            }
            else
            {
                Result.m_words.AtWherever(TextSize) = '\0';
                Result.m_words.At(0) = BOMTest[0];
                Result.m_words.At(1) = BOMTest[1];
                Result.m_words.At(2) = BOMTest[2];
                Asset::Read(TextAsset, (uint08*) Result.m_words.AtDumping(3, TextSize - 3), TextSize - 3);
            }
        }
        Asset::Close(TextAsset);
        return Result;
    }

    String String::FromWChars(wchars text, sint32 length)
    {
        chararray NewWords;
        if(text)
        {
            wchars textend = text + (length & 0x00FFFFFF);
            if(sizeof(wchar_t) == 2)
            {
                while((*text != L'\0') & (text < textend))
                {
                    const uint16& OneWChar = ((const uint16*) text)[0];
                    const uint16& TwoWChar = ((const uint16*) text)[1];
                    if((OneWChar & 0xFF80) == 0x0000) // Ascii
                    {
                        NewWords.AtAdding() = OneWChar & 0x00FF;
                        text += 1;
                    }
                    else if((OneWChar & 0xF800) == 0x0000) // 2Bytes-Less
                    {
                        NewWords.AtAdding() = 0xC0 | ((OneWChar & 0x07C0) >> 6);
                        NewWords.AtAdding() = 0x80 | (OneWChar & 0x003F);
                        text += 1;
                    }
                    else if((OneWChar & 0xFC00) != 0xD800 || (TwoWChar & 0xFC00) != 0xDC00) // 2Bytes-Full
                    {
                        NewWords.AtAdding() = 0xE0 | ((OneWChar & 0xF000) >> 12);
                        NewWords.AtAdding() = 0x80 | ((OneWChar & 0x0FC0) >> 6);
                        NewWords.AtAdding() = 0x80 | (OneWChar & 0x003F);
                        text += 1;
                    }
                    else // 4Bytes
                    {
                        const wchar_t NewValue = ((OneWChar & 0x03C0) >> 6) + 1;
                        NewWords.AtAdding() = 0xF0 | ((NewValue & 0x001C) >> 2);
                        NewWords.AtAdding() = 0x80 | ((NewValue & 0x0003) << 4) | ((OneWChar & 0x003C) >> 2);
                        NewWords.AtAdding() = 0x80 | ((OneWChar & 0x0003) << 4) | ((TwoWChar & 0x03C0) >> 6);
                        NewWords.AtAdding() = 0x80 | (TwoWChar & 0x003F);
                        text += 2;
                    }
                }
            }
            else if(sizeof(wchar_t) == 4)
            {
                while((*text != L'\0') & (text < textend))
                {
                    const uint32& OneWChar = ((const uint32*) text)[0];
                    if(OneWChar < 0x80) // Ascii
                    {
                        NewWords.AtAdding() = 0x00 | ((OneWChar >>  0) & 0xFF);
                    }
                    else if(OneWChar < 0x800) // 2Bytes
                    {
                        NewWords.AtAdding() = 0xC0 | ((OneWChar >>  6) & 0xFF);
                        NewWords.AtAdding() = 0x80 | ((OneWChar >>  0) & 0x3F);
                    }
                    else if(OneWChar < 0x10000 || 0x0010FFFF < OneWChar) // 3Bytes
                    {
                        NewWords.AtAdding() = 0xE0 | ((OneWChar >> 12) & 0xFF);
                        NewWords.AtAdding() = 0x80 | ((OneWChar >>  6) & 0x3F);
                        NewWords.AtAdding() = 0x80 | ((OneWChar >>  0) & 0x3F);
                    }
                    else // 4Bytes
                    {
                        NewWords.AtAdding() = 0xF0 | ((OneWChar >> 18) & 0xFF);
                        NewWords.AtAdding() = 0x80 | ((OneWChar >> 12) & 0x3F);
                        NewWords.AtAdding() = 0x80 | ((OneWChar >>  6) & 0x3F);
                        NewWords.AtAdding() = 0x80 | ((OneWChar >>  0) & 0x3F);
                    }
                    text += 1;
                }
            }
            else BOSS_ASSERT("지원되지 않는 wchar_t입니다", false);
        }
        NewWords.AtAdding() = '\0';
        return String(NewWords);
    }

    String String::FromUrlString(chars text, sint32 length)
    {
        auto GetCode = [](chars url)->char
        {
            sint32 HiWord = 0, LoWord = 0;
            if('0' <= url[0] && url[0] <= '9') HiWord = url[0] - '0';
            else if('A' <= url[0] && url[0] <= 'F') HiWord = url[0] - 'A' + 10;
            else if('a' <= url[0] && url[0] <= 'f') HiWord = url[0] - 'a' + 10;
            if('0' <= url[1] && url[1] <= '9') LoWord = url[1] - '0';
            else if('A' <= url[1] && url[1] <= 'F') LoWord = url[1] - 'A' + 10;
            else if('a' <= url[1] && url[1] <= 'f') LoWord = url[1] - 'a' + 10;
            return (char) ((HiWord << 4) | LoWord);
        };

        chararray NewWords;
        for(sint32 i = 0, iend = (length < 0)? 0x7FFFFFFF : length; text[i] && i < iend; ++i)
        {
            if(text[i] == '%')
            {
                const char OneWord = GetCode(&text[i + 1]);
                NewWords.AtAdding() = OneWord;
                i += 2;
            }
            else if(text[i] == '+')
                NewWords.AtAdding() = ' ';
            else NewWords.AtAdding() = text[i];
        }
        NewWords.AtAdding() = '\0';
        return String(NewWords);
    }

    String String::FromInteger(const sint32 value)
    {
        char Result[1024];
        const sint32 ResultLength = boss_snprintf(Result, 1024, "%d", value);
        return String(Result, ResultLength);
    }

    String String::FromInteger(const sint64 value)
    {
        char Result[1024];
        const sint32 ResultLength = boss_snprintf(Result, 1024, "%lld", value);
        return String(Result, ResultLength);
    }

    String String::FromFloat(const float value)
    {
        char Result[1024];
        sint32 ResultLength = boss_snprintf(Result, 1024, "%f", value);
        while(Result[ResultLength - 1] == '0') ResultLength--;
        if(Result[ResultLength - 1] == '.') ResultLength--;
        return String(Result, ResultLength);
    }

    String String::FromFloat(const double value)
    {
        char Result[1024];
        sint32 ResultLength = boss_snprintf(Result, 1024, "%lf", value);
        while(Result[ResultLength - 1] == '0') ResultLength--;
        if(Result[ResultLength - 1] == '.') ResultLength--;
        return String(Result, ResultLength);
    }

    sint32 String::Compare(chars text, chars other, sint32 maxlength)
    {
        if(maxlength == -1)
            return boss_strcmp(text, other);
        return boss_strncmp(text, other, maxlength);
    }

    sint32 String::CompareNoCase(chars text, chars other, sint32 maxlength)
    {
        if(maxlength == -1)
            return boss_stricmp(text, other);
        return boss_strnicmp(text, other, maxlength);
    }

    sint32 String::GetLengthOfFirstLetter(chars text)
    {
        const uint08 FirstLetter = *text & 0xFF;
        if(0xFC <= FirstLetter) return 6;
        else if(0xF8 <= FirstLetter) return 5;
        else if(0xF0 <= FirstLetter) return 4;
        else if(0xE0 <= FirstLetter) return 3;
        else if(0xC0 <= FirstLetter) return 2;
        return 1;
    }

    Strings String::Split(chars text, char comma)
    {
        Strings Collector;
        chars Focus = text;
        while(*text++ != '\0')
        {
            if(*text == comma || *text == '\0')
            {
                Collector.AtAdding() = String(Focus, text - Focus);
                Focus = text + 1;
            }
        }
        return Collector;
    }

    const chararray& String::NullString()
    {
        return *BOSS_STORAGE_SYS(chararray, '\0');
    }

    chararray* String::GetSafedRhs(chars& rhs)
    {
        chararray* Result = nullptr;
        if(&m_words[0] <= rhs && rhs <= &m_words[-1])
        {
            Result = new chararray();
            const sint32 Length = boss_strlen(rhs);
            Memory::Copy(Result->AtDumpingAdded(Length + 1), rhs, sizeof(char) * (Length + 1));
            rhs = &(*Result)[0];
        }
        return Result;
    }

    const sint32* String::GetFindMap() const
    {
        if(m_findmap.Count() == 0)
        {
            m_findmap.AtWherever(255);
            sint32* Findmap = m_findmap.AtDumping(0, 256);
            Memory::Set(Findmap, 0, sizeof(sint32) * 256);

            sint32 Focus = 0;
            chars String = &m_words[0];
            while(String[++Focus])
                Findmap[String[Focus - 1] & 0xFF] = Focus;
            for(sint32 i = 0; i < 256; ++i)
                Findmap[i] = Focus - Findmap[i];
        }
        BOSS_ASSERT("findmap의 규격이 잘못되었습니다", m_findmap.Count() == 256);
        return &m_findmap[0];
    }
}
