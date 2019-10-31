#pragma once
#include <boss_memory.hpp>

#define BOSS_FRIENDSHIP_FOR_BUFFER(TYPE) friend class BufferSpecPool<TYPE>;
#define BOSS_CONSTRUCTOR(PTR, IDX, TYPE, ...) new(BOSS_PTR_TO_SBLOCK(((buffer) PTR) + sizeof(TYPE) * (IDX))) TYPE(__VA_ARGS__)
#define BOSS_DECLARE_BUFFERED_CLASS(TYPE, PARENT, ...) \
    class TYPE : public PARENT {public: TYPE() : PARENT(__VA_ARGS__) {BOSS_ASSERT("사용금지", false);} ~TYPE() {} \
    TYPE& operator=(const TYPE& rhs) {BOSS_ASSERT("사용금지", false); return *this;}}
#if BOSS_MAC_OSX || BOSS_IPHONE || BOSS_X64
    #define BOSS_PTR_TO_SBLOCK(VALUE) ((sblock)(sint64) VALUE)
#else
    #define BOSS_PTR_TO_SBLOCK(VALUE) ((sblock) VALUE)
#endif

namespace BOSS
{
    /// @brief 버퍼스펙지원
    class BufferSpec
    {
    public:
        BufferSpec(void(*init)(BufferSpec*), sint32 size, chars name)
        {
            m_size = size;
            m_name = name;
            Copy = nullptr;
            Move = nullptr;
            Create = nullptr;
            Remove = nullptr;
            ResetOne = nullptr;
            ResetAll = nullptr;
            init(this);
        }
        ~BufferSpec() {}

    private:
        sint32 m_size;
        chars m_name;

    public:
        inline sint32 SizeOf() const {return m_size;}
        inline chars NameOf() const {return m_name;}
        void (*Copy)(void*, const void*, const sint32);
        void (*Move)(void*, void*, const sint32);
        void (*Create)(void*, const sint32);
        void (*Remove)(void*, const sint32, bool);
        void (*ResetOne)(void*, const sint32);
        void (*ResetAll)(void*, const sint32);
    };

    /// @brief 버퍼도구지원
    class BufferImpl
    {
    public:
        static buffer Alloc(BOSS_DBG_PRM sint32 count, sint32 max, const BufferSpec* spec, bool noConstructorOnce);
        static void Free(const buffer buf);
        static void* At(const buffer buf, sint32 index);
    };

    /// @brief 버퍼스펙풀지원
    template<typename TYPE>
    class BufferSpecPool
    {
    public:
        static const BufferSpec* Summon(EnumToType<datatype_class_nomemcpy>)
        {
            static BufferSpec Spec([](BufferSpec* self)->void
            {
                self->Copy = Copy_class_nomemcpy;
                self->Move = Move_class_nomemcpy;
                self->Create = Create_class;
                self->Remove = Remove_class;
                self->ResetOne = ResetOne_class;
                self->ResetAll = ResetAll_class;
            }, sizeof(TYPE), Naming());
            return &Spec;
        }
        static const BufferSpec* Summon(EnumToType<datatype_class_canmemcpy>)
        {
            static BufferSpec Spec([](BufferSpec* self)->void
            {
                self->Copy = Copy_default;
                self->Move = Move_default;
                self->Create = Create_class;
                self->Remove = Remove_class;
                self->ResetOne = ResetOne_class;
                self->ResetAll = ResetAll_class;
            }, sizeof(TYPE), Naming());
            return &Spec;
        }
        static const BufferSpec* Summon(EnumToType<datatype_pod_canmemcpy>)
        {
            static BufferSpec Spec([](BufferSpec* self)->void
            {
                self->Copy = Copy_default;
                self->Move = Move_default;
                self->Create = Create_default;
                self->Remove = Remove_default;
                self->ResetOne = ResetOne_default;
                self->ResetAll = ResetAll_default;
            }, sizeof(TYPE), Naming());
            return &Spec;
        }
        static const BufferSpec* Summon(EnumToType<datatype_pod_canmemcpy_zeroset>)
        {
            static BufferSpec Spec([](BufferSpec* self)->void
            {
                self->Copy = Copy_default;
                self->Move = Move_default;
                self->Create = Create_pod_canmemcpy_zeroset;
                self->Remove = Remove_default;
                self->ResetOne = ResetOne_pod_canmemcpy_zeroset;
                self->ResetAll = ResetAll_pod_canmemcpy_zeroset;
            }, sizeof(TYPE), Naming());
            return &Spec;
        }
        static chars Naming()
        {
            static char TypeName[256] = {'\0'};
            if(TypeName[0] == '\0')
            {
                chars TypeNameBegin = nullptr;
                chars TypeNameEnd = nullptr;
                #if BOSS_WINDOWS & !BOSS_WINDOWS_MINGW
                    chars FuncFocus = __FUNCSIG__;
                    while(*FuncFocus != '<') ++FuncFocus;
                    TypeNameBegin = ++FuncFocus;
                    while(*FuncFocus != '>') ++FuncFocus;
                    TypeNameEnd = FuncFocus;
                #else
                    chars FuncFocus = __PRETTY_FUNCTION__;
                    while(*FuncFocus != '[') ++FuncFocus;
                    TypeNameBegin = ++FuncFocus;
                    while(*FuncFocus != ']' && *FuncFocus != ';') ++FuncFocus;
                    TypeNameEnd = FuncFocus;
                #endif
                const sint32 TypeNameLength = (sint32)(TypeNameEnd - TypeNameBegin);
                BOSS_ASSERT("타입명이 너무 깁니다", TypeNameLength < 256);
                Memory::Copy(TypeName, TypeNameBegin, TypeNameLength);
                TypeName[TypeNameLength] = '\0';
            }
            return TypeName;
        }

    private:
        static void Copy_class_nomemcpy(void* dst, const void* src, const sint32 count) {for(sint32 i = 0; i < count; ++i) ((TYPE*) dst)[i] = ((const TYPE*) src)[i];}
        static void Copy_default(void* dst, const void* src, const sint32 count)        {Memory::Copy(dst, src, sizeof(TYPE) * count);}
        static void Move_class_nomemcpy(void* dst, void* src, const sint32 count)       {for(sint32 i = 0; i < count; ++i) ((TYPE*) dst)[i] = ToReference(((TYPE*) src)[i]);}
        static void Move_default(void* dst, void* src, const sint32 count)              {Memory::Copy(dst, src, sizeof(TYPE) * count);}
        static void Create_class(void* dst, const sint32 count)                         {for(sint32 i = 0; i < count; ++i) new(BOSS_PTR_TO_SBLOCK(&((TYPE*) dst)[i])) TYPE;}
        static void Create_pod_canmemcpy_zeroset(void* dst, const sint32 count)         {Memory::Set(dst, 0, sizeof(TYPE) * count);}
        static void Create_default(void* dst, const sint32 count)                       {}
        static void Remove_class(void* dst, const sint32 count, bool dofree)            {for(sint32 i = 0; i < count; ++i) ((TYPE*) dst)[i].~TYPE(); if(dofree) BufferImpl::Free((buffer) dst);}
        static void Remove_default(void* dst, const sint32 count, bool dofree)          {if(dofree) BufferImpl::Free((buffer) dst);}
        static void ResetOne_class(void* dst, const sint32 index)                       {((TYPE*) dst)[index].~TYPE(); new(BOSS_PTR_TO_SBLOCK(&((TYPE*) dst)[index])) TYPE;}
        static void ResetOne_pod_canmemcpy_zeroset(void* dst, const sint32 index)       {Memory::Set(&(((TYPE*) dst)[index]), 0, sizeof(TYPE));}
        static void ResetOne_default(void* dst, const sint32 index)                     {}
        static void ResetAll_class(void* dst, const sint32 count)                       {for(sint32 i = 0; i < count; ++i) {((TYPE*) dst)[i].~TYPE(); new(BOSS_PTR_TO_SBLOCK(&((TYPE*) dst)[i])) TYPE;}}
        static void ResetAll_pod_canmemcpy_zeroset(void* dst, const sint32 count)       {Memory::Set(dst, 0, sizeof(TYPE) * count);}
        static void ResetAll_default(void* dst, const sint32 count)                     {}
    };

    /// @brief 버퍼지원
    class Buffer
    {
    public:
        /// @brief 할당
        /// @param count : 배열수량
        /// @return 할당된 버퍼주소
        /// @see AllocBySample, Realloc, Free
        static buffer Alloc(BOSS_DBG_PRM sint32 count)
        {return BufferImpl::Alloc(BOSS_DBG_ARG count, count, BufferSpecPool<uint08>::Summon(EnumToType<datatype_pod_canmemcpy>()), false);}

        /// @brief 객체할당
        /// @param count : 배열수량
        /// @param type : 버퍼관리타입
        /// @return 할당된 버퍼주소
        /// @see AllocBySample, Realloc, Free
        template<typename TYPE, datatype DATATYPE = datatype_class_nomemcpy>
        static buffer Alloc(BOSS_DBG_PRM sint32 count)
        {return BufferImpl::Alloc(BOSS_DBG_ARG count, count, BufferSpecPool<TYPE>::Summon(EnumToType<DATATYPE>()), false);}

        /// @brief 생성자호출 회피식 객체할당
        /// @param count : 배열수량
        /// @param type : 버퍼관리타입
        /// @return 할당된 버퍼주소
        /// @see AllocBySample, Realloc, Free
        template<typename TYPE, datatype DATATYPE = datatype_class_nomemcpy>
        static buffer AllocNoConstructorOnce(BOSS_DBG_PRM sint32 count)
        {return BufferImpl::Alloc(BOSS_DBG_ARG count, count, BufferSpecPool<TYPE>::Summon(EnumToType<DATATYPE>()), true);}

        /// @brief 참고할당(샘플과 동일한 타입으로)
        /// @param count : 배열수량
        /// @param max : 버퍼수량
        /// @param sample : 참고되는 버퍼주소
        /// @return 할당된 버퍼주소
        /// @see Alloc, Realloc, Free
        static buffer AllocBySample(BOSS_DBG_PRM sint32 count, sint32 max, const buffer sample);

        /// @brief 재할당(배열수량이 변경되었을 때만)
        /// @param buf : 버퍼주소
        /// @param count : 재할당할 배열수량
        /// @return 재할당된 버퍼주소
        /// @see Alloc, AllocBySample, Free
        static buffer Realloc(BOSS_DBG_PRM buffer buf, sint32 count);

        /// @brief 복제
        /// @param src : 복제할 버퍼
        /// @return 복제된 버퍼주소
        /// @see Copy
        static buffer Clone(BOSS_DBG_PRM const buffer src);

        /// @brief 복사
        /// @param dstBuf : 붙여넣을 버퍼
        /// @param dstOff : 붙여넣을 오프셋
        /// @param srcBuf : 복사할 버퍼
        /// @param srcOff : 복사할 오프셋
        /// @param count : 복사할 배열수량
        /// @see Clone
        static void Copy(buffer dstBuf, sint32 dstOff, const buffer srcBuf, sint32 srcOff, sint32 count);

        /// @brief 해제
        /// @param buf : 버퍼주소
        /// @see Alloc, AllocBySample, Realloc
        static void Free(buffer buf);

        /// @brief 배열수량 구하기
        /// @param buf : 버퍼주소
        /// @return 배열수량
        /// @see SizeOf, NameOf, TypeOf
        static sint32 CountOf(const buffer buf);

        /// @brief 객체사이즈 구하기
        /// @param buf : 버퍼주소
        /// @return 객체사이즈
        /// @see CountOf, NameOf, TypeOf
        static sint32 SizeOf(const buffer buf);

        /// @brief 객체이름 구하기
        /// @param buf : 버퍼주소
        /// @return 객체이름
        /// @see CountOf, SizeOf, TypeOf
        static chars NameOf(const buffer buf);

        /// @brief 객체타입 구하기
        /// @param buf : 버퍼주소
        /// @return 객체타입(해당 BufferSpec의 주소)
        /// @see CountOf, SizeOf, NameOf, TypeCheck
        static sblock TypeOf(const buffer buf);

        /// @brief 객체타입 구하기(BufferSpec을 이용)
        /// @return 객체타입(해당 BufferSpec의 주소)
        /// @see CountOf, SizeOf, NameOf, TypeCheck
        template<typename TYPE, datatype DATATYPE>
        static sblock TypeOf()
        {return (sblock) BufferSpecPool<TYPE>::Summon(EnumToType<DATATYPE>());}

        /// @brief 객체타입 확인하기
        /// @param buf : 버퍼주소
        /// @return 타입일치여부
        /// @see TypeOf
        template<typename TYPE, datatype DATATYPE>
        static bool TypeCheck(const buffer buf)
        {return (TypeOf(buf) == TypeOf<TYPE, DATATYPE>());}

        /// @brief 쓰기접근
        /// @param buf : 버퍼주소
        /// @param index : 인덱스
        /// @return 해당 객체
        template<typename TYPE = uint08>
        static TYPE& At(const buffer buf, sint32 index)
        {return *((TYPE*) BufferImpl::At(buf, index));}

        /// @brief 개별초기화
        /// @param buf : 버퍼주소
        /// @param index : 초기화할 인덱스
        static void ResetOne(buffer buf, sint32 index);

        /// @brief 전체초기화
        /// @param buf : 버퍼주소
        static void ResetAll(buffer buf);

        /// @brief 잔존메모리 현황보고
        static void DebugPrint();
    };
}
