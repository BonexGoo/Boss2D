#include <boss.hpp>
#include "zayreviewer.hpp"

#include <resource.hpp>
#include <ctype.h>

ZAY_DECLARE_VIEW_CLASS("zayreviewerView", zayreviewerData)

ZAY_VIEW_API OnCommand(CommandType type, id_share in, id_cloned_share* out)
{
    if(type == CT_Tick)
    {
        if(m->mLoadTask)
        {
            if(!Tasking::IsAlive(m->mLoadTask))
            {
                Tasking::Release(m->mLoadTask, true);
                m->mLoadTask = nullptr;
                if(0 < m->mPool.Count())
                    m->mFileFocus = m->mPool.AccessByOrder(0);
            }
            m->invalidate();
        }
    }
}

ZAY_VIEW_API OnNotify(NotifyType type, chars topic, id_share in, id_cloned_share* out)
{
}

ZAY_VIEW_API OnGesture(GestureType type, sint32 x, sint32 y)
{
}

ZAY_VIEW_API OnRender(ZayPanel& panel)
{
    ZAY_RGB(panel, 192, 192, 192)
        panel.fill();

    if(m->mPool.Count() == 0)
    {
        ZAY_FREEFONT(panel, 34, "coding")
        {
            auto TextHeight = Platform::Graphics::GetStringHeight() + 20;
            ZAY_XYWH_UI(panel, 0, (panel.h() - TextHeight) / 2, panel.w(), TextHeight, "ProjectOpen",
                ZAY_GESTURE_T(t)
                {
                    if(t == GT_InReleased)
                    {
                        String GetPath;
                        if(Platform::Popup::FileDialog(DST_Dir, GetPath, nullptr, "Please select a source code folder."))
                            m->ProjectLoad(GetPath);
                    }
                })
            {
                const bool Focused = !!(panel.state("ProjectOpen") & PS_Focused);
                const bool Pressed = !!(panel.state("ProjectOpen") & PS_Dragging);
                const sint32 PressMove = (Pressed)? 2 : 0;
                ZAY_RGBA(panel, 0, 0, 32, (Focused || Pressed)? 80 : 64)
                    panel.fill();
                ZAY_RGB(panel, 0, 0, 32)
                ZAY_MOVE(panel, PressMove, PressMove + 3)
                    panel.text("CLICK HERE to open the source code folder!");

                // 리센트
                const sint32 Gap = 10;
                sint32 Pos = panel.w() - Gap;
                ZAY_FREEFONT(panel, 15, "coding")
                for(sint32 i = m->mRecentProjectPathes.Count() - 1; 0 <= i; --i)
                {
                    const String UIName = String::Format("Recent-%d", i);
                    const WString DirName = Platform::File::GetShortName(WString::FromChars(m->mRecentProjectPathes[i]));
                    const sint32 Width = 8 + Platform::Graphics::GetStringWidthW(DirName);
                    ZAY_XYWH_UI(panel, Pos - Width, panel.h() + Gap, Width, 20, UIName,
                        ZAY_GESTURE_T(t, i)
                        {
                            if(t == GT_InReleased)
                                m->ProjectLoad(m->mRecentProjectPathes[i]);
                        })
                    {
                        const bool Focused = !!(panel.state(UIName) & PS_Focused);
                        const bool Pressed = !!(panel.state(UIName) & PS_Dragging);
                        const sint32 PressMove = (Pressed)? 1 : 0;
                        ZAY_RGBA(panel, 255, 255, 0, (Focused || Pressed)? 128 : 64)
                            panel.fill();
                        ZAY_RGB(panel, 0, 0, 0)
                        ZAY_MOVE(panel, PressMove, PressMove)
                            panel.text(String::FromWChars(DirName));
                        ZAY_RGB(panel, 0, 0, 0)
                            panel.rect(2);
                    }
                    Pos -= Gap + Width;
                }
            }
        }
    }
    else if(m->mLoadTask)
    {
        ZAY_XYRR(panel, panel.w() / 2, panel.h() / 2, panel.w() / 3, 20)
        {
            ZAY_RGBA(panel, 0, 0, 0, 128)
                panel.fill();

            ZAY_INNER_SCISSOR(panel, 6)
            {
                ZAY_RGB(panel, 0, 0, 0)
                    panel.fill();

                ZAY_INNER(panel, 1)
                ZAY_XYWH(panel, 0, 0, panel.w() * m->mLoadPos / m->mLoadCount, panel.h())
                {
                    ZAY_RGB(panel, 64, 64, 128)
                        panel.fill();
                    if(m->mLoadPos < m->mLoadCount)
                    ZAY_RGBA(panel, 255, 255, 255, Math::Abs((m->frame() % 10) - 5) * 15)
                        panel.fill();
                }
            }

            ZAY_RGB(panel, 0, 0, 0)
                panel.rect(1);

            ZAY_RGB(panel, 0, 0, 0)
            {
                String Text;
                if(m->mLoadPos < m->mLoadCount)
                {
                    chararray GetFilePath;
                    m->mPool.AccessByOrder(m->mLoadPos, &GetFilePath);
                    Text = String::Format(" [%d/%d] %s", m->mLoadPos, m->mLoadCount, &GetFilePath[0]);
                }
                else Text = String::Format(" [%d/%d] Completed!", m->mLoadPos, m->mLoadCount);
                ZAY_LTRB(panel, 0, panel.h(), panel.w(), 100)
                    panel.text(Text, UIFA_LeftTop, UIFE_Center);
            }
        }
    }
    else
    {
        // API리스트
        ZAY_LTRB_UI_SCISSOR(panel, 0, 0, 300, panel.h(), "ApilistView",
            ZAY_GESTURE_TXY(t, x, y)
            {
                static sint32 OldY = 0;
                if(t == GT_Pressed)
                    OldY = y;
                else if(t == GT_InDragging || t == GT_OutDragging)
                {
                    m->mApilistScrollTarget += OldY - y;
                    OldY = y;
                    m->invalidate();
                }
                else if(t == GT_WheelDown || t == GT_WheelUp)
                {
                    m->mApilistScrollTarget += (t == GT_WheelDown)? 300 : -300;
                    m->invalidate();
                }
            })
        {
            ZAY_RGBA(panel, 0, 0, 0, 64)
                panel.fill();
            m->RenderAPIView(panel);
        }

        // 포커싱된 소스코드
        ZAY_LTRB_UI(panel, 300, 0, panel.w(), panel.h(), "SourceView",
            ZAY_GESTURE_TXY(t, x, y)
            {
                if(m->mFileFocus)
                {
                    static sint32 OldY = 0;
                    if(t == GT_Pressed)
                        OldY = y;
                    else if(t == GT_InDragging || t == GT_OutDragging)
                    {
                        m->mFileFocus->mScrollTarget += OldY - y;
                        OldY = y;
                        m->invalidate();
                    }
                    else if(t == GT_WheelDown || t == GT_WheelUp)
                    {
                        m->mFileFocus->mScrollTarget += (t == GT_WheelDown)? 300 : -300;
                        m->invalidate();
                    }
                }
            })
        {
            ZAY_RGBA(panel, 255, 255, 255, 64)
                panel.fill();
            m->RenderSourceView(panel);
        }

        // 체크리스트
        ZAY_LTRB_UI(panel, panel.w() - 200, 0, panel.w(), panel.h(), "ChecklistView",
            ZAY_GESTURE_TXY(t, x, y)
            {
                static sint32 OldY = 0;
                if(t == GT_Pressed)
                    OldY = y;
                else if(t == GT_InDragging || t == GT_OutDragging)
                {
                    m->mChecklistScrollTarget += OldY - y;
                    OldY = y;
                    m->invalidate();
                }
                else if(t == GT_WheelDown || t == GT_WheelUp)
                {
                    m->mChecklistScrollTarget += (t == GT_WheelDown)? 300 : -300;
                    m->invalidate();
                }
            })
        {
            ZAY_RGBA(panel, 0, 0, 0, 64)
                panel.fill();
            m->RenderChecklistView(panel);
        }
    }
}

zayreviewerData::zayreviewerData()
{
    mFonts[0] = AddOn::FreeType::Create(Asset::ToBuffer("font/D2Coding-Ver1.3.2-20180524.ttf"), "coding");
    mFonts[1] = AddOn::FreeType::Create(Asset::ToBuffer("font/D2CodingBold-Ver1.3.2-20180524.ttf"), "coding-bold");
    mFonts[2] = AddOn::FreeType::Create(Asset::ToBuffer("font/HMFMPYUN.ttf"), "talking");
    mFileFocus = nullptr;
    mApilistScrollPos = 0;
    mApilistScrollTarget = 0;
    mChecklistScrollPos = 0;
    mChecklistScrollTarget = 0;

    mLoadCount = 0;
    mLoadPos = 0;
    mLoadTask = nullptr;

    const String RecentString = String::FromAsset("recent.json");
    if(0 < RecentString.Length())
    {
        const Context Recent(ST_Json, SO_OnlyReference, RecentString, RecentString.Length());
        hook(Recent("ProjectPathes"))
        for(sint32 i = 0, iend = fish.LengthOfIndexable(); i < iend; ++i)
            mRecentProjectPathes.AtAdding() = fish[i].GetText("");
    }
}

zayreviewerData::~zayreviewerData()
{
    AddOn::FreeType::Release(mFonts[0]);
    AddOn::FreeType::Release(mFonts[1]);
    AddOn::FreeType::Release(mFonts[2]);
    Tasking::Release(mLoadTask, true);

    if(0 < mCurProjectPath.Length())
    {
        // 리센트 재구성
        for(sint32 i = mRecentProjectPathes.Count() - 1; 0 <= i; --i)
            if(!mRecentProjectPathes[i].CompareNoCase(mCurProjectPath))
                mRecentProjectPathes.SubtractionSection(i);
        while(4 < mRecentProjectPathes.Count())
            mRecentProjectPathes.SubtractionSection(0);
        mRecentProjectPathes.AtAdding() = mCurProjectPath;

        Context Recent;
        hook(Recent.At("ProjectPathes"))
        for(sint32 i = 0, iend = mRecentProjectPathes.Count(); i < iend; ++i)
            fish.AtAdding().Set(mRecentProjectPathes[i]);
        Recent.SaveJson().ToAsset("recent.json");
    }

    Context Review;
    hook(Review.At("CheckList"))
    for(sint32 i = 0, iend = mChecklist.Count(); i < iend; ++i)
    {
        const auto& CurCheck = mChecklist[i];
        if(CurCheck.mChecked)
        {
            hook(fish.At(CurCheck.mShortName))
            hook(fish.At(CurCheck.mDate))
            hook(fish.At(CurCheck.mAuthor))
            {
                if(0 < CurCheck.mKorText.Length())
                    fish.At(CurCheck.mKorText).Set("1");
                else if(0 < CurCheck.mEngText.Length())
                    fish.At(CurCheck.mEngText).Set("1");
            }
        }
    }
    const String ProjectName = String::FromWChars(Platform::File::GetShortName(WString::FromChars(mCurProjectPath)));
    Review.SaveJson().ToAsset(ProjectName + "_review.json");
}

void zayreviewerData::ProjectLoad(chars dirpath)
{
    mCurProjectPath = dirpath;
    mCurProjectPath.Replace('\\', '/');
    Tasking::Release(mLoadTask, true);

    mFileFocus = nullptr;
    mPool.Reset();
    mApilist.Clear();
    mChecklist.Clear();
    CollectSourceFile(dirpath);

    mLoadCount = mPool.Count();
    mLoadPos = 0;
    mLoadTask = Tasking::Create([](buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)->sint32
    {
        zayreviewerData& Self = *((zayreviewerData*) self);
        const String ProjectName = String::FromWChars(Platform::File::GetShortName(WString::FromChars(Self.mCurProjectPath)));
        const String ReviewString = String::FromAsset(ProjectName + "_review.json");
        const Context Review(ST_Json, SO_OnlyReference, ReviewString, ReviewString.Length());

        hook(Review("CheckList"))
        for(sint32 i = 0, iend = Self.mPool.Count(); i < iend; ++i)
        {
            chararray GetFilePath;
            if(auto CurSource = Self.mPool.AccessByOrder(i, &GetFilePath))
            if(auto File = Platform::File::OpenForRead(&GetFilePath[0]))
            {
                CurSource->mShortName = String::FromWChars(Platform::File::GetShortName(WString::FromChars(&GetFilePath[0])));
                hook(fish(CurSource->mShortName))
                {
                    sint32 FileSize = Platform::File::Size(File);
                    auto FileBuffer = Buffer::Alloc(BOSS_DBG FileSize);
                    Platform::File::Read(File, (uint08*) FileBuffer, FileSize);
                    Platform::File::Close(File);

                    // 파싱
                    chars Focus = (chars) FileBuffer;
                    chars FocusOld = Focus;
                    chars FocusComment = Focus;
                    chars FocusEnd = Focus + FileSize;
                    sint32 AreaLevel = 0;
                    const sint32 CommentCodeLength = (CurSource->mType == SourceFile::Type::Source)? 2 : 1; // "//" or "#"
                    ApilistElement* LastApi = nullptr;
                    while(Focus < FocusEnd)
                    {
                        if(*Focus == '\n')
                        {
                            auto& NewLine = CurSource->mLines.AtAdding();
                            enum class Match {Null, Api, Check};
                            Match MatchResult = Match::Null;
                            if(FocusOld <= FocusComment && FocusComment < Focus)
                            {
                                String NewBrief;
                                String NewAuthor;
                                String NewDate;
                                if(FocusComment[0] == '/' && FocusComment[1] == ' ' && FocusComment[2] == '@')
                                {
                                    MatchResult = Match::Api;
                                    if(FocusComment[3] == 'b' && FocusComment[4] == 'r' && FocusComment[5] == 'i'
                                        && FocusComment[6] == 'e' && FocusComment[7] == 'f')
                                        NewBrief = String(FocusComment + 8, (Focus - ((Focus[-1] == '\r')? 1 : 0)) - (FocusComment + 8)).Trim();
                                }
                                else if(isalpha(FocusComment[0] & 0xFF) && isalpha(FocusComment[1] & 0xFF)
                                    && isdigit(FocusComment[2] & 0xFF) && isdigit(FocusComment[3] & 0xFF)
                                    && isdigit(FocusComment[4] & 0xFF) && isdigit(FocusComment[5] & 0xFF)
                                    && isdigit(FocusComment[6] & 0xFF) && isdigit(FocusComment[7] & 0xFF)
                                    && FocusComment[8] == '-')
                                {
                                    MatchResult = Match::Check;
                                    NewAuthor = String::Format("%.*s", 2, &FocusComment[0]);
                                    NewDate = String::Format("20%.*s/%.*s/%.*s",
                                        2, &FocusComment[2], 2, &FocusComment[4], 2, &FocusComment[6]);
                                }
                                else
                                {
                                    chars FocusCheck = FocusComment;
                                    while(isalpha(*FocusCheck & 0xFF))
                                        FocusCheck++;
                                    if(FocusCheck[0] == ':' && FocusCheck[1] == ' ')
                                    {
                                        MatchResult = Match::Check;
                                        NewAuthor = "author";
                                        NewDate = "whenever";
                                    }
                                }

                                if(MatchResult == Match::Api)
                                {
                                    NewLine.mText = String(FocusOld, FocusComment - (FocusOld + CommentCodeLength));
                                    NewLine.mCommentType = SourceLine::CommentType::Api;
                                    NewLine.mComment = String(FocusComment + 3, (Focus - ((Focus[-1] == '\r')? 1 : 0)) - (FocusComment + 3));
                                    NewLine.mAreaLevel = AreaLevel;

                                    // 체크추가
                                    if(0 < NewBrief.Length())
                                    {
                                        NewLine.mRefID = Self.mApilist.Count();

                                        auto& NewApi = Self.mApilist.AtAdding();
                                        NewApi.mFilePath = &GetFilePath[0];
                                        NewApi.mShortName = CurSource->mShortName;
                                        NewApi.mCodeLine = CurSource->mLines.Count() - 1;
                                        NewApi.mTitle = "대상없음";
                                        NewApi.mBrief = NewBrief;
                                        LastApi = &NewApi;
                                    }
                                }
                                else if(MatchResult == Match::Check)
                                {
                                    NewLine.mText = String(FocusOld, FocusComment - (FocusOld + CommentCodeLength));
                                    NewLine.mCommentType = SourceLine::CommentType::Check;
                                    NewLine.mComment = String(FocusComment, (Focus - ((Focus[-1] == '\r')? 1 : 0)) - FocusComment);
                                    NewLine.mAreaLevel = AreaLevel;
                                    NewLine.mRefID = Self.mChecklist.Count();
                                    // 영역레벨 증가
                                    if(NewLine.mComment.Find(0, ":{") != -1)
                                        AreaLevel++;

                                    // 체크추가
                                    auto& NewCheck = Self.mChecklist.AtAdding();
                                    NewCheck.mFilePath = &GetFilePath[0];
                                    NewCheck.mShortName = CurSource->mShortName;
                                    NewCheck.mCodeLine = CurSource->mLines.Count() - 1;
                                    NewCheck.mAuthor = NewAuthor;
                                    NewCheck.mDate = NewDate;

                                    auto TestCheck = [](const Context& json, String& date, String& author, const String& key)->bool
                                    {
                                        hook(json(date))
                                        hook(fish(author))
                                        if(fish(key).GetInt(0))
                                            return true;

                                        // 검색을 통해 확인
                                        if(!date.Compare("whenever") && !author.Compare("author"))
                                        for(sint32 i = 0, iend = json.LengthOfNamable(); i < iend; ++i)
                                        {
                                            chararray CurDate;
                                            hook(json(i, &CurDate))
                                            for(sint32 j = 0, jend = fish.LengthOfNamable(); j < jend; ++j)
                                            {
                                                chararray CurAuthor;
                                                hook(fish(j, &CurAuthor))
                                                if(fish(key).GetInt(0))
                                                {
                                                    // 기존 정보를 유지
                                                    date = CurDate;
                                                    author = CurAuthor;
                                                    return true;
                                                }
                                            }
                                        }
                                        return false;
                                    };

                                    // 주석내용 구성 및 체크여부 확인
                                    const sint32 KorPos = NewLine.mComment.Find(0, "(kor)");
                                    if(KorPos != -1)
                                    {
                                        const String Comment = ((chars) NewLine.mComment) + KorPos + 5;
                                        NewCheck.mKorText = Comment.Trim();
                                        NewCheck.mChecked = TestCheck(fish, NewCheck.mDate, NewCheck.mAuthor, NewCheck.mKorText);
                                    }
                                    else
                                    {
                                        const sint32 EngPos = NewLine.mComment.Find(0, "(eng)");
                                        if(EngPos != -1)
                                        {
                                            const String Comment = ((chars) NewLine.mComment) + EngPos + 5;
                                            NewCheck.mEngText = Comment.Trim();
                                            NewCheck.mChecked = TestCheck(fish, NewCheck.mDate, NewCheck.mAuthor, NewCheck.mEngText);
                                        }
                                        else
                                        {
                                            sint32 ColonPos = NewLine.mComment.Find(0, ":");
                                            if(ColonPos != -1)
                                            {
                                                if(NewLine.mComment[ColonPos + 1] == '{')
                                                    ColonPos++;
                                                const String Comment = ((chars) NewLine.mComment) + ColonPos + 1;
                                                NewCheck.mEngText = Comment.Trim();
                                                NewCheck.mChecked = TestCheck(fish, NewCheck.mDate, NewCheck.mAuthor, NewCheck.mEngText);
                                            }
                                        }
                                    }
                                }
                            }

                            if(MatchResult == Match::Null)
                            {
                                NewLine.mText = String(FocusOld, Focus - FocusOld - ((Focus[-1] == '\r')? 1 : 0));
                                NewLine.mAreaLevel = AreaLevel;
                                // 영역레벨 감소
                                if(CurSource->mType == SourceFile::Type::Project)
                                {
                                    if(NewLine.mText.Find(0, "#}") != -1)
                                        AreaLevel = Math::Max(0, AreaLevel - 1);
                                }
                                else
                                {
                                    if(NewLine.mText.Find(0, "//}") != -1)
                                        AreaLevel = Math::Max(0, AreaLevel - 1);
                                }
                            }

                            // Api의 타이틀추가
                            if(MatchResult != Match::Api && LastApi)
                            {
                                sint32 ApiLevel = 0;
                                for(sint32 j = 0, jend = NewLine.mText.Length(); j < jend; ++j)
                                {
                                    if(NewLine.mText[j] == ' ')
                                        ApiLevel++;
                                    else break;
                                }

                                String NewTitle = NewLine.mText.Trim();
                                if(0 < NewTitle.Length() && NewTitle[0] != '#' && NewTitle[0] != '/')
                                {
                                    // 세미콜론 제거
                                    if(NewTitle[-2] == ';')
                                        NewTitle.SubTail(1);
                                    // 중괄호 제거
                                    const sint32 Pos = NewTitle.Find(0, "{");
                                    if(Pos != -1)
                                         NewTitle.SubTail(NewTitle.Length() - Pos);
                                    // 파라미터 제거
                                    for(sint32 j = 0, jnext = 0; jnext != -1; j = jnext + 1)
                                    {
                                        jnext = NewTitle.Find(j, "(");
                                        if(jnext != -1 && NewTitle[jnext + 1] != ')')
                                        {
                                            const sint32 Pos = NewTitle.Find(jnext + 1, ")");
                                            if(Pos != -1)
                                                NewTitle = NewTitle.Left(jnext) + "(..." + NewTitle.Right(NewTitle.Length() - Pos);
                                        }
                                    }
                                    // 좌우공백 제거
                                    NewTitle = NewTitle.Trim();

                                    // 입력
                                    if(0 < NewTitle.Length())
                                    {
                                        LastApi->mTitle = NewTitle;
                                        LastApi->mBriefType = (!NewTitle.Left(6).Compare("class "))?
                                            ApilistElement::BriefType::Class : ApilistElement::BriefType::Method;
                                        LastApi->mLevel = ApiLevel;
                                    }
                                }
                                LastApi = nullptr;
                            }

                            FocusOld = ++Focus;
                        }
                        else if(CurSource->mType == SourceFile::Type::Project)
                        {
                            if(*Focus == '#')
                                FocusComment = Focus += CommentCodeLength;
                            else Focus++;
                        }
                        else
                        {
                            if(Focus[0] == '/' && Focus[1] == '/')
                                FocusComment = Focus += CommentCodeLength;
                            else Focus++;
                        }
                    }
                    Buffer::Free(FileBuffer);
                }
            }
            Self.mLoadPos++;
            if(Tasking::IsReleased(common))
            {
                self = nullptr;
                return -1;
            }
        }

        self = nullptr;
        // 로딩완료된 장면을 화면에 보여주기 위해 잠시 기다림
        Platform::Utility::Sleep(500, true, true);
        return -1;
    }, (buffer) this);
}

void zayreviewerData::RenderSourceView(ZayPanel& panel)
{
    if(mFileFocus)
    {
        const auto& Lines = mFileFocus->mLines;
        const sint32 ScrollPos = sint32(mFileFocus->mScrollPos + 0.5);
        const sint32 iBegin = Math::Clamp(ScrollPos / LineSize, 0, Lines.Count() - 1);
        const sint32 iEnd = Math::Clamp((ScrollPos + panel.h() + LineSize - 1) / LineSize, 0, Lines.Count());

        ZAY_FREEFONT(panel, LineSize - 5, "coding")
        {
            const sint32 NumberWidth = Platform::Graphics::GetStringWidth("0");
            sint32 NumberTabSize = NumberWidth + 10;
            sint32 LineCountTest = Lines.Count();
            while(10 <= LineCountTest)
            {
                NumberTabSize += NumberWidth;
                LineCountTest /= 10;
            }

            for(sint32 i = iBegin; i < iEnd; ++i)
            {
                ZAY_XYWH(panel, 0, LineSize * i - ScrollPos, panel.w(), LineSize)
                {
                    // 넘버링
                    ZAY_LTRB(panel, 0, 0, NumberTabSize - 5, panel.h())
                    {
                        ZAY_RGB(panel, 128, 128, 128)
                            panel.text(String::FromInteger(i), UIFA_RightMiddle);
                    }

                    // 본문
                    ZAY_LTRB(panel, NumberTabSize, 0, panel.w(), panel.h())
                    {
                        ZAY_RGBA(panel, 240, 245, 255, (i & 1)? 230 : 200)
                            panel.fill();
                        if(auto Level = Lines[i].mAreaLevel)
                            ZAY_RGBA(panel, 128, Math::Max(0, 255 - 32 * Level), Math::Min(16 * Level, 255), 96)
                                panel.fill();

                        ZAY_LTRB(panel, 5, 0, panel.w(), panel.h())
                        {
                            // 코드
                            ZAY_RGB(panel, 0, 0, 0)
                                panel.text(Lines[i].mText, UIFA_LeftMiddle);
                            // 주석
                            if(0 < Lines[i].mComment.Length())
                            {
                                const sint32 TextWidth = Platform::Graphics::GetStringWidth(Lines[i].mText);
                                const sint32 CommentWidth = Platform::Graphics::GetStringWidth(Lines[i].mComment) + 10;
                                ZAY_XYWH(panel, TextWidth, 2, CommentWidth, panel.h() - 4)
                                {
                                    if(Lines[i].mCommentType == SourceLine::CommentType::Api)
                                    {
                                        ApilistElement::BriefType CurBriefType = ApilistElement::BriefType::Null;
                                        if(Lines[i].mRefID != -1)
                                            CurBriefType = mApilist[Lines[i].mRefID].mBriefType;

                                        ZAY_INNER(panel, -2)
                                        ZAY_RGBA_IF(panel, 64, 96, 255, 96, CurBriefType == ApilistElement::BriefType::Class)
                                        ZAY_RGBA_IF(panel, 255, 128, 255, 96, CurBriefType == ApilistElement::BriefType::Method)
                                        ZAY_RGBA_IF(panel, 255, 255, 0, 96, CurBriefType == ApilistElement::BriefType::Null)
                                            panel.fill();
                                        ZAY_INNER(panel, -1)
                                        ZAY_RGBA_IF(panel, 0, 0, 0, 128, CurBriefType != ApilistElement::BriefType::Null)
                                        ZAY_RGBA_IF(panel, 0, 0, 0, 6, CurBriefType == ApilistElement::BriefType::Null)
                                            panel.rect(1);
                                        ZAY_RGB_IF(panel, 0, 0, 0, CurBriefType != ApilistElement::BriefType::Null)
                                        ZAY_RGB_IF(panel, 0, 0, 192, CurBriefType == ApilistElement::BriefType::Null)
                                            panel.text(Lines[i].mComment);
                                    }
                                    else if(Lines[i].mCommentType == SourceLine::CommentType::Check)
                                    {
                                        ZAY_RGBA(panel, 0, 255, 0, 96)
                                            panel.fill();
                                        ZAY_RGB(panel, 0, 64, 0)
                                            panel.text(Lines[i].mComment);
                                        ZAY_RGB(panel, 0, 0, 0)
                                            panel.rect(1);

                                        // 리뷰체크
                                        if(Lines[i].mRefID != -1)
                                        {
                                            const bool Checked = mChecklist[Lines[i].mRefID].mChecked;
                                            const String UIName = String::Format("Check-%d", i);
                                            ZAY_XYWH_UI(panel, panel.w() + 5, 2, 30, panel.h() - 4, UIName,
                                                ZAY_GESTURE_T(t, this, i)
                                                {
                                                    if(t == GT_Pressed)
                                                    {
                                                        const sint32 CheckBoxAreaSize = 10 + CheckBoxSize;
                                                        mChecklistScrollTarget = CheckBoxAreaSize * mFileFocus->mLines[i].mRefID;
                                                    }
                                                    else if(t == GT_InReleased)
                                                    {
                                                        if(mFileFocus)
                                                            mChecklist.At(mFileFocus->mLines[i].mRefID).mChecked ^= true;
                                                    }
                                                })
                                            {
                                                const bool Focused = !!(panel.state(UIName) & PS_Focused);
                                                const bool Pressed = !!(panel.state(UIName) & PS_Dragging);
                                                if(!Checked)
                                                {
                                                    const sint32 PressMove = (Pressed)? 1 : 0;
                                                    ZAY_RGBA(panel, 255, 0, 0, (Focused)? 128 : 96)
                                                        panel.fill();
                                                    ZAY_FREEFONT(panel, 10, "coding")
                                                    ZAY_RGB(panel, 128, 0, 0)
                                                    {
                                                        ZAY_MOVE(panel, PressMove, PressMove)
                                                            panel.text("click");
                                                        panel.rect(1);
                                                    }
                                                }
                                                else ZAY_FREEFONT(panel, 30, "talking")
                                                {
                                                    const sint32 PressMove = (Pressed)? 2 : 0;
                                                    ZAY_MOVE(panel, 1, 1 + 3)
                                                    ZAY_RGBA(panel, 0, 0, 0, 96)
                                                        panel.text("CHECK!", UIFA_CenterBottom);
                                                    ZAY_MOVE(panel, -1, -1 + 3)
                                                    ZAY_MOVE(panel, PressMove, PressMove)
                                                    ZAY_RGB(panel, 0, 0, 0)
                                                        panel.text("CHECK!", UIFA_CenterBottom);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // 파일명 표시
        ZAY_FREEFONT(panel, 20, "coding")
        {
            const sint32 Width = 10 + Platform::Graphics::GetStringWidth(mFileFocus->mShortName);
            ZAY_XYWH(panel, panel.w() - 200 - 10 - Width, 10, Width, 20 + 10)
            {
                ZAY_RGBA_IF(panel, 0, 0, 64, 96, mFileFocus->mType == SourceFile::Type::Source)
                ZAY_RGBA_IF(panel, 64, 0, 0, 96, mFileFocus->mType == SourceFile::Type::Project)
                    panel.fill();
                ZAY_RGB(panel, 255, 255, 255)
                    panel.text(mFileFocus->mShortName);
                ZAY_RGBA_IF(panel, 0, 0, 64, 64, mFileFocus->mType == SourceFile::Type::Source)
                ZAY_RGBA_IF(panel, 64, 0, 0, 64, mFileFocus->mType == SourceFile::Type::Project)
                    panel.rect(2);
            }
        }

        // 스크롤한계처리
        auto CheckScrollMax = [this](float value, sint32 height)->float
        {
            if(value < 0)
            {
                value = 0;
                invalidate(2);
            }
            else if(0 < value)
            {
                const sint32 ScrollMax = LineSize * mFileFocus->mLines.Count() - height;
                if(value > ScrollMax)
                {
                    value = ScrollMax;
                    invalidate(2);
                }
            }
            return value;
        };
        mFileFocus->mScrollPos = CheckScrollMax(mFileFocus->mScrollPos, panel.h());
        mFileFocus->mScrollTarget = CheckScrollMax(mFileFocus->mScrollTarget, panel.h());

        // 부드러운 스크롤이동
        if(mFileFocus->mScrollPos != mFileFocus->mScrollTarget)
        {
            if(Math::AbsF(mFileFocus->mScrollPos - mFileFocus->mScrollTarget) < 1)
                mFileFocus->mScrollPos = mFileFocus->mScrollTarget;
            else mFileFocus->mScrollPos = mFileFocus->mScrollPos * 0.9 + mFileFocus->mScrollTarget * 0.1;
            invalidate(2);
        }
    }
}

void zayreviewerData::RenderAPIView(ZayPanel& panel)
{
    const sint32 ApiBoxAreaSize = 10 + ApiBoxSize;
    const sint32 ScrollPos = sint32(mApilistScrollPos + 0.5);
    const sint32 iBegin = Math::Clamp(ScrollPos / ApiBoxAreaSize, 0, mApilist.Count() - 1);
    const sint32 iEnd = Math::Clamp((ScrollPos + panel.h() + ApiBoxAreaSize - 1) / ApiBoxAreaSize, 0, mApilist.Count());

    for(sint32 i = iBegin; i < iEnd; ++i)
    {
        const auto& CurApi = mApilist[i];
        const String UIName = String::Format("Api-%d", i);
        const bool Focused = !!(panel.state(UIName) & PS_Focused);
        const bool Dropping = !!(panel.state(UIName) & PS_Dropping);

        ZAY_LTRB(panel, 10 * CurApi.mLevel, ApiBoxAreaSize * i - ScrollPos, panel.w(), ApiBoxAreaSize * (i + 1) - ScrollPos)
        ZAY_LTRB_UI(panel, 10, panel.h() - ApiBoxSize, panel.w() - 10, panel.h(), UIName,
            ZAY_GESTURE_TXY(t, x, y, this, i)
            {
                static sint32 OldY = 0;
                if(t == GT_Pressed)
                {
                    OldY = y;
                    if(i < mApilist.Count())
                    if(mFileFocus = mPool.Access(mApilist[i].mFilePath))
                        mFileFocus->mScrollTarget = LineSize * mApilist[i].mCodeLine;
                }
                else if(t == GT_InDragging || t == GT_OutDragging)
                {
                    mApilistScrollTarget += OldY - y;
                    OldY = y;
                    invalidate();
                }
                else if(t == GT_WheelDown || t == GT_WheelUp)
                {
                    mApilistScrollTarget += (t == GT_WheelDown)? 100 : -100;
                    invalidate();
                }
            })
        {
            // 그림자
            ZAY_MOVE(panel, 2, 2)
            ZAY_RGBA(panel, 0, 0, 0, 96)
                panel.fill();

            // 배경색
            ZAY_RGBA_IF(panel, 196, 224, 255, 192, CurApi.mBriefType == ApilistElement::BriefType::Class)
            ZAY_RGBA_IF(panel, 255, 224, 255, 192, CurApi.mBriefType == ApilistElement::BriefType::Method)
            ZAY_RGBA_IF(panel, 192, 192, 192, 192, CurApi.mBriefType == ApilistElement::BriefType::Null)
                panel.fill();

            // 타이틀
            ZAY_LTRB_SCISSOR(panel, 5, 0, panel.w(), ApiBoxTitleSize)
            {
                ZAY_FREEFONT(panel, 13, "coding-bold")
                ZAY_RGB_IF(panel, 0, 0, 0, CurApi.mBriefType != ApilistElement::BriefType::Null)
                ZAY_RGBA_IF(panel, 255, 0, 0, 96, CurApi.mBriefType == ApilistElement::BriefType::Null)
                    panel.text(CurApi.mTitle, UIFA_LeftMiddle);
            }

            // 내용
            ZAY_LTRB_SCISSOR(panel, 0, ApiBoxTitleSize, panel.w(), panel.h())
            {
                ZAY_RGBA(panel, 0, 0, 0, 128)
                    panel.fill();
                ZAY_LTRB(panel, 5, 0, panel.w(), panel.h() - 2)
                ZAY_FREEFONT(panel, 10, "coding")
                ZAY_RGBA(panel, 255, 255, 128, 224)
                    panel.text(CurApi.mBrief, UIFA_LeftBottom);
            }

            if(Focused && !Dropping) // 포커스처리
            ZAY_RGBA(panel, 255, 255, 128, 96)
                panel.fill();

            ZAY_RGB(panel, 0, 0, 0)
                panel.rect(1);
        }
    }

    // 스크롤한계처리
    auto ApiScrollMax = [this](float value, sint32 height)->float
    {
        if(value < 0)
        {
            value = 0;
            invalidate(2);
        }
        else if(0 < value)
        {
            const sint32 ApiBoxAreaSize = 10 + ApiBoxSize;
            const sint32 ScrollMax = ApiBoxAreaSize * mApilist.Count() + 10 - height;
            if(value > ScrollMax)
            {
                value = ScrollMax;
                invalidate(2);
            }
        }
        return value;
    };
    mApilistScrollPos = ApiScrollMax(mApilistScrollPos, panel.h());
    mApilistScrollTarget = ApiScrollMax(mApilistScrollTarget, panel.h());

    // 부드러운 스크롤이동
    if(mApilistScrollPos != mApilistScrollTarget)
    {
        if(Math::AbsF(mApilistScrollPos - mApilistScrollTarget) < 1)
            mApilistScrollPos = mApilistScrollTarget;
        else mApilistScrollPos = mApilistScrollPos * 0.9 + mApilistScrollTarget * 0.1;
        invalidate(2);
    }
}

void zayreviewerData::RenderChecklistView(ZayPanel& panel)
{
    const sint32 CheckBoxAreaSize = 10 + CheckBoxSize;
    const sint32 ScrollPos = sint32(mChecklistScrollPos + 0.5);
    const sint32 iBegin = Math::Clamp(ScrollPos / CheckBoxAreaSize, 0, mChecklist.Count() - 1);
    const sint32 iEnd = Math::Clamp((ScrollPos + panel.h() + CheckBoxAreaSize - 1) / CheckBoxAreaSize, 0, mChecklist.Count());

    for(sint32 i = iBegin; i < iEnd; ++i)
    {
        const auto& CurCheck = mChecklist[i];
        const String UIName = String::Format("Check-%d", i);
        const bool Focused = !!(panel.state(UIName) & PS_Focused);
        const bool Dropping = !!(panel.state(UIName) & PS_Dropping);

        ZAY_XYWH(panel, 0, CheckBoxAreaSize * i - ScrollPos, panel.w(), CheckBoxAreaSize)
        ZAY_LTRB_UI(panel, 10, panel.h() - CheckBoxSize, panel.w() - 10, panel.h(), UIName,
            ZAY_GESTURE_TXY(t, x, y, this, i)
            {
                static sint32 OldY = 0;
                if(t == GT_Pressed)
                {
                    OldY = y;
                    if(i < mChecklist.Count())
                    if(mFileFocus = mPool.Access(mChecklist[i].mFilePath))
                        mFileFocus->mScrollTarget = LineSize * mChecklist[i].mCodeLine;
                }
                else if(t == GT_InDragging || t == GT_OutDragging)
                {
                    mChecklistScrollTarget += OldY - y;
                    OldY = y;
                    invalidate();
                }
                else if(t == GT_WheelDown || t == GT_WheelUp)
                {
                    mChecklistScrollTarget += (t == GT_WheelDown)? 100 : -100;
                    invalidate();
                }
            })
        {
            // 그림자
            if(!CurCheck.mChecked)
            ZAY_MOVE(panel, 2, 2)
            ZAY_RGBA(panel, 0, 0, 0, 96)
                panel.fill();

            // 배경색
            ZAY_RGBA(panel, 224, 255, 224, 192)
                panel.fill();

            // 타이틀
            ZAY_LTRB_SCISSOR(panel, 0, 0, panel.w(), CheckBoxTitleSize)
            {
                // 썸네일
                ZAY_LTRB(panel, 0, 0, CheckBoxPicSize, panel.h())
                {
                    ZAY_RGBA(panel, 0, 0, 0, (CurCheck.mChecked)? 64 : 255)
                        panel.fill();
                    ZAY_XYWH(panel, 0, 0, panel.w() - 1, panel.h() - 1)
                        panel.stretch(GetUserPic(CurCheck.mAuthor), Image::Build::Force, UISF_Inner);
                }
                // 이름과 작성일자
                ZAY_LTRB(panel, CheckBoxPicSize + 5, 0, panel.w(), panel.h())
                {
                    const String Title = String::Format("%s - %s", (chars) GetUserName(CurCheck.mAuthor), (chars) CurCheck.mDate);
                    ZAY_FREEFONT(panel, 15, "coding-bold")
                    ZAY_RGBA(panel, 0, 0, 64, 160)
                        panel.text(Title, UIFA_LeftMiddle);
                }
                if(Focused && !Dropping) // 포커스처리
                ZAY_RGBA(panel, 255, 255, 128, 96)
                    panel.fill();
            }

            // 내용
            ZAY_LTRB_SCISSOR(panel, 0, CheckBoxTitleSize, panel.w(), panel.h())
            {
                ZAY_RGBA(panel, 0, 0, 0, 32)
                    panel.fill();

                // 출처
                ZAY_LTRB(panel, 0, panel.h() - CheckBoxDirInfoSize, panel.w(), panel.h())
                {
                    ZAY_RGBA(panel, 0, 0, 0, 64)
                        panel.fill();
                    ZAY_LTRB(panel, 0, 0, panel.w() - 5, panel.h() - 2)
                    ZAY_FREEFONT(panel, 10, "coding")
                    ZAY_RGBA(panel, 255, 255, 255, 224)
                        panel.text(CurCheck.mShortName, UIFA_RightBottom);
                }

                if(Focused && !Dropping) // 포커스처리
                ZAY_RGBA(panel, 255, 255, 128, 96)
                    panel.fill();

                // 코멘트
                ZAY_LTRB(panel, 10, 0, panel.w(), panel.h() - CheckBoxDirInfoSize)
                ZAY_FREEFONT(panel, 17, "talking")
                {
                    if(0 < CurCheck.mKorText.Length())
                    {
                        ZAY_RGB(panel, 0, 0, 0)
                            panel.text(CurCheck.mKorText, UIFA_LeftMiddle);
                    }
                    else if(0 < CurCheck.mEngText.Length())
                    {
                        ZAY_RGB(panel, 0, 0, 128)
                            panel.text(CurCheck.mEngText, UIFA_LeftMiddle);
                    }
                }
            }
            ZAY_RGBA(panel, 0, 0, 0, (CurCheck.mChecked)? 64 : 255)
                panel.rect(1);

            // 체크된 상황
            if(CurCheck.mChecked)
            ZAY_RGBA(panel, 144, 144, 144, 224)
                panel.fill();
        }
    }

    // 스크롤한계처리
    auto CheckScrollMax = [this](float value, sint32 height)->float
    {
        if(value < 0)
        {
            value = 0;
            invalidate(2);
        }
        else if(0 < value)
        {
            const sint32 CheckBoxAreaSize = 10 + CheckBoxSize;
            const sint32 ScrollMax = CheckBoxAreaSize * mChecklist.Count() + 10 - height;
            if(value > ScrollMax)
            {
                value = ScrollMax;
                invalidate(2);
            }
        }
        return value;
    };
    mChecklistScrollPos = CheckScrollMax(mChecklistScrollPos, panel.h());
    mChecklistScrollTarget = CheckScrollMax(mChecklistScrollTarget, panel.h());

    // 부드러운 스크롤이동
    if(mChecklistScrollPos != mChecklistScrollTarget)
    {
        if(Math::AbsF(mChecklistScrollPos - mChecklistScrollTarget) < 1)
            mChecklistScrollPos = mChecklistScrollTarget;
        else mChecklistScrollPos = mChecklistScrollPos * 0.9 + mChecklistScrollTarget * 0.1;
        invalidate(2);
    }
}

void zayreviewerData::CollectSourceFile(chars dirpath)
{
    Platform::File::Search(dirpath, [](chars itemname, payload data)->void
    {
        auto& Self = *((zayreviewerData*) data);
        if(Platform::File::ExistForDir(itemname))
            Self.CollectSourceFile(itemname);
        else
        {
            const String FileName = itemname;
            if(!String::CompareNoCase(".h", FileName.Right(2))
            || !String::CompareNoCase(".hpp", FileName.Right(4))
            || !String::CompareNoCase(".c", FileName.Right(2))
            || !String::CompareNoCase(".cc", FileName.Right(3))
            || !String::CompareNoCase(".cpp", FileName.Right(4)))
                Self.mPool(itemname).mType = SourceFile::Type::Source;
            else if(!String::CompareNoCase("/CMakeLists.txt", FileName.Right(15)))
                Self.mPool(itemname).mType = SourceFile::Type::Project;
        }
    }, this, true);
}

Image& zayreviewerData::GetUserPic(chars author)
{
    if(auto OldPic = mUserPics.Access(author))
        return *OldPic;

    auto& NewPic = mUserPics(author);
    auto AssetPath = String::Format("user/%s.png", author);
    if(Asset::Exist(AssetPath))
        NewPic.SetName(AssetPath).Load();
    else NewPic.SetName("image/noimg.png").Load();
    return NewPic;
}

String& zayreviewerData::GetUserName(chars author)
{
    if(auto OldName = mUserNames.Access(author))
        return *OldName;

    auto& NewName = mUserNames(author);
    auto AssetPath = String::Format("user/%s.txt", author);
    if(Asset::Exist(AssetPath))
        NewName = Context(ST_Json, SO_OnlyReference, String::FromAsset(AssetPath))("name").GetText("정보없음");
    else NewName = "파일없음";
    return NewName;
}
