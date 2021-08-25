#include <boss.hpp>
#include "boss_solver.hpp"

#include <platform/boss_platform.hpp> 

namespace BOSS
{
    static Map<SolverChain> gSolverChains;

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
                    if(auto CurChainPair = CurChain->Access(&name[i + 1]))
                        return CurChainPair->target();
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
                return SolverValue::MakeByText(mName); // 해당 Solver를 찾지 못하면 텍스트타입
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
        private: Solver* mSolver; // 자기계산식
        private: SolverChain* mChain; // 검색체인
    };

    // 계산항
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

            PrintOperand(mOperandL, collector);
            switch(mOperatorType)
            {
            case SolverOperatorType::Addition:       collector += " + "; break;
            case SolverOperatorType::Subtract:       collector += " - "; break;
            case SolverOperatorType::Multiply:       collector += " * "; break;
            case SolverOperatorType::Divide:         collector += " / "; break;
            case SolverOperatorType::Remainder:      collector += " % "; break;
            case SolverOperatorType::Variabler:      collector += " @ "; break;
            case SolverOperatorType::RangeTarget:    collector += " ~ "; break;
            case SolverOperatorType::RangeTimer:     collector += " : "; break;
            case SolverOperatorType::Greater:        collector += " < "; break;
            case SolverOperatorType::GreaterOrEqual: collector += " <= "; break;
            case SolverOperatorType::Less:           collector += " > "; break;
            case SolverOperatorType::LessOrEqual:    collector += " >= "; break;
            case SolverOperatorType::Equal:          collector += " == "; break;
            case SolverOperatorType::Different:      collector += " != "; break;
            case SolverOperatorType::Function_Min:   collector += " [min] "; break;
            case SolverOperatorType::Function_Max:   collector += " [max] "; break;
            case SolverOperatorType::Function_Abs:   collector += " [abs] "; break;
            case SolverOperatorType::Function_Pow:   collector += " [pow] "; break;
            }
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
            const SolverValue& Zero = SolverValue::MakeByInteger(0);
            const SolverValue& One = SolverValue::MakeByInteger(1);
            if(0 < reliable())
            switch(mOperatorType)
            {
            case SolverOperatorType::Addition:       return mOperandL->result(Zero).Addition(mOperandR->result(Zero));
            case SolverOperatorType::Subtract:       return mOperandL->result(Zero).Subtract(mOperandR->result(Zero));
            case SolverOperatorType::Multiply:       return mOperandL->result(Zero).Multiply(mOperandR->result(One));
            case SolverOperatorType::Divide:         return mOperandL->result(Zero).Divide(mOperandR->result(One));
            case SolverOperatorType::Remainder:      return mOperandL->result(Zero).Remainder(mOperandR->result(One));
            case SolverOperatorType::Variabler:      return mOperandL->result(Zero).Variabler(mOperandR->result(One), mChain);
            case SolverOperatorType::RangeTarget:    return mOperandL->result(Zero).RangeTarget(mOperandR->result(Zero));
            case SolverOperatorType::RangeTimer:     return mOperandL->result(Zero).RangeTimer(mOperandR->result(Zero));
            case SolverOperatorType::Greater:        return mOperandL->result(Zero).Greater(mOperandR->result(Zero));
            case SolverOperatorType::GreaterOrEqual: return mOperandL->result(Zero).GreaterOrEqual(mOperandR->result(Zero));
            case SolverOperatorType::Less:           return mOperandL->result(Zero).Less(mOperandR->result(Zero));
            case SolverOperatorType::LessOrEqual:    return mOperandL->result(Zero).LessOrEqual(mOperandR->result(Zero));
            case SolverOperatorType::Equal:          return mOperandL->result(Zero).Equal(mOperandR->result(Zero));
            case SolverOperatorType::Different:      return mOperandL->result(Zero).Different(mOperandR->result(Zero));
            case SolverOperatorType::Function_Min:   return mOperandL->result(Zero).Function_Min(mOperandR->result(One));
            case SolverOperatorType::Function_Max:   return mOperandL->result(Zero).Function_Max(mOperandR->result(One));
            case SolverOperatorType::Function_Abs:   return mOperandL->result(Zero).Function_Abs(mOperandR->result(One));
            case SolverOperatorType::Function_Pow:   return mOperandL->result(Zero).Function_Pow(mOperandR->result(One));
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
            if(!CurObserver.mUpdateID != mTarget.mUpdateID)
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
            return String::FromFloat(GetValue());
        return "@R" + String::FromFloat(mValue1) + '_' + String::FromFloat(mValue2) + '_' +
            String::FromInteger((sint64) mBeginMsec) + '_' + String::FromInteger((sint64) mEndMsec) + '@';
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

    SolverValue SolverValue::MakeByInteger(Integer value)
    {
        SolverValue Result(SolverValueType::Integer);
        Result.mInteger = value;
        return Result;
    }

    SolverValue SolverValue::MakeByFloat(Float value)
    {
        SolverValue Result(SolverValueType::Float);
        Result.mFloat = value;
        return Result;
    }

    SolverValue SolverValue::MakeByText(Text value)
    {
        value.Replace("\\\\", "\\");
        value.Replace("\\\'", "\'");
        value.Replace("\\\"", "\"");
        SolverValue Result(SolverValueType::Text);
        Result.mText = value;
        return Result;
    }

    SolverValue SolverValue::MakeByRange(Float value1, Float value2)
    {
        SolverValue Result(SolverValueType::Range);
        Result.mRange.mValue1 = value1;
        Result.mRange.mValue2 = value2;
        return Result;
    }

    SolverValue SolverValue::MakeByRangeTime(Range value, Float sec)
    {
        SolverValue Result(SolverValueType::Range);
        Result.mRange.mValue1 = value.mValue1;
        Result.mRange.mValue2 = value.mValue2;
        Result.mRange.mBeginMsec = Platform::Utility::CurrentTimeMsec();
        Result.mRange.mEndMsec = Result.mRange.mBeginMsec + Math::Max(0, sec * 1000);
        return Result;
    }

    SolverValue SolverValue::MakeByRangeTime(chars code)
    {
        SolverValue Result(SolverValueType::Range);
        if(*(code++) == '@' && *(code++) == 'R') // @R0_0_0_0@
        {
            sint32 FindStep = 0;
            chars BeginPos = code;
            while(*code != '\0')
            {
                if(*code == '_' || *code == '@')
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
        if(mType == SolverValueType::Range)
            return (Integer) mRange.GetValue();
        if(mType == SolverValueType::Integer)
            return mInteger;
        if(mType == SolverValueType::Float)
            return (Integer) mFloat;
        return Parser::GetInt<Integer>(mText);
    }

    SolverValue::Float SolverValue::ToFloat() const
    {
        if(mType == SolverValueType::Range)
            return mRange.GetValue();
        if(mType == SolverValueType::Integer)
            return (Float) mInteger;
        if(mType == SolverValueType::Float)
            return mFloat;
        return Parser::GetFloat<Float>(mText);
    }

    SolverValue::Text SolverValue::ToText(bool quotes) const
    {
        if(mType == SolverValueType::Range)
            return mRange.GetCode();
        if(mType == SolverValueType::Integer)
            return String::FromInteger(mInteger);
        if(mType == SolverValueType::Float)
            return String::FromFloat(mFloat);
        if(quotes)
            return '\'' + mText + '\'';
        return mText;
    }

    SolverValue::Range SolverValue::ToRange() const
    {
        if(mType == SolverValueType::Range)
            return mRange;
        if(mType == SolverValueType::Integer)
            return Range((Float) mInteger);
        if(mType == SolverValueType::Float)
            return Range(mFloat);
        return Range(Parser::GetFloat<Float>(mText));
    }

    SolverValueType SolverValue::GetMergedType(const SolverValue& rhs) const
    {
        return (SolverValueType) Math::Max((sint32) mType, (sint32) rhs.mType);
    }

    SolverValue SolverValue::Addition(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeByInteger(ToInteger() + rhs.ToInteger());
        case SolverValueType::Float: return MakeByFloat(ToFloat() + rhs.ToFloat());
        case SolverValueType::Text: return MakeByText(ToText() + rhs.ToText());
        }
        return SolverValue();
    }

    SolverValue SolverValue::Subtract(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeByInteger(ToInteger() - rhs.ToInteger());
        case SolverValueType::Float: return MakeByFloat(ToFloat() - rhs.ToFloat());
        case SolverValueType::Text: return MakeByText(String(ToText()).Replace(rhs.ToText(), ""));
        }
        return SolverValue();
    }

    SolverValue SolverValue::Multiply(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeByInteger(ToInteger() * rhs.ToInteger());
        case SolverValueType::Float: return MakeByFloat(ToFloat() * rhs.ToFloat());
        case SolverValueType::Text: return MakeByText(ToText() + "*" + rhs.ToText());
        }
        return SolverValue();
    }

    SolverValue SolverValue::Divide(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeByInteger(ToInteger() / rhs.ToInteger());
        case SolverValueType::Float: return MakeByFloat(ToFloat() / rhs.ToFloat());
        case SolverValueType::Text: return MakeByText(ToText() + "/" + rhs.ToText());
        }
        return SolverValue();
    }

    SolverValue SolverValue::Remainder(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeByInteger(ToInteger() % rhs.ToInteger());
        case SolverValueType::Float: return MakeByFloat(Math::Mod(ToFloat(), rhs.ToFloat()));
        case SolverValueType::Text: return MakeByText(ToText() + "%" + rhs.ToText());
        }
        return SolverValue();
    }

    SolverValue SolverValue::Variabler(const SolverValue& rhs, const SolverChain* chain) const
    {
        if(auto CurSolver = SolverVariable::FindTarget(chain, rhs.ToText()))
            return CurSolver->result();
        return *this;
    }

    SolverValue SolverValue::RangeTarget(const SolverValue& rhs) const
    {
        return MakeByRange(ToFloat(), rhs.ToFloat());
    }

    SolverValue SolverValue::RangeTimer(const SolverValue& rhs) const
    {
        return MakeByRangeTime(ToRange(), rhs.ToFloat());
    }

    SolverValue SolverValue::Greater(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeByInteger(ToInteger() < rhs.ToInteger());
        case SolverValueType::Float: return MakeByInteger(ToFloat() < rhs.ToFloat());
        case SolverValueType::Text: return MakeByInteger(ToText().Compare(rhs.ToText()) < 0);
        }
        return SolverValue();
    }

    SolverValue SolverValue::GreaterOrEqual(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeByInteger(ToInteger() <= rhs.ToInteger());
        case SolverValueType::Float: return MakeByInteger(ToFloat() <= rhs.ToFloat());
        case SolverValueType::Text: return MakeByInteger(ToText().Compare(rhs.ToText()) <= 0);
        }
        return SolverValue();
    }

    SolverValue SolverValue::Less(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeByInteger(ToInteger() > rhs.ToInteger());
        case SolverValueType::Float: return MakeByInteger(ToFloat() > rhs.ToFloat());
        case SolverValueType::Text: return MakeByInteger(ToText().Compare(rhs.ToText()) > 0);
        }
        return SolverValue();
    }

    SolverValue SolverValue::LessOrEqual(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeByInteger(ToInteger() >= rhs.ToInteger());
        case SolverValueType::Float: return MakeByInteger(ToFloat() >= rhs.ToFloat());
        case SolverValueType::Text: return MakeByInteger(ToText().Compare(rhs.ToText()) >= 0);
        }
        return SolverValue();
    }

    SolverValue SolverValue::Equal(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeByInteger(ToInteger() == rhs.ToInteger());
        case SolverValueType::Float: return MakeByInteger(ToFloat() == rhs.ToFloat());
        case SolverValueType::Text: return MakeByInteger(ToText().Compare(rhs.ToText()) == 0);
        }
        return SolverValue();
    }

    SolverValue SolverValue::Different(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeByInteger(ToInteger() != rhs.ToInteger());
        case SolverValueType::Float: return MakeByInteger(ToFloat() != rhs.ToFloat());
        case SolverValueType::Text: return MakeByInteger(ToText().Compare(rhs.ToText()) != 0);
        }
        return SolverValue();
    }

    SolverValue SolverValue::Function_Min(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeByInteger(Math::Min(ToInteger(), rhs.ToInteger()));
        case SolverValueType::Float: return MakeByFloat(Math::MinF(ToFloat(), rhs.ToFloat()));
        case SolverValueType::Text: return MakeByText((ToText().Compare(rhs.ToText()) < 0)? ToText() : rhs.ToText());
        }
        return SolverValue();
    }

    SolverValue SolverValue::Function_Max(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeByInteger(Math::Max(ToInteger(), rhs.ToInteger()));
        case SolverValueType::Float: return MakeByFloat(Math::MaxF(ToFloat(), rhs.ToFloat()));
        case SolverValueType::Text: return MakeByText((ToText().Compare(rhs.ToText()) > 0)? ToText() : rhs.ToText());
        }
        return SolverValue();
    }

    SolverValue SolverValue::Function_Abs(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeByInteger(ToInteger() + Math::Abs(rhs.ToInteger()));
        case SolverValueType::Float: return MakeByFloat(ToFloat() + Math::AbsF(rhs.ToFloat()));
        case SolverValueType::Text: return MakeByText("Abs_Error");
        }
        return SolverValue();
    }

    SolverValue SolverValue::Function_Pow(const SolverValue& rhs) const
    {
        switch(GetMergedType(rhs))
        {
        case SolverValueType::Integer: return MakeByInteger(Math::Pow(ToInteger(), rhs.ToInteger()));
        case SolverValueType::Float: return MakeByFloat(Math::Pow(ToFloat(), rhs.ToFloat()));
        case SolverValueType::Text: return MakeByText("Pow_Error");
        }
        return SolverValue();
    }

    Solver::Solver()
    {
        mLinkedChain = nullptr;
        mReliable = 0;
        mResult = SolverValue::MakeByInteger(0);
        mResultMsec = 0;
    }

    Solver::~Solver()
    {
        Unlink();
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
        mResultMsec = rhs.mResultMsec; rhs.mResultMsec = 0;

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
            return (*FindedChain)(variable).target();
        return nullptr;
    }

    Solver& Solver::Parse(chars formula)
    {
        auto AddOperator = [](SolverOperandObject*& focus, SolverOperatorType type, sint32 deep)->void
        {
            BOSS_ASSERT("잘못된 시나리오입니다", focus);
            sint32 NewPriority = deep * 7;
            switch(type)
            {
            case SolverOperatorType::Addition: case SolverOperatorType::Subtract: // 2순위> +, -
                NewPriority += 5;
                break;
            case SolverOperatorType::Multiply: case SolverOperatorType::Divide: case SolverOperatorType::Remainder: // 1순위> *, /, %
                NewPriority += 6;
                break;
            case SolverOperatorType::Variabler: // 5순위> @
                NewPriority += 2;
                break;
            case SolverOperatorType::RangeTarget: // 3순위> ~
                NewPriority += 4;
                break;
            case SolverOperatorType::RangeTimer: // 4순위> :
                NewPriority += 3;
                break;
            case SolverOperatorType::Greater: case SolverOperatorType::GreaterOrEqual: case SolverOperatorType::Less: // 7순위> <, <=, >, >=, ==, !=
            case SolverOperatorType::LessOrEqual: case SolverOperatorType::Equal: case SolverOperatorType::Different:
                NewPriority += 0;
                break;
            case SolverOperatorType::Function_Min: case SolverOperatorType::Function_Max: // 6순위> [min], [max], [abs], [pow]
            case SolverOperatorType::Function_Abs: case SolverOperatorType::Function_Pow:
                NewPriority += 1;
                break;
            }

            SolverFormula NewFormula(type, NewPriority);
            if(focus->ConstPtr()->type() != SolverOperandType::Formula) // 최초의 계산항이거나
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
                // 계산항 밀어넣기
                branch;
                jump(*formula == '+') AddOperator(OperandFocus, SolverOperatorType::Addition, deep);
                jump(*formula == '-') AddOperator(OperandFocus, SolverOperatorType::Subtract, deep);
                jump(*formula == '*') AddOperator(OperandFocus, SolverOperatorType::Multiply, deep);
                jump(*formula == '/') AddOperator(OperandFocus, SolverOperatorType::Divide, deep);
                jump(*formula == '%') AddOperator(OperandFocus, SolverOperatorType::Remainder, deep);
                jump(*formula == '@') AddOperator(OperandFocus, SolverOperatorType::Variabler, deep);
                jump(*formula == '~') AddOperator(OperandFocus, SolverOperatorType::RangeTarget, deep);
                jump(*formula == ':') AddOperator(OperandFocus, SolverOperatorType::RangeTimer, deep);
                jump(*formula == '<')
                {
                    if(formula[1] == '=')
                    {
                        AddOperator(OperandFocus, SolverOperatorType::GreaterOrEqual, deep);
                        formula += 1;
                    }
                    else AddOperator(OperandFocus, SolverOperatorType::Greater, deep);
                }
                jump(*formula == '>')
                {
                    if(formula[1] == '=')
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
                        formula += 4;
                    }
                    jump(!String::Compare("[max]", formula, 5))
                    {
                        AddOperator(OperandFocus, SolverOperatorType::Function_Max, deep);
                        formula += 4;
                    }
                    jump(!String::Compare("[abs]", formula, 5))
                    {
                        AddOperator(OperandFocus, SolverOperatorType::Function_Abs, deep);
                        formula += 4;
                    }
                    jump(!String::Compare("[pow]", formula, 5))
                    {
                        AddOperator(OperandFocus, SolverOperatorType::Function_Pow, deep);
                        formula += 4;
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
                OperatorTurn = false;
            }
            else // 피연산항턴
            {
                // 상수와 변수
                buffer NewOperand = nullptr;
                if(('0' <= *formula && *formula <= '9') || *formula == '+' || *formula == '-')
                {
                    sint32 WordSize = 0;
                    NewOperand = SolverLiteral(SolverValue::MakeByFloat(Parser::GetFloat<SolverValue::Float>(formula, -1, &WordSize))).clone();
                    formula += WordSize - 1;
                }
                else if(*formula == '\'' || *formula == '\"')
                {
                    chars End = formula;
                    while(*(++End))
                    {
                        if(End[0] == '\\' && End[1] != '\0') End++;
                        else if(*End == *formula) break;
                    }
                    NewOperand = SolverLiteral(SolverValue::MakeByText(String(formula + 1, End - formula - 1))).clone();
                    formula += (End - formula - 1 + 2) - 1;
                }
                else if(*formula == '@')
                {
                    chars End = formula;
                    while(*(++End)) if(*End == *formula) break;
                    NewOperand = SolverLiteral(SolverValue::MakeByRangeTime(String(formula, End - formula + 1))).clone();
                    formula += (End - formula - 1 + 2) - 1;
                }
                else
                {
                    chars End = formula;
                    while(*(++End))
                    {
                        switch(*End)
                        {
                        case ' ': case '\t': case '\r': case '\n': case '(': case ')':
                        case '+': case '-': case '*': case '/': case '%': break;
                        default: continue;
                        }
                        break;
                    }
                    NewOperand = SolverVariable(String(formula, End - formula)).clone();
                    formula += (End - formula) - 1;
                }
                OperatorTurn = true;

                // 피연산항 밀어넣기
                if(OperandFocus)
                {
                    auto CurFormula = (SolverFormula*) OperandFocus->Ptr();
                    BOSS_ASSERT("잘못된 시나리오입니다", CurFormula->type() == SolverOperandType::Formula);
                    CurFormula->mOperandR = NewOperand;
                }
                else OperandFocus = &(mOperandTop = NewOperand);
            }
        }

        // 파싱결과를 기록
        mParsedFormula.Empty();
        mOperandTop->PrintFormula(mParsedFormula);
        // 링크가 된 경우 새로 생긴 하위의 변수들에게 체인정보 전달
        if(mLinkedChain)
            mOperandTop->UpdateChain(this, mLinkedChain);
        return *this;
    }

    void Solver::Execute(bool updateobservers)
    {
        const float OldReliable = mReliable;
        const SolverValue OldResult = ToReference(mResult);
        mReliable = mOperandTop->reliable();
        mResult = mOperandTop->result(SolverValue::MakeByInteger(0));

        if(OldReliable != mReliable || OldResult.Different(mResult).ToInteger() != 0)
        {
            mResultMsec = Platform::Utility::CurrentTimeMsec();
            if(mLinkedChain && 0 < mLinkedVariable.Length())
                (*mLinkedChain)(mLinkedVariable).ResetTarget(this, updateobservers);
        }
    }

    SolverValue Solver::ExecuteOnly() const
    {
        return mOperandTop->result(SolverValue::MakeByInteger(0));
    }

    String Solver::ExecuteVariableName() const
    {
        switch(mOperandTop->type())
        {
        case SolverOperandType::Literal:
            return mOperandTop->result(SolverValue()).ToText();
        case SolverOperandType::Variable:
            {
                Strings GetName;
                mOperandTop->PrintVariables(GetName, false);
                return GetName[0];
            }
            break;
        case SolverOperandType::Formula:
            return ExecuteOnly().ToText();
        }
        return String();
    }

    Strings Solver::GetTargetlessVariables() const
    {
        Strings Results;
        mOperandTop->PrintVariables(Results, true);
        return Results;
    }
}
