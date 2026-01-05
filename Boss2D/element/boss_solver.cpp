#include <boss.hpp>
#include "boss_solver.hpp"

#include <platform/boss_platform.hpp> 

namespace BOSS
{
    static Map<SolverChain> gSolverChains;
    static String gReplacerBefore;
    static String gReplacerAfter;

    // 상수항
    class SolverLiteral : public SolverOperand
    {
        BOSS_DECLARE_NONCOPYABLE_INITIALIZED_CLASS(SolverLiteral, mValue(rhs.mValue))
        public: SolverLiteral(SolverValue value = SolverValue()) : SolverOperand(SolverOperandType::Literal), mValue(value) {}
        public: ~SolverLiteral() {}

        // 가상 인터페이스
        public: void PrintFormula(String& collector) const override
        {collector += mValue.ToText(true);}
        public: void PrintVariables(Strings& collector, bool targetless_only) const override {}
        public: void UpdateChain(Solver* solver, SolverChain* chain) override {}
        public: float reliable() const override {return 1;}
        public: SolverValue result(SolverValue zero) const override {return mValue;}
        public: buffer clone() const override
        {
            buffer NewBuffer = Buffer::AllocNoConstructorOnce<SolverLiteral>(BOSS_DBG 1);
            BOSS_CONSTRUCTOR(NewBuffer, 0, SolverLiteral, mValue);
            return NewBuffer;
        }

        // 멤버
        public: const SolverValue mValue; // 값
    };

    // 변수항
    class SolverVariable : public SolverOperand
    {
        BOSS_DECLARE_NONCOPYABLE_INITIALIZED_CLASS(SolverVariable, mName(rhs.mName))
        public: SolverVariable(chars name = "") : SolverOperand(SolverOperandType::Variable), mName(name)
        {mSolver = nullptr; mChain = nullptr;}
        public: ~SolverVariable() {RemoveCurrentChain();}

        public: static const Solver* FindTarget(const SolverChain* firstchain, chars name)
        {
            if(auto CurChainPair = firstchain->Access(name))
            if(auto CurSolver = CurChainPair->target())
                return CurSolver;
            for(sint32 i = 0; name[i] != '\0'; ++i)
            {
                if(name[i] == '.')
                {
                    if(auto CurChain = gSolverChains.Access(name, i))
                    {
                        if(0 < gReplacerBefore.Length() && !String::Compare(name, gReplacerBefore, gReplacerBefore.Length()))
                        {
                            if(auto CurChainPair = CurChain->Access(gReplacerAfter + &name[i + 1]))
                                return CurChainPair->target();
                        }
                        else if(auto CurChainPair = CurChain->Access(&name[i + 1]))
                            return CurChainPair->target();
                    }
                    return nullptr;
                }
            }
            return nullptr;
        }

        // 비공개부
        private: void RemoveCurrentChain()
        {
            if(mSolver && mChain)
            {
                // 기존 체인을 제거
                if(auto OldChainPair = mChain->Access(mName))
                if(OldChainPair->SubObserver(mSolver))
                    mChain->Remove(mName);
                mSolver = nullptr;
                mChain = nullptr;
            }
        }

        // 가상 인터페이스
        public: void PrintFormula(String& collector) const override
        {collector += mName;}
        public: void PrintVariables(Strings& collector, bool targetless_only) const override
        {
            if(targetless_only)
            if(FindTarget(mChain, mName))
                return;
            collector.AtAdding() = mName;
        }
        public: void UpdateChain(Solver* solver, SolverChain* chain) override
        {
            RemoveCurrentChain();
            // 신규 체인을 연결
            mSolver = solver;
            mChain = chain;
            if(mSolver && mChain)
                (*mChain)(mName).AddObserver(mSolver);
        }
        public: float reliable() const override
        {
            if(auto CurSolver = FindTarget(mChain, mName))
                return CurSolver->reliable();
            return 1; // 해당 Solver를 찾지 못하면 텍스트타입이라 오히려 신뢰도가 100%
        }
        public: SolverValue result(SolverValue zero) const override
        {
            if(0 < reliable())
            {
                if(auto CurSolver = FindTarget(mChain, mName))
                    return CurSolver->result();
                return SolverValue::MakeText(mName); // 해당 Solver를 찾지 못하면 텍스트타입
            }
            return zero;
        }
        public: buffer clone() const override
        {
            buffer NewBuffer = Buffer::AllocNoConstructorOnce<SolverVariable>(BOSS_DBG 1);
            BOSS_CONSTRUCTOR(NewBuffer, 0, SolverVariable, mName);
            ((SolverVariable*) NewBuffer)->UpdateChain(mSolver, mChain);
            return NewBuffer;
        }

        // 멤버
        private: const String mName; // 이름
        private: Solver* mSolver; // 자기연산식
        private: SolverChain* mChain; // 검색체인
    };

    // 연산항
    class SolverFormula : public SolverOperand
    {
        BOSS_DECLARE_NONCOPYABLE_INITIALIZED_CLASS(SolverFormula, mOperatorType(SolverOperatorType::Unknown), mOperatorPriority(0))
        public: SolverFormula(SolverOperatorType type = SolverOperatorType::Unknown, sint32 priority = 0)
            : SolverOperand(SolverOperandType::Formula), mOperatorType(type), mOperatorPriority(priority)
        {mOperandP = nullptr; mChain = nullptr;}
        public: ~SolverFormula() {}

        // 가상 인터페이스
        public: void PrintFormula(String& collector) const override
        {
            auto PrintOperand = [](const SolverOperandObject& operand, String& collector)->void
            {
                if(operand->type() == SolverOperandType::Formula) collector += "(";
                operand->PrintFormula(collector);
                if(operand->type() == SolverOperandType::Formula) collector += ")";
            };

            // 좌항
            const sint32 OldLength = collector.Length();
            PrintOperand(mOperandL, collector);
            if(OldLength < collector.Length())
                collector += " ";

            // 연산항
            switch(mOperatorType)
            {
            case SolverOperatorType::Addition:        collector += "+ "; break;
            case SolverOperatorType::Subtract:        collector += "- "; break;
            case SolverOperatorType::Multiply:        collector += "* "; break;
            case SolverOperatorType::Divide:          collector += "/ "; break;
            case SolverOperatorType::Remainder:       collector += "% "; break;
            case SolverOperatorType::BitAnd:          collector += "& "; break;
            case SolverOperatorType::BitOr:           collector += "| "; break;
            case SolverOperatorType::BitXor:          collector += "^ "; break;
            case SolverOperatorType::BitShiftL:       collector += "<< "; break;
            case SolverOperatorType::BitShiftR:       collector += ">> "; break;
            case SolverOperatorType::Variabler:       collector += "@ "; break;
            case SolverOperatorType::Commenter:       collector += "?"; break;
            case SolverOperatorType::RangeTarget:     collector += "~ "; break;
            case SolverOperatorType::RangeTimer:      collector += ": "; break;
            case SolverOperatorType::Greater:         collector += "< "; break;
            case SolverOperatorType::GreaterOrEqual:  collector += "<= "; break;
            case SolverOperatorType::Less:            collector += "> "; break;
            case SolverOperatorType::LessOrEqual:     collector += ">= "; break;
            case SolverOperatorType::Equal:           collector += "== "; break;
            case SolverOperatorType::Different:       collector += "!= "; break;
            case SolverOperatorType::Function_Min:    collector += "[min] "; break;
            case SolverOperatorType::Function_Max:    collector += "[max] "; break;
            case SolverOperatorType::Function_Abs:    collector += "[abs] "; break;
            case SolverOperatorType::Function_Pow:    collector += "[pow] "; break;
            case SolverOperatorType::Function_Cos:    collector += "[cos] "; break;
            case SolverOperatorType::Function_Sin:    collector += "[sin] "; break;
            case SolverOperatorType::Function_Tan:    collector += "[tan] "; break;
            case SolverOperatorType::Function_Atan:   collector += "[atan] "; break;
            case SolverOperatorType::Function_And:    collector += "[and] "; break;
            case SolverOperatorType::Function_Or:     collector += "[or] "; break;
            case SolverOperatorType::Function_Multiply: collector += "[multiply] "; break;
            case SolverOperatorType::Function_Divide: collector += "[divide] "; break;
            case SolverOperatorType::Function_Find: collector += "[find] "; break;
            case SolverOperatorType::Function_Truncate: collector += "[truncate] "; break;
            }

            // 우항
            PrintOperand(mOperandR, collector);
        }
        public: void PrintVariables(Strings& collector, bool targetless_only) const override
        {
            mOperandL->PrintVariables(collector, targetless_only);
            mOperandR->PrintVariables(collector, targetless_only);
        }
        public: void UpdateChain(Solver* solver, SolverChain* chain) override
        {
            mChain = chain;
            mOperandL->UpdateChain(solver, chain);
            mOperandR->UpdateChain(solver, chain);
        }
        public: virtual float reliable() const override
        {
            if(mOperatorType == SolverOperatorType::Addition || mOperatorType == SolverOperatorType::Subtract)
                return (mOperandL->reliable() + mOperandR->reliable()) / 2;
            return mOperandL->reliable() * mOperandR->reliable();
        }
        public: virtual SolverValue result(SolverValue zero) const override
        {
            static const SolverValue Zero = SolverValue::MakeInteger(0);
            static const SolverValue One  = SolverValue::MakeInteger(1);
            if(0 < reliable())
            switch(mOperatorType)
            {
            case SolverOperatorType::Addition:          return mOperandL->result(Zero).Addition(mOperandR->result(Zero));
            case SolverOperatorType::Subtract:          return mOperandL->result(Zero).Subtract(mOperandR->result(Zero));
            case SolverOperatorType::Multiply:          return mOperandL->result(Zero).Multiply(mOperandR->result(One));
            case SolverOperatorType::Divide:            return mOperandL->result(Zero).Divide(mOperandR->result(One));
            case SolverOperatorType::Remainder:         return mOperandL->result(Zero).Remainder(mOperandR->result(One));
            case SolverOperatorType::BitAnd:            return mOperandL->result(Zero).BitAnd(mOperandR->result(One));
            case SolverOperatorType::BitOr:             return mOperandL->result(Zero).BitOr(mOperandR->result(One));
            case SolverOperatorType::BitXor:            return mOperandL->result(Zero).BitXor(mOperandR->result(One));
            case SolverOperatorType::BitShiftL:         return mOperandL->result(Zero).BitShiftL(mOperandR->result(One));
            case SolverOperatorType::BitShiftR:         return mOperandL->result(Zero).BitShiftR(mOperandR->result(One));
            case SolverOperatorType::Variabler:         return mOperandL->result(Zero).Variabler(mOperandR->result(One), mChain);
            case SolverOperatorType::Commenter:         return mOperandL->result(Zero);
            case SolverOperatorType::RangeTarget:       return mOperandL->result(Zero).RangeTarget(mOperandR->result(Zero));
            case SolverOperatorType::RangeTimer:        return mOperandL->result(Zero).RangeTimer(mOperandR->result(Zero));
            case SolverOperatorType::Greater:           return mOperandL->result(Zero).Greater(mOperandR->result(Zero));
            case SolverOperatorType::GreaterOrEqual:    return mOperandL->result(Zero).GreaterOrEqual(mOperandR->result(Zero));
            case SolverOperatorType::Less:              return mOperandL->result(Zero).Less(mOperandR->result(Zero));
            case SolverOperatorType::LessOrEqual:       return mOperandL->result(Zero).LessOrEqual(mOperandR->result(Zero));
            case SolverOperatorType::Equal:             return mOperandL->result(Zero).Equal(mOperandR->result(Zero));
            case SolverOperatorType::Different:         return mOperandL->result(Zero).Different(mOperandR->result(Zero));
            case SolverOperatorType::Function_Min:      return mOperandL->result(Zero).Function_Min(mOperandR->result(One));
            case SolverOperatorType::Function_Max:      return mOperandL->result(Zero).Function_Max(mOperandR->result(One));
            case SolverOperatorType::Function_Abs:      return mOperandL->result(Zero).Function_Abs(mOperandR->result(One));
            case SolverOperatorType::Function_Pow:      return mOperandL->result(Zero).Function_Pow(mOperandR->result(One));
            case SolverOperatorType::Function_Cos:      return mOperandL->result(Zero).Function_Cos(mOperandR->result(One));
            case SolverOperatorType::Function_Sin:      return mOperandL->result(Zero).Function_Sin(mOperandR->result(One));
            case SolverOperatorType::Function_Tan:      return mOperandL->result(Zero).Function_Tan(mOperandR->result(One));
            case SolverOperatorType::Function_Atan:     return mOperandL->result(Zero).Function_Atan(mOperandR->result(One));
            case SolverOperatorType::Function_And:      return mOperandL->result(Zero).Function_And(mOperandR->result(One));
            case SolverOperatorType::Function_Or:       return mOperandL->result(Zero).Function_Or(mOperandR->result(One));
            case SolverOperatorType::Function_Multiply: return mOperandL->result(Zero).Function_Multiply(mOperandR->result(One));
            case SolverOperatorType::Function_Divide:   return mOperandL->result(Zero).Function_Divide(mOperandR->result(One));
            case SolverOperatorType::Function_Find:     return mOperandL->result(Zero).Function_Find(mOperandR->result(One));
            case SolverOperatorType::Function_Truncate: return mOperandL->result(Zero).Function_Truncate(mOperandR->result(One));
            }
            return zero;
        }
        public: buffer clone() const override
        {
            buffer NewBuffer = Buffer::AllocNoConstructorOnce<SolverFormula>(BOSS_DBG 1);
            BOSS_CONSTRUCTOR(NewBuffer, 0, SolverFormula, mOperatorType, mOperatorPriority);
            ((SolverFormula*) NewBuffer)->mOperandL = mOperandL;
            ((SolverFormula*) NewBuffer)->mOperandR = mOperandR;
            ((SolverFormula*) NewBuffer)->mOperandP = mOperandP;
            return NewBuffer;
        }

        // 멤버
        public: const SolverOperatorType mOperatorType; // 연산기호
        public: const sint32 mOperatorPriority; // 연산순위
        public: SolverOperandObject mOperandL; // L항
        public: SolverOperandObject mOperandR; // R항
        public: SolverOperandObject* mOperandP; // 부모항
        private: const SolverChain* mChain; // 검색체인
    };

    // 주석항
    class SolverComment : public SolverOperand
    {
        BOSS_DECLARE_NONCOPYABLE_INITIALIZED_CLASS(SolverComment, mComment(rhs.mComment))
        public: SolverComment(const String& comment = String()) : SolverOperand(SolverOperandType::Comment), mComment(comment) {}
        public: ~SolverComment() {}

        // 가상 인터페이스
        public: void PrintFormula(String& collector) const override
        {collector += mComment;}
        public: void PrintVariables(Strings& collector, bool targetless_only) const override {}
        public: void UpdateChain(Solver* solver, SolverChain* chain) override {}
        public: float reliable() const override {return 1;}
        public: SolverValue result(SolverValue zero) const override {return zero;}
        public: buffer clone() const override
        {
            buffer NewBuffer = Buffer::AllocNoConstructorOnce<SolverComment>(BOSS_DBG 1);
            BOSS_CONSTRUCTOR(NewBuffer, 0, SolverComment, mComment);
            return NewBuffer;
        }

        // 멤버
        public: const String mComment; // 주석
    };

    void SolverChainPair::ResetTarget(Solver* solver, bool updateobservers)
    {
        mTarget.mSolver = solver;
        mTarget.mUpdateID++;
        if(updateobservers)
            RenualAllObservers();
    }

    void SolverChainPair::ForcedChangeTarget(Solver* oldsolver, Solver* newsolver)
    {
        if(mTarget.mSolver == oldsolver)
            mTarget.mSolver = newsolver;
    }

    bool SolverChainPair::RemoveTarget(bool updateobservers)
    {
        mTarget.mSolver = nullptr;
        mTarget.mUpdateID++;
        if(updateobservers)
            RenualAllObservers();
        return (mObserverMap.Count() == 0);
    }

    void SolverChainPair::AddObserver(Solver* solver)
    {
        auto& CurObserver = mObserverMap[PtrToUint64(solver)];
        CurObserver.mSolver = solver;
        CurObserver.mUpdateID = 0;
    }

    bool SolverChainPair::SubObserver(Solver* solver)
    {
        mObserverMap.Remove(PtrToUint64(solver));
        return (!mTarget.mSolver && mObserverMap.Count() == 0);
    }

    void SolverChainPair::RenualAllObservers()
    {
        for(sint32 i = 0, iend = mObserverMap.Count(); i < iend; ++i)
        {
            auto& CurObserver = *mObserverMap.AccessByOrder(i);
            if(CurObserver.mUpdateID != mTarget.mUpdateID)
            {
                CurObserver.mUpdateID = mTarget.mUpdateID;
                CurObserver.mSolver->Execute(true);
            }
        }
    }

    SolverValue::Range::Range()
    {
        mValue1 = 0;
        mValue2 = 0;
        mBeginMsec = 0;
        mEndMsec = 0;
    }

    SolverValue::Range::Range(Float value)
    {
        mValue1 = value;
        mValue2 = value;
        mBeginMsec = 0;
        mEndMsec = 0;
    }

    SolverValue::Range::Range(const Range& rhs)
    {
        operator=(rhs);
    }

    SolverValue::Range::Range(Range&& rhs)
    {
        operator=(ToReference(rhs));
    }

    SolverValue::Range::~Range()
    {
    }

    SolverValue::Range& SolverValue::Range::operator=(const Range& rhs)
    {
        mValue1 = rhs.mValue1;
        mValue2 = rhs.mValue2;
        mBeginMsec = rhs.mBeginMsec;
        mEndMsec = rhs.mEndMsec;
        return *this;
    }

    SolverValue::Range& SolverValue::Range::operator=(Range&& rhs)
    {
        mValue1 = ToReference(rhs.mValue1);
        mValue2 = ToReference(rhs.mValue2);
        mBeginMsec = ToReference(rhs.mBeginMsec);
        mEndMsec = ToReference(rhs.mEndMsec);
        return *this;
    }

    SolverValue::Float SolverValue::Range::GetValue() const
    {
        if(mBeginMsec == 0 && mEndMsec == 0)
            return mValue1 + Math::Random() * (mValue2 - mValue1);
        uint64 CurMsec = Platform::Utility::CurrentTimeMsec();
        if(CurMsec <= mBeginMsec) return mValue1;
        if(mEndMsec < CurMsec) return mValue2;
        return mValue1 + (mValue2 - mValue1) * (CurMsec - mBeginMsec) / (mEndMsec - mBeginMsec);
    }

    SolverValue::Text SolverValue::Range::GetCode() const
    {
        if(mBeginMsec == 0 && mEndMsec == 0)
            return Text::FromFloat(GetValue());
        return "$R:" + Text::FromFloat(mValue1) + '_' + Text::FromFloat(mValue2) + '_' +
            Text::FromInteger((sint64) mBeginMsec) + '_' + Text::FromInteger((sint64) mEndMsec) + '$';
    }

    SolverValue::SolverValue()
    {
        mType = SolverValueType::Null;
        mInteger = 0;
        mFloat = 0;
    }

    SolverValue::SolverValue(SolverValueType type)
    {
        mType = type;
        mInteger = 0;
        mFloat = 0;
    }

    SolverValue::SolverValue(const SolverValue& rhs)
    {
        operator=(rhs);
    }

    SolverValue::SolverValue(SolverValue&& rhs)
    {
        operator=(ToReference(rhs));
    }

    SolverValue::~SolverValue()
    {
    }

    SolverValue& SolverValue::operator=(const SolverValue& rhs)
    {
        mType = rhs.mType;
        mInteger = rhs.mInteger;
        mFloat = rhs.mFloat;
        mText = rhs.mText;
        mRange = rhs.mRange;
        return *this;
    }

    SolverValue& SolverValue::operator=(SolverValue&& rhs)
    {
        mType = ToReference(rhs.mType);
        mInteger = ToReference(rhs.mInteger);
        mFloat = ToReference(rhs.mFloat);
        mText = ToReference(rhs.mText);
        mRange = ToReference(rhs.mRange);
        return *this;
    }

    SolverValueType SolverValue::GetType() const
    {
        return mType;
    }

    SolverValue SolverValue::MakeInteger(Integer value)
    {
        SolverValue Result(SolverValueType::Integer);
        Result.mInteger = value;
        return Result;
    }

    SolverValue SolverValue::MakeIntegerByRound(Float value)
    {
        SolverValue Result(SolverValueType::Integer);
        Result.mInteger = SolverValue::Integer(value + 0.5);
        return Result;
    }

    SolverValue SolverValue::MakeFloat(Float value)
    {
        SolverValue Result(SolverValueType::Float);
        Result.mFloat = value;
        return Result;
    }

    SolverValue SolverValue::MakeText(Text value)
    {
        value.Replace("\\\\", "\\");
        value.Replace("\\\'", "\'");
        value.Replace("\\\"", "\"");
        SolverValue Result(SolverValueType::Text);
        Result.mText = value;
        return Result;
    }

    SolverValue SolverValue::MakeRange(Float value1, Float value2)
    {
        SolverValue Result(SolverValueType::Range);
        Result.mRange.mValue1 = value1;
        Result.mRange.mValue2 = value2;
        return Result;
    }

    SolverValue SolverValue::MakeRangeTime(Range value, Float sec)
    {
        SolverValue Result(SolverValueType::Range);
        Result.mRange.mValue1 = value.mValue1;
        Result.mRange.mValue2 = value.mValue2;
        Result.mRange.mBeginMsec = Platform::Utility::CurrentTimeMsec();
        Result.mRange.mEndMsec = Result.mRange.mBeginMsec + Math::Max(0, sec * 1000);
        return Result;
    }

    SolverValue SolverValue::MakeRangeTime(chars code)
    {
        SolverValue Result(SolverValueType::Range);
        if(*(code++) == '$' && *(code++) == 'R' && *(code++) == ':') // $R:0_0_0_0$
        {
            sint32 FindStep = 0;
            chars BeginPos = code;
            while(*code != '\0')
            {
                if(*code == '_' || *code == '$')
                {
                    switch(FindStep++)
                    {
                    case 0: Result.mRange.mValue1 = Parser::GetFloat<Float>(BeginPos, code - BeginPos); break;
                    case 1: Result.mRange.mValue2 = Parser::GetFloat<Float>(BeginPos, code - BeginPos); break;
                    case 2: Result.mRange.mBeginMsec = Parser::GetInt<uint64>(BeginPos, code - BeginPos); break;
                    case 3: Result.mRange.mEndMsec = Parser::GetInt<uint64>(BeginPos, code - BeginPos); break;
                    }
                    BeginPos = code + 1;
                }
                code++;
            }
        }
        return Result;
    }

    SolverValue::Integer SolverValue::ToInteger() const
    {
        switch(mType)
        {
        case SolverValueType::Range: return (Integer) mRange.GetValue();
        case SolverValueType::Integer: return mInteger;
        case SolverValueType::Float: return (Integer) mFloat;
        case SolverValueType::Text: return Parser::GetInt<Integer>(mText);
        }
        return 0;
    }

    SolverValue::Float SolverValue::ToFloat() const
    {
        switch(mType)
        {
        case SolverValueType::Range: return mRange.GetValue();
        case SolverValueType::Integer: return (Float) mInteger;
        case SolverValueType::Float: return mFloat;
        case SolverValueType::Text: return Parser::GetFloat<Float>(mText);
        }
        return 0.0f;
    }

    SolverValue::Text SolverValue::ToText(bool need_quotes, bool need_rangecode) const
    {
        switch(mType)
        {
        case SolverValueType::Range: return (need_rangecode)? mRange.GetCode() : Text::FromFloat(mRange.GetValue());
        case SolverValueType::Integer: return Text::FromInteger(mInteger);
        case SolverValueType::Float: return Text::FromFloat(mFloat);
        case SolverValueType::Text: return (need_quotes)? '\'' + mText + '\'' : mText;
        }
        return Text();
    }

    SolverValue::Range SolverValue::ToRange() const
    {
        switch(mType)
        {
        case SolverValueType::Range: return mRange;
        case SolverValueType::Integer: return Range((Float) mInteger);
        case SolverValueType::Float: return Range(mFloat);
        case SolverValueType::Text: return Range(Parser::GetFloat<Float>(mText));
        }
        return Range();
    }

    SolverValueType SolverValue::GetMergedType(const SolverValue& rhs) const
    {
        const SolverValueType MyType = (mType == SolverValueType::Range)? SolverValueType::Float : mType;
        const SolverValueType RhsType = (rhs.mType == SolverValueType::Range)? SolverValueType::Float : rhs.mType;
        return (SolverValueType) Math::Max((sint32) MyType, (sint32) RhsType);
    }

    SolverValue SolverValue::Addition(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeInteger(ToInteger() + rhs.ToInteger());
        case SolverValueType::Float: return MakeFloat(ToFloat() + rhs.ToFloat());
        case SolverValueType::Text: return MakeText(ToText() + rhs.ToText());
        }
        return SolverValue();
    }

    SolverValue SolverValue::Subtract(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeInteger(ToInteger() - rhs.ToInteger());
        case SolverValueType::Float: return MakeFloat(ToFloat() - rhs.ToFloat());
        case SolverValueType::Text: return MakeText(Text(ToText()).Replace(rhs.ToText(), ""));
        }
        return SolverValue();
    }

    SolverValue SolverValue::Multiply(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeInteger(ToInteger() * rhs.ToInteger());
        case SolverValueType::Float: return MakeFloat(ToFloat() * rhs.ToFloat());
        case SolverValueType::Text:
            {
                Text Dest;
                const Text Src = ToText();
                for(float i = rhs.ToFloat(); 0 < i; i -= 1.0f)
                    Dest += Src.Left(Src.Length() * Math::MinF(i, 1.0f));
                return MakeText(Dest);
            }
        }
        return SolverValue();
    }

    SolverValue SolverValue::Divide(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer:
            {
                auto Lhs = ToInteger();
                auto Rhs = rhs.ToInteger();
                if(Rhs <= Lhs && 0 < Rhs && Lhs % Rhs == 0)
                    return MakeInteger(Lhs / Rhs);
                return MakeFloat(ToFloat() / Math::MaxF(Math::FloatMin(), rhs.ToFloat()));
            }
        case SolverValueType::Float: return MakeFloat(ToFloat() / Math::MaxF(Math::FloatMin(), rhs.ToFloat()));
        case SolverValueType::Text: return MakeFloat(ToText().Length() / Math::MaxF(Math::FloatMin(), rhs.ToFloat()));
        }
        return SolverValue();
    }

    SolverValue SolverValue::Remainder(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer:
            if(auto Rhs = rhs.ToInteger())
                return MakeInteger(ToInteger() % Rhs);
            else return MakeText("Remainder_Error");
        case SolverValueType::Float: return MakeFloat(Math::Mod(ToFloat(), Math::MaxF(Math::FloatMin(), rhs.ToFloat())));
        case SolverValueType::Text: return MakeFloat(Math::Mod(ToText().Length(), Math::MaxF(Math::FloatMin(), rhs.ToFloat())));
        }
        return SolverValue();
    }

    SolverValue SolverValue::BitAnd(const SolverValue& rhs) const
    {
        return MakeInteger(ToInteger() & rhs.ToInteger());
    }

    SolverValue SolverValue::BitOr(const SolverValue& rhs) const
    {
        return MakeInteger(ToInteger() | rhs.ToInteger());
    }

    SolverValue SolverValue::BitXor(const SolverValue& rhs) const
    {
        return MakeInteger(ToInteger() ^ rhs.ToInteger());
    }

    SolverValue SolverValue::BitShiftL(const SolverValue& rhs) const
    {
        return MakeInteger(ToInteger() << rhs.ToInteger());
    }

    SolverValue SolverValue::BitShiftR(const SolverValue& rhs) const
    {
        return MakeInteger(ToInteger() >> rhs.ToInteger());
    }

    SolverValue SolverValue::Variabler(const SolverValue& rhs, const SolverChain* chain) const
    {
        if(auto CurSolver = SolverVariable::FindTarget(chain, rhs.ToText()))
            return CurSolver->result();
        return *this;
    }

    SolverValue SolverValue::RangeTarget(const SolverValue& rhs) const
    {
        return MakeRange(ToFloat(), rhs.ToFloat());
    }

    SolverValue SolverValue::RangeTimer(const SolverValue& rhs) const
    {
        return MakeRangeTime(ToRange(), rhs.ToFloat());
    }

    SolverValue SolverValue::Greater(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeInteger(ToInteger() < rhs.ToInteger());
        case SolverValueType::Float: return MakeInteger(ToFloat() < rhs.ToFloat());
        case SolverValueType::Text: return MakeInteger(ToText().Compare(rhs.ToText()) < 0);
        }
        return SolverValue();
    }

    SolverValue SolverValue::GreaterOrEqual(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeInteger(ToInteger() <= rhs.ToInteger());
        case SolverValueType::Float: return MakeInteger(ToFloat() <= rhs.ToFloat());
        case SolverValueType::Text: return MakeInteger(ToText().Compare(rhs.ToText()) <= 0);
        }
        return SolverValue();
    }

    SolverValue SolverValue::Less(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeInteger(ToInteger() > rhs.ToInteger());
        case SolverValueType::Float: return MakeInteger(ToFloat() > rhs.ToFloat());
        case SolverValueType::Text: return MakeInteger(ToText().Compare(rhs.ToText()) > 0);
        }
        return SolverValue();
    }

    SolverValue SolverValue::LessOrEqual(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeInteger(ToInteger() >= rhs.ToInteger());
        case SolverValueType::Float: return MakeInteger(ToFloat() >= rhs.ToFloat());
        case SolverValueType::Text: return MakeInteger(ToText().Compare(rhs.ToText()) >= 0);
        }
        return SolverValue();
    }

    SolverValue SolverValue::Equal(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeInteger(ToInteger() == rhs.ToInteger());
        case SolverValueType::Float: return MakeInteger(ToFloat() == rhs.ToFloat());
        case SolverValueType::Text: return MakeInteger(ToText().Compare(rhs.ToText()) == 0);
        }
        return SolverValue();
    }

    SolverValue SolverValue::Different(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeInteger(ToInteger() != rhs.ToInteger());
        case SolverValueType::Float: return MakeInteger(ToFloat() != rhs.ToFloat());
        case SolverValueType::Text: return MakeInteger(ToText().Compare(rhs.ToText()) != 0);
        }
        return SolverValue();
    }

    SolverValue SolverValue::Function_Min(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeInteger(Math::Min(ToInteger(), rhs.ToInteger()));
        case SolverValueType::Float: return MakeFloat(Math::MinF(ToFloat(), rhs.ToFloat()));
        case SolverValueType::Text: return MakeText((ToText().Compare(rhs.ToText()) < 0)? ToText() : rhs.ToText());
        }
        return SolverValue();
    }

    SolverValue SolverValue::Function_Max(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeInteger(Math::Max(ToInteger(), rhs.ToInteger()));
        case SolverValueType::Float: return MakeFloat(Math::MaxF(ToFloat(), rhs.ToFloat()));
        case SolverValueType::Text: return MakeText((ToText().Compare(rhs.ToText()) > 0)? ToText() : rhs.ToText());
        }
        return SolverValue();
    }

    SolverValue SolverValue::Function_Abs(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeInteger(ToInteger() + Math::Abs(rhs.ToInteger()));
        case SolverValueType::Float: return MakeFloat(ToFloat() + Math::AbsF(rhs.ToFloat()));
        case SolverValueType::Text: return MakeText("Abs_Error");
        }
        return SolverValue();
    }

    SolverValue SolverValue::Function_Pow(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeIntegerByRound(Math::Pow(ToInteger(), rhs.ToInteger()));
        case SolverValueType::Float: return MakeFloat(Math::Pow(ToFloat(), rhs.ToFloat()));
        case SolverValueType::Text: return MakeText("Pow_Error");
        }
        return SolverValue();
    }

    SolverValue SolverValue::Function_Cos(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeIntegerByRound(ToInteger() * Math::Cos(Math::ToRadian(rhs.ToInteger())));
        case SolverValueType::Float: return MakeFloat(ToFloat() * Math::Cos(Math::ToRadian(rhs.ToFloat())));
        case SolverValueType::Text: return MakeText("Cos_Error");
        }
        return SolverValue();
    }

    SolverValue SolverValue::Function_Sin(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeIntegerByRound(ToInteger() * Math::Sin(Math::ToRadian(rhs.ToInteger())));
        case SolverValueType::Float: return MakeFloat(ToFloat() * Math::Sin(Math::ToRadian(rhs.ToFloat())));
        case SolverValueType::Text: return MakeText("Sin_Error");
        }
        return SolverValue();
    }

    SolverValue SolverValue::Function_Tan(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeIntegerByRound(ToInteger() * Math::Tan(Math::ToRadian(rhs.ToInteger())));
        case SolverValueType::Float: return MakeFloat(ToFloat() * Math::Tan(Math::ToRadian(rhs.ToFloat())));
        case SolverValueType::Text: return MakeText("Tan_Error");
        }
        return SolverValue();
    }

    SolverValue SolverValue::Function_Atan(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeIntegerByRound(Math::Atan(rhs.ToInteger(), ToInteger()));
        case SolverValueType::Float: return MakeFloat(Math::Atan(rhs.ToFloat(), ToFloat()));
        case SolverValueType::Text: return MakeText("Atan_Error");
        }
        return SolverValue();
    }

    SolverValue SolverValue::Function_And(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeInteger(ToInteger() != 0 && rhs.ToInteger() != 0);
        case SolverValueType::Float: return MakeInteger(ToFloat() != 0 && rhs.ToFloat() != 0);
        case SolverValueType::Text: return MakeInteger(0 < ToText().Length() && 0 < rhs.ToText().Length());
        }
        return SolverValue();
    }

    SolverValue SolverValue::Function_Or(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeInteger(ToInteger() != 0 || rhs.ToInteger() != 0);
        case SolverValueType::Float: return MakeInteger(ToFloat() != 0 || rhs.ToFloat() != 0);
        case SolverValueType::Text: return MakeInteger(0 < ToText().Length() || 0 < rhs.ToText().Length());
        }
        return SolverValue();
    }

    SolverValue SolverValue::Function_Multiply(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeInteger(ToInteger() * rhs.ToInteger());
        case SolverValueType::Float:
        case SolverValueType::Text: return MakeIntegerByRound(ToFloat() * rhs.ToFloat());
        }
        return SolverValue();
    }

    SolverValue SolverValue::Function_Divide(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer:
            if(auto Rhs = rhs.ToInteger())
                return MakeInteger(ToInteger() / Rhs);
            else return MakeText("Divide_Error");
        case SolverValueType::Float:
        case SolverValueType::Text: return MakeIntegerByRound(ToFloat() / Math::MaxF(Math::FloatMin(), rhs.ToFloat()));
        }
        return SolverValue();
    }

    SolverValue SolverValue::Function_Find(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer:
        case SolverValueType::Float:
        case SolverValueType::Text: return MakeInteger(ToText().Find(0, rhs.ToText()));
        }
        return SolverValue();
    }

    SolverValue SolverValue::Function_Truncate(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer:
        case SolverValueType::Float:
        case SolverValueType::Text:
            if(auto Rhs = rhs.ToInteger())
            {
                const bool ZeroTrim = (Rhs < 0)? false : true;
                const sint32 DotPos = Math::Log10(Math::Abs(Rhs));
                const sint32 Value = sint32(ToFloat() + 0.5);
                const bool Minus = (Value < 0);
                String ValueText = String::FromInteger((Minus)? -Value : Value); // 양수화
                while(ValueText.Length() < DotPos + 1) ValueText = '0' + ValueText; // ValueText가 '12'이고 DotPos가 3이면 ValueText가 '0012'가 되어야 함
                String Text = ValueText.Left(ValueText.Length() - DotPos) + '.' + ValueText.Right(DotPos);
                if(ZeroTrim)
                {
                    while(Text[-2] == '0') Text.SubTail(1); // ValueText가 '1230'이고 DotPos가 2이면 '12.3'이 되어야 함
                    if(Text[-2] == '.') Text.SubTail(1); // ValueText가 '1200'이고 DotPos가 2이면 '12'가 되어야 함
                }
                return (Minus)? MakeText('-' + Text) : MakeText(Text); // 부호화
            }
            else return MakeText("Truncate_Error");
        }
        return SolverValue();
    }

    int __SolverCreateCount = 0;
    int __SolverReleaseCount = 0;
    int __SolverParseCount = 0;
    int __SolverExecuteCount = 0;
    int __SolverParseNsec = 0;
    int __SolverExecuteNsec = 0;

    Solver::Solver()
    {
        mLinkedChain = nullptr;
        mReliable = 0;
        mResult = SolverValue::MakeInteger(0);
        mUpdatedFormulaMsec = 0;
        mUpdatedResultMsec = 0;
        //////////
        __SolverCreateCount++;
    }

    Solver::~Solver()
    {
        Unlink();
        //////////
        __SolverReleaseCount++;
    }

    Solver& Solver::operator=(const Solver& rhs)
    {
        BOSS_ASSERT("Solver는 복사할 수 없습니다", false);
        return *this;
    }

    Solver& Solver::operator=(Solver&& rhs)
    {
        Unlink();
        mLinkedChain = rhs.mLinkedChain; rhs.mLinkedChain = nullptr;
        mLinkedVariable = ToReference(rhs.mLinkedVariable);
        mParsedFormula = ToReference(rhs.mParsedFormula);
        mOperandTop = ToReference(rhs.mOperandTop);
        mReliable = rhs.mReliable; rhs.mReliable = 0;
        mResult = ToReference(rhs.mResult);
        mResultCB = rhs.mResultCB; rhs.mResultCB = nullptr;
        mUpdatedFormulaMsec = rhs.mUpdatedFormulaMsec; rhs.mUpdatedFormulaMsec = 0;
        mUpdatedResultMsec = rhs.mUpdatedResultMsec; rhs.mUpdatedResultMsec = 0;

        if(mLinkedChain)
        {
            if(0 < mLinkedVariable.Length())
                (*mLinkedChain)(mLinkedVariable).ForcedChangeTarget(&rhs, this);
            // 하위의 변수들에게 새로운 솔버정보(this)를 갱신
            mOperandTop->UpdateChain(this, mLinkedChain);
        }
        return *this;
    }

    Solver& Solver::Link(chars chain, chars variable, bool updateobservers)
    {
        Unlink();
        mLinkedChain = &gSolverChains(chain);
        if(variable && *variable)
        {
            mLinkedVariable = variable;
            (*mLinkedChain)(mLinkedVariable).ResetTarget(this, updateobservers);
        }
        // 하위의 변수들에게 새로운 체인정보 전달
        mOperandTop->UpdateChain(this, mLinkedChain);
        return *this;
    }

    void Solver::Unlink(bool updateobservers)
    {
        if(mLinkedChain)
        {
            if(0 < mLinkedVariable.Length())
            {
                if((*mLinkedChain)(mLinkedVariable).RemoveTarget(updateobservers))
                    mLinkedChain->Remove(mLinkedVariable);
                mLinkedVariable.Empty();
            }
            mLinkedChain = nullptr;
        }
    }

    Solver* Solver::Find(chars chain, chars variable)
    {
        if(auto FindedChain = &gSolverChains(chain))
        if(auto FindedChainPair = FindedChain->Access(variable))
            return FindedChainPair->target();
        return nullptr;
    }

    void Solver::FindVariables(chars chain, SolverVariableCB cb)
    {
        if(auto FindedChain = &gSolverChains(chain))
        {
            FindedChain->AccessByCallback(
                [](const MapPath* path, SolverChainPair* data, payload param)->void
                {
                    SolverVariableCB& OneCB = *((SolverVariableCB*) param);
                    const String Variable(ToReference(path->GetPath()));
                    OneCB(Variable, data);
                }, (payload) &cb);
        }
    }

    void Solver::Remove(chars chain, chars variable)
    {
        if(auto FindedChain = &gSolverChains(chain))
            FindedChain->Remove(variable);
    }

    void Solver::RemoveMatchedVariables(chars chain, chars keyword, SolverVariableCB cb)
    {
        BOSS_ASSERT("잘못된 시나리오입니다", keyword);
        if(auto FindedChain = &gSolverChains(chain))
        {
            struct Payload
            {
                const String mKeyword;
                Strings mMatchedVariables;
            };
            Payload OnePayload {keyword};

            // 삭제대상의 선별(AccessByCallback중 삭제불가)
            FindedChain->AccessByCallback(
                [](const MapPath* path, SolverChainPair* data, payload param)->void
                {
                    Payload& OnePayload = *((Payload*) param);
                    const String Variable(ToReference(path->GetPath()));
                    if(0 <= Variable.Find(0, OnePayload.mKeyword))
                        OnePayload.mMatchedVariables.AtAdding() = Variable;
                }, (payload) &OnePayload);

            // 삭제실행
            if(cb)
            {
                for(sint32 i = 0, iend = OnePayload.mMatchedVariables.Count(); i < iend; ++i)
                {
                    auto& CurVariable = OnePayload.mMatchedVariables[i];
                    if(auto CurChainPair = FindedChain->Access(CurVariable))
                    {
                        cb(CurVariable, CurChainPair);
                        FindedChain->Remove(CurVariable);
                    }
                }
            }
            else for(sint32 i = 0, iend = OnePayload.mMatchedVariables.Count(); i < iend; ++i)
            {
                auto& CurVariable = OnePayload.mMatchedVariables[i];
                FindedChain->Remove(CurVariable);
            }
        }
    }

    void Solver::SetReplacer(chars before, chars after)
    {
        gReplacerBefore = before;
        gReplacerAfter = after;
    }

    void Solver::ClearReplacer()
    {
        gReplacerBefore.Empty();
        gReplacerAfter.Empty();
    }

    Solver& Solver::Parse(chars formula)
    {
        //////////
        __SolverParseCount++;
        auto __Begin = Platform::Clock::CreateAsCurrent();

        auto AddOperator = [](SolverOperandObject*& focus, SolverOperatorType type, sint32 deep)->void
        {
            BOSS_ASSERT("잘못된 시나리오입니다", focus);
            const sint32 PriorityCount = 9;
            sint32 NewPriority = deep * PriorityCount;
            switch(type)
            {
            case SolverOperatorType::Multiply: case SolverOperatorType::Divide: case SolverOperatorType::Remainder:
            case SolverOperatorType::Function_Multiply: case SolverOperatorType::Function_Divide: // 1순위> *, /, %, [multiply], [divide]
                NewPriority += PriorityCount - 1;
                break;
            case SolverOperatorType::Addition: case SolverOperatorType::Subtract: // 2순위> +, -
                NewPriority += PriorityCount - 2;
                break;
            case SolverOperatorType::BitAnd: case SolverOperatorType::BitOr: case SolverOperatorType::BitXor:
            case SolverOperatorType::BitShiftL: case SolverOperatorType::BitShiftR:
            case SolverOperatorType::RangeTarget: // 3순위> &, |, <<, >>, ~
                NewPriority += PriorityCount - 3;
                break;
            case SolverOperatorType::RangeTimer: // 4순위> :
                NewPriority += PriorityCount - 4;
                break;
            case SolverOperatorType::Function_Min: case SolverOperatorType::Function_Max: // 5순위> [min], [max], [abs], [pow], [cos], [sin], [tan], [atan], [find], [truncate]
            case SolverOperatorType::Function_Abs: case SolverOperatorType::Function_Pow:
            case SolverOperatorType::Function_Cos: case SolverOperatorType::Function_Sin:
            case SolverOperatorType::Function_Tan: case SolverOperatorType::Function_Atan:
            case SolverOperatorType::Function_Find: case SolverOperatorType::Function_Truncate:
                NewPriority += PriorityCount - 5;
                break;
            case SolverOperatorType::Variabler: // 6순위> @
                NewPriority += PriorityCount - 6;
                break;
            case SolverOperatorType::Greater: case SolverOperatorType::GreaterOrEqual: case SolverOperatorType::Less: // 7순위> <, <=, >, >=, ==, !=
            case SolverOperatorType::LessOrEqual: case SolverOperatorType::Equal: case SolverOperatorType::Different:
                NewPriority += PriorityCount - 7;
                break;
            case SolverOperatorType::Function_And: case SolverOperatorType::Function_Or: // 8순위> [and], [or]
                NewPriority += PriorityCount - 8;
                break;
            case SolverOperatorType::Commenter: // 9순위> ?
                NewPriority += PriorityCount - 9;
                break;
            }

            SolverFormula NewFormula(type, NewPriority);
            if(focus->ConstPtr()->type() != SolverOperandType::Formula) // 최초의 연산항이거나
            {
                NewFormula.mOperandL = *focus;
                *focus = NewFormula.clone();
            }
            else
            {
                auto CurFormula = (SolverFormula*) focus->Ptr(); // 대상지 추적
                while(CurFormula->mOperandP && NewPriority <= ((const SolverFormula*) CurFormula->mOperandP->ConstPtr())->mOperatorPriority)
                    CurFormula = (SolverFormula*) (focus = CurFormula->mOperandP)->Ptr();

                if(CurFormula->mOperatorPriority < NewPriority) // 이전 연산기호보다 더 우선시되면
                {
                    NewFormula.mOperandL = CurFormula->mOperandR;
                    NewFormula.mOperandP = focus;
                    focus = &(CurFormula->mOperandR = NewFormula.clone());
                }
                else // 이전 연산기호와 같거나 차선시되면
                {
                    NewFormula.mOperandL = *focus;
                    NewFormula.mOperandP = CurFormula->mOperandP;
                    *focus = NewFormula.clone();
                }
            }
        };

        // 탑항을 초기화
        mOperandTop = SolverOperandObject();
        SolverOperandObject* OperandFocus = nullptr;
        bool OperatorTurn = false;
        for(sint32 deep = 0; *formula; ++formula)
        {
            // 공백과 괄호
            branch;
            jump(*formula == ' ');
            jump(*formula == '\t');
            jump(*formula == '\r');
            jump(*formula == '\n');
            jump(*formula == '(') deep++;
            jump(*formula == ')') deep--;
            jump(OperatorTurn) // 연산기호턴
            {
                // 연산항 밀어넣기
                branch;
                jump(*formula == '+') AddOperator(OperandFocus, SolverOperatorType::Addition, deep);
                jump(*formula == '-') AddOperator(OperandFocus, SolverOperatorType::Subtract, deep);
                jump(*formula == '*') AddOperator(OperandFocus, SolverOperatorType::Multiply, deep);
                jump(*formula == '/') AddOperator(OperandFocus, SolverOperatorType::Divide, deep);
                jump(*formula == '%') AddOperator(OperandFocus, SolverOperatorType::Remainder, deep);
                jump(*formula == '&') AddOperator(OperandFocus, SolverOperatorType::BitAnd, deep);
                jump(*formula == '|') AddOperator(OperandFocus, SolverOperatorType::BitOr, deep);
                jump(*formula == '^') AddOperator(OperandFocus, SolverOperatorType::BitXor, deep);
                jump(*formula == '@') AddOperator(OperandFocus, SolverOperatorType::Variabler, deep);
                jump(*formula == '?') AddOperator(OperandFocus, SolverOperatorType::Commenter, deep);
                jump(*formula == '~') AddOperator(OperandFocus, SolverOperatorType::RangeTarget, deep);
                jump(*formula == ':') AddOperator(OperandFocus, SolverOperatorType::RangeTimer, deep);
                jump(*formula == '<')
                {
                    if(formula[1] == '<')
                    {
                        AddOperator(OperandFocus, SolverOperatorType::BitShiftL, deep);
                        formula += 1;
                    }
                    else if(formula[1] == '=')
                    {
                        AddOperator(OperandFocus, SolverOperatorType::GreaterOrEqual, deep);
                        formula += 1;
                    }
                    else AddOperator(OperandFocus, SolverOperatorType::Greater, deep);
                }
                jump(*formula == '>')
                {
                    if(formula[1] == '>')
                    {
                        AddOperator(OperandFocus, SolverOperatorType::BitShiftR, deep);
                        formula += 1;
                    }
                    else if(formula[1] == '=')
                    {
                        AddOperator(OperandFocus, SolverOperatorType::LessOrEqual, deep);
                        formula += 1;
                    }
                    else AddOperator(OperandFocus, SolverOperatorType::Less, deep);
                }
                jump(formula[0] == '=' && formula[1] == '=')
                {
                    AddOperator(OperandFocus, SolverOperatorType::Equal, deep);
                    formula += 1;
                }
                jump(formula[0] == '!' && formula[1] == '=')
                {
                    AddOperator(OperandFocus, SolverOperatorType::Different, deep);
                    formula += 1;
                }
                jump(*formula == '[')
                {
                    branch;
                    jump(!String::Compare("[min]", formula, 5))
                    {
                        AddOperator(OperandFocus, SolverOperatorType::Function_Min, deep);
                        formula += 5 - 1;
                    }
                    jump(!String::Compare("[max]", formula, 5))
                    {
                        AddOperator(OperandFocus, SolverOperatorType::Function_Max, deep);
                        formula += 5 - 1;
                    }
                    jump(!String::Compare("[abs]", formula, 5))
                    {
                        AddOperator(OperandFocus, SolverOperatorType::Function_Abs, deep);
                        formula += 5 - 1;
                    }
                    jump(!String::Compare("[pow]", formula, 5))
                    {
                        AddOperator(OperandFocus, SolverOperatorType::Function_Pow, deep);
                        formula += 5 - 1;
                    }
                    jump(!String::Compare("[cos]", formula, 5))
                    {
                        AddOperator(OperandFocus, SolverOperatorType::Function_Cos, deep);
                        formula += 5 - 1;
                    }
                    jump(!String::Compare("[sin]", formula, 5))
                    {
                        AddOperator(OperandFocus, SolverOperatorType::Function_Sin, deep);
                        formula += 5 - 1;
                    }
                    jump(!String::Compare("[tan]", formula, 5))
                    {
                        AddOperator(OperandFocus, SolverOperatorType::Function_Tan, deep);
                        formula += 5 - 1;
                    }
                    jump(!String::Compare("[atan]", formula, 6))
                    {
                        AddOperator(OperandFocus, SolverOperatorType::Function_Atan, deep);
                        formula += 6 - 1;
                    }
                    jump(!String::Compare("[and]", formula, 5))
                    {
                        AddOperator(OperandFocus, SolverOperatorType::Function_And, deep);
                        formula += 5 - 1;
                    }
                    jump(!String::Compare("[or]", formula, 4))
                    {
                        AddOperator(OperandFocus, SolverOperatorType::Function_Or, deep);
                        formula += 4 - 1;
                    }
                    jump(!String::Compare("[multiply]", formula, 10))
                    {
                        AddOperator(OperandFocus, SolverOperatorType::Function_Multiply, deep);
                        formula += 10 - 1;
                    }
                    jump(!String::Compare("[divide]", formula, 8))
                    {
                        AddOperator(OperandFocus, SolverOperatorType::Function_Divide, deep);
                        formula += 8 - 1;
                    }
                    jump(!String::Compare("[find]", formula, 6))
                    {
                        AddOperator(OperandFocus, SolverOperatorType::Function_Find, deep);
                        formula += 6 - 1;
                    }
                    jump(!String::Compare("[truncate]", formula, 10))
                    {
                        AddOperator(OperandFocus, SolverOperatorType::Function_Truncate, deep);
                        formula += 10 - 1;
                    }
                    else
                    {
                        BOSS_ASSERT(String::Format("알 수 없는 함수기호입니다([%c%c...)", formula[1], formula[2]), false);
                        continue;
                    }
                }
                else
                {
                    BOSS_ASSERT(String::Format("알 수 없는 연산기호입니다(%c)", *formula), false);
                    continue;
                }

                // 주석처리
                if(*formula == '?')
                {
                    chars End = formula;
                    while(*(++End));
                    ((SolverFormula*) OperandFocus->Ptr())->mOperandR =
                        SolverComment(String(formula + 1, End - (formula + 1))).clone();
                    formula = End - 1;
                }
                else OperatorTurn = false;
            }
            else // 피연산항턴
            {
                buffer NewOperand = nullptr;
                // 상수
                branch;
                jump(('0' <= *formula && *formula <= '9') || *formula == '+' || *formula == '-')
                {
                    sint32 IntegerSize = 0, FloatSize = 0, HexSize = 0;
                    auto IntegerValue = Parser::GetInt<SolverValue::Integer>(formula, -1, &IntegerSize);
                    auto FloatValue = Parser::GetFloat<SolverValue::Float>(formula, -1, &FloatSize);
                    auto HexValue = Parser::GetHex32<uint64>(formula, -1, &HexSize);
                    if(IntegerSize < HexSize && FloatSize < HexSize)
                    {
                        NewOperand = SolverLiteral(SolverValue::MakeInteger(HexValue)).clone();
                        formula += HexSize - 1;
                    }
                    else if(IntegerSize < FloatSize)
                    {
                        NewOperand = SolverLiteral(SolverValue::MakeFloat(FloatValue)).clone();
                        formula += FloatSize - 1;
                    }
                    else
                    {
                        NewOperand = SolverLiteral(SolverValue::MakeInteger(IntegerValue)).clone();
                        formula += IntegerSize - 1;
                    }
                }
                jump(*formula == '\'' || *formula == '\"')
                {
                    chars End = formula;
                    while(*(++End))
                    {
                        if(End[0] == '\\' && End[1] != '\0') End++;
                        else if(*End == *formula) break;
                    }
                    NewOperand = SolverLiteral(SolverValue::MakeText(String(formula + 1, End - formula - 1))).clone();
                    formula += (End - formula - 1 + 2) - 1;
                }
                jump(formula[0] == '$' && formula[1] == 'R' && formula[2] == ':')
                {
                    chars End = formula;
                    while(*(++End)) if(*End == *formula) break;
                    NewOperand = SolverLiteral(SolverValue::MakeRangeTime(String(formula, End - formula + 1))).clone();
                    formula += (End - formula - 1 + 2) - 1;
                }
                // 변수
                else
                {
                    chars End = formula - 1;
                    while(*(++End))
                    {
                        switch(*End)
                        {
                        case ' ': case '\t': case '\r': case '\n':
                        case '+': case '-': case '*': case '/': case '%': case '@': case '?': case '~': case ':':
                        case '!': case '=': case '<': case '>': case '[': case ']': case '(': case ')': break;
                        default: continue;
                        }
                        break;
                    }
                    if(0 < End - formula)
                        NewOperand = SolverVariable(String(formula, End - formula)).clone();
                    else NewOperand = SolverLiteral().clone();
                    formula += (End - formula) - 1;
                }

                // 피연산항 밀어넣기
                if(OperandFocus)
                {
                    auto CurFormula = (SolverFormula*) OperandFocus->Ptr();
                    BOSS_ASSERT("잘못된 시나리오입니다", CurFormula->type() == SolverOperandType::Formula);
                    CurFormula->mOperandR = NewOperand;
                }
                else OperandFocus = &(mOperandTop = NewOperand);
                OperatorTurn = true;
            }
        }

        // 파싱결과를 기록
        const String OldFormula = mParsedFormula;
        mParsedFormula.Empty();
        mOperandTop->PrintFormula(mParsedFormula);
        if(OldFormula != mParsedFormula)
            mUpdatedFormulaMsec = Platform::Utility::CurrentTimeMsec();

        // 링크가 된 경우 새로 생긴 하위의 변수들에게 체인정보 전달
        if(mLinkedChain)
            mOperandTop->UpdateChain(this, mLinkedChain);

        //////////
        auto __End = Platform::Clock::CreateAsCurrent();
        __SolverParseNsec += Platform::Clock::GetPeriodNsec(__Begin, __End);
        Platform::Clock::Release(__Begin);
        Platform::Clock::Release(__End);
        return *this;
    }

    void Solver::Execute(bool updateobservers)
    {
        //////////
        __SolverExecuteCount++;
        auto __Begin = Platform::Clock::CreateAsCurrent();

        const float OldReliable = mReliable;
        const SolverValue OldResult = ToReference(mResult);
        const float NewReliable = mOperandTop->reliable();
        const SolverValue& NewResult = mOperandTop->result(SolverValue::MakeInteger(0));

        // 필터확인후 입력
        if(mResultCB == nullptr || mResultCB(mParsedFormula, NewResult, NewReliable))
        {
            mReliable = NewReliable;
            mResult = NewResult;
            if(OldReliable != mReliable || OldResult.Different(mResult).ToInteger() != 0)
            {
                mUpdatedResultMsec = Platform::Utility::CurrentTimeMsec();
                if(mLinkedChain && 0 < mLinkedVariable.Length())
                    (*mLinkedChain)(mLinkedVariable).ResetTarget(this, updateobservers);
            }
        }

        //////////
        auto __End = Platform::Clock::CreateAsCurrent();
        __SolverExecuteNsec += Platform::Clock::GetPeriodNsec(__Begin, __End);
        Platform::Clock::Release(__Begin);
        Platform::Clock::Release(__End);
    }

    SolverValue Solver::ExecuteOnly() const
    {
        //////////
        __SolverExecuteCount++;
        auto __Begin = Platform::Clock::CreateAsCurrent();

        SolverValue Result = mOperandTop->result(SolverValue::MakeInteger(0));

        //////////
        auto __End = Platform::Clock::CreateAsCurrent();
        __SolverExecuteNsec += Platform::Clock::GetPeriodNsec(__Begin, __End);
        Platform::Clock::Release(__Begin);
        Platform::Clock::Release(__End);
        return Result;
    }

    void Solver::SetResultDirectly(const SolverValue& value)
    {
        mParsedFormula.Empty();
        mOperandTop = SolverLiteral(value).clone();
        mReliable = 1;
        mResult = value;
    }

    String Solver::ExecuteVariableName() const
    {
        String Result;
        switch(mOperandTop->type())
        {
        case SolverOperandType::Literal:
            {
                //////////
                __SolverExecuteCount++;
                auto __Begin = Platform::Clock::CreateAsCurrent();

                Result = mOperandTop->result(SolverValue()).ToText();

                //////////
                auto __End = Platform::Clock::CreateAsCurrent();
                __SolverExecuteNsec += Platform::Clock::GetPeriodNsec(__Begin, __End);
                Platform::Clock::Release(__Begin);
                Platform::Clock::Release(__End);
            }
            break;
        case SolverOperandType::Variable:
            {
                Strings GetName;
                mOperandTop->PrintVariables(GetName, false);
                Result = GetName[0];
            }
            break;
        case SolverOperandType::Formula:
            Result = ExecuteOnly().ToText();
            break;
        }
        return Result;
    }

    Strings Solver::GetTargetlessVariables() const
    {
        Strings Results;
        mOperandTop->PrintVariables(Results, true);
        return Results;
    }
}
