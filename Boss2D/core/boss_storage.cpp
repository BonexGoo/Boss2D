#include <boss.hpp>
#include "boss_storage.hpp"

class StorageUnit;

// 관리객체
class StorageClass
{
    friend class StorageUnit;

public:
    StorageClass(StorageClearType type, Storage::NewCB ncb, Storage::DeleteCB dcb);
    ~StorageClass();

public:
    StorageUnit& GetValidUnit() const;
    bool InvalidUnit() const;
    static sint32 ClearLocalDataAll(StorageClearType type);

private:
    const StorageClearType mType;
    const Storage::NewCB mNew;
    const Storage::DeleteCB mDelete;
    const sint32 mUnitID;
};

// 단위객체
class StorageUnit
{
    friend class StorageClass;

public:
    StorageUnit()
    {
        mRefClass = nullptr;
        mInstance = nullptr;
    }
    ~StorageUnit()
    {
        if(mInstance)
        {
            mRefClass->mDelete(mInstance);
            mInstance = nullptr;
        }
    }

public:
    inline StorageClearType type() const
    {return mRefClass->mType;}

    void* GetValidInstance()
    {
        if(!mInstance)
            mInstance = mRefClass->mNew();
        return mInstance;
    }

    bool InvalidInstance() const
    {
        // 결국 자기 자신(StorageUnit)이 파괴됨
        return mRefClass->InvalidUnit();
    }

private:
    const StorageClass* mRefClass;
    void* mInstance;
};
typedef Map<StorageUnit> StorageUnitMap;

// 전역변수
static sint32 boss_storage_unitcount = 0;
#if HAS_CXX11_THREAD_LOCAL
    thread_local StorageUnitMap* boss_storage_root;
#elif defined(_MSC_VER)
    __declspec(thread) StorageUnitMap* boss_storage_root;
#elif defined(__GNUC__)
    #if BOSS_IPHONE
        static StorageUnitMap* boss_storage_root; // 수정해야 함!!!!!
    #else
        __thread StorageUnitMap* boss_storage_root;
    #endif
#else
    #error Unknown compiler
#endif

StorageClass::StorageClass(StorageClearType type, Storage::NewCB ncb, Storage::DeleteCB dcb)
    : mType(type), mNew(ncb), mDelete(dcb), mUnitID(boss_storage_unitcount++)
{
}

StorageClass::~StorageClass()
{
}

StorageUnit& StorageClass::GetValidUnit() const
{
    BOSS_ASSERT("영구제거된 스토리지에 다시 접근하려 합니다", boss_storage_root != (StorageUnitMap*) 1);
    StorageUnitMap& UnitMap = (boss_storage_root)?
        *boss_storage_root : *(boss_storage_root = new StorageUnitMap());
    if(auto OldUnit = UnitMap.Access(mUnitID))
        return *OldUnit;
    StorageUnit& NewStorageUnit = UnitMap[mUnitID];
    NewStorageUnit.mRefClass = this;
    return NewStorageUnit;
}

bool StorageClass::InvalidUnit() const
{
    BOSS_ASSERT("영구제거된 스토리지에 다시 접근하려 합니다", boss_storage_root != (StorageUnitMap*) 1);
    return boss_storage_root->Remove(mUnitID);
}

sint32 StorageClass::ClearLocalDataAll(StorageClearType type)
{
    BOSS_ASSERT("영구제거된 스토리지에 다시 접근하려 합니다", boss_storage_root != (StorageUnitMap*) 1);
    if(boss_storage_root)
    {
        sint32 ClearCount = 0;
        for(sint32 i = boss_storage_root->Count() - 1; 0 <= i; --i)
        {
            StorageUnit& CurUnit = *boss_storage_root->AccessByOrder(i);
            if(CurUnit.type() == type)
                ClearCount += CurUnit.InvalidInstance();
        }
        if(type == SCT_System)
        {
            delete boss_storage_root;
            boss_storage_root = (StorageUnitMap*) 1; // 영구제거 표식
        }
        return ClearCount;
    }
    return 0;
}

namespace BOSS
{
    id_storage Storage::Create(StorageClearType type, NewCB ncb, DeleteCB dcb)
    {
        BOSS_ASSERT("ncb 또는 dcb가 nullptr입니다", ncb && dcb);
        StorageClass* NewStorage = new StorageClass(type, ncb, dcb);
        return (id_storage) NewStorage;
    }

    void* Storage::Bind(id_storage storage)
    {
        BOSS_ASSERT("storage인수가 nullptr입니다", storage);
        StorageClass* CurStorage = (StorageClass*) storage;
        return CurStorage->GetValidUnit().GetValidInstance();
    }

    bool Storage::Clear(id_storage storage)
    {
        BOSS_ASSERT("storage인수가 nullptr입니다", storage);
        StorageClass* CurStorage = (StorageClass*) storage;
        return CurStorage->InvalidUnit();
    }

    sint32 Storage::ClearAll(StorageClearLevel level)
    {
        sint32 ClearCount = StorageClass::ClearLocalDataAll(SCT_User);
        if(level == SCL_SystemAndUser)
            ClearCount += StorageClass::ClearLocalDataAll(SCT_System);
        return ClearCount;
    }
}
