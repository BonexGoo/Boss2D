#pragma once
#include <boss.hpp>
#include <functional>

namespace BOSS
{
    class Solver;
    enum class SolverValueType {Null, Range, Integer, Float, Text};
    enum class SolverOperandType {Unknown, Literal, Variable, Formula, Comment};
    enum class SolverOperatorType {Unknown,
        Addition, Subtract, Multiply, Divide, Remainder, BitAnd, BitOr, // +, -, *, /, %, &, |
        Variabler, Commenter, RangeTarget, RangeTimer, // @, ?, ~, :
        Greater, GreaterOrEqual, Less, LessOrEqual, Equal, Different, // <, <=, >, >=, ==, !=
        Function_Min, Function_Max, Function_Abs, Function_Pow, // [min], [max], [abs], [pow]
        Function_Cos, Function_Sin, Function_Tan, Function_Atan, // [cos], [sin], [tan], [atan]
        Function_And, Function_Or, Function_Divide, Function_Find}; // [and], [or], [divide], [find]

    // 업데이트체인
    class SolverChainPair
    {
        // 인터페이스
        public: void ResetTarget(Solver* solver, bool updateobservers);
        public: void ForcedChangeTarget(Solver* oldsolver, Solver* newsolver);
        public: bool RemoveTarget(bool updateobservers);
        public: void AddObserver(Solver* solver);
        public: bool SubObserver(Solver* solver);
        public: void RenualAllObservers();
        public: Solver* target() {return mTarget.mSolver;}
        public: const Solver* target() const {return mTarget.mSolver;}

        private: class Pair
        {
            public: Pair() {mSolver = nullptr; mUpdateID = 0;}
            public: ~Pair() {}
            public: Solver* mSolver;
            public: sint32 mUpdateID;
        };

        // 멤버
        private: Pair mTarget;
        private: Map<Pair> mObserverMap;
    };
    typedef Map<SolverChainPair> SolverChain;
    typedef std::function<void(const String& variable, const SolverChainPair* pair)> SolverVariableCB;

    // 연산값
    class SolverValue
    {
        // 타입
        public: typedef sint64 Integer;
        public: typedef double Float;
        public: typedef String Text;
        public: class Range
        {
            public: Range();
            public: Range(Float value);
            public: Range(const Range& rhs);
            public: Range(Range&& rhs);
            public: ~Range();
            public: Range& operator=(const Range& rhs);
            public: Range& operator=(Range&& rhs);
            public: Float GetValue() const;
            public: Text GetCode() const;
            public: Float mValue1;
            public: Float mValue2;
            public: uint64 mBeginMsec;
            public: uint64 mEndMsec;
        };

        public: SolverValue();
        public: SolverValue(SolverValueType type);
        public: SolverValue(const SolverValue& rhs);
        public: SolverValue(SolverValue&& rhs);
        public: ~SolverValue();
        public: SolverValue& operator=(const SolverValue& rhs);
        public: SolverValue& operator=(SolverValue&& rhs);

        public: static SolverValue MakeByInteger(Integer value);
        public: static SolverValue MakeByFloat(Float value);
        public: static SolverValue MakeByText(Text value);
        public: static SolverValue MakeByRange(Float value1, Float value2);
        public: static SolverValue MakeByRangeTime(Range value, Float sec);
        public: static SolverValue MakeByRangeTime(chars code);

        // 함수
        public: SolverValueType GetType() const;
        public: Integer ToInteger() const;
        public: Float ToFloat() const;
        public: Text ToText(bool need_quotes = false, bool need_rangecode = true) const;
        public: Range ToRange() const;
        private: SolverValueType GetMergedType(const SolverValue& rhs) const;
        public: SolverValue Addition(const SolverValue& rhs) const;
        public: SolverValue Subtract(const SolverValue& rhs) const;
        public: SolverValue Multiply(const SolverValue& rhs) const;
        public: SolverValue Divide(const SolverValue& rhs) const;
        public: SolverValue Remainder(const SolverValue& rhs) const;
        public: SolverValue BitAnd(const SolverValue& rhs) const;
        public: SolverValue BitOr(const SolverValue& rhs) const;
        public: SolverValue Variabler(const SolverValue& rhs, const SolverChain* chain) const;
        public: SolverValue RangeTarget(const SolverValue& rhs) const;
        public: SolverValue RangeTimer(const SolverValue& rhs) const;
        public: SolverValue Greater(const SolverValue& rhs) const;
        public: SolverValue GreaterOrEqual(const SolverValue& rhs) const;
        public: SolverValue Less(const SolverValue& rhs) const;
        public: SolverValue LessOrEqual(const SolverValue& rhs) const;
        public: SolverValue Equal(const SolverValue& rhs) const;
        public: SolverValue Different(const SolverValue& rhs) const;
        public: SolverValue Function_Min(const SolverValue& rhs) const;
        public: SolverValue Function_Max(const SolverValue& rhs) const;
        public: SolverValue Function_Abs(const SolverValue& rhs) const;
        public: SolverValue Function_Pow(const SolverValue& rhs) const;
        public: SolverValue Function_Cos(const SolverValue& rhs) const;
        public: SolverValue Function_Sin(const SolverValue& rhs) const;
        public: SolverValue Function_Tan(const SolverValue& rhs) const;
        public: SolverValue Function_Atan(const SolverValue& rhs) const;
        public: SolverValue Function_And(const SolverValue& rhs) const;
        public: SolverValue Function_Or(const SolverValue& rhs) const;
        public: SolverValue Function_Divide(const SolverValue& rhs) const;
        public: SolverValue Function_Find(const SolverValue& rhs) const;

        // 멤버
        private: SolverValueType mType;
        private: Integer mInteger;
        private: Float mFloat;
        private: Text mText;
        private: Range mRange;
    };
    typedef Array<SolverValue> SolverValues;
    typedef std::function<bool(const String& formula, const SolverValue& value, float reliable)> SolverValueCB;

    // 피연산항
    class SolverOperand
    {
        BOSS_DECLARE_NONCOPYABLE_INITIALIZED_CLASS(SolverOperand, mOperandType(rhs.mOperandType))
        public: SolverOperand(SolverOperandType type = SolverOperandType::Unknown) : mOperandType(type) {}
        public: ~SolverOperand() {}

        // 인터페이스
        public: inline SolverOperandType type() const {return mOperandType;}
        // 가상 인터페이스
        public: virtual void PrintFormula(String& collector) const = 0; // 연산식출력
        public: virtual void PrintVariables(Strings& collector, bool targetless_only) const = 0; // 변수리스트출력
        public: virtual void UpdateChain(Solver* solver, SolverChain* chain) = 0; // 체인업데이트
        public: virtual float reliable() const = 0; // 신뢰도
        public: virtual SolverValue result(SolverValue zero) const = 0; // 결과값
        public: virtual buffer clone() const = 0; // 자기복제

        // 멤버
        protected: const SolverOperandType mOperandType;
    };
    class SolverOperandBlank : public SolverOperand
    {
        public: void PrintFormula(String& collector) const override {}
        public: void PrintVariables(Strings& collector, bool targetless_only) const override {}
        public: void UpdateChain(Solver* solver, SolverChain* chain) override {}
        public: float reliable() const override {return 0;}
        public: SolverValue result(SolverValue zero) const override {return zero;}
        public: buffer clone() const override {return nullptr;}
    };
    typedef Object<SolverOperandBlank, datatype_class_nomemcpy, ObjectAllocType::Now> SolverOperandObject;
    typedef Array<SolverOperandObject> SolverOperandObjects;

    // 연립방정식
    class Solver
    {
        public: Solver();
        public: ~Solver();
        public: Solver(const Solver& rhs) {operator=(rhs);}
        public: Solver(Solver&& rhs) {operator=(ToReference(rhs));}
        public: Solver& operator=(const Solver& rhs);
        public: Solver& operator=(Solver&& rhs);

        // 인터페이스
        public: Solver& Link(chars chain, chars variable = nullptr, bool updateobservers = false);
        public: void Unlink(bool updateobservers = false);
        public: static Solver* Find(chars chain, chars variable);
        public: static void FindVariables(chars chain, SolverVariableCB cb);
        public: static void Remove(chars chain, chars variable);
        public: static void RemoveMatchedVariables(chars chain, chars keyword, SolverVariableCB cb = nullptr);
        public: static void SetReplacer(chars before, chars after);
        public: static void ClearReplacer();
        public: Solver& Parse(chars formula);
        public: void Execute(bool updateobservers = false);
        public: SolverValue ExecuteOnly() const;
        public: String ExecuteVariableName() const;
        public: Strings GetTargetlessVariables() const;
        public: inline void SetResultFilter(SolverValueCB cb) {mResultCB = cb;}
        public: inline bool is_blank() const {return (mParsedFormula.Length() == 0);}
        public: inline const String& linked_variable() const {return mLinkedVariable;}
        public: inline const String& parsed_formula() const {return mParsedFormula;}
        public: inline float reliable() const {return mReliable;}
        public: inline SolverValue result() const
        {
            if(mResult.GetType() == SolverValueType::Text)
            {
                chars Sample = mResult.ToText();
                if(Sample[0] == '$' && Sample[1] == 'R' && Sample[2] == ':')
                    return SolverValue::MakeByRangeTime(Sample);
            }
            return mResult;
        }
        public: inline uint64 updated_msec() const
        {return (mUpdatedFormulaMsec > mUpdatedResultMsec)? mUpdatedFormulaMsec : mUpdatedResultMsec;}

        // 멤버
        private: SolverChain* mLinkedChain;
        private: String mLinkedVariable;
        private: String mParsedFormula;
        private: SolverOperandObject mOperandTop;
        private: float mReliable;
        private: SolverValue mResult;
        private: SolverValueCB mResultCB;
        private: uint64 mUpdatedFormulaMsec;
        private: uint64 mUpdatedResultMsec;
    };
    typedef Array<Solver> Solvers;
}
