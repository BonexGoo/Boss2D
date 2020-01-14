#include <boss.hpp>
#include "boss_share.hpp"

namespace BOSS
{
    const Share* Share::Create(buffer gift)
    {
        return new Share(gift);
    }

    const Share* Share::Create(const buffer& sample, sint32 mincount)
    {
        return new Share(sample, mincount);
    }

    void Share::Destroy(id_cloned_share cloned)
    {
        const Share* OldShare = (const Share*) cloned;
        Share::Remove(OldShare);
    }

    const Share* Share::CopyOnWrite(const Share* share, sint32 mincount)
    {
        if(share)
        {
            if(1 < share->mShareCount) // 독립
            {
                --_DebugShareCount();
                --share->mShareCount;
                // 사본화, 전체삭제의 경우에는 예외
                share = new Share(*share, mincount, mincount != -2);
            }
            if(share->mValidCount < mincount) // 확장
            {
                Share* wshare = (Share*) share;
                wshare->mValidCount = mincount;
                sint32 count = Math::Max(1, Buffer::CountOf(share->mData));
                while(count < mincount) count <<= 1;
                wshare->mData = Buffer::Realloc(BOSS_DBG share->mData, count);
            }
            else if(mincount == -1 && 0 < share->mValidCount) // 후미삭제
            {
                Share* wshare = (Share*) share;
                Buffer::Realloc(BOSS_DBG wshare->mData, --wshare->mValidCount);
            }
            else if(mincount == -2 && 0 < share->mValidCount) // 전체삭제
            {
                Share* wshare = (Share*) share;
                Buffer::Realloc(BOSS_DBG wshare->mData, wshare->mValidCount = 0);
            }
        }
        return share;
    }

    void Share::Remove(const Share*& share)
    {
        if(share)
        {
            --_DebugShareCount();
            if(--share->mShareCount == 0)
                delete share;
            share = nullptr;
        }
    }

    void Share::DebugPrint()
    {
        BOSS_TRACE("");
        BOSS_TRACE("==================== Share ====================");
        BOSS_TRACE(">>>> %d EA", _DebugShareCount());
        BOSS_TRACE("===============================================");
    }

    chars Share::Type() const
    {
        return Buffer::NameOf(mData);
    }

    sblock Share::TypeID() const
    {
        return Buffer::TypeOf(mData);
    }

    const Share* Share::Clone() const
    {
        ++_DebugShareCount();
        ++mShareCount;
        return this;
    }

    buffer Share::CopiedBuffer() const
    {
        buffer NewBuffer = Buffer::AllocBySample(BOSS_DBG mValidCount, mValidCount, mData);
        Buffer::Copy(NewBuffer, 0, mData, 0, mValidCount);
        return NewBuffer;
    }

    Share::Share(buffer gift)
    {
        ++_DebugShareCount();
        mShareCount = 1;
        mValidCount = Buffer::CountOf(gift);
        mData = gift;
    }

    Share::Share(const buffer& sample, sint32 mincount)
    {
        ++_DebugShareCount();
        mShareCount = 1;
        mValidCount = 0;
        mData = Buffer::AllocBySample(BOSS_DBG 0, mincount, sample);
    }

    Share::Share(const Share& rhs, sint32 mincount, bool duplicate)
    {
        const sint32 rhscount = Buffer::CountOf(rhs.mData);
        const sint32 maxcount = (rhscount > mincount)? rhscount : mincount;

        ++_DebugShareCount();
        mShareCount = 1;
        mValidCount = 0;
        mData = Buffer::AllocBySample(BOSS_DBG rhscount, maxcount, rhs.mData);

        if(duplicate) // 사본으로 구성
        {
            mValidCount = rhs.mValidCount;
            Buffer::Copy(mData, 0, rhs.mData, 0, mValidCount);
        }
    }

    Share::~Share()
    {
        Buffer::Free(mData);
    }

    sint32& Share::_DebugShareCount()
    {
        static sint32 ShareCount = 0;
        return ShareCount;
    }
}
