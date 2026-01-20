#include <boss.hpp>
#include "boss_context.hpp"

namespace BOSS
{
    Context& Context::At(chars key, sint32 length)
    {
        return m_namableChild(key, length);
    }

    Context& Context::At(sint32 index)
    {
        while(m_indexableChild.Count() <= index)
            m_indexableChild.AtAdding();
        return m_indexableChild.At(index);
    }

    Context& Context::AtAdding()
    {
        return m_indexableChild.AtAdding();
    }

    void Context::Remove(chars key, sint32 length)
    {
        m_namableChild.Remove(String(key, length));
    }

    void Context::Remove(sint32 index)
    {
        m_indexableChild.SubtractionSection(index);
    }

    void Context::Set(chars value, sint32 length, bool need_quotation)
    {
        if(value)
        {
            sint32 Length = (length == -1)? boss_strlen(value) : length;
            m_source.Clear();
            m_source.SharedValue().InitString(SO_NeedCopy, value, Length);
            m_valueNeedQuotation = need_quotation;
            m_valueOffset = m_source->GetString();
            m_valueLength = Length;
        }
        else
        {
            m_valueNeedQuotation = need_quotation;
            m_valueOffset = nullptr;
            m_valueLength = 0;
        }
    }

    void Context::Clear()
    {
        m_source.Clear();
        m_namableChild.Reset();
        m_indexableChild.Clear();
        Set(nullptr);
    }

    bool Context::LoadJson(buffer src)
    {
        Clear();
        m_source.SharedValue().InitString(src);
        return LoadJsonCore(m_source->GetString());
    }

    bool Context::LoadJson(ScriptOption option, chars src, sint32 length)
    {
        Clear();
        m_source.SharedValue().InitString(option, src, (option == SO_NeedCopy && length == -1)? boss_strlen(src) : length);
        return LoadJsonCore(m_source->GetString());
    }

    String Context::SaveJson(String dst) const
    {
        const sint32 tab = 0;
        if(0 < m_namableChild.Count())
        {
            dst.AddTailFast("{\r\n");
            sint32 count = m_namableChild.Count();
            const void* param[3] = {&tab, &dst, &count};
            m_namableChild.AccessByCallback(SaveJsonCoreCB, param);
            dst.AddTailFast("}\r\n");
        }

        if(0 < m_indexableChild.Count())
        {
            dst.AddTailFast("[\r\n");
            for(sint32 i = 0, iend = m_indexableChild.Count(); i < iend; ++i)
            {
                const Context& CurChild = m_indexableChild[i];
                CurChild.SaveJsonCore(tab + 1, String::FromInteger(i), dst, true, i + 1 == iend);
            }
            dst.AddTailFast("]\r\n");
        }
        return dst;
    }

    bool Context::LoadXml(buffer src)
    {
        Clear();
        m_source.SharedValue().InitString(src);
        return LoadXmlCore(m_source->GetString());
    }

    bool Context::LoadXml(ScriptOption option, chars src, sint32 length)
    {
        Clear();
        m_source.SharedValue().InitString(option, src, (option == SO_NeedCopy && length == -1)? boss_strlen(src) : length);
        return LoadXmlCore(m_source->GetString());
    }

    String Context::SaveXml(String dst) const
    {
        for(sint32 i = 0, iend = m_indexableChild.Count(); i < iend; ++i)
        {
            const Context& CurChild = m_indexableChild[i];
            CurChild.SaveXmlCore(0, String::FromInteger(i), dst);
        }
        return dst;
    }

    bool Context::LoadBin(bytes src)
    {
        if(GetBinHeader().Compare((chars) src))
            return false;

        m_namableChild.Reset();
        m_indexableChild.Clear();

        bytes SrcFocus = src + GetBinHeader().Length() + 1 + sizeof(sint32) + 1;
        LoadBinCore(SrcFocus);
        return true;
    }

    uint08s Context::SaveBin(uint08s dst) const
    {
        Memory::Copy(dst.AtDumping(dst.Count(), GetBinHeader().Length() + 1),
            (chars) GetBinHeader(), GetBinHeader().Length() + 1);
        SaveBinCore(dst);
        return dst;
    }

    bool Context::LoadPrm(chars src, sint32 length)
    {
        m_namableChild.Reset();
        m_indexableChild.Clear();

        const String SrcString(src, length);
        sint32 PosBegin = 0;
        while(PosBegin < SrcString.Length())
        {
            const sint32 PosMiddle = SrcString.Find(PosBegin + 1, "=");
            if(PosMiddle != -1)
            {
                const sint32 PosEnd = SrcString.Find(PosMiddle + 1, ";");
                if(PosEnd != -1)
                {
                    const String KeyName(src + PosBegin, PosMiddle - PosBegin);
                    const String KeyValue(src + PosMiddle + 1, PosEnd - (PosMiddle + 1));
                    At(KeyName).Set(KeyValue);
                    PosBegin = PosEnd + 1;
                }
                else
                {
                    BOSS_ASSERT("Prm파싱중 \';\'기호를 찾을 수 없습니다", false);
                    return false;
                }
            }
            else
            {
                BOSS_ASSERT("Prm파싱중 \'=\'기호를 찾을 수 없습니다", false);
                return false;
            }
        }
        return true;
    }

    CollectedContexts Context::CollectByMatch(chars key, chars value, CollectOption option) const
    {
        CollectedContexts Result;
        sint32 ValueLength = boss_strlen(value);
        CollectCore(nullptr, key, value, ValueLength, &Result, option);
        return Result;
    }

    const Context& Context::operator()(chars key) const
    {
        Context* CurChild = m_namableChild.Access(key);
        return (CurChild)? *CurChild : NullChild();
    }

    const Context& Context::operator()(sint32 order, chararray* getname) const
    {
        Context* CurChild = m_namableChild.AccessByOrder(order, getname);
        return (CurChild)? *CurChild : NullChild();
    }

    const Context& Context::operator[](sint32 index) const
    {
        if(0 <= index && index < m_indexableChild.Count())
            return m_indexableChild[index];
        return NullChild();
    }

    sint32 Context::LengthOfNamable() const
    {
        return m_namableChild.Count();
    }

    sint32 Context::LengthOfIndexable() const
    {
        return m_indexableChild.Count();
    }

    bool Context::IsValid() const
    {
        return (this != &NullChild());
    }

    bool Context::HasValue() const
    {
        return (m_valueOffset != nullptr);
    }

    const String Context::GetText() const
    {
        return String(m_valueOffset, m_valueLength);
    }

    const String Context::GetText(const String value) const
    {
        return (m_valueOffset)? GetText() : value;
    }

    chars_endless Context::GetTextFast(sint32& length) const
    {
        length = m_valueLength;
        return m_valueOffset;
    }

    const sint64 Context::GetInt() const
    {
        return Parser::GetInt<sint64>(m_valueOffset, m_valueLength);
    }

    const sint64 Context::GetInt(const sint64 value) const
    {
        return (m_valueOffset)? GetInt() : value;
    }

    const double Context::GetFloat() const
    {
        return Parser::GetFloat<double>(m_valueOffset, m_valueLength);
    }

    const double Context::GetFloat(const double value) const
    {
        return (m_valueOffset)? GetFloat() : value;
    }

    const bool Context::GetBool() const
    {
        return Parser::GetBool(m_valueOffset, m_valueLength);
    }

    void Context::DebugPrint() const
    {
        #if !BOSS_NDEBUG | BOSS_NEED_RELEASE_TRACE
            BOSS_TRACE("");
            BOSS_TRACE("==================== Context ====================");
            sint32 tab = -1;
            m_namableChild.AccessByCallback(DebugPrintCoreCB, &tab);
            for(sint32 i = 0, iend = m_indexableChild.Count(); i < iend; ++i)
            {
                const Context& CurChild = m_indexableChild[i];
                CurChild.DebugPrintCore(0, String::FromInteger(i), true);
            }
            BOSS_TRACE("=================================================");
        #endif
    }

    Context::Context()
    {
        m_valueNeedQuotation = false;
        m_valueOffset = nullptr;
        m_valueLength = 0;
    }

    Context::Context(const Context& rhs)
    {
        m_valueNeedQuotation = false;
        m_valueOffset = nullptr;
        m_valueLength = 0;
        operator=(rhs);
    }

    Context::Context(Context&& rhs) noexcept
    {
        m_valueNeedQuotation = false;
        m_valueOffset = nullptr;
        m_valueLength = 0;
        operator=(ToReference(rhs));
    }

    Context::Context(bytes src)
    {
        m_valueNeedQuotation = false;
        m_valueOffset = nullptr;
        m_valueLength = 0;
        LoadBin(src);
    }

    Context::Context(ScriptType type, buffer src)
    {
        m_valueNeedQuotation = false;
        m_valueOffset = nullptr;
        m_valueLength = 0;
        switch(type)
        {
        case ST_Json: LoadJson(src); break;
        case ST_Xml: LoadXml(src); break;
        }
    }

    Context::Context(ScriptType type, ScriptOption option, chars src, sint32 length)
    {
        m_valueNeedQuotation = false;
        m_valueOffset = nullptr;
        m_valueLength = 0;
        switch(type)
        {
        case ST_Json: LoadJson(option, src, length); break;
        case ST_Xml: LoadXml(option, src, length); break;
        }
    }

    Context::~Context()
    {
    }

    Context& Context::operator=(const Context& rhs)
    {
        m_source = rhs.m_source;
        m_namableChild = rhs.m_namableChild;
        m_indexableChild = rhs.m_indexableChild;
        m_valueNeedQuotation = rhs.m_valueNeedQuotation;
        m_valueOffset = rhs.m_valueOffset;
        m_valueLength = rhs.m_valueLength;
        return *this;
    }

    Context& Context::operator=(Context&& rhs) noexcept
    {
        m_source = ToReference(rhs.m_source);
        m_namableChild = ToReference(rhs.m_namableChild);
        m_indexableChild = ToReference(rhs.m_indexableChild);
        m_valueNeedQuotation = rhs.m_valueNeedQuotation; rhs.m_valueNeedQuotation = false;
        m_valueOffset = rhs.m_valueOffset; rhs.m_valueOffset = nullptr;
        m_valueLength = rhs.m_valueLength; rhs.m_valueLength = 0;
        return *this;
    }

    Context::operator bool() const
    {
        return IsValid();
    }

    const Context& Context::NullChild() const
    {
        return *BOSS_STORAGE_SYS(Context);
    }

    void Context::SetValueForSource(chars value, sint32 length)
    {
        m_valueNeedQuotation = true;
        m_valueOffset = value;
        m_valueLength = length;
    }

    chars Context::FindMark(chars value, const char mark)
    {
        chars V = value;
        while(*V && *V != mark) V++;
        return V;
    }

    chars Context::SkipBlank(chars value, bool exclude_nullzero)
    {
        chars V = value;
        while(*V && (*V == ' ' || *V == '\t' || *V == '\r' || *V == '\n')) V++;
        return V - (exclude_nullzero && *V == '\0');
    }

    chars Context::SkipBlankReverse(chars value)
    {
        chars V = value - 1;
        while(*V == ' ' || *V == '\t' || *V == '\r' || *V == '\n') V--;
        return V + 1;
    }

    Context* Context::InitSource(Context* anyparent)
    {
        m_source = anyparent->m_source;
        return this;
    }

    String Context::CalcErrorText(chars comment, chars begin, chars end)
    {
        sint32 Line = 1, Word = 1;
        for(chars pos = begin; pos < end; ++pos)
        {
            if(*pos == '\n')
            {
                Line++;
                Word = 1;
            }
            else
            {
                Word++;
                pos = pos + String::GetLengthOfFirstLetter(pos) - 1;
            }
        }
        return String::Format("%s(line:%d, word:%d)", comment, Line, Word);
    }

    bool Context::LoadJsonCore(chars src)
    {
        chars OldSrc = src;
        if(*src == '\0') return true;
        while(*src != '{' && *src != '[')
            if(*(++src) == '\0')
                return true;

        struct ParseStack
        {
            Context* Object;
            bool ChildIsIndexable;
        };
        Array<ParseStack, datatype_pod_canmemcpy_zeroset, 32> CurStack;
        CurStack.AtAdding().Object = nullptr;
        CurStack.AtAdding().Object = this;

        chars LastOffset = src;
        sint32 LastLength = 0;
        bool EtcMode = true;
        String EtcString;
        do switch(*src)
        {
        case ':':
            if(0 < LastLength)
            {
                Context* LastObject = CurStack[-1].Object;
                Context* NewChild = nullptr;
                if(LastLength == 3 && LastOffset[0] == '~' && LastOffset[1] == '[' && LastOffset[2] == ']')
                    NewChild = LastObject->m_indexableChild.AtAdding().InitSource(this);
                else NewChild = LastObject->m_namableChild(LastOffset, LastLength).InitSource(this);
                CurStack.AtAdding().Object = NewChild;
                LastLength = 0;
            }
            else
            {
                BOSS_ASSERT(CalcErrorText("잘못된 Json스크립트입니다", OldSrc, src), false);
                return false;
            }
            break;

        case '{': case '[':
            {
                LastLength = 0;
                const char EndMark = (*src == '{')? '}' : ']';
                while(*(++src) == '\0' || *src == ' ' || *src == '\t' || *src == '\r' || *src == '\n');
                if(*src != EndMark)
                {
                    --src;
                    if(EndMark == ']')
                    {
                        CurStack.At(-1).ChildIsIndexable = true;
                        Context* LastObject = CurStack[-1].Object;
                        Context* NewChild = LastObject->m_indexableChild.AtAdding().InitSource(this);
                        CurStack.AtAdding().Object = NewChild;
                    }
                }
                else if(CurStack.Count() == 2)
                    return true;
            }
            break;

        case ',': case '}': case ']':
            if(CurStack.Count() <= 2)
            {
                BOSS_ASSERT(CalcErrorText("잘못된 Json스크립트입니다", OldSrc, src), false);
                return false;
            }
            if(0 < LastLength)
            {
                Context* LastObject = CurStack[-1].Object;
                LastObject->SetValueForSource(LastOffset, LastLength);
                LastLength = 0;
            }
            else if(0 < EtcString.Length())
            {
                Context* LastObject = CurStack[-1].Object;
                LastObject->Set(EtcString, EtcString.Length(), false);
                EtcString.Empty();
            }

            CurStack.SubtractionOne();
            if(*src == ',' && CurStack[-1].ChildIsIndexable)
            {
                Context* LastObject = CurStack[-1].Object;
                Context* NewChild = LastObject->m_indexableChild.AtAdding().InitSource(this);
                CurStack.AtAdding().Object = NewChild;
            }
            else if((*src == '}' || *src == ']') && CurStack.Count() == 2)
                return true;
            EtcMode = false;
            break;

        case ' ': case '\t': case '\r': case '\n':
            EtcMode = false;
            break;

        case '\"': case '\'': // 스트링수집
            {
                LastOffset = src + 1;
                const char EndMark = *src;
                while(*(++src) && !(src[0] == EndMark && src[-1] != '\\'));
                LastLength = src - LastOffset;
                if(!*src) src--;
            }
            break;

        default:
            if(!EtcMode)
            {
                EtcMode = true;
                EtcString.Empty();
            }
            if(src[0] == '\\' && src[1] != '\0')
            {
                switch(*(++src))
                {
                case '\\': EtcString += '\\'; break;
                case 't': EtcString += '\t'; break;
                case 'r': EtcString += '\r'; break;
                case 'n': EtcString += '\n'; break;
                case '\"': EtcString += '\"'; break;
                case '\'': EtcString += '\''; break;
                }
            }
            else EtcString += *src;
            break;
        } while(*(++src) != '\0');

        BOSS_ASSERT(CalcErrorText("미완료된 Json스크립트입니다", OldSrc, src), false);
        return false;
    }

    void Context::SaveJsonCore(sint32 tab, String name, String& dst, bool indexable, bool lastchild) const
    {
        const bool HasNamableChild = (0 < m_namableChild.Count());
        const bool HasIndexableChild = (0 < m_indexableChild.Count());
        const bool HasChild = (HasNamableChild | HasIndexableChild);

        if(!indexable)
        {
            for(sint32 i = 0; i < tab; ++i)
                dst += '\t';
            dst += '\"';
            dst += name;
            dst.AddTailFast("\":");
            if(!HasChild)
            {
                if(m_valueNeedQuotation)
                {
                    dst += '\"';
                    if(m_valueOffset)
                        dst.AddTail(m_valueOffset, m_valueLength);
                    if(lastchild) dst += '\"';
                    else dst.AddTailFast("\",");
                }
                else
                {
                    if(m_valueOffset)
                        dst.AddTail(m_valueOffset, m_valueLength);
                    else dst.AddTailFast("\"\"");
                    if(!lastchild) dst += ',';
                }
            }
            dst.AddTailFast("\r\n");
        }
        else if(m_valueOffset && *m_valueOffset)
        {
            for(sint32 i = 0; i < tab; ++i)
                dst += '\t';
            if(m_valueNeedQuotation)
            {
                dst += '\"';
                dst.AddTail(m_valueOffset, m_valueLength);
                if(lastchild) dst += '\"';
                else dst.AddTailFast("\",");
            }
            else
            {
                dst.AddTail(m_valueOffset, m_valueLength);
                if(!lastchild) dst += ',';
            }
            dst.AddTailFast("\r\n");
        }
        else if(!HasChild)
        {
            for(sint32 i = 0; i < tab; ++i)
                dst += '\t';
            if(lastchild) dst.AddTailFast("{}");
            else dst.AddTailFast("{},");
            dst.AddTailFast("\r\n");
        }

        if(HasNamableChild)
        {
            for(sint32 i = 0; i < tab; ++i)
                dst += '\t';
            dst.AddTailFast("{\r\n");

            sint32 count = m_namableChild.Count();
            const void* param[3] = {&tab, &dst, &count};
            m_namableChild.AccessByCallback(SaveJsonCoreCB, param);

            for(sint32 i = 0; i < tab; ++i)
                dst += '\t';
            if(lastchild && !HasIndexableChild) dst.AddTailFast("}\r\n");
            else dst.AddTailFast("},\r\n");
        }
        if(HasIndexableChild)
        {
            for(sint32 i = 0; i < tab; ++i)
                dst += '\t';
            dst.AddTailFast("[\r\n");

            for(sint32 i = 0, iend = m_indexableChild.Count(); i < iend; ++i)
            {
                const Context& CurChild = m_indexableChild[i];
                CurChild.SaveJsonCore(tab + 1, String::FromInteger(i), dst, true, i + 1 == iend);
            }

            for(sint32 i = 0; i < tab; ++i)
                dst += '\t';
            if(lastchild) dst.AddTailFast("]\r\n");
            else dst.AddTailFast("],\r\n");
        }
    }

    void Context::SaveJsonCoreCB(const MapPath* path, Context* data, payload param)
    {
        const sint32 tab = *((sint32*) ((void**) param)[0]);
        String& dst = *((String*) ((void**) param)[1]);
        sint32& count = *((sint32*) ((void**) param)[2]);
        data->SaveJsonCore(tab + 1, path->GetPath(), dst, false, --count == 0);
    }

    bool Context::LoadXmlCore(chars src)
    {
        while(*src != '<')
            if(*(++src) == '\0')
                return true;

        Array<Context*, datatype_pod_canmemcpy> CurStack;
        CurStack.AtAdding() = nullptr;
        CurStack.AtAdding() = this;

        auto StackPop = [this](Array<Context*, datatype_pod_canmemcpy>& stack, chars value, sint32 offset)->bool
        {
            if(stack.Count() <= 2) return false;
            else if(stack[-1]->LengthOfIndexable() == 0)
            {
                stack[-2]->m_namableChild.Remove("~tree");
                if(0 < offset) stack[-2]->At("~value").InitSource(this)->SetValueForSource(value, offset);
            }
            stack.SubtractionOne();
            stack.SubtractionOne();
            return true;
        };

        auto AssertError = [](chars message)->bool
        {
            BOSS_ASSERT(message, false);
            return false;
        };

        chars LastSrc = src;
        bool ElementMode = false;
        do switch(*src)
        {
        case '<':
            if(!ElementMode)
            {
                if(src[+1] == '/')
                {
                    const Context* SavedContext = (CurStack.Count() < 2)? nullptr : CurStack[-2];
                    if(!StackPop(CurStack, LastSrc + 1, src - (LastSrc + 1)))
                        return AssertError("엘리먼트를 팝하는 과정에서 잔여스택이 부족합니다");
                    chars NameBegin = src += 2;
                    src = FindMark(src, '>');

                    if(*src != '>') return AssertError("엘리먼트를 완료하는데 실패하였습니다");
                    else
                    {
                        sint32 SavedNameLength = 0;
                        chars_endless SavedName = (*SavedContext)("@name").GetTextFast(SavedNameLength);
                        if(src - NameBegin != SavedNameLength || Memory::Compare(NameBegin, SavedName, SavedNameLength))
                            return AssertError("엘리먼트를 팝하는 과정에서 네임매칭에 실패하였습니다");
                    }
                }
                else
                {
                    Context* NewChild = CurStack[-1]->m_indexableChild.AtAdding().InitSource(this);
                    if(src[+1] == '!' && src[+2] == '-' && src[+3] == '-')
                    {
                        NewChild->At("@name").InitSource(this)->SetValueForSource(src + 1, 3);
                        chars CommentBegin = (src = SkipBlank(src + 4, false));
                        src = FindMark(src, '>');

                        if(*src == '>' && src[-1] == '-' && src[-2] == '-')
                        {
                            chars CommentEnd = SkipBlankReverse(src - 2);
                            NewChild->At("~value").InitSource(this)->SetValueForSource(CommentBegin, CommentEnd - CommentBegin);
                        }
                        else return AssertError("주석엘리먼트를 완료하는데 실패하였습니다");
                    }
                    else
                    {
                        chars NameBegin = ++src;
                        while(*src && *src != '>' && *src != ' ' && *src != '\t' && *src != '\r' && *src != '\n')
                            src++;

                        const bool HasClosing = (*src == '>');
                        const bool IsChildless = (HasClosing && (src[-1] == '/' || src[-1] == '?'));
                        NewChild->At("@name").InitSource(this)->SetValueForSource(NameBegin, src - NameBegin - IsChildless);

                        if(!HasClosing) ElementMode = true;
                        if(!IsChildless)
                        {
                            CurStack.AtAdding() = NewChild;
                            CurStack.AtAdding() = NewChild->At("~tree").InitSource(this);
                        }
                    }
                }
                LastSrc = src = SkipBlank(src, true);
            }
            else return AssertError("이전 엘리먼트가 완료되지 않고 <기호를 만났습니다");
            break;

        case '>':
            if(ElementMode)
            {
                ElementMode = false;
                if(src[-1] == '/' || src[-1] == '?')
                {
                    if(!StackPop(CurStack, nullptr, 0))
                        return AssertError("엘리먼트를 팝하는 과정에서 잔여스택이 부족합니다");
                }
                LastSrc = src;
            }
            break;

        case '=':
            if(ElementMode)
            {
                chars KeyEnd = SkipBlankReverse(src);
                Context* NewChild = CurStack[-2]->At(LastSrc, KeyEnd - LastSrc).InitSource(this);

                chars NameBegin = (src = SkipBlank(src + 1, false));
                const bool IsQuotes = (*NameBegin == '\"' || *NameBegin == '\'');
                if(IsQuotes)
                {
                    src++;
                    while(*src && *(src++) != *NameBegin);
                }
                else while(*src && *src != '>' && *src != '/' && *src != '?' && *src != ' ' && *src != '\t' && *src != '\r' && *src != '\n')
                    src++;
                NewChild->SetValueForSource(NameBegin + IsQuotes, src - NameBegin - IsQuotes * 2);

                if(*src == '>') src--;
                else LastSrc = src = SkipBlank(src, true);
            }
            break;
        } while(*(++src) != '\0');

        if(ElementMode)
            return AssertError("이전 엘리먼트가 완료되지 않고 문서가 종료되었습니다");
        if(2 < CurStack.Count())
            return AssertError("엘리먼트의 잔여스택이 남은 상태에서 문서가 종료되었습니다");
        return true;
    }

    void Context::SaveXmlCore(sint32 tab, String name, String& dst) const
    {
        String TabString;
        for(sint32 i = 0; i < tab; ++i)
            TabString += '\t';
        dst += TabString;
        dst += '<';

        // 자기 엘리먼트(@name)
        String Name;
        if(Context* NameOption = m_namableChild.Access("@name"))
        {
            sint32 ValueLength = 0;
            chars_endless Value = NameOption->GetTextFast(ValueLength);
            Name.AddTail(Value, ValueLength);
        }
        dst += Name;

        // 내부옵션들
        m_namableChild.AccessByCallback(SaveXmlCoreCB, &dst);

        const bool IsComment = (2 < Name.Length() && Name[0] == '!' && Name[1] == '-' && Name[2] == '-');
        bool HasChild = false;

        // 자기 스트링(~value)
        if(Context* ValueOption = m_namableChild.Access("~value"))
        {
            if(!IsComment) 
            {
                HasChild = true;
                dst += '>';
            }
            sint32 ValueLength = 0;
            chars_endless Value = ValueOption->GetTextFast(ValueLength);
            dst.AddTail(Value, ValueLength);
        }
        // 자식 엘리먼트(~tree)
        if(Context* TreeOption = m_namableChild.Access("~tree"))
        {
            if(!HasChild)
            {
                HasChild = true;
                dst.AddTailFast(">\r\n");
            }
            for(sint32 i = 0, iend = TreeOption->m_indexableChild.Count(); i < iend; ++i)
            {
                const Context& CurChild = TreeOption->m_indexableChild[i];
                CurChild.SaveXmlCore(tab + 1, String::FromInteger(i), dst);
            }
            dst += TabString;
        }

        // 마감
        if(HasChild)
        {
            dst.AddTailFast("</");
            dst += Name;
            dst.AddTailFast(">\r\n");
        }
        else if(IsComment)
            dst.AddTailFast("-->\r\n");
        else if(0 < Name.Length() && Name[0] == '?')
            dst.AddTailFast("?>\r\n");
        else dst.AddTailFast("/>\r\n");
    }

    void Context::SaveXmlCoreCB(const MapPath* path, Context* data, payload param)
    {
        String& dst = *((String*) param);
        const String Name = path->GetPath();
        if(String::Compare(Name, "@name") && String::Compare(Name, "~value") && String::Compare(Name, "~tree"))
        {
            dst += ' ';
            dst += Name;
            dst.AddTailFast("=\"");
            sint32 ValueLength = 0;
            chars_endless Value = data->GetTextFast(ValueLength);
            dst.AddTail(Value, ValueLength);
            dst += '\"';
        }
    }

    const String& Context::GetBinHeader()
    {return *BOSS_STORAGE_SYS(String, String::Format("bin{%s %s}", __TIME__, __DATE__));}

    bytes Context::LoadBinCore(bytes src)
    {
        // 데이터(원본)
        sint32 ValueLength = src[0];
        ValueLength |= (src[1] & 0xFF) << 8;
        ValueLength |= (src[2] & 0xFF) << 16;
        ValueLength |= (src[3] & 0xFF) << 24;
        src += sizeof(sint32);
        chars Value = (chars) src;
        src += ValueLength + 1;

        // 데이터(정수), 데이터(실수), 키워드식 자식수량, 배열식 자식수량
        sint32 DataField[4];
        Memory::Copy(DataField, src, sizeof(sint32) * 4);
        src += sizeof(sint32) * 4;

        // 셋팅
        Set(Value, ValueLength);

        // 키워드식 자식
        for(sint32 i = 0; i < DataField[2]; ++i)
        {
            // 자식의 자기명칭
            sint32 NameLength = src[0];
            NameLength |= (src[1] & 0xFF) << 8;
            NameLength |= (src[2] & 0xFF) << 16;
            NameLength |= (src[3] & 0xFF) << 24;
            src += sizeof(sint32);
            chars Name = (chars) src;
            src += NameLength + 1;

            // 자식루프
            Context& NewChild = m_namableChild(Name);
            src = NewChild.LoadBinCore(src);
        }

        // 배열식 자식
        if(0 < DataField[3])
        {
            for(sint32 i = 0; i < DataField[3]; ++i)
            {
                // 자기명칭 스킵
                src += sizeof(sint32) + 1;

                // 자식루프
                Context& NewChild = m_indexableChild.At(i);
                src = NewChild.LoadBinCore(src);
            }
        }
        return src;
    }

    void Context::SaveBinCore(uint08s& dst, const String& name) const
    {
        // 자기명칭
        const sint32 NameLength = name.Length();
        Memory::Copy(dst.AtDumping(dst.Count(), sizeof(sint32)), &NameLength, sizeof(sint32));
        Memory::Copy(dst.AtDumping(dst.Count(), NameLength + 1), (chars) name, NameLength + 1);

        // 데이터(원본)
        Memory::Copy(dst.AtDumping(dst.Count(), sizeof(sint32)), &m_valueLength, sizeof(sint32));
        Memory::Copy(dst.AtDumping(dst.Count(), m_valueLength), m_valueOffset, m_valueLength);
        *dst.AtDumping(dst.Count(), 1) = (uint08) '\0';

        // 데이터(정수)
        const sint32 IntValue = GetInt();
        Memory::Copy(dst.AtDumping(dst.Count(), sizeof(sint32)), &IntValue, sizeof(sint32));
        // 데이터(실수)
        const float FloatValue = GetFloat();
        Memory::Copy(dst.AtDumping(dst.Count(), sizeof(float)), &FloatValue, sizeof(float));
        // 키워드식 자식수량
        const sint32 NChildCount = m_namableChild.Count();
        Memory::Copy(dst.AtDumping(dst.Count(), sizeof(sint32)), &NChildCount, sizeof(sint32));
        // 배열식 자식수량
        const sint32 IChildCount = m_indexableChild.Count();
        Memory::Copy(dst.AtDumping(dst.Count(), sizeof(sint32)), &IChildCount, sizeof(sint32));

        // 키워드식 자식
        for(sint32 i = 0; i < NChildCount; ++i)
        {
            chararray GetName;
            const Context* CurChild = m_namableChild.AccessByOrder(i, &GetName);
            CurChild->SaveBinCore(dst, GetName);
        }

        // 배열식 자식
        for(sint32 i = 0; i < IChildCount; ++i)
        {
            const Context& CurChild = m_indexableChild[i];
            CurChild.SaveBinCore(dst);
        }
    }

    void Context::CollectCore(const Context* parent, chars key, chars value, sint32 length, CollectedContexts* result, CollectOption option) const
    {
        if(const Context* CurChild = m_namableChild.Access(key))
        {
            sint32 GetLength = 0;
            chars_endless GetValue = CurChild->GetTextFast(GetLength);
            if(GetLength == length && !Memory::Compare(GetValue, value, length))
            {
                if(option == CO_GetParent)
                    result->AtAdding() = this;
                else result->AtAdding() = (parent)? parent : &NullChild();
            }
        }

        const void* param[6] = {this, key, value, &length, result, &option};
        m_namableChild.AccessByCallback(CollectCoreCB, param);
        for(sint32 i = 0, iend = m_indexableChild.Count(); i < iend; ++i)
        {
            const Context& CurChild = m_indexableChild[i];
            CurChild.CollectCore(this, key, value, length, result, option);
        }
    }

    void Context::CollectCoreCB(const MapPath* path, Context* data, payload param)
    {
        const Context* parent = (const Context*) ((void**) param)[0];
        chars key = (chars) ((void**) param)[1];
        chars value = (chars) ((void**) param)[2];
        sint32 length = *((sint32*) ((void**) param)[3]);
        CollectedContexts* result = (CollectedContexts*) ((void**) param)[4];
        CollectOption option = *((CollectOption*) ((void**) param)[5]);
        data->CollectCore(parent, key, value, length, result, option);
    }

    void Context::DebugPrintCore(sint32 tab, String name, bool indexable) const
    {
        #if !BOSS_NDEBUG | BOSS_NEED_RELEASE_TRACE
            char* TabString = new char[4 * tab + 1];
            Memory::Set(TabString, ' ', 4 * tab);
            TabString[4 * tab] = '\0';
            BOSS_TRACE(String::Format((indexable)? ">>>> %s[%s] " : ">>>> %s<%s> ", TabString, (chars) name)
                .AddTail(m_valueOffset, m_valueLength));
            delete[] TabString;

            m_namableChild.AccessByCallback(DebugPrintCoreCB, &tab);
            for(sint32 i = 0, iend = m_indexableChild.Count(); i < iend; ++i)
            {
                const Context& CurChild = m_indexableChild[i];
                CurChild.DebugPrintCore(tab + 1, String::FromInteger(i), true);
            }
        #endif
    }

    void Context::DebugPrintCoreCB(const MapPath* path, Context* data, payload param)
    {data->DebugPrintCore(*((sint32*) param) + 1, path->GetPath(), false);}

    Context::StringSource::StringSource()
    {
        m_buffer = nullptr;
        m_string = nullptr;
    }

    Context::StringSource::~StringSource()
    {
        Buffer::Free(m_buffer);
    }

    void Context::StringSource::InitString(buffer src)
    {
        BOSS_ASSERT("중복된 초기화입니다", !m_buffer && !m_string);
        BOSS_ASSERT("src인수는 1바이트 단위여야 합니다", Buffer::SizeOf(src) == 1);
        BOSS_ASSERT("src인수는 null문자로 끝나야 합니다", ((chars) src)[Buffer::CountOf(src) - 1] == '\0');
        m_buffer = src;
    }

    void Context::StringSource::InitString(ScriptOption option, chars src, sint32 length)
    {
        BOSS_ASSERT("중복된 초기화입니다", !m_buffer && !m_string);
        if(option == SO_NeedCopy)
        {
            BOSS_ASSERT("SO_NeedCopy모드에서 length인수는 -1값이 될 수 없습니다", length != -1);
            m_buffer = Buffer::Alloc(BOSS_DBG length + 1);
            Memory::Copy(m_buffer, src, length);
            ((char*) m_buffer)[length] = '\0';
        }
        else m_string = src;
    }

    chars Context::StringSource::GetString() const
    {
        return (chars) (((ublock) m_buffer) | ((ublock) m_string));
    }
}
