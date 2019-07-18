#include <boss.hpp>
#include "boss_buffer.hpp"

#define CALC_BLOCK_LEN(BYTES_LEN) (((BYTES_LEN) + sizeof(sblock) - 1) / sizeof(sblock))
#define CALC_BYTES_LEN(BLOCK_LEN) (sizeof(sblock) * (2 + (BLOCK_LEN) + SIZE_OF_DEBUGINFO))
#define LINK_L (SIZE_OF_DEBUGINFO - 2)
#define LINK_R (SIZE_OF_DEBUGINFO - 1)
#if !BOSS_NDEBUG & BOSS_NEED_LOW_DEBUGING
    #define SIZE_OF_DEBUGINFO (7)
#else
    #define SIZE_OF_DEBUGINFO (0)
#endif

namespace BOSS
{
    buffer BufferImpl::Alloc(BOSS_DBG_PRM sint32 count, const BufferSpec* spec, bool noConstructorOnce)
    {
        const sint32 RealLength = spec->SizeOf() * count;
        const sint32 BlockLength = CALC_BLOCK_LEN(RealLength);
        const sint32 BytesLength = CALC_BYTES_LEN(BlockLength);
        nakedbuffer NewNaked = (nakedbuffer) Memory::Alloc(BytesLength);
        BOSS_ASSERT("메모리가 부족합니다", NewNaked);

        NewNaked[0] = count;
        NewNaked[1] = (sblock) spec;
        if(!noConstructorOnce)
            spec->Create(&NewNaked[2], count);
        BufferImpl::DebugAttach(BOSS_DBG_ARG NewNaked, BlockLength, RealLength);
        return (buffer) &NewNaked[2];
    }

    void BufferImpl::Free(const buffer buf)
    {
        const nakedbuffer OldNaked = BufferImpl::Convert(buf);
        BufferImpl::DebugDetach(OldNaked);
        Memory::Free((void*) OldNaked);
    }

    const nakedbuffer BufferImpl::Convert(const buffer buf)
    {
        BOSS_ASSERT("버퍼가 nullptr입니다", buf);
        const nakedbuffer Naked = (const nakedbuffer) &buf[-2];
        const BufferSpec* Spec = (const BufferSpec*) Naked[1];
        #if !BOSS_NDEBUG & BOSS_NEED_LOW_DEBUGING
            BOSS_ASSERT("버퍼가 손상되었습니다", Naked[0] == ~Naked[2 + CALC_BLOCK_LEN(Spec->SizeOf() * Naked[0])]);
        #endif
        return Naked;
    }

    void* BufferImpl::At(const buffer buf, sint32 index, chars castingName)
    {
        const nakedbuffer Naked = BufferImpl::Convert(buf);
        const BufferSpec* Spec = (const BufferSpec*) Naked[1];
        BOSS_ASSERT("허용범위를 초과한 인덱스입니다", 0 <= index && index < Naked[0]);

        #if !BOSS_NDEBUG & BOSS_NEED_LOW_DEBUGING
            if(Spec->NameOf() != castingName) // 동일한 타입은 명칭의 주소도 같음
            {
                chars ChildFocus = Spec->NameOf();
                chars ParentFocus = castingName;
                while(*ParentFocus && *(ChildFocus++) == *(ParentFocus++));
                BOSS_ASSERT("접근타입이 다르며 상속관계도 아닙니다", *ParentFocus == '\0');
            }
        #endif
        return ((uint08*) buf) + Spec->SizeOf() * index;
    }

    void BufferImpl::Mutex(bool lock)
    {
        static id_mutex MutexID = Mutex::Open();
        if(lock) Mutex::Lock(MutexID);
        else Mutex::Unlock(MutexID);
    }

    void BufferImpl::DebugAttach(BOSS_DBG_PRM const nakedbuffer naked, sint32 blockLen, sint32 realLen)
    {
        #if !BOSS_NDEBUG & BOSS_NEED_LOW_DEBUGING
            sblock* DebugInfo = &naked[2 + blockLen];
            DebugInfo[0] = ~naked[0]; // ~Count정보
            DebugInfo[1] = (sblock) realLen;
            DebugInfo[2] = (sblock) BOSS_DBG_FILE;
            DebugInfo[3] = (sblock) BOSS_DBG_LINE;
            DebugInfo[4] = (sblock) BOSS_DBG_FUNC;
            BufferImpl::Mutex(true);
                DebugInfo[LINK_L] = BufferImpl::DebugLink()[LINK_L];
                DebugInfo[LINK_R] = (sblock) BufferImpl::DebugLink();
                ((sblock*) BufferImpl::DebugLink()[LINK_L])[LINK_R] = (sblock) DebugInfo;
                BufferImpl::DebugLink()[LINK_L] = (sblock) DebugInfo;
            BufferImpl::Mutex(false);
        #endif
    }

    void BufferImpl::DebugDetach(const nakedbuffer naked)
    {
        #if !BOSS_NDEBUG & BOSS_NEED_LOW_DEBUGING
            const BufferSpec* Spec = (const BufferSpec*) naked[1];
            const sint32 BlockLength = CALC_BLOCK_LEN(Spec->SizeOf() * naked[0]);
            sblock* DebugInfo = &naked[2 + BlockLength];
            BufferImpl::Mutex(true);
                ((sblock*) DebugInfo[LINK_L])[LINK_R] = DebugInfo[LINK_R];
                ((sblock*) DebugInfo[LINK_R])[LINK_L] = DebugInfo[LINK_L];
            BufferImpl::Mutex(false);
        #endif
    }

    sblock* BufferImpl::DebugLink()
    {
        #if !BOSS_NDEBUG & BOSS_NEED_LOW_DEBUGING
            static sblock Root[SIZE_OF_DEBUGINFO] = {~((sblock) 0), (sblock) 0,
                (sblock) __FILE__, (sblock) __LINE__, (sblock) __FUNCTION__, (sblock) Root, (sblock) Root};
            return Root;
        #else
            return nullptr;
        #endif
    }

    buffer Buffer::AllocBySample(BOSS_DBG_PRM sint32 count, const buffer sample)
    {
        const nakedbuffer SampleNaked = BufferImpl::Convert(sample);
        const BufferSpec* SampleSpec = (const BufferSpec*) SampleNaked[1];
        return BufferImpl::Alloc(BOSS_DBG_ARG count, SampleSpec, false);
    }

    buffer Buffer::Realloc(BOSS_DBG_PRM buffer buf, sint32 count)
    {
        const nakedbuffer OldNaked = BufferImpl::Convert(buf);
        const BufferSpec* OldSpec = (const BufferSpec*) OldNaked[1];
        if(count == OldNaked[0]) return buf;

        buffer NewBuffer = BufferImpl::Alloc(BOSS_DBG_ARG count, OldSpec, false);
        if(const sint32 CopyCount = Math::Min(count, OldNaked[0]))
            OldSpec->Move(NewBuffer, buf, CopyCount);
        Free(buf);
        return NewBuffer;
    }

    buffer Buffer::Clone(BOSS_DBG_PRM const buffer src)
    {
        if(!src) return nullptr;
        const sint32 CopyCount = CountOf(src);

        buffer DstBuffer = AllocBySample(BOSS_DBG_ARG CopyCount, src);
        Copy(DstBuffer, 0, src, 0, CopyCount);
        return DstBuffer;
    }

    void Buffer::Copy(buffer dstBuf, sint32 dstOff, const buffer srcBuf, sint32 srcOff, sint32 count)
    {
        if(count <= 0) return;
        const nakedbuffer DstNaked = BufferImpl::Convert(dstBuf);
        const nakedbuffer SrcNaked = BufferImpl::Convert(srcBuf);
        BOSS_ASSERT("서로 다른 타입간 복사입니다", DstNaked[1] == SrcNaked[1]);
        BOSS_ASSERT("복사범위가 dst를 초과합니다", 0 <= dstOff && dstOff + count <= DstNaked[0]);
        BOSS_ASSERT("복사범위가 src를 초과합니다", 0 <= srcOff && srcOff + count <= SrcNaked[0]);

        const BufferSpec* DstSpec = (const BufferSpec*) DstNaked[1];
        DstSpec->Copy(((uint08*) dstBuf) + SizeOf(dstBuf) * dstOff,
            ((bytes) srcBuf) + SizeOf(srcBuf) * srcOff, count);
    }

    void Buffer::Free(buffer buf)
    {
        if(!buf) return;
        const nakedbuffer OldNaked = BufferImpl::Convert(buf);
        const BufferSpec* OldSpec = (const BufferSpec*) OldNaked[1];
        OldSpec->Remove(buf, buf[-2]);
    }

    sint32 Buffer::CountOf(const buffer buf)
    {
        const nakedbuffer Naked = BufferImpl::Convert(buf);
        return (sint32) Naked[0];
    }

    sint32 Buffer::SizeOf(const buffer buf)
    {
        const nakedbuffer Naked = BufferImpl::Convert(buf);
        const BufferSpec* Spec = (const BufferSpec*) Naked[1];
        return Spec->SizeOf();
    }

    chars Buffer::NameOf(const buffer buf)
    {
        const nakedbuffer Naked = BufferImpl::Convert(buf);
        const BufferSpec* Spec = (const BufferSpec*) Naked[1];
        return Spec->NameOf();
    }

    sblock Buffer::TypeOf(const buffer buf)
    {
        const nakedbuffer Naked = BufferImpl::Convert(buf);
        return Naked[1];
    }

    void Buffer::ResetOne(buffer buf, sint32 index)
    {
        const nakedbuffer Naked = BufferImpl::Convert(buf);
        const BufferSpec* Spec = (const BufferSpec*) Naked[1];
        Spec->ResetOne(buf, index);
    }

    void Buffer::ResetAll(buffer buf)
    {
        const nakedbuffer Naked = BufferImpl::Convert(buf);
        const BufferSpec* Spec = (const BufferSpec*) Naked[1];
        Spec->ResetAll(buf, (sint32) Naked[0]);
    }

    void Buffer::DebugPrint()
    {
        #if !BOSS_NDEBUG & BOSS_NEED_LOW_DEBUGING
            BufferImpl::Mutex(true);
                const sblock* CurDebugInfo = BufferImpl::DebugLink();
                BOSS_TRACE("");
                BOSS_TRACE("==================== Buffer ====================");
                sint32 TotalBytes = 0, TotalCount = 0;
                while((CurDebugInfo = (const sblock*) CurDebugInfo[LINK_R]) != BufferImpl::DebugLink())
                {
                    TotalBytes += (sint32) CurDebugInfo[1];
                    TotalCount++;
                    BOSS_TRACE(">>>> %8d BYTES(%s)     .....%s(%d Ln)",
                        (sint32) CurDebugInfo[1], (chars) CurDebugInfo[4], (chars) CurDebugInfo[2], (sint32) CurDebugInfo[3]);
                }
                BOSS_TRACE("================================================");
                BOSS_TRACE(">>>> %8d BYTES(%d ea)", TotalBytes, TotalCount);
                BOSS_TRACE("================================================");
            BufferImpl::Mutex(false);
        #endif
    }
}
