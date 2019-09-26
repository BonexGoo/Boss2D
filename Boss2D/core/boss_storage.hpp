#pragma once
#define BOSS_NOT_INCLUDE_FOR_INTELLIGENCE
#include <boss.hpp>

#define BOSS_STORAGE(CLASS, ...) \
    [&]()->CLASS* { \
    static id_storage NewStorage = \
        Storage::Create(SCT_User, \
            []()->void* {return new CLASS(__VA_ARGS__);}, \
            [](void* ptr)->void {delete (CLASS*) ptr;}); \
    return (CLASS*) Storage::Bind(NewStorage);}()

#define BOSS_STORAGE_SYS(CLASS, ...) \
    [&]()->CLASS* { \
    static id_storage NewStorage = \
        Storage::Create(SCT_System, \
            []()->void* {return new CLASS(__VA_ARGS__);}, \
            [](void* ptr)->void {delete (CLASS*) ptr;}); \
    return (CLASS*) Storage::Bind(NewStorage);}()

namespace BOSS
{
    BOSS_DECLARE_ID(id_storage);
    enum StorageClearType {SCT_System, SCT_User};
    enum StorageClearLevel {SCL_SystemAndUser, SCL_UserOnly};

    /// @brief 스토리지(TLS)지원
    class Storage
    {
    public:
        typedef void* (*NewCB)();
        typedef void (*DeleteCB)(void*);

    public:
        /// @brief 스토리지를 생성
        /// @param type : 인스턴스 분류타입
        /// @param ncb : 객체생성 콜백함수
        /// @param dcb : 객체소멸 콜백함수
        /// @return 스토리지ID
        /// @see Bind, Clear, ClearAll
        static id_storage Create(StorageClearType type, NewCB ncb, DeleteCB dcb);

        /// @brief 해당 스토리지에 스레드별 인스턴스를 연동(없으면 생성)
        /// @param storage : 스토리지ID
        /// @return 인스턴스의 주소
        /// @see Create
        static void* Bind(id_storage storage);

        /// @brief 해당 스토리지에 연동된 인스턴스를 소멸
        /// @param storage : 스토리지ID
        /// @return 소멸시행 여부
        /// @see ClearAll
        static bool Clear(id_storage storage);

        /// @brief 모든 스토리지에 연동된 인스턴스를 소멸
        /// @param level : 소멸수준(CL_SystemAndUser는 영구제거)
        /// @return 소멸시행 수량
        /// @see Bind, Clear
        static sint32 ClearAll(StorageClearLevel level = SCL_UserOnly);
    };
}
