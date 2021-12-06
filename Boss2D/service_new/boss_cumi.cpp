#include <boss.hpp>
#include "boss_cumi.hpp"

#include <platform/boss_platform.hpp>
#include <service/boss_zay.hpp>

namespace BOSS {

////////////////////////////////////////////////////////////////////////////////
// 토큰도구
struct CumiTokenMethodP
{
    CumiEditor::RenderCB mRenderBodyCB {nullptr};
    CumiEditor::RenderCB mRenderBlankCB {nullptr};
    CumiEditor::RenderCB mRenderCursorCB {nullptr};
    CumiEditor::KeyCB mKeyCB {nullptr};
    CumiEditor::TextCB mTextCB {nullptr};
};
static Map<String> gCumiTokenNames;
static Map<CumiTokenMethodP> gCumiTokenMethods;

////////////////////////////////////////////////////////////////////////////////
// IME시스템 선언부
class IMESystem
{
private:
    IMESystem()
    {
        mRepeatCode = 0;
        mRepeatKey = '\0';
        mRepeatMsec = 0;
        mCapturedCumi = nullptr;
        mLastCapturedCumi = nullptr;
    }
    ~IMESystem()
    {
    }

public:
    static IMESystem& ST()
    {static IMESystem _; return _;}

public:
    void Begin();
    void End();
    void BindKey(void* graphics, chars uiname, CumiNode* cumi, CumiEditor::KeyCB key);
    void BindMouse(void* graphics, chars uiname, CumiNode* cumi, sint32 group, sint32 index);
    void SetCapture(chars uiname, CumiNode* cumi);
    void EraseCapture(CumiNode* cumi);

private:
    void KeyProcess(CumiNode& cumi, CumiEditor::KeyCB key, ZayObject* view);

private:
    sint32 mRepeatCode;
    char mRepeatKey;
    sint64 mRepeatMsec;
    CumiNode* mCapturedCumi;
    CumiNode* mLastCapturedCumi;
};

////////////////////////////////////////////////////////////////////////////////
// 워드
class CumiWordP : CumiNode
{
public:
    CumiWordP(chars word, sint32 length)
    {
        char* CurUTF8 = mUTF8;
        mUTF32 = 0;
        switch(6 - length)
        {
        case 0: *(CurUTF8++) = *word; mUTF32 += (*(word++) & 0xFF); mUTF32 <<= 6;
        case 1: *(CurUTF8++) = *word; mUTF32 += (*(word++) & 0xFF); mUTF32 <<= 6;
        case 2: *(CurUTF8++) = *word; mUTF32 += (*(word++) & 0xFF); mUTF32 <<= 6;
        case 3: *(CurUTF8++) = *word; mUTF32 += (*(word++) & 0xFF); mUTF32 <<= 6;
        case 4: *(CurUTF8++) = *word; mUTF32 += (*(word++) & 0xFF); mUTF32 <<= 6;
        case 5: *(CurUTF8++) = *word; mUTF32 += (*(word++) & 0xFF);
        }
        *CurUTF8 = '\0';
        static const uint32 OffsetsFromUTF8[6] = {
            0x00000000, 0x00003080, 0x000E2080,
            0x03C82080, 0xFA082080, 0x82082080};
        mUTF32 -= OffsetsFromUTF8[length - 1];
    }
    ~CumiWordP() override {}

private:
    size64 Render(void* graphics, sint32 id, CumiEditor::RenderMode mode) const override
    {
        ZayPanel& Panel = *((ZayPanel*) graphics);
        if(mode != CumiEditor::RenderMode::SizeOnly)
            Panel.text(mUTF8, UIFA_LeftMiddle);

        size64 Result;
        Result.w = Platform::Graphics::GetStringWidth(mUTF8);
        Result.h = Platform::Graphics::GetStringHeight();
        return Result;
    }

    bool IsTypeWord(char* utf8, uint32* utf32) const override
    {
        if(utf8) Memory::Copy(utf8, mUTF8, sizeof(char) * 7);
        if(utf32) *utf32 = mUTF32;
        return true;
    }

private:
    char mUTF8[7];
    uint32 mUTF32;
};

////////////////////////////////////////////////////////////////////////////////
// 공백
class CumiBlankP : CumiNode
{
public:
    CumiBlankP(chars type, CumiNode* parent) : mType(type), mParent(parent) {}
    ~CumiBlankP() override {}

private:
    size64 Render(void* graphics, sint32 id, CumiEditor::RenderMode mode) const override
    {
        if(const auto CurTokenMethod = gCumiTokenMethods.Access(mType))
        if(CurTokenMethod->mRenderBlankCB)
            return CurTokenMethod->mRenderBlankCB(*mParent, graphics, id, mode);
        return {4, 0};
    }

    bool IsTypeBlank() const override
    {
        return true;
    }

private:
    chars const mType;
    CumiNode* const mParent;
};

////////////////////////////////////////////////////////////////////////////////
// 커서
class CumiCursorP : CumiNode
{
public:
    CumiCursorP(chars type, CumiNode* parent) : mType(type), mParent(parent) {}
    ~CumiCursorP() override {}

private:
    size64 Render(void* graphics, sint32 id, CumiEditor::RenderMode mode) const override
    {
        if(const auto CurTokenMethod = gCumiTokenMethods.Access(mType))
        if(CurTokenMethod->mRenderCursorCB)
            return CurTokenMethod->mRenderCursorCB(*mParent, graphics, id, mode);
        return {0, 0};
    }

    bool IsTypeCursor() const override
    {
        return true;
    }

private:
    chars const mType;
    CumiNode* const mParent;
};

////////////////////////////////////////////////////////////////////////////////
// 토큰
class CumiTokenP : CumiNode
{
public:
    CumiTokenP(chars type, CumiTokenP* parent) : mParent(parent)
    {
        if(auto CurTokenName = gCumiTokenNames.Access(type))
            mType = *CurTokenName;
        else
        {
            auto& NewTokenName = gCumiTokenNames(type);
            NewTokenName = type;
            mType = NewTokenName;
        }

        static sint32 IDCounter = 0;
        mID = IDCounter++;
        mChildPos = 0;
        mChildGap = 0;
        mNeedCapture = false;
    }
    ~CumiTokenP() override
    {
        Clear();
        IMESystem::ST().EraseCapture((CumiNode*) this);
    }

public:
    void Clear()
    {
        mOptions.Reset();
        for(sint32 g = 0, gend = mChildGroups.Count(); g < gend; ++g)
        for(sint32 i = 0, iend = mChildGroups[g].Count(); i < iend; ++i)
            delete mChildGroups[g][i];
        mChildGroups.Clear();
    }

    sint32 ParseOptions(chars script, sint32 length)
    {
        // 쿠미스크립트식 옵션파서
        // "["
        // ">"
        // ".opt1_v1.opt2_v2["
        // ".opt1_v1.opt2_v2>"
        for(sint32 i = 0, oldi = 0; i < length; ++i)
        {
            if(script[i] == '.' || script[i] == '[' || script[i] == '>')
            {
                if(oldi < i)
                {
                    const String NewOption(script + oldi, i - oldi);
                    const sint32 UnderBarPos = NewOption.Find(0, "_");
                    if(UnderBarPos == -1) mOptions(NewOption);
                    else mOptions(NewOption.Left(UnderBarPos)) =
                        NewOption.Right(NewOption.Length() - (UnderBarPos + 1));
                }
                oldi = i + 1;
                if(script[i] != '.')
                    return i;
            }
        }
        return 0;
    }

    sint32 ParseChildGroups(chars script, sint32 length)
    {
        // 쿠미스크립트식 토큰파서
        // ">"
        // "[...][...]>"
        for(sint32 i = 0; i < length; ++i)
        {
            if(script[i] == '[')
            {
                i++;
                const sint32 CurGroup = AddGroup();
                while(script[i] != ']')
                    i += AddChild(CurGroup, &script[i], length - i);
            }
            else if(script[i] == '>')
                return i;
        }
        return 0;
    }

    sint32 ParseLastGroup(chars script, sint32 length)
    {
        // 쿠미스크립트식 토큰파서
        // ">"
        // "[...][...]>"
        if(0 < mChildGroups.Count())
        {
            const sint32 Group = mChildGroups.Count() - 1;
            for(sint32 i = 0; i < length; ++i)
            {
                if(script[i] == '[')
                {
                    i++;
                    while(script[i] != ']')
                        i += AddChild(Group, &script[i], length - i);
                }
                else if(script[i] == '>')
                    return i;
            }
        }
        return 0;
    }

    void ParseWords(chars script, sint32 length)
    {
        if(mChildGroups.Count() == 0)
            AddGroup();

        const sint32 Group = mChildGroups.Count() - 1;
        for(sint32 i = 0; i < length;)
            i += AddWord(Group, &script[i]);
    }

    String BuildOptions() const
    {
        // ".opt1.opt2"
        // ".opt1_v1.opt2_v2"
        String Collector;
        mOptions.AccessByCallback(
            [](const MapPath* path, String* data, payload param)->void
            {
                auto& Collector = *((String*) param);
                Collector += '.';
                Collector += &path->GetPath()[0];
                if(0 < data->Length())
                {
                    Collector += '_';
                    Collector += *data;
                }
            }, &Collector);
        return Collector;
    }

    String BuildChildGroups() const
    {
        // "[...]"
        // "[...][...]"
        String Collector;
        for(sint32 g = 0, gend = mChildGroups.Count(); g < gend; ++g)
        {
            Collector += '[';
            for(sint32 i = 0, iend = mChildGroups[g].Count(); i < iend; ++i)
            {
                auto CurChild = mChildGroups[g][i];
                if(CurChild->IsTypeBlank())
                    Collector += ' ';
                else if(CurChild->IsTypeCursor())
                    Collector += '$';
                else
                {
                    char Utf8Code[7];
                    if(CurChild->IsTypeWord(Utf8Code))
                        Collector += Utf8Code;
                    else
                    {
                        String TypeName;
                        if(CurChild->IsTypeToken(&TypeName))
                        {
                            Collector += '<';
                            Collector += TypeName;
                            Collector += ((const CumiTokenP*) CurChild)->BuildOptions();
                            Collector += ((const CumiTokenP*) CurChild)->BuildChildGroups();
                            Collector += '>';
                        }
                    }
                }
            }
            Collector += ']';
        }
        return Collector;
    }

    String CutCursorGroups()
    {
        // "[...]"
        // "[...][...]"
        String Collector;
        bool IsCursorRear = false;
        for(sint32 g = 0, gend = mChildGroups.Count(); g < gend; ++g)
        {
            Collector += '[';
            for(sint32 i = 0; i < mChildGroups[g].Count();)
            {
                auto CurChild = mChildGroups[g][i];
                if(!IsCursorRear)
                if(CurChild->IsTypeCursor() || CurChild->HasCursor())
                    IsCursorRear = true;

                if(IsCursorRear)
                {
                    if(CurChild->IsTypeBlank())
                        Collector += ' ';
                    else if(CurChild->IsTypeCursor())
                        Collector += '$';
                    else
                    {
                        char Utf8Code[7];
                        if(CurChild->IsTypeWord(Utf8Code))
                            Collector += Utf8Code;
                        else
                        {
                            String TypeName;
                            if(CurChild->IsTypeToken(&TypeName))
                            {
                                Collector += '<';
                                Collector += TypeName;
                                Collector += ((const CumiTokenP*) CurChild)->BuildOptions();
                                Collector += ((const CumiTokenP*) CurChild)->BuildChildGroups();
                                Collector += '>';
                            }
                        }
                    }
                    mChildGroups.At(g).SubtractionSection(i);
                }
                else i++;
            }
            Collector += ']';
        }
        return Collector;
    }

    String BuildText() const
    {
        String Collector;
        for(sint32 g = 0, gend = mChildGroups.Count(); g < gend; ++g)
        for(sint32 i = 0, iend = mChildGroups[g].Count(); i < iend; ++i)
        {
            auto CurChild = mChildGroups[g][i];
            if(CurChild->IsTypeBlank())
                Collector += ' ';
            else if(!CurChild->IsTypeCursor())
            {
                char Utf8Code[7];
                if(CurChild->IsTypeWord(Utf8Code))
                    Collector += Utf8Code;
                else if(CurChild->IsTypeToken())
                    Collector += ((const CumiTokenP*) CurChild)->BuildText();
            }
        }

        if(const auto CurTokenMethod = gCumiTokenMethods.Access(mType))
        if(CurTokenMethod->mTextCB)
            Collector = CurTokenMethod->mTextCB(Collector);
        return Collector;
    }

    void SetUpdater(CumiEditor::UpdateCB updater)
    {
        mUpdater = updater;
    }

    bool SendNotify(chars topic, id_share in = nullptr) const
    {
        if(auto CurView = GetView())
        {
            Platform::SendNotify(*CurView, topic, in);
            return true;
        }
        BOSS_TRACE("전달되지 못한 SendNotify : %s", topic);
        return false;
    }

    bool SendDirectNotify(chars topic, id_share in = nullptr) const
    {
        if(auto CurView = GetView())
        {
            Platform::SendDirectNotify(*CurView, topic, in);
            return true;
        }
        BOSS_TRACE("전달되지 못한 SendDirectNotify : %s", topic);
        return false;
    }

    void SetView(h_view view)
    {
        mView = view;
    }

    const h_view* GetView() const
    {
        if(mView.get())
            return &mView;
        return nullptr;
    }

private:
    sint32 AddGroup()
    {
        mChildGroups.AtAdding();
        return mChildGroups.Count() - 1;
    }

    sint32 AddChild(sint32 group, chars script, sint32 length)
    {
        if(*script == '<')
        {
            // 쿠미스크립트식 자식추가
            // "<type>..."
            // "<type[...][...]>..."
            // "<type.opt1_v1.opt2_v2>..."
            // "<type.opt1_v1.opt2_v2[...][...]>..."
            for(sint32 i = 1; i < length; ++i)
            {
                if(script[i] == '.' || script[i] == '[' || script[i] == '>')
                {
                    const String NewType(&script[1], i - 1);
                    auto NewToken = new CumiTokenP(NewType, this);
                    i += NewToken->ParseOptions(&script[i], length - i);
                    i += NewToken->ParseChildGroups(&script[i], length - i);

                    // 자식노드 추가
                    auto& NewChild = mChildGroups.At(group).AtAdding();
                    NewChild = (CumiNode*) NewToken;
                    if(script[i] == '>')
                        return i + 1;
                }
            }
        }
        // 공백 자식추가
        else if(*script == ' ' || *script == '\t')
            return AddBlank(group);
        // 커서 자식추가
        else if(*script == '$')
            return AddCursorForGroup(group);

        // 일반워드식 자식추가
        const sint32 WordJump = (*script == '&')? 1 : 0;
        return AddWord(group, &script[WordJump]) + WordJump;
    }

    sint32 AddBlank(sint32 group)
    {
        // 공백노드 추가
        auto& NewChild = mChildGroups.At(group).AtAdding();
        NewChild = (CumiNode*) new CumiBlankP(mType, this);
        return 1;
    }

    sint32 AddCursorForGroup(sint32 group)
    {
        // 커서노드 추가
        auto& NewChild = mChildGroups.At(group).AtAdding();
        NewChild = (CumiNode*) new CumiCursorP(mType, this);
        NeedCapture();
        return 1;
    }

    sint32 AddWord(sint32 group, chars word)
    {
        const sint32 WordLength = String::GetLengthOfFirstLetter(word);

        // 자식노드 추가
        auto& NewChild = mChildGroups.At(group).AtAdding();
        NewChild = (CumiNode*) new CumiWordP(word, WordLength);
        return WordLength;
    }

public:
    size64 Render(void* graphics, sint32 id, CumiEditor::RenderMode mode) const override
    {
        ZayPanel& Panel = *((ZayPanel*) graphics);
        size64 Result = {0, 0};
        const sint32 LeftPos = Panel.toview(0, 0).x;

        if(const auto CurTokenMethod = gCumiTokenMethods.Access(mType))
        {
            if(mode == CumiEditor::RenderMode::Writeable && CurTokenMethod->mKeyCB)
            {
                const String UIName = String::Format("ui_cumi_ime_%d_%d", id, mID);
                IMESystem::ST().BindKey(graphics, UIName, (CumiNode*) this, CurTokenMethod->mKeyCB);
                if(mNeedCapture)
                {
                    mNeedCapture = false;
                    IMESystem::ST().SetCapture(UIName, (CumiNode*) this);
                }
            }
            Result = CurTokenMethod->mRenderBodyCB(*this, graphics, id, mode);
        }
        else
        {
            ZAY_LTRB_SCISSOR(Panel, 0, 4, Panel.w(), Panel.h() - 4)
            {
                auto ChildGroupSize = GetChildGroupSize(0, graphics, id);
                Result.w = ChildGroupSize.w + 4 * 2;
                Result.h = Panel.h();

                if(mode != CumiEditor::RenderMode::SizeOnly)
                {
                    ZAY_RGBA(Panel, 255, 0, 0, 128)
                    ZAY_LTRB(Panel, 2, 2, Result.w - 2, Result.h - 2)
                        Panel.rect(2);
                    ZAY_RGBA(Panel, 255, 0, 0, 64)
                    ZAY_XYWH(Panel, 4, 0, ChildGroupSize.w, Panel.h())
                        RenderChildGroup(0, graphics, id, mode == CumiEditor::RenderMode::ReadOnly);
                }
            }
        }

        mChildGap = mChildPos - LeftPos;
        return Result;
    }

    void RenderChildGroup(sint32 group, void* graphics, sint32 id, bool readonly) const override
    {
        ZayPanel& Panel = *((ZayPanel*) graphics);
        if(group == 0)
            mChildPos = Panel.toview(0, 0).x;

        if(0 <= group && group < mChildGroups.Count())
        for(sint32 i = 0, iend = mChildGroups[group].Count(), pos = 0; i < iend; ++i)
        {
            auto CurChild = mChildGroups[group][i];
            auto CurSize = CurChild->Render(graphics, id, CumiEditor::RenderMode::SizeOnly);
            ZAY_XYWH(Panel, pos, 0, CurSize.w, Panel.h())
            {
                if(!readonly)
                {
                    const String UIName = String::Format("ui_cumi_ime_%d_%d_%d_%d", id, mID, group, i);
                    IMESystem::ST().BindMouse(graphics, UIName, (CumiNode*) this, group, i);
                }
                CurChild->Render(graphics, id, (readonly)? CumiEditor::RenderMode::ReadOnly : CumiEditor::RenderMode::Writeable);
                if(!readonly && CurChild->IsTypeCursor())
                {
                    mCursorPos = Panel.toview(Panel.w() / 2, Panel.h() / 2);
                    ((CumiTokenP*) GetTopNode())->mCursorPos = mCursorPos;
                }
            }
            pos += CurSize.w;
        }
    }

    size64 GetChildGroupSize(sint32 group, void* graphics, sint32 id) const override
    {
        sint32 SumWidth = 0;
        sint32 MaxHeight = 0;
        if(0 <= group && group < mChildGroups.Count())
        for(sint32 i = 0, iend = mChildGroups[group].Count(); i < iend; ++i)
        {
            auto CurSize = mChildGroups[group][i]->Render(graphics, id, CumiEditor::RenderMode::SizeOnly);
            SumWidth += CurSize.w;
            MaxHeight = Math::Max(MaxHeight, CurSize.h);
        }
        return {SumWidth, MaxHeight};
    }

    void Update() const override
    {
        if(mUpdater)
            mUpdater();
    }

    bool IsTypeToken(String* type) const override
    {
        if(type) *type = mType;
        return true;
    }

    void ChangeType(chars type) override
    {
        if(auto CurTokenName = gCumiTokenNames.Access(type))
            mType = *CurTokenName;
        else
        {
            auto& NewTokenName = gCumiTokenNames(type);
            NewTokenName = type;
            mType = NewTokenName;
        }
    }

    String* GetOption(chars key) const override
    {
        return mOptions.Access(key);
    }

public:
    CumiNode* GetTopNode() const override
    {
        CumiTokenP* CurToken = (CumiTokenP*) this;
        while(CurToken->mParent)
            CurToken = CurToken->mParent;
        return CurToken;
    }

    bool CanCursor() const override
    {
        if(const auto CurTokenMethod = gCumiTokenMethods.Access(mType))
            return (CurTokenMethod->mKeyCB != nullptr);
        return false;
    }

    bool HasCursor() const override
    {
        for(sint32 g = 0, gend = mChildGroups.Count(); g < gend; ++g)
        for(sint32 i = 0, iend = mChildGroups[g].Count(); i < iend; ++i)
        {
            auto CurChild = mChildGroups[g][i];
            if(CurChild->IsTypeCursor() || CurChild->HasCursor())
                return true;
        }
        return false;
    }

    Point GetCursorPos() const override
    {
        return mCursorPos;
    }

    bool IsCursorFront() const override
    {
        if(0 < mChildGroups.Count())
        if(0 < mChildGroups[0].Count())
            return mChildGroups[0][0]->IsTypeCursor();
        return false;
    }

    bool IsCursorRear() const override
    {
        if(0 < mChildGroups.Count())
        if(0 < mChildGroups[-1].Count())
            return mChildGroups[-1][-1]->IsTypeCursor();
        return false;
    }

    void SendTextNotify(chars topic, chars text) const override
    {
        ((CumiTokenP*) GetTopNode())->SendNotify(String::Format("cumi:%s", topic), String(text));
    }

    void SendCursorNotify(chars topic) const override
    {
        sint32s XY;
        XY.AtAdding() = (sint32) mCursorPos.x;
        XY.AtAdding() = (sint32) mCursorPos.y;
        ((CumiTokenP*) GetTopNode())->SendNotify(String::Format("cumi:%s", topic), XY);
    }

    void NeedCapture() override
    {
        mNeedCapture = true;
    }

    void CursorToLeft() override
    {
        for(sint32 g = 0, gend = mChildGroups.Count(); g < gend; ++g)
        for(sint32 i = 0, iend = mChildGroups[g].Count(); i < iend; ++i)
        {
            if(mChildGroups[g][i]->IsTypeCursor())
            {
                if(i == 0)
                {
                    if(0 < g) // 앞쪽 그룹의 마지막으로 커서이동
                    {
                        auto& NewChild = mChildGroups.At(g - 1).AtAdding();
                        NewChild = (CumiNode*) new CumiCursorP(mType, this);
                        mChildGroups.At(g).SubtractionSection(i);
                    }
                    else if(mParent) // 부모로 나가서 자기 앞쪽으로 커서이동
                    {
                        mParent->AddCursorByOther(this, true);
                        mChildGroups.At(g).SubtractionSection(i);
                    }
                }
                else if(mChildGroups.At(g).At(i - 1)->CanCursor()) // 자기 앞쪽 자식의 내부로 커서이동
                {
                    mChildGroups.At(g).At(i - 1)->AddCursorToRear();
                    mChildGroups.At(g).SubtractionSection(i);
                }
                else // 자기 앞쪽으로 커서이동
                {
                    auto Temp = mChildGroups[g][i - 1];
                    mChildGroups.At(g).At(i - 1) = mChildGroups[g][i];
                    mChildGroups.At(g).At(i) = Temp;
                }
                return;
            }
        }
    }

    void CursorToRight() override
    {
        for(sint32 g = 0, gend = mChildGroups.Count(); g < gend; ++g)
        for(sint32 i = 0, iend = mChildGroups[g].Count(); i < iend; ++i)
        {
            if(mChildGroups[g][i]->IsTypeCursor())
            {
                if(i == iend - 1)
                {
                    if(g < gend - 1) // 뒷쪽 그룹의 처음으로 커서이동
                    {
                        auto NewChild = (CumiNode*) new CumiCursorP(mType, this);
                        mChildGroups.At(g + 1).DeliveryOne(0, ToReference(NewChild));
                        mChildGroups.At(g).SubtractionSection(i);
                    }
                    else if(mParent) // 부모로 나가서 자기 뒷쪽으로 커서이동
                    {
                        mParent->AddCursorByOther(this, false);
                        mChildGroups.At(g).SubtractionSection(i);
                    }
                }
                else if(mChildGroups.At(g).At(i + 1)->CanCursor()) // 자기 뒷쪽 자식의 내부로 커서이동
                {
                    mChildGroups.At(g).At(i + 1)->AddCursorToFront();
                    mChildGroups.At(g).SubtractionSection(i);
                }
                else // 자기 뒷쪽으로 커서이동
                {
                    auto Temp = mChildGroups[g][i + 1];
                    mChildGroups.At(g).At(i + 1) = mChildGroups[g][i];
                    mChildGroups.At(g).At(i) = Temp;
                }
                return;
            }
        }
    }

    void CursorToDelete() override
    {
        for(sint32 g = 0, gend = mChildGroups.Count(); g < gend; ++g)
        for(sint32 i = 0, iend = mChildGroups[g].Count(); i < iend; ++i)
        {
            if(mChildGroups[g][i]->IsTypeCursor())
            {
                if(i + 1 < iend)
                    mChildGroups.At(g).SubtractionSection(i + 1);
                NeedCapture();
                return;
            }
        }
    }

    void CursorToBackSpace() override
    {
        for(sint32 g = 0, gend = mChildGroups.Count(); g < gend; ++g)
        for(sint32 i = 0, iend = mChildGroups[g].Count(); i < iend; ++i)
        {
            if(mChildGroups[g][i]->IsTypeCursor())
            {
                if(0 <= i - 1)
                    mChildGroups.At(g).SubtractionSection(i - 1);
                NeedCapture();
                return;
            }
        }
    }

    void ClearCursor(const CumiNode* except) override
    {
        for(sint32 g = 0, gend = mChildGroups.Count(); g < gend; ++g)
        for(sint32 i = mChildGroups[g].Count() - 1; 0 <= i; --i)
        {
            if(!mChildGroups[g][i]->IsTypeCursor())
                mChildGroups[g][i]->ClearCursor(except);
            else if(mChildGroups[g][i] != except)
                mChildGroups.At(g).SubtractionSection(i);
        }
    }

    const CumiNode* AddCursor(sint32 group, sint32 index) override
    {
        if(0 <= group && group < mChildGroups.Count())
        {
            auto NewChild = (CumiNode*) new CumiCursorP(mType, this);
            if(mChildGroups[group].Count() <= index)
                mChildGroups.At(group).AtAdding() = NewChild;
            else if(0 <= index)
                mChildGroups.At(group).DeliveryOne(index, ToReference(NewChild));
            NeedCapture();
            return NewChild;
        }
        return nullptr;
    }

    const CumiNode* AddCursorForInside(void* graphics, sint32 posx) override
    {
        posx -= mChildGap;
        if(0 < mChildGroups.Count())
        for(sint32 i = 0, iend = mChildGroups[0].Count(); i < iend; ++i)
        {
            auto CurChildWidth = mChildGroups[0][i]->Render(graphics, 0, CumiEditor::RenderMode::SizeOnly).w;
            if(posx <= CurChildWidth / 2)
            {
                auto NewChild = (CumiNode*) new CumiCursorP(mType, this);
                mChildGroups.At(0).DeliveryOne(i, ToReference(NewChild));
                NeedCapture();
                return NewChild;
            }
            posx -= CurChildWidth;
        }
        return AddCursorToRear();
    }

    const CumiNode* AddCursorForOutside(sint32 posx) override
    {
        if(posx <= mChildPos)
            return AddCursorToFront();
        return AddCursorToRear();
    }

    const CumiNode* AddCursorByOther(const CumiNode* other, bool front) override
    {
        for(sint32 g = 0, gend = mChildGroups.Count(); g < gend; ++g)
        for(sint32 i = 0, iend = mChildGroups[g].Count(); i < iend; ++i)
        {
            if(mChildGroups[g][i] == other)
            {
                auto NewChild = (CumiNode*) new CumiCursorP(mType, this);
                mChildGroups.At(g).DeliveryOne((front)? i : i + 1, ToReference(NewChild));
                NeedCapture();
                return NewChild;
            }
        }
        return nullptr;
    }

    const CumiNode* AddCursorToFront() override
    {
        if(0 < mChildGroups.Count())
        {
            auto NewChild = (CumiNode*) new CumiCursorP(mType, this);
            mChildGroups.At(0).DeliveryOne(0, ToReference(NewChild));
            NeedCapture();
            return NewChild;
        }
        return nullptr;
    }

    const CumiNode* AddCursorToRear() override
    {
        if(const sint32 GroupCount = mChildGroups.Count())
        {
            auto& NewChild = mChildGroups.At(GroupCount - 1).AtAdding();
            NewChild = (CumiNode*) new CumiCursorP(mType, this);
            NeedCapture();
            return NewChild;
        }
        return nullptr;
    }

    void InsertWord(char word) override
    {
        CumiNode* NewChild = nullptr;
        // 공백 자식추가
        if(word == ' ' || word == '\t')
            NewChild = (CumiNode*) new CumiBlankP(mType, this);
        // 일반워드식 자식추가
        else NewChild = (CumiNode*) new CumiWordP(&word, 1);

        if(NewChild)
        for(sint32 g = 0, gend = mChildGroups.Count(); g < gend; ++g)
        for(sint32 i = 0, iend = mChildGroups[g].Count(); i < iend; ++i)
        {
            if(mChildGroups[g][i]->IsTypeCursor())
            {
                mChildGroups.At(g).DeliveryOne(i, ToReference(NewChild));
                return;
            }
        }
    }

    void InsertToken(chars script, bool focusing) override
    {
        CumiNode* NewChild = nullptr;
        if(*script == '<')
        {
            // 쿠미스크립트식 자식추가
            // "<type>..."
            // "<type[...][...]>..."
            // "<type.opt1_v1.opt2_v2>..."
            // "<type.opt1_v1.opt2_v2[...][...]>..."
            const sint32 Length = boss_strlen(script);
            for(sint32 i = 1; i < Length; ++i)
            {
                if(script[i] == '.' || script[i] == '[' || script[i] == '>')
                {
                    const String NewType(&script[1], i - 1);
                    auto NewToken = new CumiTokenP(NewType, this);
                    i += NewToken->ParseOptions(&script[i], Length - i);
                    i += NewToken->ParseChildGroups(&script[i], Length - i);
                    if(script[i] == '>')
                    {
                        if(focusing)
                            NewToken->NeedCapture();
                        NewChild = (CumiNode*) NewToken;
                    }
                    else delete NewToken;
                    break;
                }
            }
        }

        if(NewChild)
        for(sint32 g = 0, gend = mChildGroups.Count(); g < gend; ++g)
        for(sint32 i = 0, iend = mChildGroups[g].Count(); i < iend; ++i)
        {
            if(mChildGroups[g][i]->IsTypeCursor())
            {
                if(focusing)
                {
                    delete mChildGroups[g][i];
                    mChildGroups.At(g).At(i) = NewChild;
                }
                else mChildGroups.At(g).DeliveryOne(i, ToReference(NewChild));
                return;
            }
        }
    }

    const CumiNode* GetNodeByCursorPos(sint32 seek) const override
    {
        for(sint32 g = 0, gend = mChildGroups.Count(); g < gend; ++g)
        for(sint32 i = 0, iend = mChildGroups[g].Count(); i < iend; ++i)
        {
            if(mChildGroups[g][i]->IsTypeCursor())
            {
                if(0 <= i + seek && i + seek < iend)
                    return mChildGroups[g][i + seek];
                return nullptr;
            }
        }
        return nullptr;
    }

    const CumiNode* GetNodeBySelf(sint32 seek) const override
    {
        if(CumiTokenP* CurToken = (CumiTokenP*) mParent)
        for(sint32 g = 0, gend = CurToken->mChildGroups.Count(); g < gend; ++g)
        for(sint32 i = 0, iend = CurToken->mChildGroups[g].Count(); i < iend; ++i)
        {
            if(CurToken->mChildGroups[g][i] == this)
            {
                if(0 <= i + seek && i + seek < iend)
                    return CurToken->mChildGroups[g][i + seek];
                return nullptr;
            }
        }
        return nullptr;
    }

private:
    chars mType;
    sint32 mID;
    Map<String> mOptions;
    typedef Array<CumiNode*, datatype_pod_canmemcpy_zeroset> CumiNodes;
    Array<CumiNodes, datatype_class_nomemcpy> mChildGroups;
    mutable sint32 mChildPos;
    mutable sint32 mChildGap;
    mutable Point mCursorPos;
    CumiTokenP* const mParent;
    mutable bool mNeedCapture;
    CumiEditor::UpdateCB mUpdater;
    h_view mView;
};

////////////////////////////////////////////////////////////////////////////////
// IME시스템 구현부
void IMESystem::Begin()
{
    mCapturedCumi = nullptr;
}

void IMESystem::End()
{
    // 반복키 처리
    if(mCapturedCumi)
    if(mRepeatCode != 0 && mRepeatMsec + 40 < Platform::Utility::CurrentTimeMsec()) // 반복주기
    {
        String TypeName;
        if(mCapturedCumi->IsTypeToken(&TypeName))
        if(const auto CurTokenMethod = gCumiTokenMethods.Access(TypeName))
            KeyProcess(*mCapturedCumi, CurTokenMethod->mKeyCB, nullptr);
        mRepeatMsec += 40;
    }
}

void IMESystem::BindKey(void* graphics, chars uiname, CumiNode* cumi, CumiEditor::KeyCB key)
{
    ZayPanel& Panel = *((ZayPanel*) graphics);

    // 캡쳐된 경우 반복키 처리예약
    if(Panel.state(uiname) & PS_Captured)
        mCapturedCumi = cumi;

    // 키이벤트 처리
    ZAY_INNER_UI(Panel, 0, uiname,
        ZAY_GESTURE_VNTXY(v, n, t, x, y, this, cumi, key)
        {
            if(t == GT_KeyPressed)
            {
                BOSS_TRACE("### KeyPressed RepeatCode : %d %d", x, y);
                mRepeatCode = x;
                mRepeatKey = (char) y;
                mRepeatMsec = Platform::Utility::CurrentTimeMsec() + 300; // 첫반복
                KeyProcess(*cumi, key, v);
            }
            else if(t == GT_KeyReleased)
            {
                BOSS_TRACE("### KeyReleased RepeatCode : %d %d", x, y);
                if(mRepeatKey == (char) y)
                    mRepeatCode = 0; // 키해제
            }
            else if(t == GT_Pressed)
            {
                auto NewCursor = cumi->AddCursorForOutside(x);
                cumi->GetTopNode()->ClearCursor(NewCursor);
                mRepeatCode = 0; // 키해제
            }
            else if(t == GT_WheelUp)
                ((CumiTokenP*) cumi->GetTopNode())->SendNotify("cumi:WheelUp");
            else if(t == GT_WheelDown)
                ((CumiTokenP*) cumi->GetTopNode())->SendNotify("cumi:WheelDown");
        });
}

void IMESystem::BindMouse(void* graphics, chars uiname, CumiNode* cumi, sint32 group, sint32 index)
{
    ZayPanel& Panel = *((ZayPanel*) graphics);

    // 마우스이벤트 처리
    ZAY_INNER_UI(Panel, 0, uiname,
        ZAY_GESTURE_VNTXY(v, n, t, x, y, this, cumi, group, index)
        {
            if(t == GT_Pressed)
            {
                auto CurRect = v->rect(n);
                const sint32 CurIndex = (x < (CurRect.l + CurRect.r) / 2)? index : index + 1;
                auto NewCursor = cumi->AddCursor(group, CurIndex);
                cumi->GetTopNode()->ClearCursor(NewCursor);
                mRepeatCode = 0; // 키해제
            }
            else if(t == GT_WheelUp)
                ((CumiTokenP*) cumi->GetTopNode())->SendNotify("cumi:WheelUp");
            else if(t == GT_WheelDown)
                ((CumiTokenP*) cumi->GetTopNode())->SendNotify("cumi:WheelDown");
        });
}

void IMESystem::SetCapture(chars uiname, CumiNode* cumi)
{
    ZayObject::ReleaseCaptureCB ReleaseCapture =
        [](payload olddata, payload newdata)->void
        {
            auto OldCumi = (CumiNode*) olddata;
            auto NewCumi = (CumiNode*) newdata;
            if(OldCumi && (!NewCumi || OldCumi->GetTopNode() != NewCumi->GetTopNode()))
                OldCumi->GetTopNode()->ClearCursor(nullptr);
        };
    pointers Ptrs;
    Ptrs.AtAdding() = (void*) ReleaseCapture;
    Ptrs.AtAdding() = cumi;
    if(((CumiTokenP*) cumi->GetTopNode())->SendDirectNotify(String::Format("cumi:SetCapture:%s", uiname), Ptrs))
        mLastCapturedCumi = cumi;
}

void IMESystem::EraseCapture(CumiNode* cumi)
{
    if(mLastCapturedCumi == cumi)
    {
        mLastCapturedCumi = nullptr;
        pointers Ptrs;
        Ptrs.AtAdding() = cumi;
        ((CumiTokenP*) cumi->GetTopNode())->SendDirectNotify("cumi:EraseCapture", Ptrs);
    }
}

void IMESystem::KeyProcess(CumiNode& cumi, CumiEditor::KeyCB key, ZayObject* view)
{
    if(key(cumi, mRepeatCode, mRepeatKey))
        cumi.GetTopNode()->Update();
    else if(mRepeatCode == 0x01000000 && view) // Esc
    {
        view->clearCapture();
        mRepeatCode = 0; // 키해제
    }
    else if(mRepeatCode == 0x01000004) // Enter
        cumi.SendCursorNotify("EnterLine");
    else if(mRepeatCode == 0x01000013) // Up
        cumi.SendCursorNotify("CursorUp");
    else if(mRepeatCode == 0x01000015) // Down
        cumi.SendCursorNotify("CursorDown");
    else if(mRepeatCode == 0x01000012) // Left
    {
        if(cumi.GetTopNode()->IsCursorFront())
            cumi.SendCursorNotify("CursorUpRear");
        else cumi.CursorToLeft();
    }
    else if(mRepeatCode == 0x01000014) // Right
    {
        if(cumi.GetTopNode()->IsCursorRear())
            cumi.SendCursorNotify("CursorDownFront");
        else cumi.CursorToRight();
    }
    else if(mRepeatCode == 0x01000010) // Home
    {
        auto NewCursor = cumi.GetTopNode()->AddCursorToFront();
        cumi.GetTopNode()->ClearCursor(NewCursor);
    }
    else if(mRepeatCode == 0x01000011) // End
    {
        auto NewCursor = cumi.GetTopNode()->AddCursorToRear();
        cumi.GetTopNode()->ClearCursor(NewCursor);
    }
    else if(mRepeatCode == 0x01000003) // BackSpace
    {
        if(cumi.GetTopNode()->IsCursorFront())
            cumi.SendCursorNotify("MergeFrontLine");
        else
        {
            cumi.CursorToBackSpace();
            cumi.GetTopNode()->Update();
        }
    }
    else if(mRepeatCode == 0x01000007) // Delete
    {
        if(cumi.GetTopNode()->IsCursorRear())
            cumi.SendCursorNotify("MergeRearLine");
        else
        {
            cumi.CursorToDelete();
            cumi.GetTopNode()->Update();
        }
    }
    else if(mRepeatCode == 0x01000031) // F2
        cumi.SendCursorNotify("Paste");
    else if(!(mRepeatCode & 0x01000000)) // Ascii
    {
        cumi.InsertWord(mRepeatKey);
        cumi.GetTopNode()->Update();
    }
    else cumi.SendTextNotify("UnknownKey",
        String::Format("%d, 0x%08X", (sint32) mRepeatKey, (uint32) mRepeatCode));
}

////////////////////////////////////////////////////////////////////////////////
// CumiEditor
CumiEditor::CumiEditor()
{
    mTopToken = (CumiNode*) new CumiTokenP("cumi", nullptr);
}

CumiEditor::~CumiEditor()
{
    delete mTopToken;
}

CumiEditor::CumiEditor(CumiEditor&& rhs)
{
    mTopToken = nullptr;
    operator=(ToReference(rhs));
}

CumiEditor::CumiEditor(const CumiEditor& rhs)
{
    mTopToken = (CumiNode*) new CumiTokenP("cumi", nullptr);
    operator=(rhs);
}

CumiEditor& CumiEditor::operator=(CumiEditor&& rhs)
{
    delete mTopToken;
    mTopToken = rhs.mTopToken;
    rhs.mTopToken = nullptr;
    return *this;
}

CumiEditor& CumiEditor::operator=(const CumiEditor& rhs)
{
    auto TopToken = (CumiTokenP*) mTopToken;
    auto RhsTopToken = (CumiTokenP*) rhs.mTopToken;
    const String NewOptions = RhsTopToken->BuildOptions();
    TopToken->ParseOptions(NewOptions, NewOptions.Length());
    const String NewChildTokens = RhsTopToken->BuildChildGroups();
    TopToken->ParseChildGroups(NewChildTokens, NewChildTokens.Length());
    return *this;
}

void CumiEditor::AddToken(chars name,
    RenderCB body, RenderCB blank, RenderCB cursor, KeyCB key, TextCB text)
{
    auto& NewTokenMethod = gCumiTokenMethods(name);
    NewTokenMethod.mRenderBodyCB = body;
    NewTokenMethod.mRenderBlankCB = blank;
    NewTokenMethod.mRenderCursorCB = cursor;
    NewTokenMethod.mKeyCB = key;
    NewTokenMethod.mTextCB = text;
}

bool CumiEditor::LoadScript(chars text, sint32 length)
{
    if(length == -1)
        length = boss_strlen(text);
    auto TopToken = (CumiTokenP*) mTopToken;
    TopToken->Clear();

    // 쿠미스크립트식 로딩
    // "...<cumi>..."
    // "...<cumi[...][...]>..."
    // "...<cumi.opt1_v1.opt2_v2>..."
    // "...<cumi.opt1_v1.opt2_v2[...][...]>..."
    for(sint32 i = 5; i < length; ++i)
    {
        if((text[i] == '.' || text[i] == '[') && text[i - 5] == '<' &&
            text[i - 4] == 'c' && text[i - 3] == 'u' && text[i - 2] == 'm' && text[i - 1] == 'i')
        {
            i += TopToken->ParseOptions(&text[i], length - i);
            i += TopToken->ParseChildGroups(&text[i], length - i);
            return (text[i] == '>');
        }
    }

    // 일반워드식 로딩
    TopToken->ParseWords(text, length);
    return true;
}

bool CumiEditor::AddScript(chars text, sint32 length)
{
    if(length == -1)
        length = boss_strlen(text);
    auto TopToken = (CumiTokenP*) mTopToken;

    // 쿠미스크립트식 추가로딩
    // "...<cumi>..."
    // "...<cumi[...][...]>..."
    // "...<cumi.opt1_v1.opt2_v2>..."
    // "...<cumi.opt1_v1.opt2_v2[...][...]>..."
    for(sint32 i = 5; i < length; ++i)
    {
        if((text[i] == '.' || text[i] == '[') && text[i - 5] == '<' &&
            text[i - 4] == 'c' && text[i - 3] == 'u' && text[i - 2] == 'm' && text[i - 1] == 'i')
        {
            i += TopToken->ParseLastGroup(&text[i], length - i);
            return (text[i] == '>');
        }
    }

    // 일반워드식 추가로딩
    TopToken->ParseWords(text, length);
    return true;
}

String CumiEditor::SaveScript() const
{
    auto TopToken = (CumiTokenP*) mTopToken;
    return "<cumi" + TopToken->BuildOptions() + TopToken->BuildChildGroups() + ">";
}

String CumiEditor::CutCursorScript()
{
    auto TopToken = (CumiTokenP*) mTopToken;
    const String CursorScript = TopToken->CutCursorGroups();
    return "<cumi" + TopToken->BuildOptions() + CursorScript + ">";
}

String CumiEditor::SaveText() const
{
    auto TopToken = (CumiTokenP*) mTopToken;
    return TopToken->BuildText();
}

void CumiEditor::Render(h_view view, void* graphics, sint32 id, bool readonly) const
{
    auto TopToken = (CumiTokenP*) mTopToken;
    TopToken->SetView(view);

    IMESystem::ST().Begin();
    TopToken->Render(graphics, id, (readonly)? RenderMode::ReadOnly : RenderMode::Writeable);
    IMESystem::ST().End();
}

sint32 CumiEditor::GetRenderWidth(void* graphics) const
{
    return mTopToken->Render(graphics, 0, RenderMode::SizeOnly).w;
}

void CumiEditor::SetUpdater(UpdateCB updater)
{
    auto TopToken = (CumiTokenP*) mTopToken;
    TopToken->SetUpdater(updater);
}

void CumiEditor::SetCursor(void* graphics, sint32 pos)
{
    mTopToken->ClearCursor(nullptr);
    mTopToken->AddCursorForInside(graphics, pos);
}

void CumiEditor::SetCursorToFront()
{
    mTopToken->ClearCursor(nullptr);
    mTopToken->AddCursorToFront();
}

void CumiEditor::SetCursorToRear()
{
    mTopToken->ClearCursor(nullptr);
    mTopToken->AddCursorToRear();
}

////////////////////////////////////////////////////////////////////////////////
// 기본으로 제공되는 쿠미에디터
static autorun CreateDefaultToken()
{
    CumiEditor::AddToken("cumi",
        [](const CumiNode& self, void* graphics, sint32 id, CumiEditor::RenderMode mode)->size64
        {
            ZayPanel& Panel = *((ZayPanel*) graphics);

            if(mode != CumiEditor::RenderMode::SizeOnly)
            ZAY_RGB(Panel, 0, 0, 0)
                self.RenderChildGroup(0, graphics, id, false);
            return self.GetChildGroupSize(0, graphics, id);
        },
        nullptr,
        [](const CumiNode& self, void* graphics, sint32 id, CumiEditor::RenderMode mode)->size64
        {
            ZayPanel& Panel = *((ZayPanel*) graphics);
            size64 Result = {4, (sint32) Panel.h()};

            if(mode != CumiEditor::RenderMode::SizeOnly)
            {
                static uint64 FirstMsec = Platform::Utility::CurrentTimeMsec();
                const sint32 CursorValue = (Platform::Utility::CurrentTimeMsec() - FirstMsec) % 1000;
                const sint32 CursorAlpha = 255 * Math::Abs(CursorValue - 500) / 500;
                ZAY_RGBA(Panel, 0, 0, 0, CursorAlpha)
                ZAY_LTRB(Panel, 1, 4, Result.w - 1, Panel.h() - 4)
                    Panel.fill();
                Panel.repaintOnce();
            }
            return Result;
        },
        [](CumiNode& self, sint32 code, char key)->bool
        {
            return false;
        },
        nullptr);
    return true;
}
static autorun _ = CreateDefaultToken();

} // namespace HU
