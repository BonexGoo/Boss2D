#pragma once
#include <boss_share.hpp>

namespace BOSS
{
    enum class ObjectAllocType {Now, Later};

    /// @brief 공유객체지원
    template<typename TYPE, datatype DATATYPE = datatype_class_nomemcpy, ObjectAllocType BASETYPE = ObjectAllocType::Later>
    class Object
    {
    public:
        /// @brief 비우기
        void Clear()
        {
            Share::Remove(share);
            share = nullptr;
        }

        /// @brief 공유ID 추출
        /// @return 공유ID
        operator id_share() const {return (id_share) share;}

        /// @brief 공유ID 추출(복제방식)
        /// @return 공유ID
        operator id_cloned_share() const {return (id_cloned_share) share->Clone();}

        /// @brief 복사
        /// @param rhs : 복사할 인스턴스
        /// @return 자기 객체
        Object& operator=(const Object& rhs)
        {
            Share::Remove(share);
            share = (rhs.share)? rhs.share->Clone() : nullptr;
            return *this;
        }

        /// @brief 이동
        /// @param rhs : 이동할 인스턴스
        /// @return 자기 객체
        Object& operator=(Object&& rhs)
        {
            Share::Remove(share);
            share = rhs.share;
            rhs.share = nullptr;
            return *this;
        }

        /// @brief 복사
        /// @param rhs : 복사할 인스턴스
        /// @return 자기 객체
        Object& operator=(const TYPE& rhs)
        {
            Share::Remove(share);
            share = Share::Create(Buffer::Alloc<TYPE, DATATYPE>(BOSS_DBG 1));
            share->At<TYPE>(0) = rhs;
            return *this;
        }

        /// @brief 인수(버퍼로부터)
        /// @param rhs : 인수할 버퍼
        /// @return 자기 객체
        Object& operator=(buffer rhs)
        {
            Share::Remove(share);
            share = Share::Create(rhs);
            return *this;
        }

        /// @brief 인수(공유ID로부터)
        /// @param rhs : 복사할 공유ID
        /// @return 자기 객체
        Object& operator=(id_share_read rhs)
        {
            BOSS_ASSERT("공유ID가 배열로 구성되어서 복사할 수 없습니다", ((Share*) rhs)->count() == 1);
            Share::Remove(share);
            share = ((Share*) rhs)->Clone();
            return *this;
        }

        /// @brief 인수(복제된 공유ID로부터)
        /// @param rhs : 복사할 공유ID
        /// @return 자기 객체
        Object& operator=(id_cloned_share_read rhs)
        {
            BOSS_ASSERT("공유ID가 배열로 구성되어서 복사할 수 없습니다", ((Share*) rhs)->count() == 1);
            Share::Remove(share);
            share = (Share*) rhs;
            return *this;
        }

        /// @brief 타입명 구하기
        /// @return 타입명
        chars Type() const
        {
            return share->Type();
        }

        /// @brief 타입ID 구하기
        /// @return 타입ID
        sblock TypeID() const
        {
            return share->TypeID();
        }

        /// @brief 멤버접근
        /// @return 자기 타입
        TYPE* operator->()
        {
            BOSS_ASSERT("빈 객체입니다", share);
            return Ptr();
        }

        /// @brief 멤버접근(읽기전용)
        /// @return 자기 타입
        TYPE const* operator->() const
        {
            BOSS_ASSERT("빈 객체입니다", share);
            return ConstPtr();
        }

        /// @brief 데이터값
        /// @return 데이터
        TYPE& Value()
        {
            BOSS_ASSERT("빈 객체입니다", share);
            share = Share::CopyOnWrite(share, 0);
            return share->At<TYPE>(0);
        }

        /// @brief 데이터값(읽기전용)
        /// @return 데이터
        TYPE const& ConstValue() const
        {
            BOSS_ASSERT("빈 객체입니다", share);
            return share->At<TYPE>(0);
        }

        /// @brief 데이터값(공유방식)
        /// @return 데이터
        TYPE& SharedValue()
        {
            if(!share)
                share = Share::Create(Buffer::Alloc<TYPE, DATATYPE>(BOSS_DBG 1));
            return share->At<TYPE>(0);
        }

        /// @brief 주소값
        /// @return 주소
        TYPE* Ptr()
        {
            if(!share) return nullptr;
            share = Share::CopyOnWrite(share, 0);
            return &share->At<TYPE>(0);
        }

        /// @brief 주소값(읽기전용)
        /// @return 주소
        TYPE const* ConstPtr() const
        {
            if(!share) return nullptr;
            return &share->At<TYPE>(0);
        }

        /// @brief 버퍼생성
        /// @return 버퍼
        buffer CopiedBuffer() const
        {
            if(!share) return nullptr;
            return share->CopiedBuffer();
        }

        /// @brief 교환
        /// @param other : 교환할 인스턴스
        void Change(Object&& other)
        {
            const Share* temp = share;
            share = other.share;
            other.share = temp;
        }

        /// @brief 생성자
        /// @param doalloc : 버퍼 생성여부
        Object(ObjectAllocType type = BASETYPE) : share((type == ObjectAllocType::Now)? Share::Create(Buffer::Alloc<TYPE, DATATYPE>(BOSS_DBG 1)) : nullptr) {}

        /// @brief 복사생성자
        /// @param rhs : 복사할 인스턴스
        Object(const Object& rhs) : share(nullptr) {operator=(rhs);}

        /// @brief 이동생성자
        /// @param rhs : 이동할 인스턴스
        Object(Object&& rhs) : share(nullptr) {operator=(ToReference(rhs));}

        /// @brief 특수생성자
        /// @param rhs : 복사할 TYPE 인스턴스
        Object(const TYPE& rhs) : share(nullptr) {operator=(rhs);}

        /// @brief 특수생성자(버퍼로부터)
        /// @param rhs : 인수할 버퍼
        Object(buffer rhs) : share(nullptr) {operator=(rhs);}

        /// @brief 특수생성자(공유ID로부터)
        /// @param rhs : 복사할 공유ID
        Object(id_share_read rhs) : share(nullptr) {operator=(rhs);}

        /// @brief 생성자(복제된 공유ID로부터)
        /// @param rhs : 복사할 공유ID
        Object(id_cloned_share_read rhs) : share(nullptr) {operator=(rhs);}

        /// @brief 소멸자
        ~Object() {Share::Remove(share);}

    private:
        const Share* share;
    };

    /// @brief 기본적인 공유객체
    typedef Object<sint08, datatype_pod_canmemcpy, ObjectAllocType::Now> sint08o;
    typedef Object<uint08, datatype_pod_canmemcpy, ObjectAllocType::Now> uint08o;
    typedef Object<sint16, datatype_pod_canmemcpy, ObjectAllocType::Now> sint16o;
    typedef Object<uint16, datatype_pod_canmemcpy, ObjectAllocType::Now> uint16o;
    typedef Object<sint32, datatype_pod_canmemcpy, ObjectAllocType::Now> sint32o;
    typedef Object<uint32, datatype_pod_canmemcpy, ObjectAllocType::Now> uint32o;
    typedef Object<sint64, datatype_pod_canmemcpy, ObjectAllocType::Now> sint64o;
    typedef Object<uint64, datatype_pod_canmemcpy, ObjectAllocType::Now> uint64o;
    typedef Object<buffer, datatype_pod_canmemcpy, ObjectAllocType::Now> buffero;
    typedef Object<bool, datatype_pod_canmemcpy, ObjectAllocType::Now> boolo;
    typedef Object<float, datatype_pod_canmemcpy, ObjectAllocType::Now> floato;
    typedef Object<double, datatype_pod_canmemcpy, ObjectAllocType::Now> doubleo;
}
