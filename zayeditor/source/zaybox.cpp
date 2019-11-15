#include <boss.hpp>
#include "zaybox.hpp"

#include <service/boss_zayson.hpp>

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
    Point TopPos;
    if(const auto TopBox = boxmap.Access(0))
        TopPos = TopBox->ConstValue().mPos;

    mChildren.Clear();
    for(sint32 i = 0, iend = json.LengthOfIndexable(); i < iend; ++i)
    {
        hook(json[i])
        {
            String CurCompName;
            if(auto CurName = fish("compname").GetString(nullptr))
                CurCompName = CurName;
            else CurCompName = fish.GetString(); // 옛날 포맷으로 쓰여진 조건문인 경우

            // 자식구성
            auto NewChildBox = cb(CurCompName);
            NewChildBox->ReadJson(fish);
            NewChildBox->mPos.x = TopPos.x + fish("posx").GetFloat(mPos.x + mBodySize.w + 30 - TopPos.x);
            NewChildBox->mPos.y = TopPos.y + fish("posy").GetFloat(mPos.y + (TitleBarHeight + 20) * i - TopPos.y);
            AddChild(NewChildBox.Value()); // 자식의 HookPos설정

            // 자식재귀 및 박스추가
            NewChildBox->LoadChildren(fish("ui"), boxmap, cb);
            boxmap[NewChildBox->mID] = NewChildBox;
        }
    }
}

void ZEZayBox::SaveChildren(Context& json, const ZEZayBoxMap& boxmap) const
{
    Point TopPos;
    if(auto TopBox = boxmap.Access(0))
        TopPos = TopBox->ConstValue().mPos;

    for(sint32 i = 0, iend = mChildren.Count(); i < iend; ++i)
    {
        hook(json.At(i))
        if(auto CurBox = boxmap.Access(mChildren[i]))
        {
            CurBox->ConstValue().WriteJson(fish);
            fish.At("posx").Set(String::FromFloat(CurBox->ConstValue().mPos.x - TopPos.x));
            fish.At("posy").Set(String::FromFloat(CurBox->ConstValue().mPos.y - TopPos.y));
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

chars ZEZayBox::GetComment() const
{
    BOSS_ASSERT("잘못된 시나리오입니다", false);
    return "error";
}

void ZEZayBox::Init(sint32 id, chars type, Color color, bool expand, sint32 x, sint32 y)
{
    mID = id;
    mCompType = type;
    mColor = color;
    mExpanded = expand;
    mPos.x = x;
    mPos.y = y;
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
    child.mHookPos = GetBallPos() - child.mPos - Point(0, TitleBarHeight / 2);
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
            newchild.mHookPos = GetBallPos() - newchild.mPos - Point(0, TitleBarHeight / 2);
            mChildren.At(i) = newchild.mID;
            break;
        }
    }
    oldchild.ClearMyHook();
}

void ZEZayBox::RenderTitle(ZayPanel& panel, chars title, bool hook, bool ball, bool copy, bool expand, bool remove)
{
    const String UITitle = String::Format("%d-title", mID);
    const String UIGroupMove = String::Format("%d-groupmove", mID);
    const String UIGroupCopy = String::Format("%d-groupcopy", mID);
    const String UIExpand = String::Format("%d-expand", mID);
    const String UIRemove = String::Format("%d-remove", mID);
    const String UIHook = String::Format("%d-hook", mID);
    const String UIBall = String::Format("%d-ball", mID);
    const sint32 ButtonWidth = TitleBarHeight;
    const bool HasFocusing = !!(panel.state(UITitle) & (PS_Focused | PS_Dragging));

    // 타이틀
    ZAY_XYWH_UI(panel, (sint32) mPos.x, (sint32) mPos.y, mBodySize.w, TitleBarHeight, UITitle,
        ZAY_GESTURE_VNTXY(v, n, t, x, y, this)
        {
            if(t == GT_Moving)
                Platform::SendNotify(v->view(), "Pipe:CompFocusIn", String::FromInteger(mCompID));
            else if(t == GT_MovingLosed)
                Platform::SendNotify(v->view(), "Pipe:CompFocusOut", String());
            else if(t == GT_InDragging || t == GT_OutDragging)
            {
                auto& OldPos = v->oldxy(n);
                mTitleDrag = Point(x - OldPos.x, y - OldPos.y);
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
        ZAY_INNER_SCISSOR(panel, 0)
        {
            // 로프가림막
            ZAY_COLOR(panel, mColor)
            {
                // L로프쪽
                ZAY_LTRB(panel, 0, 0, 10, panel.h())
                ZAY_RGB(panel, 128, 128, 128)
                    panel.fill();
                ZAY_LTRB(panel, 10, 0, 13, panel.h())
                ZAY_RGBA(panel, 128, 128, 128, 96)
                    panel.fill();
                ZAY_LTRB(panel, 13, 0, 14, panel.h())
                ZAY_RGBA(panel, 128, 128, 128, 32)
                    panel.fill();
                // R로프쪽
                ZAY_LTRB(panel, panel.w() - 10, 0, panel.w(), panel.h())
                ZAY_RGB(panel, 128, 128, 128)
                    panel.fill();
            }

            // 타이틀보드 강조
            ZAY_COLOR(panel, mColor)
            ZAY_RGB_IF(panel, 160, 160, 160, HasFocusing)
                panel.fill();

            sint32 ButtonPosX = panel.w();
            // 제거버튼
            if(remove)
            {
                ButtonPosX -= ButtonWidth;
                ZAY_XYWH(panel, ButtonPosX, 0, ButtonWidth, panel.h())
                    RenderRemoveButton(panel, UIRemove, mParent == -1 && 0 < mChildren.Count());
            }
            else
            {
                ButtonPosX -= ButtonWidth;
                ZAY_XYWH(panel, ButtonPosX, 0, ButtonWidth, panel.h())
                ZAY_INNER_UI(panel, 0, UIRemove)
                ZAY_INNER(panel, 3)
                {
                    ZAY_RGB(panel, 160, 160, 160)
                        panel.fill();
                    ZAY_RGB(panel, 0, 0, 0)
                    {
                        panel.text("×", UIFA_CenterMiddle);
                        panel.rect(1);
                    }
                }
            }
            // 그룹복사버튼
            if(copy)
            {
                ButtonPosX -= ButtonWidth;
                ZAY_XYWH(panel, ButtonPosX, 0, ButtonWidth, panel.h())
                    RenderGroupCopyButton(panel, UIGroupCopy);
            }
            // 확장/최소화버튼
            if(expand)
            {
                ButtonPosX -= ButtonWidth;
                ZAY_XYWH(panel, ButtonPosX, 0, ButtonWidth, panel.h())
                    RenderExpandButton(panel, UIExpand);
            }
            // 그룹이동버튼
            if(ball)
            {
                ButtonPosX -= ButtonWidth;
                ZAY_XYWH(panel, ButtonPosX, 0, ButtonWidth, panel.h())
                    RenderGroupMoveButton(panel, UIGroupMove);
            }

            // 타이틀
            ZAY_LTRB(panel, 3, 0, ButtonPosX, panel.h())
            {
                const String TitleText((HasFocusing && mOrder != -1)?
                    (chars) String::Format("[%d] %s", mOrder, title) : title);
                ZAY_MOVE(panel, 1, 1)
                ZAY_RGBA(panel, 255, 255, 255, 96)
                    panel.text(TitleText, UIFA_LeftMiddle, UIFE_Right);
                ZAY_RGB(panel, 0, 0, 0)
                    panel.text(TitleText, UIFA_LeftMiddle, UIFE_Right);
            }
        }
        ZAY_RGB(panel, 0, 0, 0)
            panel.rect(1);
    }
}

void ZEZayBox::RenderHook(ZayPanel& panel, chars uiname)
{
    ZAY_COLOR_IF(panel, mColor, mHooked)
    ZAY_RGB_IF(panel, 80, 80, 80, mHooked)
    ZAY_RGB_IF(panel, 255, 64, 0, !mHooked)
    ZAY_RGB_IF(panel, 96, 96, 96, !mHooked && !(panel.state(uiname) & (PS_Focused | PS_Dragging)))
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

    ZAY_RGB_IF(panel, 0, 0, 0, !IsDropping)
    ZAY_RGB_IF(panel, 255, 64, 0, IsDropping)
    ZAY_RGB_IF(panel, 192, 192, 192, !IsDropping && (panel.state(uiname) & PS_Focused))
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
}

void ZEZayBox::RenderGroupMoveButton(ZayPanel& panel, chars uiname)
{
    ZAY_INNER_UI(panel, 0, uiname,
        ZAY_GESTURE_VNTXY(v, n, t, x, y, this)
        {
            if(t == GT_InDragging || t == GT_OutDragging)
            {
                auto& OldPos = v->oldxy(n);
                mTitleDrag = Point(x - OldPos.x, y - OldPos.y);
                Platform::SendNotify(v->view(), "ZayBoxMoveWith", sint32o(mID));
                v->invalidate();
            }
        })
    ZAY_INNER(panel, 3)
    {
        ZAY_RGBA(panel, 0, 0, 0, 32)
        ZAY_RGBA_IF(panel, 128, 128, 128, 144, panel.state(uiname) & PS_Focused)
            panel.fill();
        ZAY_RGB(panel, 0, 0, 0)
            panel.text("∈", UIFA_CenterMiddle);
        ZAY_RGBA(panel, 0, 0, 0, 64)
            panel.rect(1);
    }
}

void ZEZayBox::RenderGroupCopyButton(ZayPanel& panel, chars uiname)
{
    ZAY_INNER_UI(panel, 0, uiname,
        ZAY_GESTURE_VNTXY(v, n, t, x, y, this)
        {
            if(t == GT_Pressed)
                Platform::SendNotify(v->view(), "ZayBoxCopy", sint32o(mID));
            else if(t == GT_InDragging || t == GT_OutDragging)
            {
                auto& OldPos = v->oldxy(n);
                mTitleDrag = Point(x - OldPos.x, y - OldPos.y);
                Platform::SendNotify(v->view(), "ZayBoxMoveWith", sint32o(mID));
                v->invalidate();
            }
        })
    ZAY_INNER(panel, 3)
    {
        ZAY_RGBA(panel, 0, 0, 0, 64)
        ZAY_RGBA_IF(panel, 128, 128, 128, 144, panel.state(uiname) & PS_Focused)
            panel.fill();
        ZAY_RGB(panel, 0, 0, 0)
            panel.text("＋", UIFA_CenterMiddle);
        ZAY_RGBA(panel, 0, 0, 0, 96)
            panel.rect(1);
    }
}

void ZEZayBox::RenderExpandButton(ZayPanel& panel, chars uiname)
{
    ZAY_INNER_UI(panel, 0, uiname,
        ZAY_GESTURE_T(t, this)
        {
            if(t == GT_InReleased)
            {
                mExpanded ^= true;
            }
        })
    ZAY_INNER(panel, 3)
    {
        ZAY_RGBA(panel, 0, 0, 0, 32)
        ZAY_RGBA_IF(panel, 128, 128, 128, 144, panel.state(uiname) & PS_Focused)
            panel.fill();
        ZAY_RGB(panel, 0, 0, 0)
            panel.text((mExpanded)? "∧" : "∨", UIFA_CenterMiddle);
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
                    const sint32 BoxID = Parser::GetInt(UIName);

                    // 파라미터 삭제(0-param-0-remove)
                    if(ParamPos != -1)
                    {
                        const sint32 BoxParamID = Parser::GetInt(((chars) UIName) + ParamPos + 7);
                        sint32s Args;
                        Args.AtAdding() = BoxID;
                        Args.AtAdding() = BoxParamID;
                        Platform::SendNotify(v->view(), "ZayBoxParamRemove", Args);
                        mRemovingUIName.Empty();
                    }
                    // 밸류 삭제(0-value-0-remove)
                    else if(ValuePos != -1)
                    {
                        const sint32 BoxValueID = Parser::GetInt(((chars) UIName) + ValuePos + 7);
                        sint32s Args;
                        Args.AtAdding() = BoxID;
                        Args.AtAdding() = BoxValueID;
                        Platform::SendNotify(v->view(), "ZayBoxValueRemove", Args);
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
    ZAY_INNER(panel, 3 + 4 * AniValue)
    {
        ZAY_RGB_IF(panel, 255, 0, 0, Enable && !group)
        ZAY_RGB_IF(panel, 255, 0, 255, Enable && group)
        ZAY_RGB_IF(panel, 160, 160, 160, !Enable)
        ZAY_RGB_IF(panel, 192, 192, 192, panel.state(uiname) & PS_Focused)
            panel.fill();
        ZAY_RGB(panel, 0, 0, 0)
        {
            if(Enable)
                panel.text("×", UIFA_CenterMiddle);
            panel.rect(1);
        }
    }
}

void ZEZayBox::Move(Point drag)
{
    mPos += drag;
}

void ZEZayBox::FlushTitleDrag(ZEZayBoxMap& boxmap)
{
    mPos += mTitleDrag;
    if(mHooked)
        mHookPos -= mTitleDrag;
    for(sint32 i = 0, iend = mChildren.Count(); i < iend; ++i)
        if(auto CurBox = boxmap.Access(mChildren[i]))
            (*CurBox)->mHookPos += mTitleDrag;
    mTitleDrag = Point(0, 0);
}

void ZEZayBox::FlushTitleDragWith(ZEZayBoxMap& boxmap, bool withhook)
{
    mPos += mTitleDrag;
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
    if(auto CurCompName = Json("compname").GetString(nullptr))
    {
        auto NewParentBox = cb(CurCompName);
        NewParentBox->ReadJson(Json);
        NewParentBox->mPos = mPos;
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
        if(NextBox->ConstValue().mPos.y < CurBox->ConstValue().mPos.y)
        {
            CurBox->Value().mOrder = i + 1;
            NextBox->Value().mOrder = i;
            const sint32 Temp = mChildren[i];
            mChildren.At(i) = mChildren[i + 1];
            mChildren.At(i + 1) = Temp;
            i = Math::Max(-1, i - 2);
        }
    }
}

Rect ZEZayBox::GetRect() const
{
    return Rect(Point((sint32) mPos.x, (sint32) mPos.y), Size(mBodySize.w, TitleBarHeight + mBodySize.h));
}

Point ZEZayBox::GetBallPos() const
{
    return Point((sint32) mPos.x, (sint32) mPos.y) + Size(mBodySize.w + BallX, TitleBarHeight / 2);
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
    if(auto OneComment = json("comment").GetString(nullptr))
        mComment = OneComment;
    CommentTag::Update(&mBox);
}

void ZEZayBox::BodyComment::WriteJson(Context& json) const
{
    if(0 < mComment.Length())
        json.At("comment").Set(mComment);
}

sint32 ZEZayBox::BodyComment::GetCalcedSize()
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
                if(Platform::Popup::OpenEditTracker(Text, UIET_String, CurRect.l, CurRect.t, CurRect.r, CurRect.b))
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
                panel.text("(여기에 주석을 기입)", UIFA_LeftMiddle, UIFE_Right);
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
    if(auto OneName = json("uiname").GetString(nullptr))
        mName = OneName;
}

void ZEZayBox::BodyNameComment::WriteJson(Context& json) const
{
    BodyComment::WriteJson(json);
    if(0 < mName.Length())
        json.At("uiname").Set(mName);
}

sint32 ZEZayBox::BodyNameComment::GetCalcedSize()
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
                if(Platform::Popup::OpenEditTracker(Text, UIET_String, CurRect.l, CurRect.t, CurRect.r, CurRect.b))
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
                panel.text("(여기에 이름#주석을 기입)", UIFA_LeftMiddle, UIFE_Right);
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
                    ParamCollector += fish[j].GetString();
                    if(j < ParamLength - 1)
                        ParamCollector += ", ";
                }
                mParams.AtAdding() = ParamCollector;
            }
            else mParams.AtAdding() = fish.GetString();
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

sint32 ZEZayBox::BodyParamGroup::GetCalcedSize()
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
                                    panel.text(panel.w() / 2, panel.h() / 2 - 2, "＋", UIFA_CenterMiddle);
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
                if(Platform::Popup::OpenEditTracker(Text, UIET_String, CurRect.l, CurRect.t, CurRect.r, CurRect.b))
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
                panel.text("(여기에 파라미터를 기입)", UIFA_LeftMiddle, UIFE_Right);
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
                    NewInput.mKey = OneKey;
                    NewInput.mValue = fish.GetString();
                }
            }
            else NewInput.mValue = fish.GetString();
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
        else json.At(i).At(mInputs[i].mKey).Set(mInputs[i].mValue);
    }
}

sint32 ZEZayBox::BodyInputGroup::GetCalcedSize()
{
    return (ValueHeight + 4) * (mInputs.Count() + 1) + 9;
}

void ZEZayBox::BodyInputGroup::RenderValueGroup(ZayPanel& panel, chars name)
{
    ZAY_INNER_SCISSOR(panel, 3)
    ZAY_INNER(panel, 1)
    {
        ZAY_RGBA(panel, 128, 192, 128, 72)
            panel.fill();
        ZAY_INNER(panel, 3)
        {
            for(sint32 i = 0, iend = mInputs.Count(); i <= iend; ++i)
            {
                ZAY_XYWH(panel, 0, (ValueHeight + 4) * i, panel.w(), ValueHeight)
                {
                    if(i == iend)
                    {
                        ZAY_RGB(panel, 0, 0, 0)
                            panel.text(name, UIFA_LeftBottom);

                        // 추가버튼
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
                                    panel.text(panel.w() / 2, panel.h() / 2 - 2, "＋", UIFA_CenterMiddle);
                                    panel.rect(1);
                                }
                            }
                        }
                    }
                    else
                    {
                        const String UIValue = String::Format("%d-value-%d", mBox.mID, i);
                        RenderValueEditor(panel, UIValue, i);
                    }
                }
            }
        }
        ZAY_RGB(panel, 0, 0, 0)
            panel.rect(1);
    }
}

void ZEZayBox::BodyInputGroup::RenderValueEditor(ZayPanel& panel, chars uiname, sint32 i)
{
    const String UIKey = String::Format("%s-key", uiname);
    const String UIValue = String::Format("%s-value", uiname);
    const String UIRemove = String::Format("%s-remove", uiname);
    const sint32 ButtonWidth = panel.h();

    // 키에디터
    ZAY_LTRB(panel, 0, 0, panel.w() * 0.4 - 2, panel.h())
    {
        ZAY_RGB(panel, 255, 255, 255)
            panel.fill();
        ZAY_LTRB_UI(panel, 3, 0, panel.w() - 3, panel.h(), UIKey,
            ZAY_GESTURE_VNT(v, n, t, this, i)
            {
                if(t == GT_Pressed)
                {
                    auto CurRect = v->rect(n);
                    String Text = mInputs[i].mKey;
                    if(Platform::Popup::OpenEditTracker(Text, UIET_String, CurRect.l, CurRect.t, CurRect.r, CurRect.b))
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
                    panel.text("(이름)", UIFA_LeftMiddle, UIFE_Right);
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
        ZAY_RGB(panel, 255, 255, 255)
            panel.fill();
        ZAY_LTRB_UI(panel, 3, 0, panel.w() - ButtonWidth, panel.h(), UIValue,
            ZAY_GESTURE_VNT(v, n, t, this, i)
            {
                if(t == GT_Pressed)
                {
                    auto CurRect = v->rect(n);
                    String Text = mInputs[i].mValue;
                    if(Platform::Popup::OpenEditTracker(Text, UIET_String, CurRect.l, CurRect.t, CurRect.r, CurRect.b))
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
                    panel.text("(값)", UIFA_LeftMiddle, UIFE_Right);
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
    if(auto OneLoop = json("uiloop").GetString(nullptr))
        mOperation = OneLoop;
}

void ZEZayBox::BodyLoopOperation::WriteJson(Context& json) const
{
    if(0 < mOperation.Length())
        json.At("uiloop").Set(mOperation);
}

sint32 ZEZayBox::BodyLoopOperation::GetCalcedSize()
{
    return EditorHeight;
}

void ZEZayBox::BodyLoopOperation::RenderOperationEditor(ZayPanel& panel, chars uiname, chars itname)
{
    const String RuleText(String::Format("0 ≤ %s <", (*itname != '\0')? itname : "(noname)"));
    const sint32 RuleTextWidth = Platform::Graphics::GetStringWidth(RuleText);

    // 규칙
    ZAY_LTRB(panel, 0, 0, RuleTextWidth, panel.h())
    {
        ZAY_MOVE(panel, 1, 1)
        ZAY_RGBA(panel, 255, 255, 255, 96)
            panel.text(RuleText, UIFA_CenterMiddle);
        ZAY_RGB(panel, 0, 0, 0)
            panel.text(RuleText, UIFA_CenterMiddle);
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
                    if(Platform::Popup::OpenEditTracker(Text, UIET_String, CurRect.l, CurRect.t, CurRect.r, CurRect.b))
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
                    panel.text("(연산식)", UIFA_LeftMiddle, UIFE_Right);
            }
            else ZAY_RGB(panel, 0, 0, 0)
                panel.text(mOperation, UIFA_LeftMiddle, UIFE_Right);
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

sint32 ZEZayBox::BodyConditionOperation::GetCalcedSize()
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
            panel.text("ⓔ", UIFA_CenterMiddle, UIFE_Right);
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
                    if(Platform::Popup::OpenEditTracker(Text, UIET_String, CurRect.l, CurRect.t, CurRect.r, CurRect.b))
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
                    panel.text("(연산식)", UIFA_LeftMiddle, UIFE_Right);
            }
            else ZAY_RGB(panel, 0, 0, 0)
                panel.text(mOperation, UIFA_LeftMiddle, UIFE_Right);
        }
        ZAY_RGB(panel, 0, 0, 0)
            panel.rect(1);
    }

    // 규칙
    ZAY_LTRB(panel, panel.w() - RuleTextWidth, 0, panel.w(), panel.h())
    {
        ZAY_MOVE(panel, 1, 1)
        ZAY_RGBA(panel, 255, 255, 255, 96)
            panel.text(RuleText, UIFA_CenterMiddle);
        ZAY_RGB(panel, 0, 0, 0)
            panel.text(RuleText, UIFA_CenterMiddle);
    }
}

////////////////////////////////////////////////////////////////////////////////
// ZEZayBoxStarter
////////////////////////////////////////////////////////////////////////////////
ZEZayBoxStarter::ZEZayBoxStarter() : mComment(*this), mInputGroup(*this)
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
    mComment.ReadJson(json);
    mInputGroup.ReadJson(json("oncreate"));
}

void ZEZayBoxStarter::WriteJson(Context& json) const
{
    mComment.WriteJson(json);
    if(0 < mInputGroup.mInputs.Count())
        mInputGroup.WriteJson(json.At("oncreate"));
}

void ZEZayBoxStarter::Render(ZayPanel& panel)
{
    const String UIBody = String::Format("%d-body", mID);
    const String UIComment = String::Format("%d-comment", mID);

    // 타이틀
    RenderTitle(panel, mCompType, false, true, false, true, false);

    // 바디
    if(mExpanded)
    ZAY_XYWH_UI_SCISSOR(panel, (sint32) mPos.x, ((sint32) mPos.y) + TitleBarHeight, mBodySize.w, mBodySize.h, UIBody)
    {
        ZAY_RGBA(panel, 64, 64, 64, 128)
            panel.fill();
        ZAY_INNER(panel, 4)
        {
            // 주석에디터
            ZAY_LTRB(panel, 0, 0, panel.w(), EditorHeight)
            ZAY_INNER(panel, 4)
                mComment.RenderCommentEditor(panel, UIComment);

            // 인풋그룹
            ZAY_LTRB(panel, 0, EditorHeight, panel.w(), panel.h())
                mInputGroup.RenderValueGroup(panel, "OnCreate");
        }
    }
}

void ZEZayBoxStarter::RecalcSize()
{
    mBodySize.h = 8 + mComment.GetCalcedSize() + mInputGroup.GetCalcedSize();
}

void ZEZayBoxStarter::SubInput(sint32 i)
{
    mInputGroup.SubValue(i);
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
    if(auto CurID = json("compid").GetString(nullptr))
        mCompID = ValidLastID(Parser::GetInt(CurID));

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

    // 타이틀
    RenderTitle(panel, mCompType, true, mHasChild, true, true, true);

    // 바디
    if(mExpanded)
    ZAY_XYWH_UI_SCISSOR(panel, (sint32) mPos.x, ((sint32) mPos.y) + TitleBarHeight, mBodySize.w, mBodySize.h, UIBody)
    {
        ZAY_RGBA(panel, 64, 64, 64, 128)
            panel.fill();
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
ZEZayBoxLayout::ZEZayBoxLayout() : mNameComment(*this), mParamGroup(*this), mInputGroup(*this)
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
    if(auto CurID = json("compid").GetString(nullptr))
        mCompID = ValidLastID(Parser::GetInt(CurID));

    mNameComment.ReadJson(json);
    mParamGroup.ReadJson(json("compvalues"));
    mInputGroup.ReadJson(json("onclick"));
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
    if(0 < mInputGroup.mInputs.Count())
        mInputGroup.WriteJson(json.At("onclick"));
}

void ZEZayBoxLayout::Render(ZayPanel& panel)
{
    const String UIBody = String::Format("%d-body", mID);
    const String UINameComment = String::Format("%d-namecomment", mID);

    // 타이틀
    RenderTitle(panel, mCompType, true, true, true, true, true);

    // 바디
    if(mExpanded)
    ZAY_XYWH_UI_SCISSOR(panel, (sint32) mPos.x, ((sint32) mPos.y) + TitleBarHeight, mBodySize.w, mBodySize.h, UIBody)
    {
        ZAY_RGBA(panel, 64, 64, 64, 128)
            panel.fill();
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
                mInputGroup.RenderValueGroup(panel, "OnClick");
        }
    }
}

void ZEZayBoxLayout::RecalcSize()
{
    mBodySize.h = 8 + mNameComment.GetCalcedSize() + mParamGroup.GetCalcedSize() + mInputGroup.GetCalcedSize();
}

void ZEZayBoxLayout::SubParam(sint32 i)
{
    mParamGroup.SubParam(i);
}

void ZEZayBoxLayout::SubInput(sint32 i)
{
    mInputGroup.SubValue(i);
}

chars ZEZayBoxLayout::GetComment() const
{
    return mNameComment.mComment;
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
    if(auto CurID = json("compid").GetString(nullptr))
        mCompID = ValidLastID(Parser::GetInt(CurID));

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

    // 타이틀
    RenderTitle(panel, mCompType, true, true, true, true, true);

    // 바디
    if(mExpanded)
    ZAY_XYWH_UI_SCISSOR(panel, (sint32) mPos.x, ((sint32) mPos.y) + TitleBarHeight, mBodySize.w, mBodySize.h, UIBody)
    {
        ZAY_RGBA(panel, 64, 64, 64, 128)
            panel.fill();
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
            json.At("compname").Set("el" + OneType + +'(' + mOperation.mOperation + ')');
        else json.At("compname").Set(OneType + +'(' + mOperation.mOperation + ')');
    }
    else json.At("compname").Set(OneType);
}

void ZEZayBoxCondition::Render(ZayPanel& panel)
{
    const String UIBody = String::Format("%d-body", mID);
    const String UIOperation = String::Format("%d-operation", mID);

    // 타이틀
    if(mOperation.mWithElse)
        RenderTitle(panel, "el" + mCompType, true, false, true, mHasElseAndOperation, true);
    else RenderTitle(panel, mCompType, true, false, true, mHasElseAndOperation, true);

    // 바디
    if(mExpanded && mHasElseAndOperation)
    ZAY_XYWH_UI_SCISSOR(panel, (sint32) mPos.x, ((sint32) mPos.y) + TitleBarHeight, mBodySize.w, mBodySize.h, UIBody)
    {
        ZAY_RGBA(panel, 64, 64, 64, 128)
            panel.fill();
        ZAY_INNER(panel, 4)
        {
            // 연산에디터
            ZAY_LTRB(panel, 0, 0, panel.w(), EditorHeight)
            ZAY_INNER(panel, 4)
                mOperation.RenderOperationEditor(panel, UIOperation);
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
ZEZayBoxError::ZEZayBoxError() : mNameComment(*this), mParamGroup(*this), mInputGroup(*this)
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
    if(auto CurID = json("compid").GetString(nullptr))
        mCompID = ValidLastID(Parser::GetInt(CurID));

    mNameComment.ReadJson(json);
    mParamGroup.ReadJson(json("compvalues"));
    mInputGroup.ReadJson(json("onclick"));
}

void ZEZayBoxError::WriteJson(Context& json) const
{
    json.At("compname").Set(mCompType);

    if(mCompID < 0) mCompID = MakeLastID();
    json.At("compid").Set(String::FromInteger(mCompID));

    mNameComment.WriteJson(json);
    if(0 < mParamGroup.mParams.Count())
        mParamGroup.WriteJson(json.At("compvalues"));
    if(0 < mInputGroup.mInputs.Count())
        mInputGroup.WriteJson(json.At("onclick"));
}

void ZEZayBoxError::Render(ZayPanel& panel)
{
    const String UIBody = String::Format("%d-body", mID);
    const String UIOperation = String::Format("%d-operation", mID);

    // 타이틀
    RenderTitle(panel, mCompType, true, true, true, false, true);
}

void ZEZayBoxError::RecalcSize()
{
    mBodySize.h = 0;
}

chars ZEZayBoxError::GetComment() const
{
    return mNameComment.mComment;
}
