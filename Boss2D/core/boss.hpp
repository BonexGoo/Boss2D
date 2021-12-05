#pragma once
#include <boss.h>

namespace BOSS {}
using namespace BOSS;

namespace BOSS
{
    // About base types
    typedef signed char                       sint08; // "signed char" for __GNUC__
    typedef unsigned char                     uint08;
    typedef signed short                      sint16;
    typedef unsigned short                    uint16;
    typedef signed int                        sint32;
    typedef unsigned int                      uint32;
    typedef signed long long                  sint64;
    typedef unsigned long long                uint64;
    typedef const unsigned char*              bytes;
    typedef const char*                       chars;
    typedef const char*                       chars_cp949;
    typedef const char*                       chars_kssm;
    typedef const char*                       chars_endless;
    typedef const wchar_t*                    wchars;
    typedef const wchar_t*                    wchars_endless;
    typedef unsigned int                      argb32;
    typedef unsigned int                      ahsv32;
    typedef struct {uint08 v, u;}             uv16;
    typedef const uv16*                       uv16s;
    typedef struct {unsigned char ip[4];}     ip4address;
    typedef struct {unsigned char ip[16];}    ip6address;

    typedef struct {sint32 x, y;}             point64;
    typedef struct {float x, y;}              point64f;
    typedef struct {sint32 w, h;}             size64;
    typedef struct {float w, h;}              size64f;
    typedef struct {sint32 l, t, r, b;}       rect128;
    typedef struct {float l, t, r, b;}        rect128f;
    typedef struct {sint32 x, y, vx, vy;}     vector128;
    typedef struct {float x, y, vx, vy;}      vector128f;
    typedef struct {sint32 ox, oy, r, rad;}   segment128;
    typedef struct {float ox, oy, r, rad;}    segment128f;
    typedef struct {sint32 i, iend;}          zone64;
    typedef struct {sint32 x, xend;}          xzone64;
    typedef struct {sint32 y, yend;}          yzone64;
    typedef struct {sint16 i, iend, x, xend;} ixzone64;
    typedef struct {sint16 i, iend, y, yend;} iyzone64;

    // About extended types
    typedef void*                             payload;
    typedef signed long long                  unknown;
    typedef bool                              autorun;
    typedef signed long long                  dependency;
    typedef const signed long long            linkstep1;
    typedef const signed long long            linkstep2;
    typedef const signed long long            linkstep3;
    typedef const signed long long            linkstep4;
    typedef const signed long long            linkstep5;
    typedef const signed long long            linkstep6;
    typedef const signed long long            linkstep7;
    typedef const signed long long            linkstep8;
    typedef const signed long long            linkstep9;

    // About buffer
    #if BOSS_X64
        typedef sint64                        sblock;
        typedef uint64                        ublock;
    #else
        typedef sint32                        sblock;
        typedef uint32                        ublock;
    #endif
    typedef ublock*                           buffer;
    typedef buffer                            nullbuffer;

    // About pointer checker
    template<typename TYPE>
    inline bool _IsTypePointer(TYPE&) {return false;}
    template<typename TYPE>
    inline bool _IsTypePointer(TYPE*&) {return true;}
    template<typename TYPE>
    inline bool IsTypePointer()
    {
        TYPE* Pointer = nullptr;
        TYPE& Variable = *Pointer;
        return _IsTypePointer(Variable);
    }

    // About reference converter
    template<class TYPE>
    struct _ToReference {typedef TYPE type;};
    template<class TYPE>
    struct _ToReference<TYPE&> {typedef TYPE type;};
    template<class TYPE>
    struct _ToReference<TYPE&&> {typedef TYPE type;};
    template<typename TYPE>
    inline typename _ToReference<TYPE>::type&& ToReference(TYPE&& param)
    {
        using ReturnType = typename _ToReference<TYPE>::type&&;
        return static_cast<ReturnType>(param);
    }

    // About enum
    enum datatype:uint08 {
        datatype_class_nomemcpy,
        datatype_class_canmemcpy,
        datatype_pod_canmemcpy,
        datatype_pod_canmemcpy_zeroset};
    enum packettype:uint08 {packettype_null = 0,
        packettype_entrance, packettype_message, packettype_leaved, packettype_kicked};
    enum orientationtype:uint08 {
        orientationtype_normal0, orientationtype_normal90, orientationtype_normal180, orientationtype_normal270,
        orientationtype_fliped0, orientationtype_fliped90, orientationtype_fliped180, orientationtype_fliped270};
    enum prioritytype:uint08 {
        prioritytype_lowest, prioritytype_low, prioritytype_normal, prioritytype_high, prioritytype_highest};
    enum jumper:sint32   {jumper_null = 0};
    enum jumper16:sint16 {jumper16_null = 0};
    template<sint32 ENUM> struct EnumToType {};
}

// About code styling
#define branch           if(false)
#define jump(QUERY)      else if(QUERY)
#define nothing          do{} while(0)
#define hook(INSTANCE)   for(auto &_hook_ = (INSTANCE), *_fish_ = &_hook_; _fish_; _fish_ = nullptr)
#define fish             (*_fish_)
#define codeid(NAME)     (((NAME)[0] & 0xFF) | (((NAME)[1] & 0xFF) << 8) | (((NAME)[2] & 0xFF) << 16) | (((NAME)[3] & 0xFF) << 24))
#define strpair(NAME)    ("" NAME), strsize(NAME)
#define strsize(NAME)    (sizeof("" NAME) - 1)

// About allocator
inline void* operator new(boss_size_t, sblock ptr) {return (void*) ptr;}
inline void* operator new[](boss_size_t, sblock ptr) {return (void*) ptr;}
inline void operator delete(void*, sblock) {}
inline void operator delete[](void*, sblock) {}

// About constant
#if defined(__GNUC__) || defined(__ARMCC_VERSION)
    #define ox100000000        0x100000000LL
    #define ox00000000000000FF 0x00000000000000FFULL
    #define ox000000000000FF00 0x000000000000FF00ULL
    #define ox0000000000FF0000 0x0000000000FF0000ULL
    #define ox00000000FF000000 0x00000000FF000000ULL
    #define ox000000FF00000000 0x000000FF00000000ULL
    #define ox0000FF0000000000 0x0000FF0000000000ULL
    #define ox00FF000000000000 0x00FF000000000000ULL
    #define oxFFFFFFFFFFFFFFFF 0xFFFFFFFFFFFFFFFFULL
    #define oxFFFFFFFF00000000 0xFFFFFFFF00000000ULL
    #define oxFFFF000000000000 0xFFFF000000000000ULL
    #define oxFF00000000000000 0xFF00000000000000ULL
    #define oxF000000000000000 0xF000000000000000ULL
    #define ox0000000000000000 0x0000000000000000ULL
    #define Signed64(NUMBER)   NUMBER##LL
    #define Unsigned64(NUMBER) NUMBER##ULL
    #define EpochToWindow(UTC) (((uint64) UTC) +  11644473600000ULL)
    #define WindowToEpoch(MS)  (((uint64) MS)  -  11644473600000ULL)
    #define EpochToJulian(UTC) (((uint64) UTC) + 210866803200000ULL)
    #define JulianToEpoch(JUL) (((uint64) JUL) - 210866803200000ULL)
#else
    #define ox100000000        0x100000000i64
    #define ox00000000000000FF 0x00000000000000FFui64
    #define ox000000000000FF00 0x000000000000FF00ui64
    #define ox0000000000FF0000 0x0000000000FF0000ui64
    #define ox00000000FF000000 0x00000000FF000000ui64
    #define ox000000FF00000000 0x000000FF00000000ui64
    #define ox0000FF0000000000 0x0000FF0000000000ui64
    #define ox00FF000000000000 0x00FF000000000000ui64
    #define oxFFFFFFFFFFFFFFFF 0xFFFFFFFFFFFFFFFFui64
    #define oxFFFFFFFF00000000 0xFFFFFFFF00000000ui64
    #define oxFFFF000000000000 0xFFFF000000000000ui64
    #define oxFF00000000000000 0xFF00000000000000ui64
    #define oxF000000000000000 0xF000000000000000ui64
    #define ox0000000000000000 0x0000000000000000ui64
    #define Signed64(NUMBER)   NUMBER##i64
    #define Unsigned64(NUMBER) NUMBER##ui64
    #define EpochToWindow(UTC) (((uint64) UTC) +  11644473600000ui64)
    #define WindowToEpoch(MS)  (((uint64) MS)  -  11644473600000ui64)
    #define EpochToJulian(UTC) (((uint64) UTC) + 210866803200000ui64)
    #define JulianToEpoch(JUL) (((uint64) JUL) - 210866803200000ui64)
#endif
#if BOSS_X64
    #define PtrToUint64(PTR)  (*((const uint64*) &(PTR)))
    #define AnyTypeToPtr(ANY) ((void*) (ANY))
#else
    #define PtrToUint64(PTR)  ((const uint64) *((const uint32*) &(PTR)))
    #define AnyTypeToPtr(ANY) ((void*) (ANY))
#endif
#define JumperPos(JMP)     ((const sint32*) &(JMP))
#define JumperToPtr(JMP)   (((sint32*) &(JMP)) + (JMP))
#define Jumper16Pos(JMP)   ((const sint16*) &(JMP))
#define Jumper16ToPtr(JMP) (((sint16*) &(JMP)) + (JMP))

// About declare
#define BOSS_DECLARE_ID(NAME) \
    struct struct_##NAME {const buffer unused;}; \
    typedef struct struct_##NAME* NAME; \
    typedef const struct struct_##NAME* NAME##_read
#define BOSS_DECLARE_HANDLE(NAME) \
    class NAME \
    { \
    private: \
        struct data \
        { \
            sint32 mRef; \
            buffer mBuf; \
            void* mPtr; \
        }; \
        data* mData; \
    private: \
        NAME(data* gift) {mData = gift; mData->mRef++;} \
    public: \
        NAME() {alloc();} \
        NAME(NAME&& rhs) {mData = rhs.mData; rhs.alloc();} \
        NAME(const NAME& rhs) {mData = rhs.mData; mData->mRef++;} \
        NAME(id_share rhs) {mData = (data*) rhs; mData->mRef++;} \
        NAME(id_cloned_share rhs) {mData = (data*) rhs;} \
        ~NAME() {release();} \
        NAME& operator=(NAME&& rhs) \
        { \
            release(); \
            mData = rhs.mData; \
            rhs.alloc(); \
            return *this; \
        } \
        NAME& operator=(const NAME& rhs) \
        { \
            release(); \
            mData = rhs.mData; \
            mData->mRef++; \
            return *this; \
        } \
    public: \
        static inline NAME null() \
        { \
            return NAME((data*) Buffer::Alloc<data, datatype_pod_canmemcpy_zeroset>(BOSS_DBG 1)); \
        } \
        static inline NAME create_by_buf(BOSS_DBG_PRM buffer buf) \
        { \
            data* newdata = (data*) Buffer::Alloc<data, datatype_pod_canmemcpy_zeroset>(BOSS_DBG_ARG 1); \
            newdata->mBuf = buf; \
            return NAME(newdata); \
        } \
        static inline NAME create_by_ptr(BOSS_DBG_PRM void* ptr) \
        { \
            data* newdata = (data*) Buffer::Alloc<data, datatype_pod_canmemcpy_zeroset>(BOSS_DBG_ARG 1); \
            newdata->mPtr = ptr; \
            return NAME(newdata); \
        } \
    public: \
        inline void clear_buf() {mData->mBuf = nullptr;} \
        inline void set_buf(buffer buf) {buffer oldbuf = mData->mBuf; mData->mBuf = buf; Buffer::Free(oldbuf);} \
        inline void* set_ptr(void* ptr) {void* oldptr = mData->mPtr; mData->mPtr = ptr; return oldptr;} \
        inline void* get() const {return (void*) (((ublock) mData->mBuf) | ((ublock) mData->mPtr));} \
        inline id_share share() {return (id_share) mData;} \
        inline id_cloned_share cloned_share() {mData->mRef++; return (id_cloned_share) mData;} \
    private: \
        inline void alloc() \
        { \
            mData = (data*) Buffer::Alloc<data, datatype_pod_canmemcpy_zeroset>(BOSS_DBG 1); \
            mData->mRef = 1; \
        } \
        inline void release() \
        { \
            if(--mData->mRef == 0) \
            { \
                Buffer::Free(mData->mBuf); \
                Buffer::Free((buffer) mData); \
                mData = nullptr; \
            } \
        } \
    }; \
    typedef const NAME NAME##_read
#define BOSS_DECLARE_ADDON_FUNCTION(GROUP, NAME, RET_TYPE, ...) \
    typedef RET_TYPE (*Type_AddOn_##GROUP##_##NAME)(__VA_ARGS__); \
    Type_AddOn_##GROUP##_##NAME& Core_AddOn_##GROUP##_##NAME(); \
    static RET_TYPE Customized_AddOn_##GROUP##_##NAME(__VA_ARGS__);
#define BOSS_DEFINE_ADDON_FUNCTION(GROUP, NAME, RET_TYPE, RET_CODE, ...) \
    static RET_TYPE Default_AddOn_##GROUP##_##NAME(__VA_ARGS__) {GROUP##_Error(); RET_CODE;} \
    typedef RET_TYPE (*Type_AddOn_##GROUP##_##NAME)(__VA_ARGS__); \
    Type_AddOn_##GROUP##_##NAME& Core_AddOn_##GROUP##_##NAME() \
    {static Type_AddOn_##GROUP##_##NAME _ = Default_AddOn_##GROUP##_##NAME; return _;}
#define BOSS_DECLARE_STANDARD_CLASS(NAME) \
    public: \
    NAME(const NAME&) = default; \
    NAME& operator=(const NAME&) = default; \
    NAME(NAME&&) = default; \
    NAME& operator=(NAME&&) = default; \
    private:
#define BOSS_DECLARE_NONCOPYABLE_CLASS(NAME) \
    public: \
    NAME(const NAME& rhs) {operator=(rhs);} \
    NAME& operator=(const NAME& rhs) \
    {BOSS_ASSERT("This class is not allowed to be copied", false); return *this;} \
    private:
#define BOSS_DECLARE_NONCOPYABLE_INITIALIZED_CLASS(NAME, ...) \
    public: \
    NAME(const NAME& rhs) : __VA_ARGS__ {operator=(rhs);} \
    NAME& operator=(const NAME& rhs) \
    {BOSS_ASSERT("This class is not allowed to be copied", false); return *this;} \
    private:

// About opengl
#if BOSS_ANDROID
    #define GL_RGBA8 0x8058
    #define GL_BGRA 0x80E1
    #define GL_RGB16F 0x881B
    #define GL_HALF_FLOAT 0x140B
    #define GL_RED 0x1903
    #define GL_RG 0x8227
#endif

// About based declare id
namespace BOSS
{
    BOSS_DECLARE_ID(id_file);
    BOSS_DECLARE_ID(id_sound);
    BOSS_DECLARE_ID(id_socket);
    BOSS_DECLARE_ID(id_server);
    BOSS_DECLARE_ID(id_pipe);
    BOSS_DECLARE_ID(id_purchase);
    BOSS_DECLARE_ID(id_bluetooth);
    BOSS_DECLARE_ID(id_serial);
    BOSS_DECLARE_ID(id_bitmap);
    BOSS_DECLARE_ID(id_flash);
    BOSS_DECLARE_ID(id_image);
    BOSS_DECLARE_ID(id_image_routine);
    BOSS_DECLARE_ID(id_texture);
    BOSS_DECLARE_ID(id_surface);
    BOSS_DECLARE_ID(id_animate);
    BOSS_DECLARE_ID(id_clock);
    BOSS_DECLARE_ID(id_camera);
    BOSS_DECLARE_ID(id_microphone);
}

// About based include and declare handle
#ifndef BOSS_NOT_INCLUDE_FOR_INTELLIGENCE
    #include <boss_addon.hpp>
    #include <boss_array.hpp>
    #include <boss_asset.hpp>
    #include <boss_buffer.hpp>
    #include <boss_context.hpp>
    #include <boss_counter.hpp>
    #include <boss_library.hpp>
    #include <boss_map.hpp>
    #include <boss_math.hpp>
    #include <boss_memory.hpp>
    #include <boss_mutex.hpp>
    #include <boss_object.hpp>
    #include <boss_parser.hpp>
    #include <boss_profile.hpp>
    #include <boss_queue.hpp>
    #include <boss_remote.hpp>
    #include <boss_share.hpp>
    #include <boss_storage.hpp>
    #include <boss_string.hpp>
    #include <boss_tasking.hpp>
    #include <boss_tree.hpp>
    #include <boss_updater.hpp>
    #include <boss_view.hpp>
    #include <boss_wstring.hpp>

    namespace BOSS
    {
        BOSS_DECLARE_HANDLE(h_icon);
        BOSS_DECLARE_HANDLE(h_action);
        BOSS_DECLARE_HANDLE(h_policy);
        BOSS_DECLARE_HANDLE(h_dock);
        BOSS_DECLARE_HANDLE(h_window);
        BOSS_DECLARE_HANDLE(h_web);
        BOSS_DECLARE_HANDLE(h_web_native);
    }
#endif
