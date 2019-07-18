﻿#include <boss.hpp>
#include "boss_zayson.hpp"

namespace BOSS
{
    ////////////////////////////////////////////////////////////////////////////////
    // ZaySonDocument
    ////////////////////////////////////////////////////////////////////////////////
    ZaySonDocument::ZaySonDocument(chars chain) : mChain(chain)
    {
        mExecutedCount = 0;
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
        mExecutedCount = rhs.mExecutedCount; rhs.mExecutedCount = 0;
        return *this;
    }

    void ZaySonDocument::Add(chars variable, chars formula)
    {
        auto& NewSolver = mSolvers.AtAdding();
        NewSolver.Link(mChain, variable);
        NewSolver.Parse(formula);
    }

    void ZaySonDocument::AddJson(const Context& json, const String nameheader)
    {
        if(auto Length = json.LengthOfNamable())
        {
            for(sint32 i = 0; i < Length; ++i)
            {
                chararray GetKey;
                auto& ChildJson = json(i, &GetKey);
                AddJson(ChildJson, nameheader + String::Format("%s.", &GetKey[0]));
            }
        }
        else if(auto Length = json.LengthOfIndexable())
        {
            for(sint32 i = 0; i < Length; ++i)
                AddJson(json[i], nameheader + String::Format("%d.", i));
            // 수량정보
            auto& NewSolver = mSolvers.AtAdding();
            NewSolver.Link(mChain, nameheader + "count");
            NewSolver.Parse(String::FromInteger(Length));
        }
        else if(chars CurValue = json.GetString(nullptr))
        {
            const String Key = nameheader.Left(nameheader.Length() - 1);
            auto& NewSolver = mSolvers.AtAdding();
            NewSolver.Link(mChain, Key);
            NewSolver.Parse(String::Format("\"%s\"", CurValue));
            PostProcess(Key, CurValue);
        }
    }

    void ZaySonDocument::AddFlush()
    {
        for(sint32 i = mExecutedCount, iend = mSolvers.Count(); i < iend; ++i)
            mSolvers.At(i).Execute(true);
        mExecutedCount = mSolvers.Count();
    }

    void ZaySonDocument::Update(chars variable, chars formula)
    {
        if(auto CurSolver = Solver::Find(mChain, variable))
        {
            CurSolver->Parse(formula);
            CurSolver->Execute(true);
        }
        else BOSS_ASSERT(String::Format("해당 변수(%s)를 찾을 수 없습니다", variable), false);
    }

    void ZaySonDocument::CheckUpdatedSolvers(uint64 msec, UpdateCB cb)
    {
        for(sint32 i = 0; i < mExecutedCount; ++i)
            if(msec <= mSolvers[i].resultmsec())
                cb(&mSolvers[i]);
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
                const String LaidText = String(text + cold, c - cold).Trim();
                if(0 < LaidText.Length())
                    Collector += LaidText + ' ';
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
        jump(!String::Compare(text, "if(", 3))
            return ZaySonInterface::ConditionType::If;
        jump(!String::Compare(text, "iffocused(", 10))
            return ZaySonInterface::ConditionType::IfFocused;
        jump(!String::Compare(text, "ifhovered(", 10))
            return ZaySonInterface::ConditionType::IfHovered;
        jump(!String::Compare(text, "ifpressed(", 10))
            return ZaySonInterface::ConditionType::IfPressed;
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
        jump(!String::Compare(text, "else"))
            return ZaySonInterface::ConditionType::Else;
        jump(!String::Compare(text, "endif"))
            return ZaySonInterface::ConditionType::Endif;
        return ZaySonInterface::ConditionType::Unknown;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ZayUIElement
    ////////////////////////////////////////////////////////////////////////////////
    class ZayUIElement
    {
        BOSS_DECLARE_STANDARD_CLASS(ZayUIElement)
    public:
        enum class Type {Unknown, Condition, Asset, Param, Request, Component, View};

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

    protected:
        virtual void Load(const ZaySon& root, const Context& context)
        {
            mRefRoot = &root;
            hook(context("uiname"))
                mUINameSolver.Link(root.ViewName()).Parse(fish.GetString());
            hook(context("uiloop"))
                mUILoopSolver.Link(root.ViewName()).Parse(fish.GetString());
            hook(context("comment"))
                mComment = fish.GetString();
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
        virtual void OnCursor(CursorRole role) {}
        virtual void OnTouch(chars uiname) {}

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
    // ZayExtendCursor
    ////////////////////////////////////////////////////////////////////////////////
    void ZayExtendCursor(CursorRole role, sint32 elementid)
    {
        if(auto CurUIElement = ZayUIElement::Get(elementid))
            CurUIElement->OnCursor(role);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // ZayExtendTouch
    ////////////////////////////////////////////////////////////////////////////////
    void ZayExtendTouch(chars uiname, sint32 elementid)
    {
        if(auto CurUIElement = ZayUIElement::Get(elementid))
            CurUIElement->OnTouch(uiname);
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

            const String ConditionText = context.GetString();
            mConditionType = ZaySonUtility::ToCondition(ConditionText, &mWithElse);
            if(mConditionType == ZaySonInterface::ConditionType::Unknown)
                root.SendErrorLog("조건문 판독실패", String::Format("알 수 없는 조건문입니다(%s, Load)", (chars) ConditionText));

            sint32 PosB, PosE;
            if(ZaySonUtility::IsFunctionCall(ConditionText, &PosB, &PosE)) // ()사용여부와 파라미터 발라내기
            {
                mConditionSolver.Link(root.ViewName());
                mConditionSolver.Parse(String(((chars) ConditionText) + PosB, PosE - PosB));
            }
        }

    public:
        static bool Test(const ZaySon& root, ZayUIs& dest, const Context& src)
        {
            if(ZaySonUtility::ToCondition(src.GetString("")) != ZaySonInterface::ConditionType::Unknown) // oncreate, onclick, compvalues의 경우
            {
                Object<ZayConditionElement> NewCondition(ObjectAllocType::Now);
                NewCondition->Load(root, src);
                dest.AtAdding() = (id_share) NewCondition;
                return true;
            }
            else if(ZaySonUtility::ToCondition(src("compname").GetString("")) != ZaySonInterface::ConditionType::Unknown) // compname의 경우
            {
                Object<ZayConditionElement> NewCondition(ObjectAllocType::Now);
                NewCondition->Load(root, src("compname"));
                dest.AtAdding() = (id_share) NewCondition;
                return true;
            }
            return false;
        };
        static sint32s Collect(chars viewname, const ZayUIs& uis, const ZayPanel* panel = nullptr)
        {
            sint32s Collector;
            // 조건문처리로 유효한 CompValue를 수집
            for(sint32 i = 0, iend = uis.Count(); i < iend; ++i)
            {
                if(uis[i].ConstValue().mType == ZayUIElement::Type::Condition)
                {
                    // 조건의 성공여부
                    bool IsTrue = true;
                    auto CurCondition = (const ZayConditionElement*) uis[i].ConstPtr();
                    if(CurCondition->mConditionType == ZaySonInterface::ConditionType::If)
                        IsTrue = (CurCondition->mConditionSolver.ExecuteOnly().ToInteger() != 0);
                    // 포커스확인
                    else if(CurCondition->mConditionType == ZaySonInterface::ConditionType::IfFocused)
                    {
                        if(panel)
                        {
                            const String UIName = CurCondition->mConditionSolver.ExecuteOnly().ToText();
                            IsTrue = !!(panel->state(viewname + ('.' + UIName)) & PS_Focused);
                        }
                    }
                    // 호버확인
                    else if(CurCondition->mConditionType == ZaySonInterface::ConditionType::IfHovered)
                    {
                        if(panel)
                        {
                            const String UIName = CurCondition->mConditionSolver.ExecuteOnly().ToText();
                            IsTrue = !!(panel->state(viewname + ('.' + UIName)) & PS_Hovered);
                        }
                    }
                    // 프레스확인
                    else if(CurCondition->mConditionType == ZaySonInterface::ConditionType::IfPressed)
                    {
                        if(panel)
                        {
                            const String UIName = CurCondition->mConditionSolver.ExecuteOnly().ToText();
                            IsTrue = !!(panel->state(viewname + ('.' + UIName)) & PS_Dragging);
                        }
                    }

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
    // ZayAssetElement
    ////////////////////////////////////////////////////////////////////////////////
    class ZayAssetElement : public ZayUIElement
    {
    public:
        ZayAssetElement() : ZayUIElement(Type::Asset)
        {mDataType = ZaySonInterface::DataType::Unknown;}
        ~ZayAssetElement() override {}

    public:
        void Load(const ZaySon& root, const Context& context) override
        {
            ZayUIElement::Load(root, context);

            mDataName = context("dataname").GetString();
            const String Type = context("datatype").GetString();
            if(!Type.Compare("viewscript"))
                mDataType = ZaySonInterface::DataType::ViewScript;
            else if(!Type.Compare("imagemap"))
                mDataType = ZaySonInterface::DataType::ImageMap;
            mFilePath = context("filepath").GetString();
            mUrl = context("url").GetString();
        }

    public:
        String mDataName;
        ZaySonInterface::DataType mDataType;
        String mFilePath;
        String mUrl;
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
                mParamSolvers.AtAdding().Link(root.ViewName()).Parse(context[i].GetString());
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

            const String TextTest = context.GetString("");
            sint32 PosB, PosE;
            if(ZaySonUtility::IsFunctionCall(TextTest, &PosB, &PosE)) // 함수호출
            {
                mRequestType = ZaySonInterface::RequestType::Function;
                mGlueFunction = root.FindGlue(TextTest.Left(PosB - 1));
                if(!mGlueFunction)
                    root.SendWarningLog("글루 바인딩실패", String::Format("글루함수를 찾을 수 없습니다(%s, Load)", (chars) TextTest.Left(PosB - 1)));
                auto Params = ZaySonUtility::GetCommaStrings(TextTest.Middle(PosB, PosE - PosB));
                for(sint32 i = 0, iend = Params.Count(); i < iend; ++i)
                    mRSolvers.AtAdding().Link(root.ViewName()).Parse(Params[i]); // 파라미터들
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
                    mRSolvers.AtAdding().Parse(GetValue.GetString()); // 우항
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
                else mRefRoot->SendErrorLog("변수 업데이트실패", String::Format("변수를 찾을 수 없습니다(%s, Transaction)",
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
    // ZayComponentElement
    ////////////////////////////////////////////////////////////////////////////////
    class ZayComponentElement : public ZayUIElement
    {
    public:
        ZayComponentElement() : ZayUIElement(Type::Component) {mCompID = -2;}
        ~ZayComponentElement() override {}

    public:
        void Load(const ZaySon& root, const Context& context) override
        {
            ZayUIElement::Load(root, context);

            hook(context("compname"))
                mCompName = fish.GetString();

            hook(context("compid"))
                mCompID = fish.GetInt(-2);

            hook(context("compvalues"))
            for(sint32 i = 0, iend = fish.LengthOfIndexable(); i < iend; ++i)
            {
                if(ZayConditionElement::Test(root, mCompValues, fish[i]))
                    continue;
                Object<ZayParamElement> NewParam(ObjectAllocType::Now);
                NewParam->Load(root, fish[i]);
                mCompValues.AtAdding() = (id_share) NewParam;
            }

            Map<String> CaptureCollector;
            hook(context("onclick"))
            for(sint32 i = 0, iend = fish.LengthOfIndexable(); i < iend; ++i)
            {
                if(ZayConditionElement::Test(root, mClickCodes, fish[i]))
                    continue;
                Object<ZayRequestElement> NewRequest(ObjectAllocType::Now);
                NewRequest->Load(root, fish[i]);
                NewRequest->InitForClick(CaptureCollector);
                mClickCodes.AtAdding() = (id_share) NewRequest;
            }
            // 캡쳐목록 정리
            for(sint32 i = 0, iend = CaptureCollector.Count(); i < iend; ++i)
                mClickCodeCaptures.AtAdding() = *CaptureCollector.AccessByOrder(i);

            hook(context("ui"))
            for(sint32 i = 0, iend = fish.LengthOfIndexable(); i < iend; ++i)
            {
                if(ZayConditionElement::Test(root, mChildren, fish[i]))
                    continue;
                Object<ZayComponentElement> NewRenderer(ObjectAllocType::Now);
                NewRenderer->Load(root, fish[i]);
                mChildren.AtAdding() = (id_share) NewRenderer;
            }
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
                    NewLog.mRect = Rect(panel.toview(0, 0), panel.toview(panel.w(), panel.h()));
                    NewLog.mFill = fill;
                    NewLog.mUIName = uiname;
                };

                chars ViewName = mRefRoot->ViewName();
                const String UIName((mUINameSolver.is_blank())? "" : (chars) mUINameSolver.ExecuteVariableName());
                if(mCompValues.Count() == 0)
                {
                    if(mUILoopSolver.is_blank())
                    {
                        String UINameTemp;
                        chars ComponentName = nullptr;
                        if(0 < UIName.Length()) ComponentName = UINameTemp = ViewName + ('.' + UIName);
                        else if(0 < mClickCodes.Count()) ComponentName = defaultname;

                        // 파라미터 없음
                        ZayExtend::Payload ParamCollector = CurComponent->MakePayload(ComponentName, mID);
                        ZAY_EXTEND(ParamCollector >> panel)
                        {
                            if(mCompID == mRefRoot->debugFocusedCompID())
                                AddDebugLog(logs, panel, CurComponent->HasContentComponent(), ComponentName);
                            RenderChildren(panel, ComponentName, defaultname, logs);
                        }
                    }
                    else // 반복문
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
                            RenderChildren(panel, nullptr, defaultname + String::Format("_%d", i), logs);
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
                        else if(0 < mClickCodes.Count()) ComponentName = DefaultName;

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
                        ZayExtend::Payload ParamCollector = CurComponent->MakePayload(ComponentName, mID);
                        if(auto CurCompValue = (const ZayParamElement*) mCompValues[CollectedCompValues[i]].ConstPtr())
                        {
                            for(sint32 j = 0, jend = CurCompValue->mParamSolvers.Count(); j < jend; ++j)
                                ParamCollector(CurCompValue->mParamSolvers[j].ExecuteOnly());
                        }
                        ZAY_EXTEND(ParamCollector >> panel)
                        {
                            if(mCompID == mRefRoot->debugFocusedCompID())
                                AddDebugLog(logs, panel, CurComponent->HasContentComponent(), ComponentName);
                            RenderChildren(panel, ComponentName, DefaultName, logs);
                        }
                    }
                }
            }
            else mRefRoot->SendWarningLog("컴포넌트 랜더링실패", String::Format("컴포넌트함수를 찾을 수 없습니다(%s, Render)", (chars) mCompName));
        }
        void RenderChildren(ZayPanel& panel, chars uiname, const String& defaultname, DebugLogs& logs) const
        {
            // 클릭코드를 위한 변수를 사전 캡쳐
            if(uiname != nullptr && 0 < mClickCodeCaptures.Count())
            {
                const Point ViewPos = panel.toview(0, 0);
                hook(mClickCodeCaptureValues(uiname))
                for(sint32 i = 0, iend = mClickCodeCaptures.Count(); i < iend; ++i)
                {
                    chars CurVariable = mClickCodeCaptures[i];
                    branch;
                    jump(!String::Compare(CurVariable, "pX")) fish(CurVariable) = String::FromFloat(ViewPos.x);
                    jump(!String::Compare(CurVariable, "pY")) fish(CurVariable) = String::FromFloat(ViewPos.y);
                    jump(!String::Compare(CurVariable, "pW")) fish(CurVariable) = String::FromFloat(panel.w());
                    jump(!String::Compare(CurVariable, "pH")) fish(CurVariable) = String::FromFloat(panel.h());
                    else fish(CurVariable) = Solver().Link(mRefRoot->ViewName()).Parse(CurVariable).ExecuteOnly().ToText(true);
                }
            }

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
        void OnCursor(CursorRole role) override
        {
            if(0 < mClickCodes.Count())
                mRefRoot->SendCursor(role);
        }
        void OnTouch(chars uiname) override
        {
            if(0 < mClickCodes.Count())
            {
                // 사전 캡쳐된 변수를 지역변수화
                Solvers LocalSolvers;
                hook(mClickCodeCaptureValues(uiname))
                for(sint32 i = 0, iend = fish.Count(); i < iend; ++i)
                {
                    chararray Variable;
                    auto& Value = *fish.AccessByOrder(i, &Variable);
                    LocalSolvers.AtAdding().Link(mRefRoot->ViewName(), &Variable[0]).Parse(Value).Execute();
                }

                // 클릭코드의 실행
                sint32s CollectedClickCodes = ZayConditionElement::Collect(mRefRoot->ViewName(), mClickCodes);
                for(sint32 i = 0, iend = CollectedClickCodes.Count(); i < iend; ++i)
                {
                    auto CurClickCode = (ZayRequestElement*) mClickCodes.At(CollectedClickCodes[i]).Ptr();
                    CurClickCode->Transaction();
                }
            }
        }

    public:
        String mCompName;
        sint32 mCompID;
        ZayUIs mCompValues;
        ZayUIs mClickCodes;
        Strings mClickCodeCaptures;
        typedef Map<String> CaptureValue;
        mutable Map<CaptureValue> mClickCodeCaptureValues;
        ZayUIs mChildren;
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

            hook(context("asset"))
            for(sint32 i = 0, iend = fish.LengthOfIndexable(); i < iend; ++i)
            {
                if(ZayConditionElement::Test(root, mAssets, fish[i]))
                    continue;
                Object<ZayAssetElement> NewResource(ObjectAllocType::Now);
                NewResource->Load(root, fish[i]);
                mAssets.AtAdding() = (id_share) NewResource;
            }

            hook(context("oncreate"))
            for(sint32 i = 0, iend = fish.LengthOfIndexable(); i < iend; ++i)
            {
                if(ZayConditionElement::Test(root, mCreateCodes, fish[i]))
                    continue;
                Object<ZayRequestElement> NewRequest(ObjectAllocType::Now);
                NewRequest->Load(root, fish[i]);
                NewRequest->InitForCreate();
                mCreateCodes.AtAdding() = (id_share) NewRequest;
            }

            hook(context("ui"))
            for(sint32 i = 0, iend = fish.LengthOfIndexable(); i < iend; ++i)
            {
                if(ZayConditionElement::Test(root, mChildren, fish[i]))
                    continue;
                Object<ZayComponentElement> NewLayer(ObjectAllocType::Now);
                NewLayer->Load(root, fish[i]);
                mChildren.AtAdding() = (id_share) NewLayer;
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
        ZayUIs mAssets;
        ZayUIs mCreateCodes;
        ZayUIs mChildren;
    };

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
        SendInfoLog("제이썬 로드성공", String::Format("제이썬을 로드하였습니다(%s, Reload)", (chars) mViewName));
    }

    void ZaySon::SetLogger(LoggerCB cb)
    {
        mDebugLogger = cb;
    }

    const String& ZaySon::ViewName() const
    {
        return mViewName;
    }

    ZaySonInterface& ZaySon::AddComponent(ZayExtend::ComponentType type, chars name, ZayExtend::ComponentCB cb, chars paramcomment)
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

    void ZaySon::Render(ZayPanel& panel)
    {
        if(mUIElement)
        {
            ZayUIElement::DebugLogs LogCollector;
            ((ZayUIElement*) mUIElement)->Render(panel, mViewName, LogCollector);

            // 수집된 디버그로그(GUI툴에 의한 포커스표현)
            const Point ViewPos = panel.toview(0, 0);
            ZAY_MOVE(panel, -ViewPos.x, -ViewPos.y)
            for(sint32 i = 0, iend = LogCollector.Count(); i < iend; ++i)
            {
                hook(LogCollector[i])
                ZAY_RECT(panel, fish.mRect)
                ZAY_FONT(panel, Math::MaxF(0.8, (fish.mRect.r - fish.mRect.l) * 0.005))
                {
                    // 영역표시
                    ZAY_RGBA(panel, 255, 0, 0, 128)
                    if(fish.mFill)
                        panel.fill();
                    else ZAY_INNER(panel, 5)
                        panel.rect(10);
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
