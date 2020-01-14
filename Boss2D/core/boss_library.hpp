#pragma once
#define BOSS_NOT_INCLUDE_FOR_INTELLIGENCE
#include <boss.hpp>

#if BOSS_WINDOWS
    #define BOSS_API(PACK, TYPE, FUNC, ...) \
        static autorun API_##PACK##_##FUNC = CollectForPlatform(#TYPE, #FUNC, #PACK "_" #FUNC, #__VA_ARGS__); \
        BOSS_API_FUNC(PACK, TYPE, FUNC)(__VA_ARGS__)
    #define BOSS_API_VOID(PACK, TYPE, FUNC) \
        static autorun API_##PACK##_##FUNC = CollectForPlatform(#TYPE, #FUNC, #PACK "_" #FUNC, "void"); \
        BOSS_API_FUNC(PACK, TYPE, FUNC)()
    #define BOSS_API_FUNC(PACK, TYPE, FUNC) \
        extern "C" __declspec(dllexport) TYPE PACK##_##FUNC
    #define BOSS_API_ENV nullptr
#elif BOSS_LINUX
    #define BOSS_API(PACK, TYPE, FUNC, ...) \
        static autorun API_##PACK##_##FUNC = CollectForPlatform(#TYPE, #FUNC, #PACK "_" #FUNC, #__VA_ARGS__); \
        BOSS_API_FUNC(PACK, TYPE, FUNC)(__VA_ARGS__)
    #define BOSS_API_VOID(PACK, TYPE, FUNC) \
        static autorun API_##PACK##_##FUNC = CollectForPlatform(#TYPE, #FUNC, #PACK "_" #FUNC, "void"); \
        BOSS_API_FUNC(PACK, TYPE, FUNC)()
    #define BOSS_API_FUNC(PACK, TYPE, FUNC) \
        extern "C" __attribute__((visibility("default"))) TYPE PACK##_##FUNC
    #define BOSS_API_ENV nullptr
#elif BOSS_ANDROID
    #define BOSS_API(PACK, TYPE, FUNC, ...) \
        static autorun API_##PACK##_##FUNC = CollectForPlatform(#TYPE, #FUNC, "Java_com_" #PACK "_" #FUNC, #__VA_ARGS__); \
        BOSS_API_FUNC(PACK, TYPE, FUNC)(void* env, void*, __VA_ARGS__)
    #define BOSS_API_VOID(PACK, TYPE, FUNC) \
        static autorun API_##PACK##_##FUNC = CollectForPlatform(#TYPE, #FUNC, "Java_com_" #PACK "_" #FUNC, "void"); \
        BOSS_API_FUNC(PACK, TYPE, FUNC)(void* env, void*)
    #define BOSS_API_FUNC(PACK, TYPE, FUNC) \
        extern "C" TYPE Java_com_##PACK##_##FUNC
    #define BOSS_API_ENV env
#elif BOSS_MAC_OSX || BOSS_IPHONE
    #define BOSS_API(PACK, TYPE, FUNC, ...) \
        static autorun API_##PACK##_##FUNC = CollectForPlatform(#TYPE, #FUNC, #PACK "_" #FUNC, #__VA_ARGS__); \
        BOSS_API_FUNC(PACK, TYPE, FUNC)(__VA_ARGS__)
    #define BOSS_API_VOID(PACK, TYPE, FUNC) \
        static autorun API_##PACK##_##FUNC = CollectForPlatform(#TYPE, #FUNC, #PACK "_" #FUNC, "void"); \
        BOSS_API_FUNC(PACK, TYPE, FUNC)()
    #define BOSS_API_FUNC(PACK, TYPE, FUNC) \
        extern "C" TYPE PACK##_##FUNC
    #define BOSS_API_ENV nullptr
#else
    #error 준비되지 않은 플랫폼입니다
#endif
#define BOSS_API_IMPL(CODE) \
    { \
        ReadyForPlatform(BOSS_API_ENV); \
        CODE; \
    }
#define BOSS_API_IMPL_RETURN(...) \
    { \
        ReadyForPlatform(BOSS_API_ENV); \
        return ReturnForPlatform(BOSS_API_ENV, __VA_ARGS__); \
    }
#define BOSS_API_IMPL_RESULTS(CODE, ...) \
    { \
        ReadyForPlatform(BOSS_API_ENV); \
        CODE; \
        static char temp[1024]; \
        return ResultsForPlatform(BOSS_API_ENV, temp, 1024, __VA_ARGS__); \
    }
#define BOSS_API_OBJ(OBJ) ObjectForPlatform(BOSS_API_ENV, OBJ)
#define BOSS_API_STR(STR) StringForPlatform(BOSS_API_ENV, STR)
#define BOSS_API_BRR(BRR) ByteArrayForPlatform(BOSS_API_ENV, BRR)
#define BOSS_API_FRR(FRR) FloatArrayForPlatform(BOSS_API_ENV, FRR)

namespace BOSS
{
    /// @brief 라이브러리지원
    #if BOSS_ANDROID
        typedef sint32          api_int;
        typedef uint32          api_uint;
        typedef sint32          api_flag;
        typedef sint32          api_enum;
        typedef sint64          api_long;
        typedef float           api_float;
        typedef void*           api_object;
        typedef void*           api_string;
        typedef void*           api_bytes;
        typedef void*           api_floats;
        typedef void*           api_results;
    #else
        typedef sint32          api_int;
        typedef uint32          api_uint;
        typedef sint32          api_flag;
        typedef sint32          api_enum;
        typedef sint64          api_long;
        typedef float           api_float;
        typedef void*           api_object;
        typedef chars           api_string;
        typedef bytes           api_bytes;
        typedef const float*    api_floats;
        typedef chars           api_results;
    #endif

    autorun CollectForPlatform(chars type, chars funcname, chars funcfullname, chars param);
    void ReadyForPlatform(void* env);
    api_string ReturnForPlatform(void* env, chars data);
    api_bytes ReturnForPlatform(void* env, bytes data, const sint32 len);
    api_floats ReturnForPlatform(void* env, const float* data, const sint32 len);
    api_results ResultsForPlatform(void* env, char* temp, sint32 tempsize, uint32 a, uint32 b);
    api_results ResultsForPlatform(void* env, char* temp, sint32 tempsize, float a, float b, float c);
    api_results ResultsForPlatform(void* env, char* temp, sint32 tempsize, float a, float b, float c, float d);

    class ObjectForPlatform
    {
        private: void* GlobalRef;
        private: void* Env;
        private: api_object Obj;
        public: ObjectForPlatform(void* env, api_object obj);
        public: ~ObjectForPlatform();
        public: operator void*() const;
    };
    class StringForPlatform
    {
        private: const char* Utf8;
        private: void* Env;
        private: api_string Str;
        public: StringForPlatform(void* env, api_string str);
        public: ~StringForPlatform();
        public: operator const char*() const;
    };
    class ByteArrayForPlatform
    {
        private: const unsigned char* Bytes;
        private: void* Env;
        private: api_bytes Brr;
        public: ByteArrayForPlatform(void* env, api_bytes brr);
        public: ~ByteArrayForPlatform();
        public: operator const unsigned char*() const;
    };
    class FloatArrayForPlatform
    {
        private: const float* Floats;
        private: void* Env;
        private: api_floats Frr;
        public: FloatArrayForPlatform(void* env, api_floats frr);
        public: ~FloatArrayForPlatform();
        public: operator const float*() const;
    };
}
