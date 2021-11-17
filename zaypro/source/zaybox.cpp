#include <boss.hpp>
#include "zaybox.hpp"

#include <service/boss_zayson.hpp>
#include <resource.hpp>

////////////////////////////////////////////////////////////////////////////////
// ZEZayBox
////////////////////////////////////////////////////////////////////////////////
ZEZayBox::ZEZayBox()
{
    mCompType = "null";
    mCompID = -2;
    mParent = -1;
    mOrder = -1;
    mExpanded = true;
    mHooked = false;
    mHookPos = Point(-BallX, 0);
    mRemovingCount = 0;
}

ZEZayBox::~ZEZayBox()
{
}

static sint32 gLastZayBoxID = -1;
void ZEZayBox::ResetLastID()
{
    gLastZayBoxID = -1;
}

sint32 ZEZayBox::MakeLastID()
{
    return ++gLastZayBoxID;
}

sint32 ZEZayBox::ValidLastID(sint32 id)
{
    gLastZayBoxID = Math::Max(gLastZayBoxID, id);
    return id;
}

void ZEZayBox::LoadChildren(const Context& json, ZEZayBoxMap& boxmap, CreatorCB cb)
{
    double TopPosX = 0, TopPosY = 0;
    if(const auto TopBox = boxmap.Access(0))
    {
        TopPosX = TopBox->ConstValue().mPosX;
        TopPosY = TopBox->ConstValue().mPosY;
    }

    mChildren.Clear();
    for(sint32 i = 0, iend = json.LengthOfIndexable(); i < iend; ++i)
    {
        hook(json[i])
        {
            String CurCompName;
            if(fish("compname").HasValue())
                CurCompName = fish("compname").GetText();
            else CurCompName = fish.GetText(); // 옛날 포맷으로 쓰여진 조건문인 경우

            // 자식구성
            auto NewChildBox = cb(CurCompName);
            NewChildBox->ReadJson(fish);
            NewChildBox->mExpanded = (fish("expanded").GetInt(1) != 0);
            NewChildBox->mPosX = TopPosX + fish("posx").GetFloat(mPosX + mBodySize.w + mAddW + 30 - TopPosX);
            NewChildBox->mPosY = TopPosY + fish("posy").GetFloat(mPosY + (TitleBarHeight + 20) * i - TopPosY);
            NewChildBox->mAddW = fish("addw").GetInt(0);
            AddChild(NewChildBox.Value()); // 자식의 HookPos설정

            // 자식재귀 및 박스추가
            NewChildBox->LoadChildren(fish("ui"), boxmap, cb);
            boxmap[NewChildBox->mID] = NewChildBox;
        }
    }
}

void ZEZayBox::SaveChildren(Context& json, const ZEZayBoxMap& boxmap) const
{
    double TopPosX = 0, TopPosY = 0;
    if(const auto TopBox = boxmap.Access(0))
    {
        TopPosX = TopBox->ConstValue().mPosX;
        TopPosY = TopBox->ConstValue().mPosY;
    }

    for(sint32 i = 0, iend = mChildren.Count(); i < iend; ++i)
    {
        hook(json.At(i))
        if(auto CurBox = boxmap.Access(mChildren[i]))
        {
            CurBox->ConstValue().WriteJson(fish);
            fish.At("expanded").Set(String::FromInteger((CurBox->ConstValue().mExpanded)? 1 : 0));
            fish.At("posx").Set(String::FromInteger(sint32(CurBox->ConstValue().mPosX - TopPosX + 0.5)));
            fish.At("posy").Set(String::FromInteger(sint32(CurBox->ConstValue().mPosY - TopPosY + 0.5)));
            fish.At("addw").Set(String::FromInteger(CurBox->ConstValue().mAddW));
            if(0 < CurBox->ConstValue().mChildren.Count())
                CurBox->ConstValue().SaveChildren(fish.At("ui"), boxmap);
        }
    }
}

void ZEZayBox::ReadJson(const Context& json)
{
    BOSS_ASSERT("잘못된 시나리오입니다", false);
}

void ZEZayBox::WriteJson(Context& json) const
{
    BOSS_ASSERT("잘못된 시나리오입니다", false);
}

void ZEZayBox::Render(ZayPanel& panel)
{
    BOSS_ASSERT("잘못된 시나리오입니다", false);
}

void ZEZayBox::RecalcSize()
{
    BOSS_ASSERT("잘못된 시나리오입니다", false);
}

void ZEZayBox::SubParam(sint32 i)
{
    BOSS_ASSERT("잘못된 시나리오입니다", false);
}

void ZEZayBox::SubInput(sint32 i)
{
    BOSS_ASSERT("잘못된 시나리오입니다", false);
}

void ZEZayBox::SubExtInput(sint32 i)
{
    BOSS_ASSERT("잘못된 시나리오입니다", false);
}

chars ZEZayBox::GetComment() const
{
    BOSS_ASSERT("잘못된 시나리오입니다", false);
    return "error";
}

void ZEZayBox::Init(sint32 id, chars type, Color color, chars colorres, bool expand, sint32 x, sint32 y)
{
    mID = id;
    mCompType = type;
    mColor = color;
    mColorRes = colorres;
    mExpanded = expand;
    mPosX = x;
    mPosY = y;
    mAddW = 0;
    mBodySize.w = BodyWidth;
    mBodySize.h = 0;
    RecalcSize();
}

void ZEZayBox::InitCompID()
{
    mCompID = MakeLastID();
}

void ZEZayBox::AddChild(ZEZayBox& child)
{
    child.mParent = mID;
    child.mOrder = mChildren.Count();
    child.mHooked = true;
    child.mHookPos = GetBallPos() - Point(child.mPosX, child.mPosY + TitleBarHeight / 2);
    mChildren.AtAdding() = child.mID;
}

void ZEZayBox::SubChild(ZEZayBox& child)
{
    for(sint32 i = 0, iend = mChildren.Count(); i < iend; ++i)
    {
        if(mChildren[i] == child.mID)
        {
            mChildren.SubtractionSection(i);
            break;
        }
    }
    child.ClearMyHook();
}

void ZEZayBox::ChangeChild(ZEZayBox& oldchild, ZEZayBox& newchild)
{
    for(sint32 i = 0, iend = mChildren.Count(); i < iend; ++i)
    {
        if(mChildren[i] == oldchild.mID)
        {
            newchild.mParent = mID;
            newchild.mOrder = i;
            newchild.mHooked = true;
            newchild.mHookPos = GetBallPos() - Point(newchild.mPosX, newchild.mPosY + TitleBarHeight / 2);
            mChildren.At(i) = newchild.mID;
            break;
        }
    }
    oldchild.ClearMyHook();
}

void ZEZayBox::RenderTitle(ZayPanel& panel, chars title, bool hook, bool ball, bool copy, bool expand, bool resize, bool remove)
{
    const String UITitle = String::Format("%d-title", mID);
    const String UIGroupMove = String::Format("%d-groupmove", mID);
    const String UIGroupCopy = String::Format("%d-groupcopy", mID);
    const String UIExpand = String::Format("%d-expand", mID);
    const String UIResize = String::Format("%d-resize", mID);
    const String UIRemove = String::Format("%d-remove", mID);
    const String UIHookRemove = String::Format("%d-hookremove", mID);
    const String UIHook = String::Format("%d-hook", mID);
    const String UIBall = String::Format("%d-ball", mID);
    const sint32 ButtonWidth = 20;
    const bool IsFocusing = !!(panel.state(UITitle) & (PS_Focused | PS_Dragging));
    const bool IsDropping = !!(panel.state(UITitle) & PS_Dropping);

    // 타이틀
    ZAY_XYWH_UI(panel, 0, 0, panel.w(), TitleBarHeight, UITitle,
        ZAY_GESTURE_VNTXY(v, n, t, x, y, this)
        {
            if(t == GT_Moving)
                Platform::SendNotify(v->view(), "Pipe:CompFocusIn", String::FromInteger(mCompID));
            else if(t == GT_MovingLosed)
                Platform::SendNotify(v->view(), "Pipe:CompFocusOut", String());
            else if(t == GT_InDragging || t == GT_OutDragging)
            {
                auto& OldPos = v->oldxy(n);
                mTitleDrag += Point(x - OldPos.x, y - OldPos.y);
                Platform::SendNotify(v->view(), "ZayBoxMove", sint32o(mID));
                v->invalidate();
            }
            else if((t == GT_InReleased || t == GT_OutReleased) && mParent != -1)
                Platform::SendNotify(v->view(), "ZayBoxSort", sint32o(mParent));
        })
    {
        // L로프(후크)
        if(hook)
            RenderHook(panel, UIHook);

        // R로프(볼)
        if(ball)
            RenderBall(panel, UIBall);

        ZAY_LTRB(panel, 6, 4, panel.w() - 4, panel.h() - 4)
        {
            // 타이틀배경
            ZAY_RGB_IF(panel, 160, 160, 160, IsFocusing && !IsDropping)
                panel.ninepatch(R(mColorRes));

            sint32 TitleEndX = panel.w() - 2;
            // 제거버튼
            if(remove)
            {
                TitleEndX -= ButtonWidth;
                ZAY_XYWH(panel, TitleEndX, 0, ButtonWidth, panel.h())
                    RenderRemoveButton(panel, UIRemove, mParent == -1 && 0 < mChildren.Count());
            }
            else
            {
                TitleEndX -= ButtonWidth;
                ZAY_XYWH(panel, TitleEndX, 0, ButtonWidth, panel.h())
                ZAY_RGBA(panel, 128, 128, 128, 20)
                    panel.icon(R("wid_x_n"), UIA_CenterMiddle);
            }
            // 그룹복사버튼
            if(copy)
            {
                TitleEndX -= ButtonWidth - 1;
                ZAY_XYWH(panel, TitleEndX, 0, ButtonWidth, panel.h())
                    RenderGroupCopyButton(panel, UIGroupCopy);
            }
            // 확장/최소화버튼
            if(expand)
            {
                TitleEndX -= ButtonWidth - 1;
                ZAY_XYWH(panel, TitleEndX, 0, ButtonWidth, panel.h())
                    RenderExpandButton(panel, UIExpand);
            }
            // 리사이즈버튼
            if(resize)
            {
                TitleEndX -= ButtonWidth - 1;
                ZAY_XYWH(panel, TitleEndX, 0, ButtonWidth, panel.h())
                    RenderResizeButton(panel, UIResize);
            }
            // 그룹이동버튼
            if(hook && ball)
            {
                TitleEndX -= ButtonWidth - 1;
                ZAY_XYWH(panel, TitleEndX, 0, ButtonWidth, panel.h())
                    RenderGroupMoveButton(panel, UIGroupMove);
            }

            // 타이틀
            ZAY_LTRB(panel, 5 + 3, 0, TitleEndX + 3, panel.h())
            {
                String TitleText((IsFocusing && mOrder != -1)?
                    (chars) String::Format("[%d] %s", mOrder, title) : title);

                // 디버그용 자기ID 표시
                #if !BOSS_NDEBUG
                    TitleText += String::Format(" <%d>", mID);
                #endif

                ZAY_RGB(panel, 255, 255, 255)
                    panel.text(TitleText, UIFA_LeftMiddle, UIFE_Right);
            }
        }

        // 후크제거버튼
        if(mHooked)
        ZAY_XYWH(panel, -ButtonWidth - 6, 3, ButtonWidth, panel.h())
            RenderHookRemoveButton(panel, UIHookRemove);
    }
}

void ZEZayBox::RenderHook(ZayPanel& panel, chars uiname)
{
    const bool IsFocusing = !!(panel.state(uiname) & (PS_Focused | PS_Dragging));
    ZAY_COLOR_IF(panel, mColor, mHooked)
    ZAY_RGB_IF(panel, 80, 80, 80, mHooked)
    ZAY_RGB_IF(panel, 255, 64, 0, !mHooked)
    ZAY_RGB_IF(panel, 96, 96, 96, !mHooked && !IsFocusing)
    {
        ZAY_MOVE(panel, 0, panel.h() / 2)
        {
            Points RopeDots;
            RopeDots.AtAdding() = Point(5, 0);
            RopeDots.AtAdding() = Point(-1, 0);
            RopeDots.AtAdding() = Point(mHookPos.x + mHookDrag.x + 6, mHookPos.y + mHookDrag.y);
            RopeDots.AtAdding() = Point(mHookPos.x + mHookDrag.x, mHookPos.y + mHookDrag.y);
            panel.polybezier(RopeDots, 2, true, false);
        }

        ZAY_XYRR_UI(panel, mHookPos.x, panel.h() / 2 + mHookPos.y, 8, 8, (mHooked)? "" : uiname,
            ZAY_GESTURE_VNTXY(v, n, t, x, y, this)
            {
                if(t == GT_Pressed)
                    Platform::SendNotify(v->view(), "HookPressed", sint32o(mID));
                else if(t == GT_InDragging || t == GT_OutDragging)
                {
                    auto& OldPos = v->oldxy(n);
                    mHookDrag.x += x - OldPos.x;
                    mHookDrag.y += y - OldPos.y;
                    v->invalidate();
                }
                else if(t == GT_InReleased || t == GT_OutReleased)
                {
                    if(mParent == -1)
                    {
                        if(mHookDrag == Point())
                            ClearMyHook();
                        else mHookPos += mHookDrag;
                    }
                    mHookDrag = Point();
                    Platform::SendNotify(v->view(), "HookReleased", nullptr);
                }
            })
        {
            if(mHooked)
            {
                ZAY_INNER(panel, 2)
                {
                    panel.fill();
                    // 드로잉순서에 따른 이질감방지
                    ZAY_RGB(panel, 0, 0, 0)
                    {
                        ZAY_INNER(panel, 2)
                            panel.fill();
                        ZAY_XYRR(panel, 0, panel.h() / 2, 0, 0)
                        ZAY_LTRB(panel, 0, -1, 2, +1)
                            panel.fill();
                    }
                }
            }
            else ZAY_INNER(panel, 5)
            {
                const sint32 W = panel.w(), H = panel.h();
                const sint32 Wh = W / 2, Hh = H / 2;
                const sint32 B = 3;
                Points HookDots;
                HookDots.AtAdding() = Point(0, 0);
                HookDots.AtAdding() = Point(0, H);
                HookDots.AtAdding() = Point(Wh + B, H);
                HookDots.AtAdding() = Point(Wh, H + B);
                HookDots.AtAdding() = Point(-B, H + B);
                HookDots.AtAdding() = Point(-B, -B);
                HookDots.AtAdding() = Point(W + B, -B);
                HookDots.AtAdding() = Point(W + B, Hh + 1);
                HookDots.AtAdding() = Point(W, Hh + 1);
                HookDots.AtAdding() = Point(W, 0);
                ZAY_MOVE(panel, mHookDrag.x, mHookDrag.y)
                    panel.polygon(HookDots);
            }
        }
    }
}

void ZEZayBox::RenderBall(ZayPanel& panel, chars uiname)
{
    const bool IsDropping = !!(panel.state(uiname) & PS_Dropping);
    const bool IsFocused = !!(panel.state(uiname) & PS_Focused);
    ZAY_RGB_IF(panel, 0, 0, 0, !IsDropping)
    ZAY_RGB_IF(panel, 255, 64, 0, IsDropping)
    ZAY_RGB_IF(panel, 192, 192, 192, !IsDropping && IsFocused)
    {
        panel.line(Point(panel.w() - 5, panel.h() / 2), Point(panel.w() + BallX, panel.h() / 2), 2);
        ZAY_XYRR_UI(panel, panel.w() + BallX, panel.h() / 2, 9, 9, uiname,
            ZAY_GESTURE_VNT(v, n, t, this)
            {
                if(t == GT_Dropped)
                {
                    Platform::SendNotify(v->view(), "HookDropped", sint32o(mID));
                }
                else if(t == GT_InReleased)
                {
                    Platform::SendNotify(v->view(), "HookClear", sint32o(mID));
                }
            })
        ZAY_INNER(panel, 5)
            panel.fill();
    }

    // 디버그용 자식ID 표시
    #if !BOSS_NDEBUG
        ZAY_RGB(panel, 0, 0, 0)
        for(sint32 i = 0, iend = mChildren.Count(); i < iend; ++i)
            panel.text(panel.w(), 20 + 15 * i, String::Format(" <%d>", mChildren[i]), UIFA_LeftTop);
    #endif
}

void ZEZayBox::RenderGroupMoveButton(ZayPanel& panel, chars uiname)
{
    const bool IsFocusing = !!(panel.state(uiname) & (PS_Focused | PS_Dragging));
    ZAY_INNER_UI(panel, 0, uiname,
        ZAY_GESTURE_VNTXY(v, n, t, x, y, this)
        {
            if(t == GT_InDragging || t == GT_OutDragging)
            {
                auto& OldPos = v->oldxy(n);
                mTitleDrag += Point(x - OldPos.x, y - OldPos.y);
                Platform::SendNotify(v->view(), "ZayBoxMoveWith", sint32o(mID));
                v->invalidate();
            }
            else if((t == GT_InReleased || t == GT_OutReleased) && mParent != -1)
                Platform::SendNotify(v->view(), "ZayBoxSort", sint32o(mParent));
        })
    {
        if(IsFocusing)
            panel.icon(R("wid_move_o"), UIA_CenterMiddle);
        else panel.icon(R("wid_move_n"), UIA_CenterMiddle);
    }
}

void ZEZayBox::RenderGroupCopyButton(ZayPanel& panel, chars uiname)
{
    const bool IsFocusing = !!(panel.state(uiname) & (PS_Focused | PS_Dragging));
    ZAY_INNER_UI(panel, 0, uiname,
        ZAY_GESTURE_VNTXY(v, n, t, x, y, this)
        {
            static bool Created = false;
            static Point OldPos;
            if(t == GT_Pressed)
            {
                Created = false;
                OldPos = Point(x, y);
            }
            else if(t == GT_InDragging || t == GT_OutDragging)
            {
                if(t == GT_OutDragging && !Created)
                {
                    Created = true;
                    Platform::SendNotify(v->view(), "ZayBoxCopy", sint32o(mID));
                    v->invalidate();
                }
                if(Created)
                {
                    mTitleDrag += Point(x - OldPos.x, y - OldPos.y);
                    OldPos = Point(x, y);
                    Platform::SendNotify(v->view(), "ZayBoxMoveWith", sint32o(mID));
                    v->invalidate();
                }
            }
        })
    {
        if(IsFocusing)
            panel.icon(R("wid_copy_o"), UIA_CenterMiddle);
        else panel.icon(R("wid_copy_n"), UIA_CenterMiddle);
    }
}

void ZEZayBox::RenderExpandButton(ZayPanel& panel, chars uiname)
{
    const bool IsFocused = !!(panel.state(uiname) & PS_Focused);
    ZAY_INNER_UI(panel, 0, uiname,
        ZAY_GESTURE_T(t, this)
        {
            if(t == GT_InReleased)
                mExpanded ^= true;
        })
    ZAY_INNER(panel, 3)
    {
        ZAY_RGBA(panel, 0, 0, 0, 32)
        ZAY_RGBA_IF(panel, 128, 128, 128, 144, IsFocused)
            panel.fill();
        ZAY_RGB(panel, 0, 0, 0)
            panel.text((mExpanded)? "∧" : "∨", UIFA_CenterMiddle);
        ZAY_RGBA(panel, 0, 0, 0, 64)
            panel.rect(1);
    }
}

void ZEZayBox::RenderResizeButton(ZayPanel& panel, chars uiname)
{
    const bool IsFocused = !!(panel.state(uiname) & PS_Focused);
    ZAY_INNER_UI(panel, 0, uiname,
        ZAY_GESTURE_VNTXY(v, n, t, x, y, this)
        {
            if(t == GT_InDragging || t == GT_OutDragging)
            {
                auto& OldPos = v->oldxy(n);
                sint32s Values;
                Values.AtAdding() = mID;
                Values.AtAdding() = sint32(x - OldPos.x);
                Platform::SendNotify(v->view(), "ZayBoxResize", Values);
                v->invalidate();
            }
        })
    ZAY_INNER(panel, 3)
    {
        ZAY_RGBA(panel, 0, 0, 0, 32)
        ZAY_RGBA_IF(panel, 128, 128, 128, 144, IsFocused)
            panel.fill();
        ZAY_RGB(panel, 0, 0, 0)
            panel.text("⇔", UIFA_CenterMiddle);
        ZAY_RGBA(panel, 0, 0, 0, 64)
            panel.rect(1);
    }
}

void ZEZayBox::RenderRemoveButton(ZayPanel& panel, chars uiname, bool group)
{
    const sint32 AniCount = 50;
    const bool Removing = (!mRemovingUIName.Compare(uiname));
    const bool Enable = (!Removing || 0 < mRemovingCount);
    const float AniValue = (Removing)? (AniCount - mRemovingCount) / (float) AniCount : 0;
    if(Removing && 0 < mRemovingCount)
        mRemovingCount--;

    const bool IsFocused = ((panel.state(uiname) & (PS_Focused | PS_Dropping)) == PS_Focused);
    const bool IsPressed = ((panel.state(uiname) & (PS_Pressed | PS_Dragging)) != 0);
    ZAY_INNER_UI(panel, 0, uiname,
        ZAY_GESTURE_VNT(v, n, t, this, group, AniCount)
        {
            if(t == GT_Pressed)
            {
                mRemovingUIName = n;
                mRemovingCount = AniCount;
                Platform::SendNotify(v->view(), "Update", sint32o(mRemovingCount));
            }
            else if(t == GT_InReleased || t == GT_OutReleased)
            {
                if(mRemovingCount == 0)
                {
                    const String UIName(n);
                    const sint32 ParamPos = UIName.Find(0, "-param-");
                    const sint32 ValuePos = UIName.Find(0, "-value-");
                    const sint32 ExtValuePos = UIName.Find(0, "-extvalue-");
                    const sint32 BoxID = Parser::GetInt(UIName);

                    // 파라미터 삭제(0-param-0-remove)
                    if(ParamPos != -1)
                    {
                        const sint32 ParamID = Parser::GetInt(((chars) UIName) + ParamPos + 7); // "-param-"
                        sint32s Args;
                        Args.AtAdding() = BoxID;
                        Args.AtAdding() = ParamID;
                        Platform::SendNotify(v->view(), "ZayBoxParamRemove", Args);
                        mRemovingUIName.Empty();
                    }
                    // 밸류 삭제(0-value-0-remove)
                    else if(ValuePos != -1)
                    {
                        const sint32 ValueID = Parser::GetInt(((chars) UIName) + ValuePos + 7); // "-value-"
                        sint32s Args;
                        Args.AtAdding() = BoxID;
                        Args.AtAdding() = ValueID;
                        Platform::SendNotify(v->view(), "ZayBoxValueRemove", Args);
                        mRemovingUIName.Empty();
                    }
                    // 확장밸류 삭제(0-extvalue-0-remove)
                    else if(ExtValuePos != -1)
                    {
                        const sint32 ExtValueID = Parser::GetInt(((chars) UIName) + ExtValuePos + 10); // "-extvalue-"
                        sint32s Args;
                        Args.AtAdding() = BoxID;
                        Args.AtAdding() = ExtValueID;
                        Platform::SendNotify(v->view(), "ZayBoxExtValueRemove", Args);
                        mRemovingUIName.Empty();
                    }
                    // 제이박스 삭제(0-remove)
                    else Platform::SendNotify(v->view(), (group)? "ZayBoxRemoveGroup" : "ZayBoxRemove", sint32o(BoxID));
                }
                else
                {
                    mRemovingUIName.Empty();
                    mRemovingCount = 0;
                }
            }
        })
    {
        if(!Enable)
        {
            ZAY_RGBA(panel, 192, 64, 64, 128)
                panel.icon(R("wid_x_n"), UIA_CenterMiddle);
        }
        else if(IsPressed)
            panel.icon(R("wid_x_p"), UIA_CenterMiddle);
        else if(IsFocused)
            panel.icon(R("wid_x_o"), UIA_CenterMiddle);
        else panel.icon(R("wid_x_n"), UIA_CenterMiddle);
    }
}

void ZEZayBox::RenderHookRemoveButton(ZayPanel& panel, chars uiname)
{
    const bool IsFocused = ((panel.state(uiname) & (PS_Focused | PS_Dropping)) == PS_Focused);
    const bool IsPressed = ((panel.state(uiname) & (PS_Pressed | PS_Dragging)) != 0);
    ZAY_INNER_UI(panel, 0, uiname,
        ZAY_GESTURE_VNT(v, n, t, this)
        {
            if(t == GT_InReleased)
            {
                const sint32 OldParent = mParent;
                Platform::SendNotify(v->view(), "ZayBoxHookRemove", sint32o(mID));
                if(OldParent != -1)
                    Platform::SendNotify(v->view(), "ZayBoxSort", sint32o(OldParent));
            }
        })
    {
        if(IsPressed)
            panel.icon(R("cut_o"), UIA_CenterMiddle);
        else if(IsFocused)
            panel.icon(R("cut_n"), UIA_CenterMiddle);
        else ZAY_RGBA(panel, 128, 128, 128, 20)
            panel.icon(R("cut_n"), UIA_CenterMiddle);
    }
}

void ZEZayBox::Move(Point drag)
{
    mPosX += drag.x;
    mPosY += drag.y;
}

void ZEZayBox::Resize(ZEZayBoxMap& boxmap, sint32 add)
{
    add = Math::Max(-mAddW, add);
    if(add != 0)
    {
        mAddW += add;
        for(sint32 i = 0, iend = mChildren.Count(); i < iend; ++i)
            if(auto CurBox = boxmap.Access(mChildren[i]))
                (*CurBox)->mHookPos.x += add;
    }
}

void ZEZayBox::FlushTitleDrag(ZEZayBoxMap& boxmap)
{
    mPosX += mTitleDrag.x;
    mPosY += mTitleDrag.y;
    if(mHooked)
        mHookPos -= mTitleDrag;
    for(sint32 i = 0, iend = mChildren.Count(); i < iend; ++i)
        if(auto CurBox = boxmap.Access(mChildren[i]))
            (*CurBox)->mHookPos += mTitleDrag;
    mTitleDrag = Point(0, 0);
}

void ZEZayBox::FlushTitleDragWith(ZEZayBoxMap& boxmap, bool withhook)
{
    mPosX += mTitleDrag.x;
    mPosY += mTitleDrag.y;
    if(mHooked && withhook)
        mHookPos -= mTitleDrag;
    for(sint32 i = 0, iend = mChildren.Count(); i < iend; ++i)
        if(auto CurBox = boxmap.Access(mChildren[i]))
        {
            (*CurBox)->mTitleDrag = mTitleDrag;
            (*CurBox)->FlushTitleDragWith(boxmap, false);
        }
    mTitleDrag = Point(0, 0);
}

sint32 ZEZayBox::Copy(ZEZayBoxMap& boxmap, CreatorCB cb)
{
    Context Json;
    WriteJson(Json);

    // 복사버튼을 누른채 드래그하면 자연스러운 전개를 위해
    // 원래 드래그하던 것이 새로운 컴포넌트가 되고
    // 새로 만든 것이 기존의 컴포넌트가 되어야 함
    // 따라서 입장이 바뀌므로 자신이 컴포넌트ID를 신규발급
    mCompID = MakeLastID();

    sint32 NewID = -1;
    if(Json("compname").HasValue())
    {
        auto NewParentBox = cb(Json("compname").GetText());
        NewParentBox->ReadJson(Json);
        NewParentBox->mExpanded = mExpanded;
        NewParentBox->mPosX = mPosX;
        NewParentBox->mPosY = mPosY;
        NewParentBox->mAddW = mAddW;
        NewParentBox->mHookPos = mHookPos;
        // 박스추가
        NewID = NewParentBox->mID;
        boxmap[NewID] = NewParentBox;
    }

    // 자식재귀
    if(NewID != -1)
    for(sint32 i = 0, iend = mChildren.Count(); i < iend; ++i)
        if(auto CurBox = boxmap.Access(mChildren[i]))
        {
            const sint32 NewChildBoxID = CurBox->Value().Copy(boxmap, cb);
            if(NewChildBoxID != -1)
            if(auto CurChildBox = boxmap.Access(NewChildBoxID))
                boxmap[NewID]->AddChild(CurChildBox->Value());
        }
    return NewID;
}

void ZEZayBox::Sort(ZEZayBoxMap& boxmap)
{
    for(sint32 i = 0, iend = mChildren.Count() - 1; i < iend; ++i)
    {
        auto CurBox = boxmap.Access(mChildren[i]);
        auto NextBox = boxmap.Access(mChildren[i + 1]);
        if(NextBox->ConstValue().mPosY < CurBox->ConstValue().mPosY)
        {
            const sint32 Temp = mChildren[i];
            mChildren.At(i) = mChildren[i + 1];
            mChildren.At(i + 1) = Temp;
            i = Math::Max(-1, i - 2);
        }
    }
    for(sint32 i = 0, iend = mChildren.Count(); i < iend; ++i)
        boxmap.Access(mChildren[i])->Value().mOrder = i;
}

Rect ZEZayBox::GetRect() const
{
    return Rect(Point(sint32(mPosX), sint32(mPosY)),
        Size(mBodySize.w + mAddW, TitleBarHeight + ((mExpanded)? mBodySize.h : 0)));
}

Point ZEZayBox::GetBallPos() const
{
    return Point(sint32(mPosX), sint32(mPosY)) + Size(mBodySize.w + mAddW + BallX, TitleBarHeight / 2);
}

void ZEZayBox::RemoveChildren(ZEZayBoxMap& boxmap)
{
    for(sint32 i = 0, iend = mChildren.Count(); i < iend; ++i)
    {
        if(auto CurBox = boxmap.Access(mChildren[i]))
        {
            CurBox->Value().RemoveChildren(boxmap);
            boxmap.Remove(mChildren[i]);
        }
    }
    mChildren.Clear();
}

void ZEZayBox::ClearParentHook(ZEZayBoxMap& boxmap)
{
    if(mParent != -1)
    {
        if(auto ParentBox = boxmap.Access(mParent))
        for(sint32 i = (*ParentBox)->mChildren.Count() - 1; 0 <= i; --i)
            if((*ParentBox)->mChildren[i] == mID)
                (*ParentBox)->mChildren.SubtractionSection(i);
        ClearMyHook();
    }
}

void ZEZayBox::ClearChildrenHook(ZEZayBoxMap& boxmap)
{
    for(sint32 i = 0, iend = mChildren.Count(); i < iend; ++i)
        if(auto CurBox = boxmap.Access(mChildren[i]))
            (*CurBox)->ClearMyHook();
    mChildren.Clear();
}

void ZEZayBox::ClearMyHook()
{
    mParent = -1;
    mOrder = -1;
    mHooked = false;
    mHookPos = Point(-BallX, 0);
}

////////////////////////////////////////////////////////////////////////////////
// ZEZayBox::CommentTagService
////////////////////////////////////////////////////////////////////////////////
class CommentTag
{
public:
    CommentTag(ZEZayBox* box) : mRefBox(box)
    {
        mNext = nullptr;
    }
    ~CommentTag()
    {
        CommentTag* CurTag = mNext;
        while(CurTag)
        {
            CommentTag* RemoveTag = CurTag;
            CurTag = RemoveTag->mNext;
            RemoveTag->mNext = nullptr;
            delete RemoveTag;
        }
    }

public:
    inline chars text() const {return mRefBox->GetComment();}
    inline Color color() const {return mRefBox->color();}
    inline Point pos() const {return mRefBox->GetRect().Center();}
    inline CommentTag* next() {return mNext;}

public:
    static void Add(ZEZayBox* box);
    static void Sub(ZEZayBox* box);
    static void Update(ZEZayBox* box);

private:
    ZEZayBox* const mRefBox;
    CommentTag* mNext;
};
static CommentTag gCommentTagRoot(nullptr);
static CommentTag* gFocusedCommentTag = nullptr;
static sint32 gCommentTagCount = 0;

void CommentTag::Add(ZEZayBox* box)
{
    auto NewTag = new CommentTag(box);
    CommentTag* CurTag = &gCommentTagRoot;
    while(CurTag->mNext)
    {
        if(0 < String::Compare(CurTag->mNext->text(), NewTag->text()))
            break;
        CurTag = CurTag->mNext;
    }
    NewTag->mNext = CurTag->mNext;
    CurTag->mNext = NewTag;
    gCommentTagCount++;
}

void CommentTag::Sub(ZEZayBox* box)
{
    CommentTag* CurTag = &gCommentTagRoot;
    while(CurTag->mNext)
    {
        if(CurTag->mNext->mRefBox == box)
        {
            CommentTag* RemoveTag = CurTag->mNext;
            CurTag->mNext = RemoveTag->mNext;
            RemoveTag->mNext = nullptr;
            delete RemoveTag;
            gCommentTagCount--;
            break;
        }
        CurTag = CurTag->mNext;
    }
}

void CommentTag::Update(ZEZayBox* box)
{
    CommentTag* FindTag = nullptr;
    CommentTag* CurTag = &gCommentTagRoot;
    while(CurTag->mNext)
    {
        if(CurTag->mNext->mRefBox == box)
        {
            FindTag = CurTag->mNext;
            CurTag->mNext = FindTag->mNext;
            FindTag->mNext = nullptr;
            break;
        }
        CurTag = CurTag->mNext;
    }
    if(FindTag)
    {
        CurTag = &gCommentTagRoot;
        while(CurTag->mNext)
        {
            if(0 < String::Compare(CurTag->mNext->text(), FindTag->text()))
                break;
            CurTag = CurTag->mNext;
        }
        FindTag->mNext = CurTag->mNext;
        CurTag->mNext = FindTag;
    }
}

sint32 ZEZayBox::CommentTagService::GetCount()
{
    return gCommentTagCount;
}

bool ZEZayBox::CommentTagService::SetFirstFocus()
{
    gFocusedCommentTag = gCommentTagRoot.next();
    while(gFocusedCommentTag && gFocusedCommentTag->text()[0] == '\0')
        gFocusedCommentTag = gFocusedCommentTag->next();
    return (gFocusedCommentTag != nullptr);
}

bool ZEZayBox::CommentTagService::SetNextFocus()
{
    gFocusedCommentTag = gFocusedCommentTag->next();
    return (gFocusedCommentTag != nullptr);
}

chars ZEZayBox::CommentTagService::GetFocusText()
{
    chars Result = gFocusedCommentTag->text();
    return Result + (Result[0] == '#');
}

Color ZEZayBox::CommentTagService::GetFocusColor()
{
    return gFocusedCommentTag->color();
}

Point ZEZayBox::CommentTagService::GetFocusPos()
{
    return gFocusedCommentTag->pos();
}

////////////////////////////////////////////////////////////////////////////////
// ZEZayBox::BodyElement
////////////////////////////////////////////////////////////////////////////////
ZEZayBox::BodyElement::BodyElement(ZEZayBox& box) : mBox(box)
{
}

ZEZayBox::BodyElement::~BodyElement()
{
}

////////////////////////////////////////////////////////////////////////////////
// ZEZayBox::BodyComment
////////////////////////////////////////////////////////////////////////////////
ZEZayBox::BodyComment::BodyComment(ZEZayBox& box) : BodyElement(box)
{
    CommentTag::Add(&mBox);
}

ZEZayBox::BodyComment::~BodyComment()
{
    CommentTag::Sub(&mBox);
}

void ZEZayBox::BodyComment::ReadJson(const Context& json)
{
    mComment.Empty();
    if(json("comment").HasValue())
        mComment = json("comment").GetText();
    CommentTag::Update(&mBox);
}

void ZEZayBox::BodyComment::WriteJson(Context& json) const
{
    if(0 < mComment.Length())
        json.At("comment").Set(mComment);
}

sint32 ZEZayBox::BodyComment::GetCalcedSize(const BodyElement* sub) const
{
    return EditorHeight;
}

void ZEZayBox::BodyComment::RenderCommentEditor(ZayPanel& panel, chars uiname)
{
    ZAY_RGB(panel, 255, 255, 255)
        panel.fill();

    ZAY_LTRB_UI(panel, 3, 0, panel.w() - 3, panel.h(), uiname,
        ZAY_GESTURE_VNT(v, n, t, this)
        {
            if(t == GT_Pressed)
            {
                auto CurRect = v->rect(n);
                String Text = mComment;
                if(Platform::Popup::OpenEditTracker(Text, UIET_String, CurRect.l, CurRect.t + 1, CurRect.r, CurRect.b - 1))
                {
                    mComment = Text.Trim();
                    CommentTag::Update(&mBox);
                    v->invalidate();
                }
            }
            else if(t == GT_ToolTip)
                Platform::Popup::ShowToolTip(mComment);
        })
    {
        if(mComment.Length() == 0)
        {
            ZAY_RGBA(panel, 0, 0, 0, 64)
                panel.text("(Write your comment here)", UIFA_LeftMiddle, UIFE_Right);
        }
        else ZAY_RGB(panel, 0, 128, 0)
            panel.text(mComment, UIFA_LeftMiddle, UIFE_Right);
    }

    ZAY_RGB(panel, 0, 0, 0)
        panel.rect(1);
}

////////////////////////////////////////////////////////////////////////////////
// ZEZayBox::BodyNameComment
////////////////////////////////////////////////////////////////////////////////
ZEZayBox::BodyNameComment::BodyNameComment(ZEZayBox& box) : BodyComment(box)
{
}

ZEZayBox::BodyNameComment::~BodyNameComment()
{
}

void ZEZayBox::BodyNameComment::ReadJson(const Context& json)
{
    BodyComment::ReadJson(json);
    mName.Empty();
    if(json("uiname").HasValue())
        mName = json("uiname").GetText();
}

void ZEZayBox::BodyNameComment::WriteJson(Context& json) const
{
    BodyComment::WriteJson(json);
    if(0 < mName.Length())
        json.At("uiname").Set(mName);
}

sint32 ZEZayBox::BodyNameComment::GetCalcedSize(const BodyElement* sub) const
{
    return EditorHeight;
}

void ZEZayBox::BodyNameComment::RenderNameCommentEditor(ZayPanel& panel, chars uiname)
{
    ZAY_RGB(panel, 255, 255, 255)
        panel.fill();

    ZAY_LTRB_UI(panel, 3, 0, panel.w() - 3, panel.h(), uiname,
        ZAY_GESTURE_VNT(v, n, t, this)
        {
            if(t == GT_Pressed)
            {
                auto CurRect = v->rect(n);
                String Text = (mComment.Length() == 0)? mName : ((mName.Length() == 0)? "#" + mComment : mName + " #" + mComment);
                if(Platform::Popup::OpenEditTracker(Text, UIET_String, CurRect.l, CurRect.t + 1, CurRect.r, CurRect.b - 1))
                {
                    const sint32 FindedPos = Text.Find(0, "#");
                    if(FindedPos != -1)
                    {
                        mName = Text.Left(FindedPos).Trim();
                        mComment = Text.Right(Text.Length() - FindedPos - 1).Trim();
                    }
                    else
                    {
                        mName = Text.Trim();
                        mComment.Empty();
                    }
                    CommentTag::Update(&mBox);
                    v->invalidate();
                }
            }
            else if(t == GT_ToolTip)
                Platform::Popup::ShowToolTip(mName + " #" + mComment);
        })
    {
        if(mName.Length() == 0 && mComment.Length() == 0)
        {
            ZAY_RGBA(panel, 0, 0, 0, 64)
                panel.text("(Write your name#comment here)", UIFA_LeftMiddle, UIFE_Right);
        }
        else if(0 < mComment.Length())
        {
            ZAY_RGB(panel, 0, 0, 0)
                panel.text(mName, UIFA_LeftMiddle, UIFE_Right);
            ZAY_LTRB(panel, Platform::Graphics::GetStringWidth(mName), 0, panel.w(), panel.h())
            ZAY_RGB(panel, 0, 128, 0)
                panel.text(" #" + mComment, UIFA_LeftMiddle, UIFE_Right);
        }
        else ZAY_RGB(panel, 0, 0, 0)
            panel.text(mName, UIFA_LeftMiddle, UIFE_Right);
    }

    ZAY_RGB(panel, 0, 0, 0)
        panel.rect(1);
}

////////////////////////////////////////////////////////////////////////////////
// ZEZayBox::BodyParamGroup
////////////////////////////////////////////////////////////////////////////////
ZEZayBox::BodyParamGroup::BodyParamGroup(ZEZayBox& box) : BodyElement(box)
{
}

ZEZayBox::BodyParamGroup::~BodyParamGroup()
{
}

void ZEZayBox::BodyParamGroup::AddParam(chars param)
{
    mParams.AtAdding() = param;
    mBox.RecalcSize();
}

void ZEZayBox::BodyParamGroup::SubParam(sint32 i)
{
    mParams.SubtractionSection(i);
    mBox.RecalcSize();
}

void ZEZayBox::BodyParamGroup::ReadJson(const Context& json)
{
    mParams.Clear();
    for(sint32 i = 0, iend = json.LengthOfIndexable(); i< iend; ++i)
    {
        hook(json[i])
        {
            if(const sint32 ParamLength = fish.LengthOfIndexable())
            {
                String ParamCollector;
                for(sint32 j = 0; j < ParamLength; ++j)
                {
                    ParamCollector += fish[j].GetText();
                    if(j < ParamLength - 1)
                        ParamCollector += ", ";
                }
                mParams.AtAdding() = ParamCollector;
            }
            else mParams.AtAdding() = fish.GetText();
        }
    }
    mBox.RecalcSize();
}

void ZEZayBox::BodyParamGroup::WriteJson(Context& json) const
{
    for(sint32 i = 0, iend = mParams.Count(); i < iend; ++i)
    {
        if(ZaySonUtility::ToCondition(mParams[i]) != ZaySonInterface::ConditionType::Unknown)
            json.At(i).Set(mParams[i]);
        else
        {
            Strings SubParams = ZaySonUtility::GetCommaStrings(mParams[i]);
            for(sint32 j = 0; j < SubParams.Count(); ++j)
                json.At(i).At(j).Set(SubParams[j]);
        }
    }
}

sint32 ZEZayBox::BodyParamGroup::GetCalcedSize(const BodyElement* sub) const
{
    return (ParamHeight + 4) * (mParams.Count() + 1) + 9;
}

void ZEZayBox::BodyParamGroup::RenderParamGroup(ZayPanel& panel)
{
    ZAY_INNER_SCISSOR(panel, 3)
    ZAY_INNER(panel, 1)
    {
        ZAY_COLOR(panel, mBox.mColor)
        ZAY_RGBA(panel, 128, 128, 128, 48)
            panel.fill();
        ZAY_INNER(panel, 3)
        {
            for(sint32 i = 0, iend = mParams.Count(); i <= iend; ++i)
            {
                ZAY_XYWH(panel, 0, (ParamHeight + 4) * i, panel.w(), ParamHeight)
                {
                    if(i == iend)
                    {
                        // 추가버튼
                        const String UIParamAdd = String::Format("%d-param-add", mBox.mID);
                        const sint32 PressMove = (panel.state(UIParamAdd) & PS_Pressed)? 1 : 0;
                        ZAY_XYWH_UI(panel, (panel.w() - ParamAddWidth) / 2, (panel.h() - ParamHeight) / 2, ParamAddWidth - 1, ParamHeight - 1, UIParamAdd,
                            ZAY_GESTURE_T(t, this)
                            {
                                if(t == GT_InReleased)
                                    AddParam("");
                            })
                        {
                            ZAY_MOVE(panel, 2, 2)
                            ZAY_RGBA(panel, 0, 0, 0, 64)
                                panel.fill();
                            ZAY_MOVE(panel, PressMove, PressMove)
                            {
                                ZAY_COLOR(panel, mBox.mColor)
                                ZAY_RGBA(panel, 128, 128, 128, 80)
                                ZAY_RGB_IF(panel, 160, 160, 160, panel.state(UIParamAdd) & (PS_Focused | PS_Dragging))
                                    panel.fill();
                                ZAY_RGB(panel, 0, 0, 0)
                                {
                                    panel.text(panel.w() / 2, panel.h() / 2 - 2, "+", UIFA_CenterMiddle);
                                    panel.rect(1);
                                }
                            }
                        }
                    }
                    else
                    {
                        const String UIParam = String::Format("%d-param-%d", mBox.mID, i);
                        RenderParamEditor(panel, UIParam, i);
                    }
                }
            }
        }
        ZAY_RGB(panel, 0, 0, 0)
            panel.rect(1);
    }
}

void ZEZayBox::BodyParamGroup::RenderParamEditor(ZayPanel& panel, chars uiname, sint32 i)
{
    const String UIRemove = String::Format("%s-remove", uiname);
    const sint32 ButtonWidth = panel.h();

    // 파라미터에디터
    ZAY_RGB(panel, 255, 255, 255)
        panel.fill();
    ZAY_LTRB_UI(panel, 3, 0, panel.w() - ButtonWidth, panel.h(), uiname,
        ZAY_GESTURE_VNT(v, n, t, this, i)
        {
            if(t == GT_Pressed)
            {
                auto CurRect = v->rect(n);
                String Text = mParams[i];
                if(Platform::Popup::OpenEditTracker(Text, UIET_String, CurRect.l, CurRect.t + 1, CurRect.r, CurRect.b - 1))
                {
                    mParams.At(i) = ZaySonUtility::GetSafetyString(Text);
                    v->invalidate();
                }
            }
            else if(t == GT_ToolTip)
                Platform::Popup::ShowToolTip(mParams[i]);
        })
    {
        if(mParams[i].Length() == 0)
        {
            if(0 < mParamComment.Length())
            {
                ZAY_LTRB_SCISSOR(panel, 10, 3, panel.w(), panel.h() - 3)
                ZAY_INNER(panel, 1)
                    RenderParamComment(panel, uiname, mParamComment);
            }
            else ZAY_RGBA(panel, 0, 0, 0, 64)
                panel.text("(Write your parameter here)", UIFA_LeftMiddle, UIFE_Right);
        }
        else ZAY_RGB(panel, 0, 0, 0)
            panel.text(mParams[i], UIFA_LeftMiddle, UIFE_Right);
    }
    ZAY_RGB(panel, 0, 0, 0)
        panel.rect(1);

    // 제거버튼
    ZAY_XYWH(panel, panel.w() - ButtonWidth, 0, ButtonWidth, panel.h())
        mBox.RenderRemoveButton(panel, UIRemove, false);
}

void ZEZayBox::BodyParamGroup::RenderParamComment(ZayPanel& panel, chars uiname, chars comment)
{
    sint32 PosX = 0;
    sint32 ParamID = 0;
    sint32 ParamBegin = -1, ParamEnd = -1;
    sint32s OptionBegins;
    bool HasSharp = false;

    ZAY_FONT(panel, 0.8)
    for(sint32 i = 0; comment[i] != '\0'; ++i)
    switch(comment[i])
    {
    case '[':
        if(ParamBegin == -1)
        {
            ParamBegin = i + 1;
            ParamEnd = -1;
            OptionBegins.SubtractionAll();
        }
        break;
    case ':':
        if(ParamBegin != -1)
        {
            ParamEnd = i;
            OptionBegins.AtAdding() = i + 1;
        }
        break;
    case '|':
        if(ParamBegin != -1)
            OptionBegins.AtAdding() = i + 1;
        break;
    case ']':
        if(ParamBegin != -1)
        {
            if(ParamEnd == -1) ParamEnd = i;
            else OptionBegins.AtAdding() = i + 1;
            // 코멘트형 파라미터
            if(OptionBegins.Count() == 0)
            {
                const sint32 TextWidth = 4 + Platform::Graphics::GetStringWidth(&comment[ParamBegin], ParamEnd - ParamBegin);
                ZAY_XYWH(panel, PosX, 0, TextWidth, panel.h())
                {
                    ZAY_RGBA_IF(panel, 128, 128, 128, 64, !HasSharp)
                    ZAY_RGBA_IF(panel, 128, 0, 128, 64, HasSharp)
                        panel.fill();
                    ZAY_RGB(panel, 128, 128, 128)
                        panel.text(&comment[ParamBegin], ParamEnd - ParamBegin);
                    ZAY_RGBA_IF(panel, 128, 128, 128, 128, !HasSharp)
                    ZAY_RGBA_IF(panel, 128, 0, 128, 128, HasSharp)
                        panel.rect(1);
                }
                PosX += TextWidth + 5;
            }
            // 사례형 파라미터
            else
            {
                // 옵션디폴트값 재조정
                const sint32 OptionLength = OptionBegins.Count() - 1;
                if(mParamCommentDefaults.Count() <= ParamID)
                    mParamCommentDefaults.AtWherever(ParamID) = 0;
                mParamCommentDefaults.At(ParamID) %= OptionLength;

                const sint32 CurOptionDefault = mParamCommentDefaults[ParamID];
                const sint32 CurOptionBegin = OptionBegins[CurOptionDefault];
                const sint32 CurOptionEnd = OptionBegins[CurOptionDefault + 1] - 1;
                const sint32 TextWidth = 4 + Platform::Graphics::GetStringWidth(&comment[CurOptionBegin], CurOptionEnd - CurOptionBegin);
                const String UIComment = String::Format("%s-comment-%d", uiname, ParamID);
                const String ToolTipText(&comment[ParamBegin], ParamEnd - ParamBegin);
                const bool IsOnlyOne = (OptionBegins.Count() == 2);
                ZAY_XYWH_UI(panel, PosX, 0, TextWidth, panel.h(), UIComment,
                    ZAY_GESTURE_T(t, this, ParamID, ToolTipText)
                    {
                        if(t == GT_Pressed)
                            mParamCommentDefaults.AtWherever(ParamID)++;
                        else if(t == GT_ToolTip)
                            Platform::Popup::ShowToolTip(ToolTipText);
                    })
                {
                    ZAY_RGBA_IF(panel, 128, 128, 128, 64, !HasSharp)
                    ZAY_RGBA_IF(panel, 128, 0, 128, 64, HasSharp)
                        panel.fill();
                    ZAY_RGB_IF(panel, 0, 0, 0, IsOnlyOne)
                    ZAY_RGB_IF(panel, 0, 0, 255, !IsOnlyOne && CurOptionDefault == 0)
                    ZAY_RGB_IF(panel, 0, 128, 0, !IsOnlyOne && CurOptionDefault != 0)
                        panel.text(&comment[CurOptionBegin], CurOptionEnd - CurOptionBegin);
                    ZAY_RGBA_IF(panel, 128, 128, 128, 128, !HasSharp)
                    ZAY_RGBA_IF(panel, 128, 0, 128, 128, HasSharp)
                        panel.rect(1);
                }
                PosX += TextWidth + 5;
            }
            ParamID++;
            ParamBegin = -1;
        }
        break;
    case '#':
        if(ParamBegin == -1)
            HasSharp = true;
        break;
    default:
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// ZEZayBox::BodyInputGroup
////////////////////////////////////////////////////////////////////////////////
ZEZayBox::BodyInputGroup::BodyInputGroup(ZEZayBox& box) : BodyElement(box)
{
}

ZEZayBox::BodyInputGroup::~BodyInputGroup()
{
}

void ZEZayBox::BodyInputGroup::AddValue(chars key, chars value)
{
    auto& NewInput = mInputs.AtAdding();
    NewInput.mKey = key;
    NewInput.mValue = value;
    mBox.RecalcSize();
}

void ZEZayBox::BodyInputGroup::SubValue(sint32 i)
{
    mInputs.SubtractionSection(i);
    mBox.RecalcSize();
}

void ZEZayBox::BodyInputGroup::ReadJson(const Context& json)
{
    mInputs.Clear();
    for(sint32 i = 0, iend = json.LengthOfIndexable(); i< iend; ++i)
    {
        hook(json[i])
        {
            auto& NewInput = mInputs.AtAdding();
            if(fish.LengthOfNamable() == 1)
            {
                chararray OneKey;
                hook(fish(0, &OneKey))
                {
                    NewInput.mKey = &OneKey[0];
                    NewInput.mValue = fish.GetText();
                }
            }
            else NewInput.mValue = fish.GetText();
        }
    }
    mBox.RecalcSize();
}

void ZEZayBox::BodyInputGroup::WriteJson(Context& json) const
{
    for(sint32 i = 0, iend = mInputs.Count(); i < iend; ++i)
    {
        if(ZaySonUtility::ToCondition(mInputs[i].mValue) != ZaySonInterface::ConditionType::Unknown)
            json.At(i).Set(mInputs[i].mValue);
        else if(ZaySonUtility::IsFunctionCall(mInputs[i].mValue))
            json.At(i).Set(mInputs[i].mValue);
        else if(mInputs[i].mKey.Length() == 0)
            json.At(i).Set(mInputs[i].mValue);
        else json.At(i).At(mInputs[i].mKey).Set(mInputs[i].mValue);
    }
}

sint32 ZEZayBox::BodyInputGroup::GetCalcedSize(const BodyElement* sub) const
{
    return (ValueHeight + 4) * (mInputs.Count() + 1 + ((sub)? ((const BodyInputGroup*) sub)->mInputs.Count() : 0)) + 9;
}

void ZEZayBox::BodyInputGroup::RenderValueGroup(ZayPanel& panel, chars name, BodyInputGroup* sub)
{
    ZAY_INNER_SCISSOR(panel, 3)
    ZAY_INNER(panel, 1)
    {
        ZAY_RGBA(panel, 128, 192, 128, 72)
            panel.fill();
        ZAY_INNER(panel, 3)
        {
            for(sint32 i = 0, iend = ((sub)? sub->mInputs.Count() : 0) + mInputs.Count(); i <= iend; ++i)
            {
                ZAY_XYWH(panel, 0, (ValueHeight + 4) * i, panel.w(), ValueHeight)
                {
                    if(i == iend)
                    {
                        ZAY_RGB(panel, 0, 0, 0)
                            panel.text(name, UIFA_LeftBottom);

                        // 밸류 추가버튼
                        const String UIValueAdd = String::Format("%d-value-add", mBox.mID);
                        const sint32 PressMove = (panel.state(UIValueAdd) & PS_Pressed)? 1 : 0;
                        ZAY_XYWH_UI(panel, (panel.w() - ValueAddWidth) / 2, (panel.h() - ValueHeight) / 2, ValueAddWidth - 1, ValueHeight - 1, UIValueAdd,
                            ZAY_GESTURE_T(t, this)
                            {
                                if(t == GT_InReleased)
                                    AddValue("", "");
                            })
                        {
                            ZAY_MOVE(panel, 2, 2)
                            ZAY_RGBA(panel, 0, 0, 0, 64)
                                panel.fill();
                            ZAY_MOVE(panel, PressMove, PressMove)
                            {
                                ZAY_RGBA(panel, 128, 192, 128, 120)
                                ZAY_RGB_IF(panel, 160, 160, 160, panel.state(UIValueAdd) & (PS_Focused | PS_Dragging))
                                    panel.fill();
                                ZAY_RGB(panel, 0, 0, 0)
                                {
                                    panel.text(panel.w() / 2, panel.h() / 2 - 2, "+", UIFA_CenterMiddle);
                                    panel.rect(1);
                                }
                            }
                        }

                        // 확장밸류 추가버튼
                        if(sub)
                        {
                            const String UIValueAddExt = String::Format("%d-extvalue-add", mBox.mID);
                            const sint32 PressMoveExt = (panel.state(UIValueAddExt) & PS_Pressed)? 1 : 0;
                            ZAY_XYWH_UI(panel, panel.w() - ValueAddWidth + 1, (panel.h() - ValueHeight) / 2, ValueAddWidth - 1, ValueHeight - 1, UIValueAddExt,
                                ZAY_GESTURE_T(t, this, sub)
                                {
                                    if(t == GT_InReleased)
                                        sub->AddValue("", "");
                                })
                            {
                                ZAY_MOVE(panel, 2, 2)
                                ZAY_RGBA(panel, 0, 0, 0, 64)
                                    panel.fill();
                                ZAY_MOVE(panel, PressMoveExt, PressMoveExt)
                                {
                                    ZAY_RGBA(panel, 128, 192, 255, 120)
                                    ZAY_RGB_IF(panel, 160, 160, 160, panel.state(UIValueAddExt) & (PS_Focused | PS_Dragging))
                                        panel.fill();
                                    ZAY_RGB(panel, 0, 0, 0)
                                    {
                                        panel.text(panel.w() / 2, panel.h() / 2, "T", UIFA_CenterMiddle);
                                        panel.rect(1);
                                    }
                                }
                            }
                        }
                    }
                    else if(sub && 0 < sub->mInputs.Count())
                    {
                        if(i < sub->mInputs.Count())
                        {
                            const String UIValue = String::Format("%d-extvalue-%d", mBox.mID, i);
                            sub->RenderValueEditor(panel, UIValue, i, 1);
                        }
                        else
                        {
                            const String UIValue = String::Format("%d-value-%d", mBox.mID, i - sub->mInputs.Count());
                            RenderValueEditor(panel, UIValue, i - sub->mInputs.Count(), 2);
                        }
                    }
                    else
                    {
                        const String UIValue = String::Format("%d-value-%d", mBox.mID, i);
                        RenderValueEditor(panel, UIValue, i, 0);
                    }
                }
            }
        }
        ZAY_RGB(panel, 0, 0, 0)
            panel.rect(1);
    }
}

void ZEZayBox::BodyInputGroup::RenderValueEditor(ZayPanel& panel, chars uiname, sint32 i, sint32 extmode)
{
    const String UIKey = String::Format("%s-key", uiname);
    const String UIValue = String::Format("%s-value", uiname);
    const String UIRemove = String::Format("%s-remove", uiname);
    const sint32 ButtonWidth = panel.h();

    // 키에디터
    ZAY_LTRB(panel, 0, 0, panel.w() * 0.4 - 2, panel.h())
    {
        ZAY_RGB_IF(panel, 255, 255, 255, extmode == 0)
        ZAY_RGB_IF(panel, 224, 240, 255, extmode == 1)
        ZAY_RGB_IF(panel, 255, 224, 240, extmode == 2)
            panel.fill();

        ZAY_LTRB_UI(panel, 3, 0, panel.w() - 3, panel.h(), UIKey,
            ZAY_GESTURE_VNT(v, n, t, this, i)
            {
                if(t == GT_Pressed)
                {
                    auto CurRect = v->rect(n);
                    String Text = mInputs[i].mKey;
                    if(Platform::Popup::OpenEditTracker(Text, UIET_String, CurRect.l, CurRect.t + 1, CurRect.r, CurRect.b - 1))
                    {
                        mInputs.At(i).mKey = ZaySonUtility::GetSafetyString(Text);
                        v->invalidate();
                    }
                }
                else if(t == GT_ToolTip)
                    Platform::Popup::ShowToolTip(mInputs[i].mKey);
            })
        {
            if(mInputs[i].mKey.Length() == 0)
            {
                ZAY_RGBA(panel, 0, 0, 0, 64)
                    panel.text("(Name)", UIFA_LeftMiddle, UIFE_Right);
            }
            else ZAY_RGB(panel, 0, 0, 0)
                panel.text(mInputs[i].mKey, UIFA_LeftMiddle, UIFE_Right);
        }
        ZAY_RGB(panel, 0, 0, 0)
            panel.rect(1);
    }

    // 밸류에디터
    ZAY_LTRB(panel, panel.w() * 0.4 + 2, 0, panel.w(), panel.h())
    {
        ZAY_RGB_IF(panel, 255, 255, 255, extmode == 0)
        ZAY_RGB_IF(panel, 224, 240, 255, extmode == 1)
        ZAY_RGB_IF(panel, 255, 224, 240, extmode == 2)
            panel.fill();

        ZAY_LTRB_UI(panel, 3, 0, panel.w() - ButtonWidth, panel.h(), UIValue,
            ZAY_GESTURE_VNT(v, n, t, this, i)
            {
                if(t == GT_Pressed)
                {
                    auto CurRect = v->rect(n);
                    String Text = mInputs[i].mValue;
                    if(Platform::Popup::OpenEditTracker(Text, UIET_String, CurRect.l, CurRect.t + 1, CurRect.r, CurRect.b - 1))
                    {
                        mInputs.At(i).mValue = ZaySonUtility::GetSafetyString(Text);
                        v->invalidate();
                    }
                }
                else if(t == GT_ToolTip)
                    Platform::Popup::ShowToolTip(mInputs[i].mValue);
            })
        {
            if(mInputs[i].mValue.Length() == 0)
            {
                ZAY_RGBA(panel, 0, 0, 0, 64)
                    panel.text("(Value)", UIFA_LeftMiddle, UIFE_Right);
            }
            else ZAY_RGB(panel, 0, 0, 0)
                panel.text(mInputs[i].mValue, UIFA_LeftMiddle, UIFE_Right);
        }
        ZAY_RGB(panel, 0, 0, 0)
            panel.rect(1);
    }

    // 제거버튼
    ZAY_XYWH(panel, panel.w() - ButtonWidth, 0, ButtonWidth, panel.h())
        mBox.RenderRemoveButton(panel, UIRemove, false);
}

////////////////////////////////////////////////////////////////////////////////
// ZEZayBox::BodyLoopOperation
////////////////////////////////////////////////////////////////////////////////
ZEZayBox::BodyLoopOperation::BodyLoopOperation(ZEZayBox& box) : BodyElement(box)
{
}

ZEZayBox::BodyLoopOperation::~BodyLoopOperation()
{
}

void ZEZayBox::BodyLoopOperation::ReadJson(const Context& json)
{
    mOperation.Empty();
    if(json("uiloop").HasValue())
        mOperation = json("uiloop").GetText();
}

void ZEZayBox::BodyLoopOperation::WriteJson(Context& json) const
{
    if(0 < mOperation.Length())
        json.At("uiloop").Set(mOperation);
}

sint32 ZEZayBox::BodyLoopOperation::GetCalcedSize(const BodyElement* sub) const
{
    return EditorHeight;
}

void ZEZayBox::BodyLoopOperation::RenderOperationEditor(ZayPanel& panel, chars uiname, chars itname)
{
    const String LoopName = String::Format("%sV", (*itname != '\0')? itname : "");
    const String RuleText(String::Format("0 ≤ %s <", (*itname != '\0')? (chars) LoopName : "(Noname)"));
    const sint32 RuleTextWidth = Platform::Graphics::GetStringWidth(RuleText);

    // 규칙
    ZAY_LTRB(panel, 0, 0, RuleTextWidth + 2, panel.h())
    {
        ZAY_MOVE(panel, 1, 1)
        ZAY_RGBA(panel, 255, 255, 255, 96)
            panel.text(RuleText, UIFA_LeftMiddle);
        ZAY_RGB(panel, 0, 0, 0)
            panel.text(RuleText, UIFA_LeftMiddle);
    }

    // 연산식
    ZAY_LTRB(panel, RuleTextWidth + 4, 0, panel.w(), panel.h())
    {
        ZAY_RGB(panel, 255, 255, 255)
            panel.fill();
        ZAY_LTRB_UI(panel, 3, 0, panel.w() - 3, panel.h(), uiname,
            ZAY_GESTURE_VNT(v, n, t, this)
            {
                if(t == GT_Pressed)
                {
                    auto CurRect = v->rect(n);
                    String Text = mOperation;
                    if(Platform::Popup::OpenEditTracker(Text, UIET_String, CurRect.l, CurRect.t + 1, CurRect.r, CurRect.b - 1))
                    {
                        mOperation = Text.Trim();
                        v->invalidate();
                    }
                }
                else if(t == GT_ToolTip)
                    Platform::Popup::ShowToolTip(mOperation);
            })
        {
            if(mOperation.Length() == 0)
            {
                ZAY_RGBA(panel, 0, 0, 0, 64)
                    panel.text("(Formula)", UIFA_LeftMiddle, UIFE_Right);
            }
            else
            {
                ZAY_RGB(panel, 0, 0, 0)
                    panel.text(mOperation, UIFA_LeftMiddle, UIFE_Right);
                const sint32 OperationSize = Platform::Graphics::GetStringWidth(mOperation);
                if(OperationSize < panel.w())
                ZAY_RGB(panel, 0, 128, 0)
                ZAY_LTRB(panel, OperationSize, 0, panel.w(), panel.h())
                    panel.text(String::Format(" (%sN)", (*itname != '\0')? itname : ""), UIFA_LeftMiddle, UIFE_Right);
            }
        }
        ZAY_RGB(panel, 0, 0, 0)
            panel.rect(1);
    }
}

////////////////////////////////////////////////////////////////////////////////
// ZEZayBox::BodyConditionOperation
////////////////////////////////////////////////////////////////////////////////
ZEZayBox::BodyConditionOperation::BodyConditionOperation(ZEZayBox& box) : BodyElement(box)
{
    mWithElse = false;
    mEventFlag = false;
}

ZEZayBox::BodyConditionOperation::~BodyConditionOperation()
{
}

void ZEZayBox::BodyConditionOperation::Init(chars operation, bool withelse, bool eventflag)
{
    if(operation && *operation)
    {
        String OneOperation = operation;
        if(OneOperation[-2] == ')')
        {
            const sint32 OnePos = OneOperation.Find(0, "(");
            mOperation = OneOperation.Middle(OnePos + 1, OneOperation.Length() - OnePos - 2);
        }
    }
    mWithElse = withelse;
    mEventFlag = eventflag;
}

void ZEZayBox::BodyConditionOperation::ReadJson(const Context& json)
{
}

void ZEZayBox::BodyConditionOperation::WriteJson(Context& json) const
{
}

sint32 ZEZayBox::BodyConditionOperation::GetCalcedSize(const BodyElement* sub) const
{
    return EditorHeight;
}

void ZEZayBox::BodyConditionOperation::RenderOperationEditor(ZayPanel& panel, chars uiname)
{
    const String UIElseCheck = String::Format("%s-else", uiname);
    const sint32 ElseCheckWidth = panel.h();
    const String RuleText((mEventFlag)? "event checking" : "!= zero");
    const sint32 RuleTextWidth = Platform::Graphics::GetStringWidth(RuleText);

    // 체크박스
    ZAY_LTRB_UI(panel, 0, 0, ElseCheckWidth, panel.h(), UIElseCheck,
        ZAY_GESTURE_T(t, this)
        {
            if(t == GT_Pressed)
                mWithElse = !mWithElse;
        })
    {
        ZAY_RGB(panel, 255, 255, 255)
            panel.fill();
        ZAY_RGB_IF(panel, 0, 0, 0, mWithElse)
        ZAY_RGBA_IF(panel, 0, 0, 0, 32, !mWithElse)
            panel.text("el", UIFA_CenterMiddle, UIFE_Right);
        ZAY_RGB(panel, 0, 0, 0)
            panel.rect(1);
    }

    // 연산식
    ZAY_LTRB(panel, ElseCheckWidth + 4, 0, panel.w() - RuleTextWidth - 4, panel.h())
    {
        ZAY_RGB(panel, 255, 255, 255)
            panel.fill();
        ZAY_LTRB_UI(panel, 3, 0, panel.w() - 3, panel.h(), uiname,
            ZAY_GESTURE_VNT(v, n, t, this)
            {
                if(t == GT_Pressed)
                {
                    auto CurRect = v->rect(n);
                    String Text = mOperation;
                    if(Platform::Popup::OpenEditTracker(Text, UIET_String, CurRect.l, CurRect.t + 1, CurRect.r, CurRect.b - 1))
                    {
                        mOperation = Text.Trim();
                        v->invalidate();
                    }
                }
                else if(t == GT_ToolTip)
                    Platform::Popup::ShowToolTip(mOperation);
            })
        {
            if(mOperation.Length() == 0)
            {
                ZAY_RGBA(panel, 0, 0, 0, 64)
                    panel.text("(Formula)", UIFA_LeftMiddle, UIFE_Right);
            }
            else ZAY_RGB(panel, 0, 0, 0)
                panel.text(mOperation, UIFA_LeftMiddle, UIFE_Right);
        }
        ZAY_RGB(panel, 0, 0, 0)
            panel.rect(1);
    }

    // 규칙
    ZAY_LTRB(panel, panel.w() - RuleTextWidth - 2, 0, panel.w(), panel.h())
    {
        ZAY_MOVE(panel, 1, 1)
        ZAY_RGBA(panel, 255, 255, 255, 96)
            panel.text(RuleText, UIFA_RightMiddle);
        ZAY_RGB(panel, 0, 0, 0)
            panel.text(RuleText, UIFA_RightMiddle);
    }
}

////////////////////////////////////////////////////////////////////////////////
// ZEZayBoxStarter
////////////////////////////////////////////////////////////////////////////////
ZEZayBoxStarter::ZEZayBoxStarter() : mComment(*this), mCreateGroup(*this)
{
}

ZEZayBoxStarter::~ZEZayBoxStarter()
{
}

ZEZayBoxObject ZEZayBoxStarter::Create()
{
    buffer NewZayBox = Buffer::Alloc<ZEZayBoxStarter>(BOSS_DBG 1);
    return ZEZayBoxObject(NewZayBox);
}

void ZEZayBoxStarter::ReadJson(const Context& json)
{
    mExpanded = (json("expanded").GetInt(1) != 0);
    mAddW = json("addw").GetInt(0);

    mComment.ReadJson(json);
    mCreateGroup.ReadJson(json("oncreate"));
}

void ZEZayBoxStarter::WriteJson(Context& json) const
{
    json.At("expanded").Set(String::FromInteger((mExpanded)? 1 : 0));
    json.At("addw").Set(String::FromInteger(mAddW));

    mComment.WriteJson(json);
    if(0 < mCreateGroup.mInputs.Count())
        mCreateGroup.WriteJson(json.At("oncreate"));
}

void ZEZayBoxStarter::Render(ZayPanel& panel)
{
    const String UIBody = String::Format("%d-body", mID);
    const String UIComment = String::Format("%d-comment", mID);
    ZAY_XYWH(panel, sint32(mPosX), sint32(mPosY), mBodySize.w + mAddW, TitleBarHeight + ((mExpanded)? mBodySize.h : 0))
    {
        panel.ninepatch(R("box_bg"));

        // 타이틀
        RenderTitle(panel, mCompType, false, true, false, true, true, false);

        // 바디
        if(mExpanded)
        ZAY_XYWH_UI_SCISSOR(panel, 0, TitleBarHeight, panel.w(), mBodySize.h, UIBody)
        {
            ZAY_INNER(panel, 4)
            {
                // 주석에디터
                ZAY_LTRB(panel, 0, 0, panel.w(), EditorHeight)
                ZAY_INNER(panel, 4)
                    mComment.RenderCommentEditor(panel, UIComment);

                // 인풋그룹
                ZAY_LTRB(panel, 0, EditorHeight, panel.w(), panel.h())
                    mCreateGroup.RenderValueGroup(panel, "OnCreate");
            }
        }
    }
}

void ZEZayBoxStarter::RecalcSize()
{
    mBodySize.h = 8 + mComment.GetCalcedSize() + mCreateGroup.GetCalcedSize();
}

void ZEZayBoxStarter::SubInput(sint32 i)
{
    mCreateGroup.SubValue(i);
}

chars ZEZayBoxStarter::GetComment() const
{
    return mComment.mComment;
}

////////////////////////////////////////////////////////////////////////////////
// ZEZayBoxContent
////////////////////////////////////////////////////////////////////////////////
ZEZayBoxContent::ZEZayBoxContent() : mComment(*this), mParamGroup(*this)
{
    mHasChild = false;
    mHasParam = false;
}

ZEZayBoxContent::~ZEZayBoxContent()
{
}

ZEZayBoxObject ZEZayBoxContent::Create(bool child, bool param, chars paramcomment)
{
    buffer NewZayBox = Buffer::Alloc<ZEZayBoxContent>(BOSS_DBG 1);
    if(param)
        ((ZEZayBoxContent*) NewZayBox)->mParamGroup.AddParam("");
    ((ZEZayBoxContent*) NewZayBox)->mParamGroup.mParamComment = paramcomment;
    ((ZEZayBoxContent*) NewZayBox)->mHasChild = child;
    ((ZEZayBoxContent*) NewZayBox)->mHasParam = param;
    return ZEZayBoxObject(NewZayBox);
}

void ZEZayBoxContent::ReadJson(const Context& json)
{
    if(json("compid").HasValue())
        mCompID = ValidLastID(json("compid").GetInt());

    mComment.ReadJson(json);
    mParamGroup.ReadJson(json("compvalues"));
}

void ZEZayBoxContent::WriteJson(Context& json) const
{
    String OneType = mCompType;
    const sint32 Pos = OneType.Find(0, ' ');
    if(Pos != -1) OneType = OneType.Left(Pos);
    json.At("compname").Set(OneType);

    if(mCompID < 0) mCompID = MakeLastID();
    json.At("compid").Set(String::FromInteger(mCompID));

    mComment.WriteJson(json);
    if(0 < mParamGroup.mParams.Count())
        mParamGroup.WriteJson(json.At("compvalues"));
}

void ZEZayBoxContent::Render(ZayPanel& panel)
{
    const String UIBody = String::Format("%d-body", mID);
    const String UIComment = String::Format("%d-comment", mID);
    ZAY_XYWH(panel, sint32(mPosX), sint32(mPosY), mBodySize.w + mAddW, TitleBarHeight + ((mExpanded)? mBodySize.h : 0))
    {
        panel.ninepatch(R("box_bg"));

        // 타이틀
        RenderTitle(panel, mCompType, true, mHasChild, true, true, true, true);

        // 바디
        if(mExpanded)
        ZAY_XYWH_UI_SCISSOR(panel, 0, TitleBarHeight, panel.w(), mBodySize.h, UIBody)
        {
            ZAY_INNER(panel, 4)
            {
                // 주석에디터
                ZAY_LTRB(panel, 0, 0, panel.w(), EditorHeight)
                ZAY_INNER(panel, 4)
                    mComment.RenderCommentEditor(panel, UIComment);

                // 파라미터그룹
                if(mHasParam)
                ZAY_LTRB(panel, 0, EditorHeight, panel.w(), panel.h())
                    mParamGroup.RenderParamGroup(panel);
            }
        }
    }
}

void ZEZayBoxContent::RecalcSize()
{
    mBodySize.h = 8 + mComment.GetCalcedSize();
    if(mHasParam)
        mBodySize.h += mParamGroup.GetCalcedSize();
}

void ZEZayBoxContent::SubParam(sint32 i)
{
    mParamGroup.SubParam(i);
}

chars ZEZayBoxContent::GetComment() const
{
    return mComment.mComment;
}

////////////////////////////////////////////////////////////////////////////////
// ZEZayBoxLayout
////////////////////////////////////////////////////////////////////////////////
ZEZayBoxLayout::ZEZayBoxLayout() : mNameComment(*this), mParamGroup(*this), mTouchGroup(*this), mClickGroup(*this)
{
}

ZEZayBoxLayout::~ZEZayBoxLayout()
{
}

ZEZayBoxObject ZEZayBoxLayout::Create(chars paramcomment)
{
    buffer NewZayBox = Buffer::Alloc<ZEZayBoxLayout>(BOSS_DBG 1);
    ((ZEZayBoxLayout*) NewZayBox)->mParamGroup.AddParam("");
    ((ZEZayBoxLayout*) NewZayBox)->mParamGroup.mParamComment = paramcomment;
    return ZEZayBoxObject(NewZayBox);
}

void ZEZayBoxLayout::ReadJson(const Context& json)
{
    if(json("compid").HasValue())
        mCompID = ValidLastID(json("compid").GetInt());

    mNameComment.ReadJson(json);
    mParamGroup.ReadJson(json("compvalues"));
    mTouchGroup.ReadJson(json("ontouch"));
    mClickGroup.ReadJson(json("onclick"));
}

void ZEZayBoxLayout::WriteJson(Context& json) const
{
    String OneType = mCompType;
    const sint32 Pos = OneType.Find(0, ' ');
    if(Pos != -1) OneType = OneType.Left(Pos);
    json.At("compname").Set(OneType);

    if(mCompID < 0) mCompID = MakeLastID();
    json.At("compid").Set(String::FromInteger(mCompID));

    mNameComment.WriteJson(json);
    if(0 < mParamGroup.mParams.Count())
        mParamGroup.WriteJson(json.At("compvalues"));
    if(0 < mTouchGroup.mInputs.Count())
        mTouchGroup.WriteJson(json.At("ontouch"));
    if(0 < mClickGroup.mInputs.Count())
        mClickGroup.WriteJson(json.At("onclick"));
}

void ZEZayBoxLayout::Render(ZayPanel& panel)
{
    const String UIBody = String::Format("%d-body", mID);
    const String UINameComment = String::Format("%d-namecomment", mID);
    ZAY_XYWH(panel, sint32(mPosX), sint32(mPosY), mBodySize.w + mAddW, TitleBarHeight + ((mExpanded)? mBodySize.h : 0))
    {
        panel.ninepatch(R("box_bg"));

        // 타이틀
        RenderTitle(panel, mCompType, true, true, true, true, true, true);

        // 바디
        if(mExpanded)
        ZAY_XYWH_UI_SCISSOR(panel, 0, TitleBarHeight, panel.w(), mBodySize.h, UIBody)
        {
            ZAY_INNER(panel, 4)
            {
                // UI명칭주석에디터
                ZAY_LTRB(panel, 0, 0, panel.w(), EditorHeight)
                ZAY_INNER(panel, 4)
                    mNameComment.RenderNameCommentEditor(panel, UINameComment);

                // 파라미터그룹
                const sint32 ParamGroupHeight = (ParamHeight + 4) * (mParamGroup.mParams.Count() + 1) + 9;
                ZAY_LTRB(panel, 0, EditorHeight, panel.w(), EditorHeight + ParamGroupHeight)
                    mParamGroup.RenderParamGroup(panel);

                // 인풋그룹
                ZAY_LTRB(panel, 0, EditorHeight + ParamGroupHeight, panel.w(), panel.h())
                    mClickGroup.RenderValueGroup(panel, "OnClick", &mTouchGroup);
            }
        }
    }
}

void ZEZayBoxLayout::RecalcSize()
{
    mBodySize.h = 8 + mNameComment.GetCalcedSize() + mParamGroup.GetCalcedSize() + mClickGroup.GetCalcedSize(&mTouchGroup);
}

void ZEZayBoxLayout::SubParam(sint32 i)
{
    mParamGroup.SubParam(i);
}

void ZEZayBoxLayout::SubInput(sint32 i)
{
    mClickGroup.SubValue(i);
}

void ZEZayBoxLayout::SubExtInput(sint32 i)
{
    mTouchGroup.SubValue(i);
}

chars ZEZayBoxLayout::GetComment() const
{
    return mNameComment.mComment;
}

////////////////////////////////////////////////////////////////////////////////
// ZEZayBoxCode
////////////////////////////////////////////////////////////////////////////////
ZEZayBoxCode::ZEZayBoxCode() : mComment(*this), mCodeGroup(*this)
{
}

ZEZayBoxCode::~ZEZayBoxCode()
{
}

ZEZayBoxObject ZEZayBoxCode::Create()
{
    buffer NewZayBox = Buffer::Alloc<ZEZayBoxCode>(BOSS_DBG 1);
    ((ZEZayBoxCode*) NewZayBox)->mCodeGroup.AddValue("", "");
    return ZEZayBoxObject(NewZayBox);
}

void ZEZayBoxCode::ReadJson(const Context& json)
{
    if(json("compid").HasValue())
        mCompID = ValidLastID(json("compid").GetInt());

    mComment.ReadJson(json);
    mCodeGroup.ReadJson(json("compinputs"));
}

void ZEZayBoxCode::WriteJson(Context& json) const
{
    String OneType = mCompType;
    const sint32 Pos = OneType.Find(0, ' ');
    if(Pos != -1) OneType = OneType.Left(Pos);
    json.At("compname").Set(OneType);

    if(mCompID < 0) mCompID = MakeLastID();
    json.At("compid").Set(String::FromInteger(mCompID));

    mComment.WriteJson(json);
    if(0 < mCodeGroup.mInputs.Count())
        mCodeGroup.WriteJson(json.At("compinputs"));
}

void ZEZayBoxCode::Render(ZayPanel& panel)
{
    const String UIBody = String::Format("%d-body", mID);
    const String UIComment = String::Format("%d-comment", mID);
    ZAY_XYWH(panel, sint32(mPosX), sint32(mPosY), mBodySize.w + mAddW, TitleBarHeight + ((mExpanded)? mBodySize.h : 0))
    {
        panel.ninepatch(R("box_bg"));

        // 타이틀
        RenderTitle(panel, mCompType, true, true, true, true, true, true);

        // 바디
        if(mExpanded)
        ZAY_XYWH_UI_SCISSOR(panel, 0, TitleBarHeight, panel.w(), mBodySize.h, UIBody)
        {
            ZAY_INNER(panel, 4)
            {
                // 주석에디터
                ZAY_LTRB(panel, 0, 0, panel.w(), EditorHeight)
                ZAY_INNER(panel, 4)
                    mComment.RenderCommentEditor(panel, UIComment);

                // 인풋그룹
                ZAY_LTRB(panel, 0, EditorHeight, panel.w(), panel.h())
                    mCodeGroup.RenderValueGroup(panel, "");
            }
        }
    }
}

void ZEZayBoxCode::RecalcSize()
{
    mBodySize.h = 8 + mComment.GetCalcedSize() + mCodeGroup.GetCalcedSize();
}

void ZEZayBoxCode::SubInput(sint32 i)
{
    mCodeGroup.SubValue(i);
}

chars ZEZayBoxCode::GetComment() const
{
    return mComment.mComment;
}

////////////////////////////////////////////////////////////////////////////////
// ZEZayBoxLoop
////////////////////////////////////////////////////////////////////////////////
ZEZayBoxLoop::ZEZayBoxLoop() : mNameComment(*this), mOperation(*this)
{
}

ZEZayBoxLoop::~ZEZayBoxLoop()
{
}

ZEZayBoxObject ZEZayBoxLoop::Create()
{
    buffer NewZayBox = Buffer::Alloc<ZEZayBoxLoop>(BOSS_DBG 1);
    return ZEZayBoxObject(NewZayBox);
}

void ZEZayBoxLoop::ReadJson(const Context& json)
{
    if(json("compid").HasValue())
        mCompID = ValidLastID(json("compid").GetInt());

    mNameComment.ReadJson(json);
    mOperation.ReadJson(json);
}

void ZEZayBoxLoop::WriteJson(Context& json) const
{
    String OneType = mCompType;
    const sint32 Pos = OneType.Find(0, ' ');
    if(Pos != -1) OneType = OneType.Left(Pos);
    json.At("compname").Set(OneType);

    if(mCompID < 0) mCompID = MakeLastID();
    json.At("compid").Set(String::FromInteger(mCompID));

    mNameComment.WriteJson(json);
    mOperation.WriteJson(json);
}

void ZEZayBoxLoop::Render(ZayPanel& panel)
{
    const String UIBody = String::Format("%d-body", mID);
    const String UINameComment = String::Format("%d-namecomment", mID);
    const String UIOperation = String::Format("%d-operation", mID);
    ZAY_XYWH(panel, sint32(mPosX), sint32(mPosY), mBodySize.w + mAddW, TitleBarHeight + ((mExpanded)? mBodySize.h : 0))
    {
        panel.ninepatch(R("box_bg"));

        // 타이틀
        RenderTitle(panel, mCompType, true, true, true, true, true, true);

        // 바디
        if(mExpanded)
        ZAY_XYWH_UI_SCISSOR(panel, 0, TitleBarHeight, panel.w(), mBodySize.h, UIBody)
        {
            ZAY_INNER(panel, 4)
            {
                // UI명칭주석에디터
                ZAY_LTRB(panel, 0, 0, panel.w(), EditorHeight)
                ZAY_INNER(panel, 4)
                    mNameComment.RenderNameCommentEditor(panel, UINameComment);

                // 연산에디터
                ZAY_LTRB(panel, 0, EditorHeight, panel.w(), EditorHeight + EditorHeight)
                ZAY_INNER(panel, 4)
                    mOperation.RenderOperationEditor(panel, UIOperation, mNameComment.mName);
            }
        }
    }
}

void ZEZayBoxLoop::RecalcSize()
{
    mBodySize.h = 8 + mNameComment.GetCalcedSize() + mOperation.GetCalcedSize();
}

chars ZEZayBoxLoop::GetComment() const
{
    return mNameComment.mComment;
}

////////////////////////////////////////////////////////////////////////////////
// ZEZayBoxCondition
////////////////////////////////////////////////////////////////////////////////
ZEZayBoxCondition::ZEZayBoxCondition() : mOperation(*this)
{
    mHasElseAndOperation = false;
}

ZEZayBoxCondition::~ZEZayBoxCondition()
{
}

ZEZayBoxObject ZEZayBoxCondition::Create(chars operation, bool eventflag)
{
    buffer NewZayBox = Buffer::Alloc<ZEZayBoxCondition>(BOSS_DBG 1);
    ((ZEZayBoxCondition*) NewZayBox)->mOperation.Init(operation, operation && !String::Compare(operation, "elif", 4), eventflag);
    ((ZEZayBoxCondition*) NewZayBox)->mHasElseAndOperation = (operation != nullptr);
    return ZEZayBoxObject(NewZayBox);
}

void ZEZayBoxCondition::ReadJson(const Context& json)
{
}

void ZEZayBoxCondition::WriteJson(Context& json) const
{
    String OneType = mCompType;
    const sint32 Pos = OneType.Find(0, ' ');
    if(Pos != -1) OneType = OneType.Left(Pos);

    if(mHasElseAndOperation)
    {
        if(mOperation.mWithElse)
            json.At("compname").Set("el" + OneType + '(' + mOperation.mOperation + ')');
        else json.At("compname").Set(OneType + '(' + mOperation.mOperation + ')');
    }
    else json.At("compname").Set(OneType);
}

void ZEZayBoxCondition::Render(ZayPanel& panel)
{
    const String UIBody = String::Format("%d-body", mID);
    const String UIOperation = String::Format("%d-operation", mID);
    ZAY_XYWH(panel, sint32(mPosX), sint32(mPosY), mBodySize.w + mAddW, TitleBarHeight + ((mExpanded && mHasElseAndOperation)? mBodySize.h : 0))
    {
        panel.ninepatch(R("box_bg"));

        // 타이틀
        if(mOperation.mWithElse)
            RenderTitle(panel, "el" + mCompType, true, false, true, mHasElseAndOperation, true, true);
        else RenderTitle(panel, mCompType, true, false, true, mHasElseAndOperation, true, true);

        // 바디
        if(mExpanded && mHasElseAndOperation)
        ZAY_XYWH_UI_SCISSOR(panel, 0, TitleBarHeight, panel.w(), mBodySize.h, UIBody)
        {
            ZAY_INNER(panel, 4)
            {
                // 연산에디터
                ZAY_LTRB(panel, 0, 0, panel.w(), EditorHeight)
                ZAY_INNER(panel, 4)
                    mOperation.RenderOperationEditor(panel, UIOperation);
            }
        }
    }
}

void ZEZayBoxCondition::RecalcSize()
{
    mBodySize.h = 0;
    if(mHasElseAndOperation)
        mBodySize.h += 8 + mOperation.GetCalcedSize();
}

////////////////////////////////////////////////////////////////////////////////
// ZEZayBoxError
////////////////////////////////////////////////////////////////////////////////
ZEZayBoxError::ZEZayBoxError() : mNameComment(*this), mParamGroup(*this), mTouchGroup(*this), mClickGroup(*this)
{
}

ZEZayBoxError::~ZEZayBoxError()
{
}

ZEZayBoxObject ZEZayBoxError::Create()
{
    buffer NewZayBox = Buffer::Alloc<ZEZayBoxError>(BOSS_DBG 1);
    return ZEZayBoxObject(NewZayBox);
}

void ZEZayBoxError::ReadJson(const Context& json)
{
    if(json("compid").HasValue())
        mCompID = ValidLastID(json("compid").GetInt());

    mNameComment.ReadJson(json);
    mParamGroup.ReadJson(json("compvalues"));
    mTouchGroup.ReadJson(json("ontouch"));
    mClickGroup.ReadJson(json("onclick"));
}

void ZEZayBoxError::WriteJson(Context& json) const
{
    json.At("compname").Set(mCompType);

    if(mCompID < 0) mCompID = MakeLastID();
    json.At("compid").Set(String::FromInteger(mCompID));

    mNameComment.WriteJson(json);
    if(0 < mParamGroup.mParams.Count())
        mParamGroup.WriteJson(json.At("compvalues"));
    if(0 < mTouchGroup.mInputs.Count())
        mTouchGroup.WriteJson(json.At("ontouch"));
    if(0 < mClickGroup.mInputs.Count())
        mClickGroup.WriteJson(json.At("onclick"));
}

void ZEZayBoxError::Render(ZayPanel& panel)
{
    const String UIBody = String::Format("%d-body", mID);
    const String UIOperation = String::Format("%d-operation", mID);
    ZAY_XYWH(panel, sint32(mPosX), sint32(mPosY), mBodySize.w + mAddW, TitleBarHeight)
    {
        panel.ninepatch(R("box_bg"));

        // 타이틀
        RenderTitle(panel, mCompType, true, true, true, false, true, true);
    }
}

void ZEZayBoxError::RecalcSize()
{
    mBodySize.h = 0;
}

chars ZEZayBoxError::GetComment() const
{
    return mNameComment.mComment;
}
