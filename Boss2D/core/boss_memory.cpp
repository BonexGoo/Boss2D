#include <boss.hpp>
#include "boss_memory.hpp"

extern "C"
{
    extern void* malloc(boss_size_t);
    extern void free(void*);
    extern void* memset(void*, sint32, boss_size_t);
    extern void* memcpy(void*, const void*, boss_size_t);
    extern sint32 memcmp(const void*, const void*, boss_size_t);
}

#define CASE256(A) \
	case 0x00: A case 0x01: A case 0x02: A case 0x03: A \
	case 0x04: A case 0x05: A case 0x06: A case 0x07: A \
	case 0x08: A case 0x09: A case 0x0A: A case 0x0B: A \
	case 0x0C: A case 0x0D: A case 0x0E: A case 0x0F: A \
	case 0x10: A case 0x11: A case 0x12: A case 0x13: A \
	case 0x14: A case 0x15: A case 0x16: A case 0x17: A \
	case 0x18: A case 0x19: A case 0x1A: A case 0x1B: A \
	case 0x1C: A case 0x1D: A case 0x1E: A case 0x1F: A \
	case 0x20: A case 0x21: A case 0x22: A case 0x23: A \
	case 0x24: A case 0x25: A case 0x26: A case 0x27: A \
	case 0x28: A case 0x29: A case 0x2A: A case 0x2B: A \
	case 0x2C: A case 0x2D: A case 0x2E: A case 0x2F: A \
	case 0x30: A case 0x31: A case 0x32: A case 0x33: A \
	case 0x34: A case 0x35: A case 0x36: A case 0x37: A \
	case 0x38: A case 0x39: A case 0x3A: A case 0x3B: A \
	case 0x3C: A case 0x3D: A case 0x3E: A case 0x3F: A \
	case 0x40: A case 0x41: A case 0x42: A case 0x43: A \
	case 0x44: A case 0x45: A case 0x46: A case 0x47: A \
	case 0x48: A case 0x49: A case 0x4A: A case 0x4B: A \
	case 0x4C: A case 0x4D: A case 0x4E: A case 0x4F: A \
	case 0x50: A case 0x51: A case 0x52: A case 0x53: A \
	case 0x54: A case 0x55: A case 0x56: A case 0x57: A \
	case 0x58: A case 0x59: A case 0x5A: A case 0x5B: A \
	case 0x5C: A case 0x5D: A case 0x5E: A case 0x5F: A \
	case 0x60: A case 0x61: A case 0x62: A case 0x63: A \
	case 0x64: A case 0x65: A case 0x66: A case 0x67: A \
	case 0x68: A case 0x69: A case 0x6A: A case 0x6B: A \
	case 0x6C: A case 0x6D: A case 0x6E: A case 0x6F: A \
	case 0x70: A case 0x71: A case 0x72: A case 0x73: A \
	case 0x74: A case 0x75: A case 0x76: A case 0x77: A \
	case 0x78: A case 0x79: A case 0x7A: A case 0x7B: A \
	case 0x7C: A case 0x7D: A case 0x7E: A case 0x7F: A \
	case 0x80: A case 0x81: A case 0x82: A case 0x83: A \
	case 0x84: A case 0x85: A case 0x86: A case 0x87: A \
	case 0x88: A case 0x89: A case 0x8A: A case 0x8B: A \
	case 0x8C: A case 0x8D: A case 0x8E: A case 0x8F: A \
	case 0x90: A case 0x91: A case 0x92: A case 0x93: A \
	case 0x94: A case 0x95: A case 0x96: A case 0x97: A \
	case 0x98: A case 0x99: A case 0x9A: A case 0x9B: A \
	case 0x9C: A case 0x9D: A case 0x9E: A case 0x9F: A \
	case 0xA0: A case 0xA1: A case 0xA2: A case 0xA3: A \
	case 0xA4: A case 0xA5: A case 0xA6: A case 0xA7: A \
	case 0xA8: A case 0xA9: A case 0xAA: A case 0xAB: A \
	case 0xAC: A case 0xAD: A case 0xAE: A case 0xAF: A \
	case 0xB0: A case 0xB1: A case 0xB2: A case 0xB3: A \
	case 0xB4: A case 0xB5: A case 0xB6: A case 0xB7: A \
	case 0xB8: A case 0xB9: A case 0xBA: A case 0xBB: A \
	case 0xBC: A case 0xBD: A case 0xBE: A case 0xBF: A \
	case 0xC0: A case 0xC1: A case 0xC2: A case 0xC3: A \
	case 0xC4: A case 0xC5: A case 0xC6: A case 0xC7: A \
	case 0xC8: A case 0xC9: A case 0xCA: A case 0xCB: A \
	case 0xCC: A case 0xCD: A case 0xCE: A case 0xCF: A \
	case 0xD0: A case 0xD1: A case 0xD2: A case 0xD3: A \
	case 0xD4: A case 0xD5: A case 0xD6: A case 0xD7: A \
	case 0xD8: A case 0xD9: A case 0xDA: A case 0xDB: A \
	case 0xDC: A case 0xDD: A case 0xDE: A case 0xDF: A \
	case 0xE0: A case 0xE1: A case 0xE2: A case 0xE3: A \
	case 0xE4: A case 0xE5: A case 0xE6: A case 0xE7: A \
	case 0xE8: A case 0xE9: A case 0xEA: A case 0xEB: A \
	case 0xEC: A case 0xED: A case 0xEE: A case 0xEF: A \
	case 0xF0: A case 0xF1: A case 0xF2: A case 0xF3: A \
	case 0xF4: A case 0xF5: A case 0xF6: A case 0xF7: A \
	case 0xF8: A case 0xF9: A case 0xFA: A case 0xFB: A \
	case 0xFC: A case 0xFD: A case 0xFE: A case 0xFF: A

namespace BOSS
{
    void* Memory::Alloc(const sint32 size)
    {
        void* Result = malloc(size);
        BOSS_ASSERT("메모리가 부족합니다", Result);
        return Result;
    }

    void Memory::Free(void* ptr)
    {
        free(ptr);
    }

    void Memory::Set(void* ptr, const uint08 value, const sint32 size)
    {
        memset(ptr, value, size);
    }

    void Memory::Copy(void* dst, const void* src, const sint32 size)
    {
        memcpy(dst, src, size);
    }

    void Memory::CopyStencil(void* dst, const void* src, const sint32 size)
    {
        uint08* CurDst = (uint08*) dst;
        bytes CurSrc = (bytes) src;
        bytes CurSrcEnd = CurSrc + size;
        switch(~(size - 1) & 0xFF)
        while(CurSrc < CurSrcEnd)
        {
            CASE256(
                *CurDst = *(CurSrc++);
                CurDst += 2;)
        }
    }

    void Memory::Fill(void* dst, const sint32 dstsize, const void* src, sint32 srcsize)
    {
        memcpy(dst, src, Math::Min(dstsize, srcsize));
        // 번짐효과
        while(srcsize < dstsize)
        {
            memcpy((void*) (((bytes) dst) + srcsize), dst,
                Math::Min(dstsize, srcsize + srcsize) - srcsize);
            srcsize *= 2;
        }
    }

    void Memory::ReplaceVPTR(void* dst, const void* src)
    {
        BOSS_ASSERT("모든 컴파일러에 대한 검증이 안된 코드입니다.", false);
        switch(sizeof(void*))
        {
        case 4:
            *((uint32*) dst) = *((const uint32*) src);
            break;
        case 8:
            *((uint64*) dst) = *((const uint64*) src);
            break;
        default:
            BOSS_ASSERT("VPTR의 복사에 실패하였습니다", false);
            break;
        }
    }

    sint32 Memory::Compare(const void* dst, const void* src, const sint32 size)
    {
        return memcmp(dst, src, size);
    }
}
