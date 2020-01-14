#pragma once
#include <boss_share.hpp>
#include <boss_storage.hpp>

namespace BOSS
{
    /// @brief 공유배열지원
    template<typename TYPE, datatype DATATYPE = datatype_class_nomemcpy, sint32 MINCOUNT = 4>
    class Array
    {
    public:
        /// @brief 비우기
        void Clear()
        {
            if(0 < share->count())
            {
                Share::Remove(share);
                share = Share::Create(SampleBuffer(), MINCOUNT);
            }
        }

        /// @brief 공유ID 추출
        /// @return 공유ID
        operator id_share() const {return (id_share) share;}

        /// @brief 공유ID 추출(복제방식)
        /// @return 공유ID
        operator id_cloned_share() const {return (id_cloned_share) share->Clone();}

        /// @brief 강제 현변환에 대한 처리
        /// @return nullptr값
        operator void*() const {return nullptr;}

        /// @brief 논리부정연산에 대한 처리
        /// @return 데이터가 존재하지 않으면 true
        bool operator!() const {return (share->count() == 0);}

        /// @brief 복사
        /// @param rhs : 복사할 인스턴스
        /// @return 자기 객체
        Array& operator=(const Array& rhs)
        {
            Share::Remove(share);
            share = rhs.share->Clone();
            return *this;
        }

        /// @brief 이동
        /// @param rhs : 이동할 인스턴스
        /// @return 자기 객체
        Array& operator=(Array&& rhs)
        {
            Share::Remove(share);
            share = rhs.share;
            rhs.share = Share::Create(SampleBuffer(), MINCOUNT);
            return *this;
        }

        /// @brief 읽기접근
        /// @param index : 인덱스(음수는 역인덱스)
        /// @return 해당 객체
        const TYPE& operator[](sint32 index) const
        {
            if(index < 0) index = share->count() + index;
            BOSS_ASSERT("배열범위를 초과한 인덱스입니다", 0 <= index && index < share->count());
            return share->At<TYPE>(index);
        }

        /// @brief 쓰기접근
        /// @param index : 인덱스(음수는 역인덱스)
        /// @return 해당 객체
        TYPE& At(sint32 index)
        {
            if(index < 0) index = share->count() + index;
            BOSS_ASSERT("배열범위를 초과한 인덱스입니다", 0 <= index && index < share->count());
            share = Share::CopyOnWrite(share, 0);
            return share->At<TYPE>(index);
        }

        /// @brief 추가(후미+1)하여 쓰기접근
        /// @return 해당 객체
        TYPE& AtAdding()
        {
            const sint32 index = share->count();
            share = Share::CopyOnWrite(share, index + 1);
            return share->At<TYPE>(index);
        }

        /// @brief 자율추가하여 쓰기접근
        /// @param index : 인덱스
        /// @return 해당 객체
        TYPE& AtWherever(sint32 index)
        {
            BOSS_ASSERT("배열범위를 초과한 인덱스입니다", 0 <= index);
            share = Share::CopyOnWrite(share, index + 1);
            return share->At<TYPE>(index);
        }

        /// @brief 저수준식 쓰기접근
        /// @param index : 시작인덱스
        /// @param length : 시작인덱스로부터 보장길이
        /// @return 해당 객체의 포인터
        TYPE* AtDumping(sint32 index, sint32 length)
        {
            BOSS_ASSERT("배열범위를 초과한 인덱스입니다", 0 <= index);
            share = Share::CopyOnWrite(share, index + length);
            return &share->At<TYPE>(index);
        }

        /// @brief 저수준식 추가형 쓰기접근
        /// @param length : 후미로부터 추가할 길이
        /// @return 해당 객체의 포인터
        TYPE* AtDumpingAdded(sint32 length)
        {
            BOSS_ASSERT("length는 1이상이어야 합니다", 1 <= length);
            const sint32 index = share->count();
            share = Share::CopyOnWrite(share, index + length);
            return &share->At<TYPE>(index);
        }

        /// @brief 후미삭제(메모리공간은 보존)
        /// @return 삭제 수행여부(배열수량이 0일 경우 false)
        bool SubtractionOne()
        {
            if(share->count() == 0) return false;
            share = Share::CopyOnWrite(share, -1);
            return true;
        }

        /// @brief 전체삭제(메모리공간은 보존)
        /// @return 삭제 수행여부(배열수량이 0일 경우 false)
        bool SubtractionAll()
        {
            if(share->count() == 0) return false;
            share = Share::CopyOnWrite(share, -2);
            return true;
        }

        /// @brief 구간삭제(메모리공간은 보존)
        /// @param index : 시작인덱스
        /// @param length : 시작인덱스로부터 삭제길이
        /// @return 삭제 정상수행여부
        bool SubtractionSection(sint32 index, sint32 length = 1)
        {
            BOSS_ASSERT("length는 0보다 커야 합니다", 0 < length);
            for(sint32 i = index, iend = share->count() - length; i < iend; ++i)
                share->At<TYPE>(i) = ToReference(share->At<TYPE>(i + length));
            bool IsSuccess = (0 < length);
            while(length--) IsSuccess &= SubtractionOne();
            return IsSuccess;
        }

        /// @brief 개별이동
        /// @param index : 삽입할 시작인덱스
        /// @param src : 이동할 대상인스턴스
        void DeliveryOne(sint32 index, TYPE&& src)
        {
            const Share* OldShare = share;
            share = Share::Create(SampleBuffer(), OldShare->count() + 1);
            for(sint32 i = 0; i < index; ++i)
                AtAdding() = ToReference(OldShare->At<TYPE>(i));
            AtAdding() = ToReference(src);
            for(sint32 i = index, iend = OldShare->count(); i < iend; ++i)
                AtAdding() = ToReference(OldShare->At<TYPE>(i));
            Share::Remove(OldShare);
        }

        /// @brief 구간이동(대상인스턴스의 메모리공간은 보존)
        /// @param index : 삽입할 시작인덱스
        /// @param src : 이동할 대상인스턴스
        /// @param srcindex : 이동할 시작인덱스
        /// @param srclength : 시작인덱스로부터 이동길이
        /// @return 이동 정상수행여부
        bool DeliverySection(sint32 index, Array&& src, sint32 srcindex, sint32 srclength = 1)
        {
            BOSS_ASSERT("length는 0보다 커야 합니다", 0 < srclength);
            const Share* OldShare = share;
            const Share* NewShare = src.share;
            share = Share::Create(SampleBuffer(), OldShare->count() + srclength);
            for(sint32 i = 0; i < index; ++i)
                AtAdding() = ToReference(OldShare->At<TYPE>(i));
            for(sint32 i = srcindex, iend = i + srclength; i < iend; ++i)
                AtAdding() = ToReference(NewShare->At<TYPE>(i));
            for(sint32 i = index, iend = OldShare->count(); i < iend; ++i)
                AtAdding() = ToReference(OldShare->At<TYPE>(i));
            Share::Remove(OldShare);
            if(srcindex == 0 && srclength == src.Count())
                return src.SubtractionAll();
            return src.SubtractionSection(srcindex, srclength);
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

        /// @brief 배열수량 구하기
        /// @return 배열수량
        sint32 Count() const
        {
            return share->count();
        }

        /// @brief 생성자
        Array() : share(Share::Create(SampleBuffer(), MINCOUNT)) {}

        /// @brief 복사생성자
        /// @param rhs : 복사할 인스턴스
        Array(const Array& rhs) : share(rhs.share->Clone()) {}

        /// @brief 이동생성자
        /// @param rhs : 복사할 인스턴스
        Array(Array&& rhs) : share(rhs.share) {rhs.share = Share::Create(SampleBuffer(), MINCOUNT);}

        /// @brief 특수생성자(값으로부터)
        /// @param rhs : 복사할 값
        Array(const TYPE& rhs) : share(Share::Create(SampleBuffer(), MINCOUNT))
        {
            AtAdding() = rhs;
        }

        /// @brief 특수생성자(공유ID로부터)
        /// @param rhs : 복사할 공유ID
        Array(id_share_read rhs) : share(((Share*) rhs)->Clone())
        {
            BOSS_ASSERT("공유ID의 객체타입이 달라서 복사할 수 없습니다",
                share->TypeID() == Buffer::TypeOf(SampleBuffer()));
        }

        /// @brief 특수생성자(복제된 공유ID로부터)
        /// @param rhs : 복사할 공유ID
        Array(id_cloned_share_read rhs) : share((Share*) rhs)
        {
            BOSS_ASSERT("공유ID의 객체타입이 달라서 복사할 수 없습니다",
                share->TypeID() == Buffer::TypeOf(SampleBuffer()));
        }

        /// @brief 소멸자
        ~Array()
        {
            Share::Remove(share);
        }

    public:
        /// @brief Null객체 얻기
        /// @return Null객체
        static const Array& Null()
        {return *BOSS_STORAGE_SYS(Array);}

        /// @brief Sample버퍼 얻기
        /// @return Sample버퍼
        static const buffer& SampleBuffer()
        {return *BOSS_STORAGE_SYS(buffer, Buffer::Alloc<TYPE, DATATYPE>(BOSS_DBG 0));}

    private:
        const Share* share;
    };

    /// @brief 기본적인 공유배열
    typedef Array<sint08, datatype_pod_canmemcpy> sint08s;
    typedef Array<uint08, datatype_pod_canmemcpy> uint08s;
    typedef Array<sint16, datatype_pod_canmemcpy> sint16s;
    typedef Array<uint16, datatype_pod_canmemcpy> uint16s;
    typedef Array<sint32, datatype_pod_canmemcpy> sint32s;
    typedef Array<uint32, datatype_pod_canmemcpy> uint32s;
    typedef Array<sint64, datatype_pod_canmemcpy> sint64s;
    typedef Array<uint64, datatype_pod_canmemcpy> uint64s;
    typedef Array<buffer, datatype_pod_canmemcpy> buffers;
    typedef Array<bool, datatype_pod_canmemcpy> bools;
    typedef Array<float, datatype_pod_canmemcpy> floats;
    typedef Array<double, datatype_pod_canmemcpy> doubles;
    typedef Array<void*, datatype_pod_canmemcpy> pointers;
    typedef Array<char, datatype_pod_canmemcpy> chararray;
    typedef Array<wchar_t, datatype_pod_canmemcpy> wchararray;
}
