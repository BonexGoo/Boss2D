#pragma once
#include <boss_buffer.hpp>

namespace BOSS
{
    BOSS_DECLARE_ID(id_share);
    BOSS_DECLARE_ID(id_cloned_share);

    /// @brief 공유자원관리
    class Share
    {
    public:
        static const Share* Create(buffer gift);
        static const Share* Create(const buffer& sample, sint32 mincount);
        static void Destroy(id_cloned_share cloned);
        static const Share* CopyOnWrite(const Share* share, sint32 mincount);
        static void Remove(const Share*& share);
        static void DebugPrint();

    public:
        chars Type() const;
        sblock TypeID() const;
        const Share* Clone() const;
        template<typename TYPE>
        TYPE& At(sint32 index) const
        {return Buffer::At<TYPE>(mData, index);}
        buffer CopiedBuffer() const;

    public:
        inline sint32 count() const {return mValidCount;}
        inline const buffer const_data() const {return mData;}

    private:
        Share(buffer gift);
        Share(const buffer& sample, sint32 mincount);
        Share(const Share& rhs, sint32 mincount, bool spaceonly);
        ~Share();

    private:
        static sint32& _DebugShareCount();

    private:
        mutable sint32 mShareCount;
        sint32 mValidCount;
        buffer mData;
    };
}
