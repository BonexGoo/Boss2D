#include <boss.hpp>
#include "boss_buffer.hpp"

namespace BOSS
{
    class NakedBuffer
    {
    public:
        const BufferSpec* mSpec;
        sint32 mCount;
        sint32 mMax;

    public:
        void* DataPtr() const
        {return ((uint08*) this) + sizeof(NakedBuffer);}

    public:
        static NakedBuffer* Alloc(sint32 datasize)
        {return (NakedBuffer*) Memory::Alloc(sizeof(NakedBuffer) + datasize);}

        static NakedBuffer* From(const buffer buf)
        {return (NakedBuffer*) (((uint08*) buf) - sizeof(NakedBuffer));}
    };

    buffer BufferImpl::Alloc(BOSS_DBG_PRM sint32 count, sint32 max, const BufferSpec* spec, bool noConstructorOnce)
    {
        NakedBuffer* NewNaked = NakedBuffer::Alloc(spec->SizeOf() * max);
        NewNaked->mSpec = spec;
        NewNaked->mCount = count;
        NewNaked->mMax = max;
        if(!noConstructorOnce)
            spec->Create(NewNaked->DataPtr(), count);
        return (buffer) NewNaked->DataPtr();
    }

    void BufferImpl::Free(const buffer buf)
    {
        const NakedBuffer* OldNaked = NakedBuffer::From(buf);
        Memory::Free((void*) OldNaked);
    }

    void* BufferImpl::At(const buffer buf, sint32 index)
    {
        const NakedBuffer* Naked = NakedBuffer::From(buf);
        BOSS_ASSERT("허용범위를 초과한 인덱스입니다", 0 <= index && index < Naked->mCount);
        return ((uint08*) buf) + Naked->mSpec->SizeOf() * index;
    }

    buffer Buffer::AllocBySample(BOSS_DBG_PRM sint32 count, sint32 max, const buffer sample)
    {
        const NakedBuffer* SampleNaked = NakedBuffer::From(sample);
        return BufferImpl::Alloc(BOSS_DBG_ARG count, max, SampleNaked->mSpec, false);
    }

    buffer Buffer::Realloc(BOSS_DBG_PRM buffer buf, sint32 count)
    {
        NakedBuffer* OldNaked = NakedBuffer::From(buf);
        if(OldNaked->mCount == count) return buf;

        // 버퍼확장
        if(OldNaked->mMax < count)
        {
            buffer NewBuffer = BufferImpl::Alloc(BOSS_DBG_ARG count, count, OldNaked->mSpec, false);
            OldNaked->mSpec->Move(NewBuffer, buf, OldNaked->mCount);
            Free(buf);
            return NewBuffer;
        }

        // 추가생성
        if(OldNaked->mCount < count)
        {
            void* Ptr = ((uint08*) buf) + (OldNaked->mSpec->SizeOf() * OldNaked->mCount);
            OldNaked->mSpec->Create(Ptr, count - OldNaked->mCount);
            OldNaked->mCount = count;
            return buf;
        }

        // 추가소멸
        void* Ptr = ((uint08*) buf) + (OldNaked->mSpec->SizeOf() * count);
        OldNaked->mSpec->Remove(Ptr, OldNaked->mCount - count, false);
        OldNaked->mCount = count;
        return buf;
    }

    buffer Buffer::Clone(BOSS_DBG_PRM const buffer src)
    {
        if(!src) return nullptr;
        const sint32 CopyCount = CountOf(src);
        buffer DstBuffer = AllocBySample(BOSS_DBG_ARG CopyCount, CopyCount, src);
        Copy(DstBuffer, 0, src, 0, CopyCount);
        return DstBuffer;
    }

    void Buffer::Copy(buffer dstBuf, sint32 dstOff, const buffer srcBuf, sint32 srcOff, sint32 count)
    {
        if(count <= 0) return;
        const NakedBuffer* DstNaked = NakedBuffer::From(dstBuf);
        const NakedBuffer* SrcNaked = NakedBuffer::From(srcBuf);
        BOSS_ASSERT("서로 다른 타입간 복사입니다", DstNaked->mSpec == SrcNaked->mSpec);
        BOSS_ASSERT("복사범위가 dst를 초과합니다", 0 <= dstOff && dstOff + count <= DstNaked->mCount);
        BOSS_ASSERT("복사범위가 src를 초과합니다", 0 <= srcOff && srcOff + count <= SrcNaked->mCount);
        DstNaked->mSpec->Copy(((uint08*) dstBuf) + SizeOf(dstBuf) * dstOff,
            ((bytes) srcBuf) + SizeOf(srcBuf) * srcOff, count);
    }

    void Buffer::Free(buffer buf)
    {
        if(!buf) return;
        const NakedBuffer* OldNaked = NakedBuffer::From(buf);
        OldNaked->mSpec->Remove(buf, OldNaked->mCount, true);
    }

    sint32 Buffer::CountOf(const buffer buf)
    {
        const NakedBuffer* Naked = NakedBuffer::From(buf);
        return Naked->mCount;
    }

    sint32 Buffer::SizeOf(const buffer buf)
    {
        const NakedBuffer* Naked = NakedBuffer::From(buf);
        return Naked->mSpec->SizeOf();
    }

    chars Buffer::NameOf(const buffer buf)
    {
        const NakedBuffer* Naked = NakedBuffer::From(buf);
        return Naked->mSpec->NameOf();
    }

    sblock Buffer::TypeOf(const buffer buf)
    {
        const NakedBuffer* Naked = NakedBuffer::From(buf);
        return BOSS_PTR_TO_SBLOCK(Naked->mSpec);
    }

    void Buffer::ResetOne(buffer buf, sint32 index)
    {
        const NakedBuffer* Naked = NakedBuffer::From(buf);
        Naked->mSpec->ResetOne(buf, index);
    }

    void Buffer::ResetAll(buffer buf)
    {
        const NakedBuffer* Naked = NakedBuffer::From(buf);
        Naked->mSpec->ResetAll(buf, Naked->mCount);
    }
}
