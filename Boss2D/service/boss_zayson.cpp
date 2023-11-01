#include <boss.hpp>
#include "boss_zayson.hpp"

namespace BOSS
{
    ////////////////////////////////////////////////////////////////////////////////
    // ZayUIElement
    ////////////////////////////////////////////////////////////////////////////////
    class ZayUIElement
    {
        BOSS_DECLARE_STANDARD_CLASS(ZayUIElement)
    public:
        enum class Type {Unknown, Condition, Param, Request, Inside, Gate, Component, View};
        enum class LambdaID {OnSet, OnTouch, OnClick, Max};

    public:
        ZayUIElement(Type type = Type::Unknown) : mType(type)
        {
            mID = MakeID();
            STMAP()[mID] = this;
            mRefRoot = nullptr;
        }
        virtual ~ZayUIElement()
        {
            STMAP().Remove(mID);
        }

    private:
        sint32 MakeID()
        {
            static sint32 LastID = -1;
            return ++LastID;
        }
        static Map<ZayUIElement*>& STMAP()
        {static Map<ZayUIElement*> _; return _;}

    public:
        static ZayUIElement* Get(sint32 id)
        {
            if(auto Result = STMAP().Access(id))
                return *Result;
            return nullptr;
        }
        static SolverValue GetResult(chars viewname, chars formula)
        {
            Solver NewSolver;
            NewSolver.Link(viewname);
            NewSolver.Parse(formula);
            NewSolver.Execute();
            return NewSolver.result();
        }
        static buffer Create(Type type);

    public:
        virtual void Load(const ZaySon& root, const Context& context)
        {
            mRefRoot = &root;
            hook(context("uiname"))
                mUINameSolver.Link(root.ViewName()).Parse(fish.GetText());
            hook(context("uiloop"))
                mUILoopSolver.Link(root.ViewName()).Parse(fish.GetText());
            hook(context("comment"))
                mComment = fish.GetText();
        }

    public:
        class DebugLog
        {
        public:
            Rect mRect;
            bool mFill;
            String mUIName;
        };
        typedef Array<DebugLog> DebugLogs;

    public:
        virtual void Render(ZayPanel& panel, const String& defaultname, DebugLogs& logs) const {}
        virtual void SetCursor(CursorRole role) {}
        virtual bool OnLambda(chars uiname, LambdaID id, chars key = nullptr, chars value = nullptr,
            bool doubleclicked = false, bool longpressed = false, bool outreleased = false, bool cancelreleased = false) {return false;}
        virtual bool IsValidDoubleClick() {return false;}
        virtual bool IsValidLongPress() {return false;}

    public:
        Type mType;
        sint32 mID;
        const ZaySon* mRefRoot;
        Solver mUINameSolver;
        Solver mUILoopSolver;
        String mComment;
    };
    typedef Object<ZayUIElement> ZayUI;
    typedef Array<ZayUI> ZayUIs;

    ////////////////////////////////////////////////////////////////////////////////
    // ZaySonDocument
    ////////////////////////////////////////////////////////////////////////////////
    ZaySonDocument::ZaySonDocument(chars chain) : mChain(chain)
    {
    }

    ZaySonDocument::~ZaySonDocument()
    {
    }

    ZaySonDocument::ZaySonDocument(ZaySonDocument&& rhs) : mChain(ToReference(rhs.mChain))
    {
        operator=(ToReference(rhs));
    }

    ZaySonDocument& ZaySonDocument::operator=(ZaySonDocument&& rhs)
    {
        mSolvers = ToReference(rhs.mSolvers);
        return *this;
    }

    bool ZaySonDocument::ExistVariable(chars variable) const
    {
        return (Solver::Find(mChain, variable) != nullptr);
    }

    void ZaySonDocument::SetVariableFilter(chars variable, SolverValueCB cb)
    {
        auto& CurSolver = LinkedSolver(variable);
        CurSolver.SetResultFilter(cb);
    }

    SolverValue ZaySonDocument::GetValue(chars variable) const
    {
        if(auto CurSolver = Solver::Find(mChain, variable))
            return CurSolver->result();
        return SolverValue();
    }

    void ZaySonDocument::SetValue(chars variable, chars formula)
    {
        auto& CurSolver = LinkedSolver(variable);
        CurSolver.Parse(formula);
        CurSolver.Execute(true);
    }

    String ZaySonDocument::GetComment(chars variable) const
    {
        if(auto CurSolver = Solver::Find(mChain, variable))
        {
            auto& CurComment = CurSolver->parsed_formula();
            if(0 < CurComment.Length() && CurComment[0] == '?')
                return CurComment.Offset(1);
        }
        return String();
    }

    void ZaySonDocument::SetComment(chars variable, chars text)
    {
        auto& CurSolver = LinkedSolver(variable);
        CurSolver.Parse(String('?') + text);
        CurSolver.Execute(true);
    }

    void ZaySonDocument::SetJson(const Context& json, const String nameheader)
    {
        if(auto Length = json.LengthOfNamable())
        {
            for(sint32 i = 0; i < Length; ++i)
            {
                chararray GetKey;
                auto& ChildJson = json(i, &GetKey);
                SetJson(ChildJson, nameheader + String::Format("%s.", &GetKey[0]));
            }
        }
        else if(auto Length = json.LengthOfIndexable())
        {
            for(sint32 i = 0; i < Length; ++i)
                SetJson(json[i], nameheader + String::Format("%d.", i));
            // 수량정보
            const String Key = nameheader + "count";
            auto& CurSolver = LinkedSolver(Key);
            CurSolver.Parse(String::FromInteger(Length));
            CurSolver.Execute(true);
        }
        else
        {
            const String Key = nameheader.Left(nameheader.Length() - 1);
            const String Value = json.GetText();
            auto& CurSolver = LinkedSolver(Key);
            sint32 IntOffset = 0;
            Parser::GetInt<sint64>((chars) Value, Value.Length(), &IntOffset);
            if(IntOffset == Value.Length())
                CurSolver.Parse(Value);
            else
            {
                sint32 FloatOffset = 0;
                Parser::GetFloat<double>((chars) Value, Value.Length(), &FloatOffset);
                if(FloatOffset == Value.Length())
                    CurSolver.Parse(Value);
                else CurSolver.Parse(String::Format("\'%s\'", (chars) Value));
            }
            CurSolver.Execute(true);
        }
    }

    void ZaySonDocument::Remove(chars variable)
    {
        Solver::Remove(mChain, variable);
    }

    void ZaySonDocument::RemoveMatchedVariables(chars keyword, SolverVariableCB cb)
    {
        return Solver::RemoveMatchedVariables(mChain, keyword, cb);
    }

    void ZaySonDocument::CheckUpdatedSolvers(uint64 msec, UpdateCB cb)
    {
        for(sint32 i = 0, iend = mSolvers.Count(); i < iend; ++i)
            if(msec < mSolvers[i].updated_msec())
                cb(&mSolvers[i]);
    }

    Solver& ZaySonDocument::LinkedSolver(chars variable)
    {
        if(auto CurSolver = Solver::Find(mChain, variable))
            return *CurSolver;
        auto& NewSolver = mSolvers.AtAdding();
        NewSolver.Link(mChain, variable);
        return NewSolver;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ZaySonElementCall
    ////////////////////////////////////////////////////////////////////////////////
    void ZaySonElementCall::SetCursor(sint32 elementid, CursorRole role)
    {
        if(auto CurUIElement = ZayUIElement::Get(elementid))
            CurUIElement->SetCursor(role);
    }

    bool ZaySonElementCall::SetVariable(sint32 elementid, chars uiname, chars key, chars value)
    {
        if(auto CurUIElement = ZayUIElement::Get(elementid))
            return CurUIElement->OnLambda(uiname, ZayUIElement::LambdaID::OnSet, key, value);
        return false;
    }

    bool ZaySonElementCall::SendPress(sint32 elementid, chars uiname)
    {
        if(auto CurUIElement = ZayUIElement::Get(elementid))
            return CurUIElement->OnLambda(uiname, ZayUIElement::LambdaID::OnTouch);
        return false;
    }

    bool ZaySonElementCall::SendClick(sint32 elementid, chars uiname, bool doubleclicked, bool longpressed, bool outreleased, bool cancelreleased)
    {
        if(auto CurUIElement = ZayUIElement::Get(elementid))
            return CurUIElement->OnLambda(uiname, ZayUIElement::LambdaID::OnClick, nullptr, nullptr,
                doubleclicked, longpressed, outreleased, cancelreleased);
        return false;
    }

    bool ZaySonElementCall::IsValidDoubleClick(sint32 elementid)
    {
        if(auto CurUIElement = ZayUIElement::Get(elementid))
            return CurUIElement->IsValidDoubleClick();
        return false;
    }

    bool ZaySonElementCall::IsValidLongPress(sint32 elementid)
    {
        if(auto CurUIElement = ZayUIElement::Get(elementid))
            return CurUIElement->IsValidLongPress();
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ZaySonUtility
    ////////////////////////////////////////////////////////////////////////////////
    String ZaySonUtility::GetSafetyString(chars text)
    {
        String Collector;
        for(sint32 c = 0, cold = 0; true; ++c)
        {
            if(text[c] == '\0')
            {
                Collector += String(text + cold, c - cold).Trim();
                break;
            }
            else if(text[c] == ',')
            {
                Collector += String(text + cold, c - cold).Trim() + ", ";
                cold = c + 1;
            }
            else if(text[c] == '\'' || text[c] == '\"')
            {
                Collector += String(text + cold, c - cold).Trim();
                cold = c + 1;

                const char EndCode = text[c];
                while(text[++c] != '\0' && (text[c] != EndCode || text[c - 1] == '\\'));

                String LiteralText(text + cold, c - cold); // 따옴표안쪽
                LiteralText.Replace("\\\\", "([****])");
                LiteralText.Replace("\\\'", "\'");
                LiteralText.Replace("\\\"", "\"");
                LiteralText.Replace("\\", "\\\\"); // 슬래시기호는 쌍슬래시로
                LiteralText.Replace("([****])", "\\\\");
                LiteralText.Replace("\'", "\\\'"); // 내부 따옴표는 쌍슬래시+따옴표로
                LiteralText.Replace("\"", "\\\"");
                Collector += '\'' + LiteralText + '\''; // 외부 따옴표는 단따옴표만 인정
                cold = c + 1;

                // 따옴표 마무리가 안된 경우
                if(text[c] == '\0')
                    break;
            }
        }
        return Collector;
    }

    Strings ZaySonUtility::GetCommaStrings(chars text)
    {
        Strings SubStrings;
        for(sint32 c = 0, cold = 0; true; ++c)
        {
            if(text[c] == '\0')
            {
                SubStrings.AtAdding() = String(text + cold, c - cold).Trim();
                break;
            }
            else if(text[c] == ',')
            {
                SubStrings.AtAdding() = String(text + cold, c - cold).Trim();
                cold = c + 1;
            }
            else if(text[c] == '\'' || text[c] == '\"')
            {
                const char EndCode = text[c];
                while(text[++c] != '\0' && (text[c] != EndCode || text[c - 1] == '\\'));
            }
        }
        return SubStrings;
    }

    bool ZaySonUtility::IsFunctionCall(chars text, sint32* prmbegin, sint32* prmend)
    {
        chars Focus = text;
        // 공백스킵
        while(*Focus == ' ') Focus++;
        // 함수명의 탈락조건
        if(boss_isalpha(*Focus) == 0)
            return false; // 첫글자가 영문이 아닐 경우 탈락
        while(*(++Focus) != '(' && *Focus != '\0')
            if(boss_isalnum(*Focus) == 0 && *Focus != '_')
                return false; // 영문, 숫자가 아닐 경우 함수탈락

        // 파라미터 발라내기
        if(*Focus == '(')
        {
            const sint32 ParamBeginPos = (Focus + 1) - text;
            sint32 DeepLevel = 1;
            while(*(++Focus) != '\0')
            {
                if(*Focus == '(') DeepLevel++;
                else if(*Focus == ')')
                {
                    if(--DeepLevel == 0)
                    {
                        if(prmbegin) *prmbegin = ParamBeginPos;
                        if(prmend) *prmend = Focus - text;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    ZaySonInterface::ConditionType ZaySonUtility::ToCondition(chars text, bool* withelse)
    {
        if(withelse) *withelse = false;
        branch;
        // if계열
        jump(!String::Compare(text, "if(", 3))
            return ZaySonInterface::ConditionType::If;
        jump(!String::Compare(text, "iffocused(", 10))
            return ZaySonInterface::ConditionType::IfFocused;
        jump(!String::Compare(text, "ifhovered(", 10))
            return ZaySonInterface::ConditionType::IfHovered;
        jump(!String::Compare(text, "ifpressed(", 10))
            return ZaySonInterface::ConditionType::IfPressed;
        jump(!String::Compare(text, "ifdoubleclicked"))
            return ZaySonInterface::ConditionType::IfDoubleClicked;
        jump(!String::Compare(text, "ifndoubleclicked"))
            return ZaySonInterface::ConditionType::IfNDoubleClicked;
        jump(!String::Compare(text, "iflongpressed"))
            return ZaySonInterface::ConditionType::IfLongPressed;
        jump(!String::Compare(text, "ifnlongpressed"))
            return ZaySonInterface::ConditionType::IfNLongPressed;
        jump(!String::Compare(text, "ifoutreleased"))
            return ZaySonInterface::ConditionType::IfOutReleased;
        jump(!String::Compare(text, "ifcancelreleased"))
            return ZaySonInterface::ConditionType::IfCancelReleased;
        // elif계열
        jump(!String::Compare(text, "elif(", 5))
        {
            if(withelse) *withelse = true;
            return ZaySonInterface::ConditionType::If;
        }
        jump(!String::Compare(text, "eliffocused(", 12))
        {
            if(withelse) *withelse = true;
            return ZaySonInterface::ConditionType::IfFocused;
        }
        jump(!String::Compare(text, "elifhovered(", 12))
        {
            if(withelse) *withelse = true;
            return ZaySonInterface::ConditionType::IfHovered;
        }
        jump(!String::Compare(text, "elifpressed(", 12))
        {
            if(withelse) *withelse = true;
            return ZaySonInterface::ConditionType::IfPressed;
        }
        jump(!String::Compare(text, "elifdoubleclicked"))
        {
            if(withelse) *withelse = true;
            return ZaySonInterface::ConditionType::IfDoubleClicked;
        }
        jump(!String::Compare(text, "elifndoubleclicked"))
        {
            if(withelse) *withelse = true;
            return ZaySonInterface::ConditionType::IfNDoubleClicked;
        }
        jump(!String::Compare(text, "eliflongpressed"))
        {
            if(withelse) *withelse = true;
            return ZaySonInterface::ConditionType::IfLongPressed;
        }
        jump(!String::Compare(text, "elifnlongpressed"))
        {
            if(withelse) *withelse = true;
            return ZaySonInterface::ConditionType::IfNLongPressed;
        }
        jump(!String::Compare(text, "elifoutreleased"))
        {
            if(withelse) *withelse = true;
            return ZaySonInterface::ConditionType::IfOutReleased;
        }
        jump(!String::Compare(text, "elifcancelreleased"))
        {
            if(withelse) *withelse = true;
            return ZaySonInterface::ConditionType::IfCancelReleased;
        }
        // 기타
        jump(!String::Compare(text, "else"))
            return ZaySonInterface::ConditionType::Else;
        jump(!String::Compare(text, "endif"))
            return ZaySonInterface::ConditionType::Endif;
        return ZaySonInterface::ConditionType::Unknown;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ZayConditionElement
    ////////////////////////////////////////////////////////////////////////////////
    class ZayConditionElement : public ZayUIElement
    {
    public:
        ZayConditionElement() : ZayUIElement(Type::Condition)
        {
            mConditionType = ZaySonInterface::ConditionType::Unknown;
            mWithElse = false;
        }
        ~ZayConditionElement() override {}

    public:
        void Load(const ZaySon& root, const Context& context) override
        {
            ZayUIElement::Load(root, context);

            const String ConditionText = context.GetText();
            mConditionType = ZaySonUtility::ToCondition(ConditionText, &mWithElse);
            if(mConditionType == ZaySonInterface::ConditionType::Unknown)
                root.SendErrorLog("Script Parsing Failed", String::Format(" ● Unknown conditional statement. (%s/Load)", (chars) ConditionText));

            sint32 PosB, PosE;
            if(ZaySonUtility::IsFunctionCall(ConditionText, &PosB, &PosE)) // ()사용여부와 파라미터 발라내기
            {
                mConditionSolver.Link(root.ViewName());
                mConditionSolver.Parse(String(((chars) ConditionText) + PosB, PosE - PosB));
            }
        }

    public:
        void CollectCapture(Map<String>& collector)
        {
            // 캡쳐가 필요한 변수의 목록화
            const Strings Variables = mConditionSolver.GetTargetlessVariables();
            for(sint32 i = 0, iend = Variables.Count(); i < iend; ++i)
                collector(Variables[i]) = Variables[i];
        }

    public:
        static bool Test(const ZaySon& root, ZayUIs& dest, const Context& src, Map<String>* collector = nullptr)
        {
            if(ZaySonUtility::ToCondition(src.GetText()) != ZaySonInterface::ConditionType::Unknown) // oncreate, onclick, compvalues의 경우
            {
                ZayUI NewUI(ZayUIElement::Create(Type::Condition));
                NewUI->Load(root, src);
                if(collector) ((ZayConditionElement*) NewUI.Ptr())->CollectCapture(*collector);
                dest.AtAdding() = (id_share) NewUI;
                return true;
            }
            else if(ZaySonUtility::ToCondition(src("compname").GetText()) != ZaySonInterface::ConditionType::Unknown) // compname의 경우
            {
                ZayUI NewUI(ZayUIElement::Create(Type::Condition));
                NewUI->Load(root, src("compname"));
                if(collector) ((ZayConditionElement*) NewUI.Ptr())->CollectCapture(*collector);
                dest.AtAdding() = (id_share) NewUI;
                return true;
            }
            return false;
        };
        static sint32s Collect(chars viewname, const ZayUIs& uis, const ZayPanel* panel = nullptr,
            bool doubleclicked = false, bool longpressed = false, bool outreleased = false, bool cancelreleased = false)
        {
            sint32s Collector;
            // 조건문처리로 유효한 CompValue를 수집
            for(sint32 i = 0, iend = uis.Count(); i < iend; ++i)
            {
                if(uis[i].ConstValue().mType == ZayUIElement::Type::Condition)
                {
                    // 조건의 성공여부
                    bool IsTrue = false;
                    auto CurCondition = (const ZayConditionElement*) uis[i].ConstPtr();
                    if(CurCondition->mConditionType == ZaySonInterface::ConditionType::If)
                        IsTrue = (CurCondition->mConditionSolver.ExecuteOnly().ToInteger() != 0);
                    // 포커스확인
                    else if(CurCondition->mConditionType == ZaySonInterface::ConditionType::IfFocused)
                    {
                        if(panel)
                        {
                            const String UIName = CurCondition->mConditionSolver.ExecuteOnly().ToText();
                            IsTrue = ((panel->state(viewname + ('.' + UIName)) & (PS_Focused | PS_Dropping)) == PS_Focused);
                        }
                    }
                    // 호버확인
                    else if(CurCondition->mConditionType == ZaySonInterface::ConditionType::IfHovered)
                    {
                        if(panel)
                        {
                            const String UIName = CurCondition->mConditionSolver.ExecuteOnly().ToText();
                            IsTrue = ((panel->state(viewname + ('.' + UIName)) & PS_Hovered) == PS_Hovered);
                        }
                    }
                    // 프레스확인
                    else if(CurCondition->mConditionType == ZaySonInterface::ConditionType::IfPressed)
                    {
                        if(panel)
                        {
                            const String UIName = CurCondition->mConditionSolver.ExecuteOnly().ToText();
                            IsTrue = ((panel->state(viewname + ('.' + UIName)) & PS_Dragging) == PS_Dragging);
                        }
                    }
                    // 더블클릭확인
                    else if(CurCondition->mConditionType == ZaySonInterface::ConditionType::IfDoubleClicked)
                        IsTrue = doubleclicked;
                    // 더블클릭이 아님을 확인
                    else if(CurCondition->mConditionType == ZaySonInterface::ConditionType::IfNDoubleClicked)
                        IsTrue = !doubleclicked;
                    // 롱프레스확인
                    else if(CurCondition->mConditionType == ZaySonInterface::ConditionType::IfLongPressed)
                        IsTrue = longpressed;
                    // 롱프레스가 아님을 확인
                    else if(CurCondition->mConditionType == ZaySonInterface::ConditionType::IfNLongPressed)
                        IsTrue = !longpressed;
                    // 아웃릴리즈확인
                    else if(CurCondition->mConditionType == ZaySonInterface::ConditionType::IfOutReleased)
                        IsTrue = outreleased;
                    // 캔슬릴리즈확인
                    else if(CurCondition->mConditionType == ZaySonInterface::ConditionType::IfCancelReleased)
                        IsTrue = cancelreleased;
                    else IsTrue = true;

                    if(IsTrue)
                    {
                        while(i + 1 < iend && uis[i + 1].ConstValue().mType != ZayUIElement::Type::Condition)
                            Collector.AtAdding() = ++i;
                        // 선진입
                        while(i + 1 < iend)
                        {
                            if(uis[++i].ConstValue().mType == ZayUIElement::Type::Condition)
                            {
                                CurCondition = (const ZayConditionElement*) uis[i].ConstPtr();
                                if(CurCondition->mConditionType == ZaySonInterface::ConditionType::Endif) // endif는 조건그룹을 빠져나오게 하고
                                    break;
                                else if(!CurCondition->mWithElse && // 새로운 조건그룹을 만나면 수락
                                    (CurCondition->mConditionType == ZaySonInterface::ConditionType::If ||
                                    CurCondition->mConditionType == ZaySonInterface::ConditionType::IfFocused ||
                                    CurCondition->mConditionType == ZaySonInterface::ConditionType::IfHovered ||
                                    CurCondition->mConditionType == ZaySonInterface::ConditionType::IfPressed))
                                {
                                    i--;
                                    break;
                                }
                            }
                        }
                    }
                    else while(i + 1 < iend)
                    {
                        if(uis[++i].ConstValue().mType == ZayUIElement::Type::Condition)
                        {
                            CurCondition = (const ZayConditionElement*) uis[i].ConstPtr();
                            if(CurCondition->mConditionType == ZaySonInterface::ConditionType::Endif) // endif는 조건그룹을 빠져나오게 하고
                                break;
                            else // 다른 모든 조건은 수락
                            {
                                i--;
                                break;
                            }
                        }
                    }
                }
                else Collector.AtAdding() = i;
            }
            return Collector;
        }

    public:
        ZaySonInterface::ConditionType mConditionType;
        bool mWithElse;
        Solver mConditionSolver;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // ZayParamElement
    ////////////////////////////////////////////////////////////////////////////////
    class ZayParamElement : public ZayUIElement
    {
    public:
        ZayParamElement() : ZayUIElement(Type::Param) {}
        ~ZayParamElement() override {}

    public:
        void Load(const ZaySon& root, const Context& context) override
        {
            ZayUIElement::Load(root, context);

            for(sint32 i = 0, iend = context.LengthOfIndexable(); i < iend; ++i)
                mParamSolvers.AtAdding().Link(root.ViewName()).Parse(context[i].GetText());
        }

    public:
        Solvers mParamSolvers;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // ZayRequestElement
    ////////////////////////////////////////////////////////////////////////////////
    class ZayRequestElement : public ZayUIElement
    {
    public:
        ZayRequestElement() : ZayUIElement(Type::Request)
        {
            mRequestType = ZaySonInterface::RequestType::Unknown;
            mGlueFunction = nullptr;
        }
        ~ZayRequestElement() override {}

    public:
        void Load(const ZaySon& root, const Context& context) override
        {
            ZayUIElement::Load(root, context);

            const String TextTest = context.GetText();
            sint32 PosB, PosE;
            if(ZaySonUtility::IsFunctionCall(TextTest, &PosB, &PosE)) // 함수호출
            {
                mRequestType = ZaySonInterface::RequestType::Function;
                mGlueFunction = root.FindGlue(TextTest.Left(PosB - 1));
                if(!mGlueFunction)
                    root.SendWarningLog("Binding Failed", String::Format(" ● Glue function not found. (%s/Load)", (chars) TextTest.Left(PosB - 1)));
                if(PosB < PosE)
                {
                    auto Params = ZaySonUtility::GetCommaStrings(TextTest.Middle(PosB, PosE - PosB));
                    for(sint32 i = 0, iend = Params.Count(); i < iend; ++i)
                        mRSolvers.AtAdding().Link(root.ViewName()).Parse(Params[i]); // 파라미터들
                }
            }
            else // 변수입력
            {
                chararray GetName;
                const auto& GetValue = context(0, &GetName);
                BOSS_ASSERT("함수콜이 아닌 Request는 좌우항으로 나누어져야 합니다", 0 < GetName.Count());
                if(0 < GetName.Count())
                {
                    mRequestType = ZaySonInterface::RequestType::Variable;
                    mLSolver.Parse(&GetName[0]); // 좌항
                    mRSolvers.AtAdding().Parse(GetValue.GetText()); // 우항
                }
            }
        }

    public:
        void InitForCreate()
        {
            if(mRequestType == ZaySonInterface::RequestType::Variable)
            {
                mLSolver.Link(mRefRoot->ViewName());
                mRSolvers.At(0).Link(mRefRoot->ViewName(), mLSolver.ExecuteVariableName());
                mRSolvers.At(0).Execute();
            }
            else if(mRequestType == ZaySonInterface::RequestType::Function)
            {
                if(mGlueFunction)
                {
                    ZayExtend::Payload ParamCollector = mGlueFunction->MakePayload();
                    for(sint32 i = 0, iend = mRSolvers.Count(); i < iend; ++i)
                        ParamCollector(mRSolvers[i].ExecuteOnly());
                    // ParamCollector가 소멸되면서 Glue함수가 호출됨
                }
            }
        }
        void InitForInput()
        {
            if(mRequestType == ZaySonInterface::RequestType::Variable)
            {
                mLSolver.Link(mRefRoot->ViewName());
                mRSolvers.At(0).Link(mRefRoot->ViewName());
            }
        }
        void InitForClick(Map<String>& collector)
        {
            if(mRequestType == ZaySonInterface::RequestType::Variable)
            {
                mLSolver.Link(mRefRoot->ViewName());
                mRSolvers.At(0).Link(mRefRoot->ViewName());

                // 변수때는 변수명에서도 캡쳐 목록화
                const Strings Variables = mLSolver.GetTargetlessVariables();
                for(sint32 i = 0, iend = Variables.Count(); i < iend; ++i)
                    collector(Variables[i]) = Variables[i];
            }
            // 변수와 함수 모두 캡쳐가 필요한 변수의 목록화
            for(sint32 i = 0, iend = mRSolvers.Count(); i < iend; ++i)
            {
                const Strings Variables = mRSolvers[i].GetTargetlessVariables();
                for(sint32 j = 0, jend = Variables.Count(); j < jend; ++j)
                    collector(Variables[j]) = Variables[j];
            }
        }
        void Transaction()
        {
            if(mRequestType == ZaySonInterface::RequestType::Variable)
            {
                if(auto FindedSolver = Solver::Find(mRefRoot->ViewName(), mLSolver.ExecuteVariableName()))
                {
                    FindedSolver->Parse(mRSolvers[0].ExecuteOnly().ToText(true));
                    FindedSolver->Execute();
                }
                else mRefRoot->SendErrorLog("Update Failed", String::Format(" ● Variable not found. (%s/Transaction)",
                    (chars) mLSolver.ExecuteVariableName()));
            }
            else if(mRequestType == ZaySonInterface::RequestType::Function)
            {
                if(mGlueFunction)
                {
                    ZayExtend::Payload ParamCollector = mGlueFunction->MakePayload();
                    for(sint32 i = 0, iend = mRSolvers.Count(); i < iend; ++i)
                        ParamCollector(mRSolvers[i].ExecuteOnly());
                    // ParamCollector가 소멸되면서 Glue함수가 호출됨
                }
            }
        }

    public:
        ZaySonInterface::RequestType mRequestType;
        Solver mLSolver;
        Solvers mRSolvers; // 변수, 함수파라미터들 겸용
        const ZayExtend* mGlueFunction;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // ZayInsideElement
    ////////////////////////////////////////////////////////////////////////////////
    class ZayInsideElement : public ZayUIElement
    {
    public:
        ZayInsideElement() : ZayUIElement(Type::Inside) {}
        ~ZayInsideElement() override {}

    public:
        void Load(const ZaySon& root, const Context& context) override
        {
            ZayUIElement::Load(root, context);

            hook(context("name"))
                mName = fish.GetText();

            hook(context("ui"))
            for(sint32 i = 0, iend = fish.LengthOfIndexable(); i < iend; ++i)
            {
                if(ZayConditionElement::Test(root, mChildren, fish[i]))
                    continue;
                ZayUI NewUI(ZayUIElement::Create(Type::Component));
                NewUI->Load(root, fish[i]);
                mChildren.AtAdding() = (id_share) NewUI;
            }
        }

    public:
        String mName;
        ZayUIs mChildren;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // ZayGateElement
    ////////////////////////////////////////////////////////////////////////////////
    class ZayGateElement : public ZayUIElement
    {
    public:
        ZayGateElement() : ZayUIElement(Type::Gate) {}
        ~ZayGateElement() override {}

    public:
        void Load(const ZaySon& root, const Context& context) override
        {
            ZayUIElement::Load(root, context);

            hook(context("ui"))
            for(sint32 i = 0, iend = fish.LengthOfIndexable(); i < iend; ++i)
            {
                if(ZayConditionElement::Test(root, mChildren, fish[i]))
                    continue;
                ZayUI NewUI(ZayUIElement::Create(Type::Component));
                NewUI->Load(root, fish[i]);
                mChildren.AtAdding() = (id_share) NewUI;
            }
        }

    public:
        void RenderByCall(ZayPanel& panel, const String& defaultname, DebugLogs& logs) const
        {
            // 자식으로 재귀
            if(0 < mChildren.Count())
            {
                sint32s CollectedChildren = ZayConditionElement::Collect(mRefRoot->ViewName(), mChildren, &panel);
                for(sint32 i = 0, iend = CollectedChildren.Count(); i < iend; ++i)
                {
                    auto CurChildren = (const ZayUIElement*) mChildren[CollectedChildren[i]].ConstPtr();
                    CurChildren->Render(panel, defaultname + String::Format(".%d", i), logs);
                }
            }
        }

    public:
        ZayUIs mChildren;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // ZayComponentElement
    ////////////////////////////////////////////////////////////////////////////////
    class ZayComponentElement : public ZayUIElement, ZayExtend::Renderer
    {
    public:
        struct Lambda
        {
            ZayUIs mCodes;
            Strings mCaptureKeys;
            typedef Map<String> CaptureValue; // [CaptureKey]
            mutable Map<CaptureValue> mCaptureValues; // [UIName][CaptureKey]
        };

    public:
        ZayComponentElement() : ZayUIElement(Type::Component)
        {
            mCompID = -2;
            mValidDoubleClicked = false;
            mValidLongPressed = false;
        }
        ~ZayComponentElement() override {}

    public:
        void Load(const ZaySon& root, const Context& context) override
        {
            ZayUIElement::Load(root, context);

            hook(context("compname"))
                mCompName = fish.GetText();

            hook(context("compid"))
                mCompID = fish.GetInt(-2);

            hook(context("compvalues"))
            for(sint32 i = 0, iend = fish.LengthOfIndexable(); i < iend; ++i)
            {
                if(ZayConditionElement::Test(root, mCompValues, fish[i]))
                    continue;
                ZayUI NewUI(ZayUIElement::Create(Type::Param));
                NewUI->Load(root, fish[i]);
                mCompValues.AtAdding() = (id_share) NewUI;
            }

            hook(context("compinputs")) // 코드문
            for(sint32 i = 0, iend = fish.LengthOfIndexable(); i < iend; ++i)
            {
                if(ZayConditionElement::Test(root, mInputCodes, fish[i]))
                    continue;
                ZayUI NewUI(ZayUIElement::Create(Type::Request));
                NewUI->Load(root, fish[i]);
                ((ZayRequestElement*) NewUI.Ptr())->InitForInput();
                mInputCodes.AtAdding() = (id_share) NewUI;
            }

            hook(context("onset"))
                LoadCode(root, fish, ZayUIElement::LambdaID::OnSet);

            hook(context("ontouch"))
                LoadCode(root, fish, ZayUIElement::LambdaID::OnTouch);

            hook(context("onclick"))
                LoadCode(root, fish, ZayUIElement::LambdaID::OnClick);

            hook(context("clicktype"))
            {
                if(!fish.GetText().Compare("Click_DoubleClick"))
                {
                    mValidDoubleClicked = true;
                    mValidLongPressed = false;
                }
                else if(!fish.GetText().Compare("Click_LongPress"))
                {
                    mValidDoubleClicked = false;
                    mValidLongPressed = true;
                }
                else if(!fish.GetText().Compare("Click_DoubleClick_LongPress"))
                {
                    mValidDoubleClicked = true;
                    mValidLongPressed = true;
                }
                else
                {
                    mValidDoubleClicked = false;
                    mValidLongPressed = false;
                }
            }

            hook(context("ui"))
            for(sint32 i = 0, iend = fish.LengthOfIndexable(); i < iend; ++i)
            {
                if(ZayConditionElement::Test(root, mChildren, fish[i]))
                    continue;
                ZayUI NewUI(ZayUIElement::Create(Type::Component));
                NewUI->Load(root, fish[i]);
                mChildren.AtAdding() = (id_share) NewUI;
            }

            hook(context("insiders"))
            for(sint32 i = 0, iend = fish.LengthOfIndexable(); i < iend; ++i)
            {
                ZayUI NewUI(ZayUIElement::Create(Type::Inside));
                NewUI->Load(root, fish[i]);
                mInsiders.AtAdding() = (id_share) NewUI;
            }
        }

    private:
        void LoadCode(const ZaySon& root, const Context& json, LambdaID id)
        {
            Map<String> CaptureCollector;
            for(sint32 i = 0, iend = json.LengthOfIndexable(); i < iend; ++i)
            {
                if(ZayConditionElement::Test(root, mLambdas[(sint32) id].mCodes, json[i], &CaptureCollector))
                    continue;
                ZayUI NewUI(ZayUIElement::Create(Type::Request));
                NewUI->Load(root, json[i]);
                ((ZayRequestElement*) NewUI.Ptr())->InitForClick(CaptureCollector);
                mLambdas[(sint32) id].mCodes.AtAdding() = (id_share) NewUI;
            }
            // 캡쳐목록 정리
            for(sint32 i = 0, iend = CaptureCollector.Count(); i < iend; ++i)
                mLambdas[(sint32) id].mCaptureKeys.AtAdding() = *CaptureCollector.AccessByOrder(i);
        }

    private:
        void Render(ZayPanel& panel, const String& defaultname, DebugLogs& logs) const override
        {
            if(auto CurComponent = mRefRoot->FindComponent(mCompName))
            {
                // 디버깅 정보수집
                auto AddDebugLog = [](DebugLogs& logs, ZayPanel& panel, bool fill, chars uiname)->void
                {
                    auto& NewLog = logs.AtAdding();
                    NewLog.mRect = Rect(panel.toview(0, 0), panel.toview(panel.w(), panel.h())) * panel.zoom().scale;
                    NewLog.mFill = fill;
                    NewLog.mUIName = uiname;
                };

                chars ViewName = mRefRoot->ViewName();
                const String UIName((mUINameSolver.is_blank())? "" : (chars) mUINameSolver.ExecuteVariableName());
                if(mCompValues.Count() == 0)
                {
                    if(0 < mInputCodes.Count()) // 코드문
                    {
                        // 지역변수 수집
                        Solvers LocalSolvers;
                        const Point ViewPos = panel.toview(0, 0);
                        LocalSolvers.AtAdding().Link(ViewName, "pX").Parse(String::FromFloat(ViewPos.x)).Execute();
                        LocalSolvers.AtAdding().Link(ViewName, "pY").Parse(String::FromFloat(ViewPos.y)).Execute();
                        LocalSolvers.AtAdding().Link(ViewName, "pW").Parse(String::FromFloat(panel.w())).Execute();
                        LocalSolvers.AtAdding().Link(ViewName, "pH").Parse(String::FromFloat(panel.h())).Execute();

                        sint32s CollectedCodes = ZayConditionElement::Collect(ViewName, mInputCodes);
                        for(sint32 i = 0, iend = CollectedCodes.Count(); i < iend; ++i)
                        {
                            auto CurCompCode = (ZayRequestElement*) mInputCodes.At(CollectedCodes[i]).Ptr();
                            CurCompCode->Transaction();
                        }
                        RenderChildren(mChildren, panel, nullptr, defaultname, logs);
                    }
                    else if(!mUILoopSolver.is_blank()) // 반복문
                    {
                        const sint32 LoopCount = Math::Max(0, (sint32) mUILoopSolver.ExecuteOnly().ToInteger());
                        for(sint32 i = 0; i < LoopCount; ++i)
                        {
                            // 지역변수 수집
                            Solvers LocalSolvers;
                            if(0 < UIName.Length())
                            {
                                LocalSolvers.AtAdding().Link(ViewName, UIName + "V").Parse(String::FromInteger(i)).Execute();
                                LocalSolvers.AtAdding().Link(ViewName, UIName + "N").Parse(String::FromInteger(LoopCount)).Execute();
                            }
                            RenderChildren(mChildren, panel, nullptr, defaultname + String::Format("_%d", i), logs);
                        }
                    }
                    else if(!mCompName.Compare("jump")) // 호출문
                    {
                        if(auto CurGate = (ZayGateElement*)(ZayUIElement*) mRefRoot->FindGate(UIName))
                        {
                            if(mCompID == mRefRoot->debugFocusedCompID())
                                AddDebugLog(logs, panel, CurComponent->HasContentComponent(), "jump to '" + UIName + "'");
                            RenderChildren(CurGate->mChildren, panel, defaultname, defaultname, logs);
                        }
                        else if(mCompID == mRefRoot->debugFocusedCompID())
                            AddDebugLog(logs, panel, CurComponent->HasContentComponent(), "no gate called '" + UIName + "'");
                    }
                    else
                    {
                        String UINameTemp;
                        chars ComponentName = nullptr;
                        if(0 < UIName.Length())
                            ComponentName = UINameTemp = ViewName + ('.' + UIName);
                        else for(sint32 i = 0, iend = (sint32) LambdaID::Max; i < iend; ++i)
                        {
                            if(0 < mLambdas[i].mCodes.Count())
                            {
                                ComponentName = defaultname;
                                break;
                            }
                        }

                        // 파라미터 없음
                        mInsidersLogs = &logs;
                        mInsidersComponent = CurComponent;
                        mInsidersComponentName = ComponentName;
                        mInsidersDefaultName = defaultname;
                        ZayExtend::Payload ParamCollector = CurComponent->MakePayload(ComponentName, mID, this);
                        ZAY_EXTEND(ParamCollector >> panel)
                        {
                            if(mCompID == mRefRoot->debugFocusedCompID())
                                AddDebugLog(logs, panel, CurComponent->HasContentComponent(), ComponentName);
                            RenderChildren(mChildren, panel, ComponentName, defaultname, logs);
                        }
                    }
                }
                else // CompValue항목이 존재할 경우
                {
                    // 조건문에 의해 살아남은 유효한 CompValue를 모두 실행
                    sint32s CollectedCompValues = ZayConditionElement::Collect(ViewName, mCompValues, &panel);
                    for(sint32 i = 0, iend = CollectedCompValues.Count(); i < iend; ++i)
                    {
                        const String DefaultName(defaultname + ((1 < iend)? (chars) String::Format("_%d", i) : ""));
                        String UINameTemp;
                        chars ComponentName = nullptr;
                        if(0 < UIName.Length())
                            ComponentName = UINameTemp = ViewName + ('.' + UIName) + ((1 < iend)? (chars) String::FromInteger(i) : "");
                        else for(sint32 j = 0, jend = (sint32) LambdaID::Max; j < jend; ++j)
                        {
                            if(0 < mLambdas[j].mCodes.Count())
                            {
                                ComponentName = DefaultName;
                                break;
                            }
                        }

                        // 지역변수 수집
                        Solvers LocalSolvers;
                        const Point ViewPos = panel.toview(0, 0);
                        LocalSolvers.AtAdding().Link(ViewName, "pX").Parse(String::FromFloat(ViewPos.x)).Execute();
                        LocalSolvers.AtAdding().Link(ViewName, "pY").Parse(String::FromFloat(ViewPos.y)).Execute();
                        LocalSolvers.AtAdding().Link(ViewName, "pW").Parse(String::FromFloat(panel.w())).Execute();
                        LocalSolvers.AtAdding().Link(ViewName, "pH").Parse(String::FromFloat(panel.h())).Execute();
                        if(1 < iend && 0 < UIName.Length())
                        {
                            LocalSolvers.AtAdding().Link(ViewName, UIName + "V").Parse(String::FromInteger(i)).Execute();
                            LocalSolvers.AtAdding().Link(ViewName, UIName + "N").Parse(String::FromInteger(iend)).Execute();
                        }

                        // 파라미터 수집
                        mInsidersLogs = &logs;
                        mInsidersComponent = CurComponent;
                        mInsidersComponentName = ComponentName;
                        mInsidersDefaultName = DefaultName;
                        ZayExtend::Payload ParamCollector = CurComponent->MakePayload(ComponentName, mID, this);
                        if(auto CurCompValue = (const ZayParamElement*) mCompValues[CollectedCompValues[i]].ConstPtr())
                        {
                            for(sint32 j = 0, jend = CurCompValue->mParamSolvers.Count(); j < jend; ++j)
                                ParamCollector(CurCompValue->mParamSolvers[j].ExecuteOnly());
                        }
                        ZAY_EXTEND(ParamCollector >> panel)
                        {
                            if(mCompID == mRefRoot->debugFocusedCompID())
                                AddDebugLog(logs, panel, CurComponent->HasContentComponent(), ComponentName);
                            RenderChildren(mChildren, panel, ComponentName, DefaultName, logs);
                        }
                    }
                }
            }
            else mRefRoot->SendWarningLog("Rendering Failed", String::Format(" ● Component not found. (%s/Render)", (chars) mCompName));
        }
        void RenderChildren(const ZayUIs& children, ZayPanel& panel, chars uiname, const String& defaultname, DebugLogs& logs) const
        {
            // 클릭코드를 위한 변수를 사전 캡쳐
            for(sint32 i = 0, iend = (sint32) LambdaID::Max; i < iend; ++i)
            {
                if(uiname != nullptr && 0 < mLambdas[i].mCaptureKeys.Count())
                {
                    const Point ViewPos = panel.toview(0, 0);
                    hook(mLambdas[i].mCaptureValues(uiname))
                    for(sint32 j = 0, jend = mLambdas[i].mCaptureKeys.Count(); j < jend; ++j)
                    {
                        chars CurVariable = mLambdas[i].mCaptureKeys[j];
                        branch;
                        jump(!String::Compare(CurVariable, "pX")) fish(CurVariable) = String::FromFloat(ViewPos.x);
                        jump(!String::Compare(CurVariable, "pY")) fish(CurVariable) = String::FromFloat(ViewPos.y);
                        jump(!String::Compare(CurVariable, "pW")) fish(CurVariable) = String::FromFloat(panel.w());
                        jump(!String::Compare(CurVariable, "pH")) fish(CurVariable) = String::FromFloat(panel.h());
                        else fish(CurVariable) = Solver().Link(mRefRoot->ViewName()).Parse(CurVariable).ExecuteOnly().ToText(true);
                    }
                }
            }

            // 자식으로 재귀
            if(0 < children.Count())
            {
                sint32s CollectedChildren = ZayConditionElement::Collect(mRefRoot->ViewName(), children, &panel);
                for(sint32 i = 0, iend = CollectedChildren.Count(); i < iend; ++i)
                {
                    auto CurChildren = (const ZayUIElement*) children[CollectedChildren[i]].ConstPtr();
                    CurChildren->Render(panel, defaultname + String::Format(".%d", i), logs);
                }
            }
        }
        void SetCursor(CursorRole role) override
        {
            mRefRoot->SendCursor(role);
        }
        bool OnLambda(chars uiname, LambdaID id, chars key, chars value, bool doubleclicked, bool longpressed, bool outreleased, bool cancelreleased) override
        {
            if(0 < mLambdas[(sint32) id].mCodes.Count())
            {
                // 사전 캡쳐된 변수를 지역변수화
                Solvers LocalSolvers;
                hook(mLambdas[(sint32) id].mCaptureValues(uiname))
                for(sint32 i = 0, iend = fish.Count(); i < iend; ++i)
                {
                    chararray GetVariable;
                    auto& Value = *fish.AccessByOrder(i, &GetVariable);
                    LocalSolvers.AtAdding().Link(mRefRoot->ViewName(), &GetVariable[0]).Parse(Value).Execute();
                }

                // 변수를 지역변수화
                if(key && value)
                    LocalSolvers.AtAdding().Link(mRefRoot->ViewName(), key).Parse(String::Format("\'%s\'", value)).Execute();

                // 클릭코드의 실행
                sint32s CollectedClickCodes = ZayConditionElement::Collect(mRefRoot->ViewName(), mLambdas[(sint32) id].mCodes,
                    nullptr, doubleclicked, longpressed, outreleased, cancelreleased);
                for(sint32 i = 0, iend = CollectedClickCodes.Count(); i < iend; ++i)
                {
                    auto CurClickCode = (ZayRequestElement*) mLambdas[(sint32) id].mCodes.At(CollectedClickCodes[i]).Ptr();
                    CurClickCode->Transaction();
                }
                return true;
            }
            return false;
        }
        bool IsValidDoubleClick() override
        {
            return mValidDoubleClicked;
        }
        bool IsValidLongPress() override
        {
            return mValidLongPressed;
        }

    public: // ZayExtend::Renderer 구현부
        bool HasInsider(chars name) const override
        {
            for(sint32 i = 0, iend = mInsiders.Count(); i < iend; ++i)
            {
                auto CurInsider = (const ZayInsideElement*) mInsiders[i].ConstPtr();
                if(!CurInsider->mName.Compare(name))
                    return true;
            }
            return false;
        }
        bool RenderInsider(chars name, ZayPanel& panel, sint32 pv) const override
        {
            // 디버깅 정보수집
            auto AddDebugLog = [](DebugLogs& logs, ZayPanel& panel, bool fill, chars uiname)->void
            {
                auto& NewLog = logs.AtAdding();
                NewLog.mRect = Rect(panel.toview(0, 0), panel.toview(panel.w(), panel.h())) * panel.zoom().scale;
                NewLog.mFill = fill;
                NewLog.mUIName = uiname;
            };

            for(sint32 i = 0, iend = mInsiders.Count(); i < iend; ++i)
            {
                auto CurInsider = (const ZayInsideElement*) mInsiders[i].ConstPtr();
                if(!CurInsider->mName.Compare(name))
                {
                    if(mCompID == mRefRoot->debugFocusedCompID())
                        AddDebugLog(*mInsidersLogs, panel, mInsidersComponent->HasContentComponent(), mInsidersComponentName);
                    if(pv != -1)
                    {
                        const String PV = String::FromInteger(pv);
                        Solvers LocalSolvers;
                        LocalSolvers.AtAdding().Link(mRefRoot->ViewName(), "pV").Parse(PV).Execute();
                        RenderChildren(CurInsider->mChildren, panel, mInsidersComponentName,
                            mInsidersDefaultName + ("." + CurInsider->mName + "." + PV), *mInsidersLogs);
                    }
                    else RenderChildren(CurInsider->mChildren, panel, mInsidersComponentName,
                        mInsidersDefaultName + ("." + CurInsider->mName), *mInsidersLogs);
                    return true;
                }
            }
            return false;
        }

    public:
        String mCompName;
        sint32 mCompID;
        ZayUIs mCompValues;
        mutable ZayUIs mInputCodes;
        Lambda mLambdas[(sint32) LambdaID::Max];
        bool mValidDoubleClicked;
        bool mValidLongPressed;
        ZayUIs mChildren;

    public:
        ZayUIs mInsiders;
        mutable DebugLogs* mInsidersLogs {nullptr};
        mutable const ZayExtend* mInsidersComponent {nullptr};
        mutable chars mInsidersComponentName {nullptr};
        mutable chars mInsidersDefaultName {nullptr};
    };

    ////////////////////////////////////////////////////////////////////////////////
    // ZayViewElement
    ////////////////////////////////////////////////////////////////////////////////
    class ZayViewElement : public ZayUIElement
    {
    public:
        ZayViewElement() : ZayUIElement(Type::View) {}
        ~ZayViewElement() override {}

    public:
        void Load(const ZaySon& root, const Context& context) override
        {
            ZayUIElement::Load(root, context);

            hook(context("oncreate"))
            for(sint32 i = 0, iend = fish.LengthOfIndexable(); i < iend; ++i)
            {
                if(ZayConditionElement::Test(root, mCreateCodes, fish[i]))
                    continue;
                ZayUI NewUI(ZayUIElement::Create(Type::Request));
                NewUI->Load(root, fish[i]);
                ((ZayRequestElement*) NewUI.Ptr())->InitForCreate();
                mCreateCodes.AtAdding() = (id_share) NewUI;
            }

            hook(context("ui"))
            for(sint32 i = 0, iend = fish.LengthOfIndexable(); i < iend; ++i)
            {
                if(ZayConditionElement::Test(root, mChildren, fish[i]))
                    continue;
                ZayUI NewUI(ZayUIElement::Create(Type::Component));
                NewUI->Load(root, fish[i]);
                mChildren.AtAdding() = (id_share) NewUI;
            }

            hook(context("extends"))
            for(sint32 i = 0, iend = fish.LengthOfIndexable(); i < iend; ++i)
            {
                if(!fish[i]("compname").GetText().Compare("gate"))
                {
                    const String GateName = fish[i]("uiname").GetText();
                    if(0 < GateName.Length())
                    {
                        ZayUI NewUI(ZayUIElement::Create(Type::Gate));
                        NewUI->Load(root, fish[i]);
                        mGates(GateName) = (id_share) NewUI;
                    }
                }
            }
        }

    private:
        void Render(ZayPanel& panel, const String& defaultname, DebugLogs& logs) const override
        {
            sint32s CollectedChildren = ZayConditionElement::Collect(mRefRoot->ViewName(), mChildren, &panel);
            for(sint32 i = 0, iend = CollectedChildren.Count(); i < iend; ++i)
            {
                auto CurChildren = (const ZayUIElement*) mChildren[CollectedChildren[i]].ConstPtr();
                CurChildren->Render(panel, defaultname + String::Format(".%d", i), logs);
            }
        }

    public:
        ZayUIs mCreateCodes;
        ZayUIs mChildren;
        Map<ZayUI> mGates;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // ZayUIElement::Create
    ////////////////////////////////////////////////////////////////////////////////
    buffer ZayUIElement::Create(Type type)
    {
        switch(type)
        {
        case Type::Condition: return Buffer::Alloc<ZayConditionElement>(BOSS_DBG 1);
        case Type::Param: return Buffer::Alloc<ZayParamElement>(BOSS_DBG 1);
        case Type::Request: return Buffer::Alloc<ZayRequestElement>(BOSS_DBG 1);
        case Type::Inside: return Buffer::Alloc<ZayInsideElement>(BOSS_DBG 1);
        case Type::Gate: return Buffer::Alloc<ZayGateElement>(BOSS_DBG 1);
        case Type::Component: return Buffer::Alloc<ZayComponentElement>(BOSS_DBG 1);
        case Type::View: return Buffer::Alloc<ZayViewElement>(BOSS_DBG 1);
        }
        return nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ZaySon
    ////////////////////////////////////////////////////////////////////////////////
    ZaySon::ZaySon()
    {
        mUIElement = nullptr;
        // 디버그정보
        mDebugFocusedCompID = -1;
    }

    ZaySon::~ZaySon()
    {
        delete (ZayUIElement*) mUIElement;
    }

    ZaySon::ZaySon(ZaySon&& rhs)
    {
        operator=(ToReference(rhs));
    }

    ZaySon& ZaySon::operator=(ZaySon&& rhs)
    {
        mViewName = ToReference(rhs.mViewName);
        delete (ZayUIElement*) mUIElement;
        mUIElement = rhs.mUIElement;
        rhs.mUIElement = nullptr;
        mExtendMap = ToReference(rhs.mExtendMap);
        mJumpCalls = ToReference(rhs.mJumpCalls);
        mDebugLogger = ToReference(mDebugLogger);
        mDebugFocusedCompID = ToReference(rhs.mDebugFocusedCompID);
        return *this;
    }

    void ZaySon::Load(chars viewname, const Context& context)
    {
        mViewName = viewname;
        Reload(context);
    }

    void ZaySon::Reload(const Context& context)
    {
        BOSS_ASSERT("Reload는 Load후 호출가능합니다", 0 < mViewName.Length());
        delete (ZayUIElement*) mUIElement;
        auto NewView = new ZayViewElement();
        NewView->Load(*this, context);
        mUIElement = NewView;

        SendInfoLog("ZaySon Loaded", String::Format(" ● %s (Reload)", (chars) mViewName));
        auto ComponentNames = AllComponentNames();
        for(sint32 i = 0, iend = ComponentNames.Count(); i < iend; ++i)
            SendInfoLog("Component Added", String::Format(" ● %s", (chars) ComponentNames[i]));
        auto GlueNames = AllGlueNames();
        for(sint32 i = 0, iend = GlueNames.Count(); i < iend; ++i)
            SendInfoLog("Glue Added", String::Format(" ● %s", (chars) GlueNames[i]));
        auto GateNames = AllGateNames();
        for(sint32 i = 0, iend = GateNames.Count(); i < iend; ++i)
            SendInfoLog("Gate Added", String::Format(" ● %s", (chars) GateNames[i]));
    }

    void ZaySon::SetLogger(LoggerCB cb)
    {
        mDebugLogger = cb;
    }

    const String& ZaySon::ViewName() const
    {
        return mViewName;
    }

    ZaySonInterface& ZaySon::AddComponent(ZayExtend::ComponentType type, chars name,
        ZayExtend::ComponentCB cb, chars comments, chars samples)
    {
        String OneName = name;
        const sint32 Pos = OneName.Find(0, ' ');
        if(Pos != -1) OneName = OneName.Left(Pos);

        auto& NewFunction = mExtendMap(OneName);
        NewFunction.ResetForComponent(type, cb);
        return *this;
    }

    ZaySonInterface& ZaySon::AddGlue(chars name, ZayExtend::GlueCB cb)
    {
        String OneName = name;
        const sint32 Pos = OneName.Find(0, ' ');
        if(Pos != -1) OneName = OneName.Left(Pos);

        auto& NewFunction = mExtendMap(OneName);
        NewFunction.ResetForGlue(cb);
        return *this;
    }

    void ZaySon::JumpCall(chars gatename, sint32 runcount)
    {
        mJumpCalls.AtAdding() = String::Format("%s,0,%d", gatename, runcount);
    }

    void ZaySon::JumpCallWithArea(chars gatename, sint32 runcount, float x, float y, float w, float h)
    {
        mJumpCalls.AtAdding() = String::Format("%s,0,%d,%f,%f,%f,%f", gatename, runcount, x, y, w, h);
    }

    void ZaySon::JumpClear()
    {
        mJumpCalls.Clear();
    }

    const ZayExtend* ZaySon::FindComponent(chars name) const
    {
        if(auto FindedFunc = mExtendMap.Access(name))
        {
            const ZayExtend* Result = (FindedFunc->HasComponent())? FindedFunc : nullptr;
            BOSS_ASSERT(String::Format("\"%s\"는 Component함수가 아닙니다", name), Result);
            return Result;
        }
        BOSS_ASSERT(String::Format("\"%s\"으로 등록된 ZayExtend를 찾을 수 없습니다", name), false);
        return nullptr;
    }

    const ZayExtend* ZaySon::FindGlue(chars name) const
    {
        if(auto FindedFunc = mExtendMap.Access(name))
        {
            const ZayExtend* Result = (FindedFunc->HasGlue())? FindedFunc : nullptr;
            BOSS_ASSERT(String::Format("\"%s\"는 Glue함수가 아닙니다", name), Result);
            return Result;
        }
        BOSS_ASSERT(String::Format("\"%s\"으로 등록된 ZayExtend를 찾을 수 없습니다", name), false);
        return nullptr;
    }

    const void* ZaySon::FindGate(chars name) const
    {
        if(auto TopElement = (ZayViewElement*)(ZayUIElement*) mUIElement)
        if(auto GateElement = TopElement->mGates.Access(name))
            return (*GateElement).ConstPtr();
        return nullptr;
    }

    const Strings ZaySon::AllComponentNames() const
    {
        Strings Collector;
        for(sint32 i = 0, iend = mExtendMap.Count(); i < iend; ++i)
        {
            chararray GetName;
            if(auto FindedFunc = mExtendMap.AccessByOrder(i, &GetName))
            if(FindedFunc->HasComponent())
                Collector.AtAdding() = &GetName[0];
        }
        return Collector;
    }

    const Strings ZaySon::AllGlueNames() const
    {
        Strings Collector;
        for(sint32 i = 0, iend = mExtendMap.Count(); i < iend; ++i)
        {
            chararray GetName;
            if(auto FindedFunc = mExtendMap.AccessByOrder(i, &GetName))
            if(FindedFunc->HasGlue())
                Collector.AtAdding() = &GetName[0];
        }
        return Collector;
    }

    const Strings ZaySon::AllGateNames() const
    {
        Strings Collector;
        if(auto TopElement = (ZayViewElement*)(ZayUIElement*) mUIElement)
        for(sint32 i = 0, iend = TopElement->mGates.Count(); i < iend; ++i)
        {
            chararray GetName;
            if(auto FindedGate = TopElement->mGates.AccessByOrder(i, &GetName))
                Collector.AtAdding() = &GetName[0];
        }
        return Collector;
    }

    void ZaySon::Render(ZayPanel& panel)
    {
        if(mUIElement)
        {
            ZayUIElement::DebugLogs LogCollector;
            ((ZayUIElement*) mUIElement)->Render(panel, mViewName, LogCollector);

            // 수집된 점프콜 처리
            bool HasJumpCall = false;
            for(sint32 i = 0, iend = mJumpCalls.Count(); i < iend; ++i)
            {
                const Strings& JumpParams = String::Split(mJumpCalls[i]);
                const String GateName = JumpParams[0];
                const sint32 RunIndex = Parser::GetInt(JumpParams[1]);
                const sint32 RunCount = Parser::GetInt(JumpParams[2]);
                if(RunIndex < RunCount)
                {
                    HasJumpCall = true;
                    if(auto CurGate = (ZayGateElement*)(ZayUIElement*) FindGate(GateName))
                    {
                        // 지역변수 수집
                        Solvers LocalSolvers;
                        const Point ViewPos = panel.toview(0, 0);
                        LocalSolvers.AtAdding().Link(mViewName, "pV").Parse(String::FromInteger(RunIndex)).Execute();
                        if(JumpParams.Count() == 7)
                        {
                            LocalSolvers.AtAdding().Link(mViewName, "pX").Parse(JumpParams[3]).Execute();
                            LocalSolvers.AtAdding().Link(mViewName, "pY").Parse(JumpParams[4]).Execute();
                            LocalSolvers.AtAdding().Link(mViewName, "pW").Parse(JumpParams[5]).Execute();
                            LocalSolvers.AtAdding().Link(mViewName, "pH").Parse(JumpParams[6]).Execute();
                            const float X = Parser::GetFloat(JumpParams[3]);
                            const float Y = Parser::GetFloat(JumpParams[4]);
                            const float W = Parser::GetFloat(JumpParams[5]);
                            const float H = Parser::GetFloat(JumpParams[6]);
                            ZAY_XYWH(panel, X - ViewPos.x, Y - ViewPos.y, W, H)
                                CurGate->RenderByCall(panel, mViewName + String::Format(".jumpcall_%d", i), LogCollector);
                        }
                        else
                        {
                            LocalSolvers.AtAdding().Link(mViewName, "pX").Parse(String::FromFloat(ViewPos.x)).Execute();
                            LocalSolvers.AtAdding().Link(mViewName, "pY").Parse(String::FromFloat(ViewPos.y)).Execute();
                            LocalSolvers.AtAdding().Link(mViewName, "pW").Parse(String::FromFloat(panel.w())).Execute();
                            LocalSolvers.AtAdding().Link(mViewName, "pH").Parse(String::FromFloat(panel.h())).Execute();
                            CurGate->RenderByCall(panel, mViewName + String::Format(".jumpcall_%d", i), LogCollector);
                        }
                    }
                    // 점프콜 변경
                    String NextJumpCall = String::Format("%s,%d", (chars) GateName, RunIndex + 1);
                    for(sint32 i = 2, iend = JumpParams.Count(); i < iend; ++i)
                        NextJumpCall += ',' + JumpParams[i];
                    mJumpCalls.At(i) = NextJumpCall;
                }
            }
            if(HasJumpCall)
                panel.repaint(2);
            else mJumpCalls.Clear();

            // 수집된 디버그로그(GUI툴에 의한 포커스표현)
            const Point ViewPos = panel.toview(0, 0);
            ZAY_ZOOM_CLEAR(panel)
            ZAY_MOVE(panel, -ViewPos.x, -ViewPos.y)
            for(sint32 i = 0, iend = LogCollector.Count(); i < iend; ++i)
            {
                hook(LogCollector[i])
                ZAY_RECT(panel, fish.mRect)
                ZAY_FONT(panel, Math::MaxF(0.8, (fish.mRect.r - fish.mRect.l) * 0.005))
                {
                    // 영역표시
                    if(fish.mFill)
                    {
                        ZAY_RGBA(panel, 255, 0, 0, 96)
                            panel.fill();
                    }
                    else
                    {
                        ZAY_RGBA(panel, 255, 0, 0, 96)
                            panel.rect(5);
                        ZAY_INNER(panel, 5)
                        ZAY_RGBA(panel, 0, 255, 0, 64)
                            panel.rect(5);
                    }

                    // UI명칭
                    if(0 < fish.mUIName.Length())
                    {
                        ZAY_RGB(panel, 0, 0, 0)
                        for(sint32 y = -1; y <= 1; ++y)
                        for(sint32 x = -1; x <= 1; ++x)
                            ZAY_MOVE(panel, x, y)
                                panel.text(panel.w() / 2, panel.h() / 2, fish.mUIName);
                        ZAY_RGB(panel, 255, 0, 0)
                            panel.text(panel.w() / 2, panel.h() / 2, fish.mUIName);
                    }
                }
            }
        }
    }

    void ZaySon::SendCursor(CursorRole role) const
    {
        if(mDebugLogger != nullptr)
            mDebugLogger(LogType::Option, "Cursor", String::FromInteger((sint32) role));
    }

    void ZaySon::SendInfoLog(chars title, chars detail) const
    {
        if(mDebugLogger == nullptr)
            BOSS_TRACE("[ZaySonInfo] %s (%s)", title, detail);
        else mDebugLogger(LogType::Info, title, detail);
    }

    void ZaySon::SendWarningLog(chars title, chars detail) const
    {
        if(mDebugLogger == nullptr)
            BOSS_TRACE("[ZaySonWarning] %s (%s)", title, detail);
        else mDebugLogger(LogType::Warning, title, detail);
    }

    void ZaySon::SendErrorLog(chars title, chars detail) const
    {
        if(mDebugLogger == nullptr)
            BOSS_TRACE("[ZaySonError] %s (%s)", title, detail);
        else mDebugLogger(LogType::Error, title, detail);
    }

    void ZaySon::SetFocusCompID(sint32 id)
    {
        mDebugFocusedCompID = id;
    }

    void ZaySon::ClearFocusCompID()
    {
        mDebugFocusedCompID = -1;
    }
}
