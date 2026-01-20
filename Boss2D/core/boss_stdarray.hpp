#pragma once
#define BOSS_NOT_INCLUDE_FOR_INTELLIGENCE
#include <boss.hpp>

#include <vector>
#include <typeinfo>
#include <cstddef>
#include <utility>

namespace BOSS
{
    /// @brief Std배열지원
    template<typename TYPE, datatype DATATYPE = datatype_class_nomemcpy, sint32 MINCOUNT = 4>
    class StdArray
    {
    public:
        /// @brief 비우기
        void Clear()
        {
            m_v.clear();
        }

        /// @brief 강제 현변환에 대한 처리
        /// @return nullptr값
        operator void*() const { return nullptr; }

        /// @brief 논리부정연산에 대한 처리
        /// @return 데이터가 존재하지 않으면 true
        bool operator!() const { return (m_v.size() == 0); }

        /// @brief 복사
        /// @param rhs : 복사할 인스턴스
        /// @return 자기 객체
        StdArray& operator=(const StdArray& rhs)
        {
            m_v = rhs.m_v;
            return *this;
        }

        /// @brief 이동
        /// @param rhs : 이동할 인스턴스
        /// @return 자기 객체
        StdArray& operator=(StdArray&& rhs) noexcept
        {
            m_v = std::move(rhs.m_v);
            return *this;
        }

        /// @brief 읽기접근
        /// @param index : 인덱스(음수는 역인덱스)
        /// @return 해당 객체
        const TYPE& operator[](sint32 index) const
        {
            index = norm_index(index);
            BOSS_ASSERT("배열범위를 초과한 인덱스입니다", 0 <= index && (size_t) index < m_v.size());
            return m_v[(size_t) index];
        }

        /// @brief 쓰기접근
        /// @param index : 인덱스(음수는 역인덱스)
        /// @return 해당 객체
        TYPE& At(sint32 index)
        {
            index = norm_index(index);
            BOSS_ASSERT("배열범위를 초과한 인덱스입니다", 0 <= index && (size_t) index < m_v.size());
            return m_v[(size_t) index];
        }

        /// @brief 추가(후미+1)하여 쓰기접근
        /// @return 해당 객체
        TYPE& AtAdding()
        {
            // NOTE: std::vector는 capacity를 초과하는 순간 기존 요소들의 move/copy가 발생한다.
            // 기존 Boss2D Array의 사용패턴(AtAdding/AtWherever/AtDumping*)에서
            // 재할당(reallocation) 빈도를 최소화하기 위해 필요량 기반으로 선제 reserve를 수행한다.
            ensure_capacity(m_v.size() + 1);
            m_v.emplace_back();
            return m_v.back();
        }

        /// @brief 자율추가하여 쓰기접근
        /// @param index : 인덱스
        /// @return 해당 객체
        TYPE& AtWherever(sint32 index)
        {
            BOSS_ASSERT("배열범위를 초과한 인덱스입니다", 0 <= index);
            const size_t need = (size_t) index + 1;
            if(m_v.size() < need)
            {
                ensure_capacity(need);
                m_v.resize(need);
            }
            return m_v[(size_t) index];
        }

        /// @brief 저수준식 쓰기접근
        /// @param index : 시작인덱스
        /// @param length : 시작인덱스로부터 보장길이
        /// @return 해당 객체의 포인터
        TYPE* AtDumping(sint32 index, sint32 length)
        {
            BOSS_ASSERT("배열범위를 초과한 인덱스입니다", 0 <= index);
            BOSS_ASSERT("length는 0보다 커야 합니다", 0 < length);
            const size_t need = (size_t) index + (size_t) length;
            if(m_v.size() < need)
            {
                ensure_capacity(need);
                m_v.resize(need);
            }
            return &m_v[(size_t) index];
        }

        /// @brief 저수준식 추가형 쓰기접근
        /// @param length : 후미로부터 추가할 길이
        /// @return 해당 객체의 포인터
        TYPE* AtDumpingAdded(sint32 length)
        {
            BOSS_ASSERT("length는 1이상이어야 합니다", 1 <= length);
            const size_t index = m_v.size();
            ensure_capacity(index + (size_t) length);
            m_v.resize(index + (size_t) length);
            return &m_v[index];
        }

        /// @brief 후미삭제(메모리공간은 보존)
        /// @return 삭제 수행여부(배열수량이 0일 경우 false)
        bool SubtractionOne()
        {
            if(m_v.empty()) return false;
            m_v.pop_back();
            return true;
        }

        /// @brief 전체삭제(메모리공간은 보존)
        /// @return 삭제 수행여부(배열수량이 0일 경우 false)
        bool SubtractionAll()
        {
            if(m_v.empty()) return false;
            m_v.clear();
            return true;
        }

        /// @brief 구간삭제(메모리공간은 보존)
        /// @param index : 시작인덱스
        /// @param length : 시작인덱스로부터 삭제길이
        /// @return 삭제 정상수행여부
        bool SubtractionSection(sint32 index, sint32 length = 1)
        {
            BOSS_ASSERT("length는 0보다 커야 합니다", 0 < length);
            if(m_v.empty()) return false;
            index = norm_index(index);
            if(index < 0 || (size_t) index >= m_v.size()) return false;

            const size_t b = (size_t) index;
            const size_t e = (size_t) index + (size_t) length;
            const size_t ee = (e > m_v.size()) ? m_v.size() : e;
            if(b >= ee) return false;
            m_v.erase(m_v.begin() + (ptrdiff_t) b, m_v.begin() + (ptrdiff_t) ee);
            return true;
        }

        /// @brief 개별이동
        /// @param index : 삽입할 시작인덱스
        /// @param src : 이동할 대상인스턴스
        void DeliveryOne(sint32 index, TYPE&& src)
        {
            if(index < 0) index = 0;
            if((size_t) index > m_v.size()) index = (sint32) m_v.size();
            m_v.insert(m_v.begin() + (ptrdiff_t) index, std::move(src));
        }

        /// @brief 구간이동(대상인스턴스의 메모리공간은 보존)
        /// @param index : 삽입할 시작인덱스
        /// @param src : 이동할 대상인스턴스
        /// @param srcindex : 이동할 시작인덱스
        /// @param srclength : 시작인덱스로부터 이동길이
        /// @return 이동 정상수행여부
        bool DeliverySection(sint32 index, StdArray&& src, sint32 srcindex, sint32 srclength = 1)
        {
            BOSS_ASSERT("length는 0보다 커야 합니다", 0 < srclength);
            if(srcindex < 0 || (size_t) srcindex >= src.m_v.size()) return false;

            const size_t sb = (size_t) srcindex;
            const size_t se = (size_t) srcindex + (size_t) srclength;
            const size_t see = (se > src.m_v.size()) ? src.m_v.size() : se;
            if(sb >= see) return false;

            if(index < 0) index = 0;
            if((size_t) index > m_v.size()) index = (sint32) m_v.size();
            auto it_ins = m_v.begin() + (ptrdiff_t) index;

            // move-insert
            m_v.insert(it_ins,
                std::make_move_iterator(src.m_v.begin() + (ptrdiff_t) sb),
                std::make_move_iterator(src.m_v.begin() + (ptrdiff_t) see));

            // remove from src
            src.m_v.erase(src.m_v.begin() + (ptrdiff_t) sb, src.m_v.begin() + (ptrdiff_t) see);
            return true;
        }

        /// @brief 타입명 구하기
        /// @return 타입명
        chars Type() const
        {
            return typeid(TYPE).name();
        }

        /// @brief 타입ID 구하기
        /// @return 타입ID
        sblock TypeID() const
        {
            // Boss2D의 Buffer::TypeOf 대체(호환용): type_info hash를 반환
            return (sblock) (size_t) typeid(TYPE).hash_code();
        }

        /// @brief 배열수량 구하기
        /// @return 배열수량
        sint32 Count() const
        {
            return (sint32) m_v.size();
        }

        /// @brief 생성자
        StdArray()
        {
            if(0 < MINCOUNT) m_v.reserve((size_t) MINCOUNT);
        }

        /// @brief 복사생성자
        /// @param rhs : 복사할 인스턴스
        StdArray(const StdArray& rhs) : m_v(rhs.m_v)
        {
        }

        /// @brief 이동생성자
        /// @param rhs : 복사할 인스턴스
        StdArray(StdArray&& rhs) noexcept : m_v(std::move(rhs.m_v))
        {
        }

        /// @brief 특수생성자(값으로부터)
        /// @param rhs : 복사할 값
        StdArray(const TYPE& rhs)
        {
            if(0 < MINCOUNT) m_v.reserve((size_t) MINCOUNT);
            m_v.push_back(rhs);
        }

        /// @brief 소멸자
        ~StdArray()
        {
        }

    public:
        /// @brief Null객체 얻기
        /// @return Null객체
        static const StdArray& Null()
        {
            static const StdArray s_null;
            return s_null;
        }

        /// @brief Sample버퍼 얻기
        /// @return Sample버퍼
        static const buffer& SampleBuffer()
        {
            // Boss2D 환경에서 buffer/Buffer::Alloc가 유효할 때를 가정
            static const buffer s_sample = Buffer::Alloc<TYPE, DATATYPE>(BOSS_DBG 0);
            return s_sample;
        }

    private:
        sint32 norm_index(sint32 index) const
        {
            if(index < 0) index = (sint32) m_v.size() + index;
            return index;
        }

        void ensure_capacity(size_t need)
        {
            // vector의 성장에 따른 요소 이동/복사 비용을 줄이기 위한 capacity 보장.
            // (요소 "이동" 자체를 없애는 것은 vector 특성상 불가능하므로,
            //  재할당 횟수를 최소화하는 방향으로 최적화한다.)
            if(m_v.capacity() >= need) return;

            size_t newcap = m_v.capacity();
            if(newcap == 0) newcap = (0 < MINCOUNT) ? (size_t) MINCOUNT : (size_t) 1;
            while(newcap < need) newcap <<= 1; // 2배 성장
            m_v.reserve(newcap);
        }

    private:
        std::vector<TYPE> m_v;
    };

    /// @brief 기본적인 Std배열
    typedef StdArray<sint08, datatype_pod_canmemcpy> std_sint08s;
    typedef StdArray<uint08, datatype_pod_canmemcpy> std_uint08s;
    typedef StdArray<sint16, datatype_pod_canmemcpy> std_sint16s;
    typedef StdArray<uint16, datatype_pod_canmemcpy> std_uint16s;
    typedef StdArray<sint32, datatype_pod_canmemcpy> std_sint32s;
    typedef StdArray<uint32, datatype_pod_canmemcpy> std_uint32s;
    typedef StdArray<sint64, datatype_pod_canmemcpy> std_sint64s;
    typedef StdArray<uint64, datatype_pod_canmemcpy> std_uint64s;
    typedef StdArray<buffer, datatype_pod_canmemcpy> std_buffers;
    typedef StdArray<bool, datatype_pod_canmemcpy> std_bools;
    typedef StdArray<float, datatype_pod_canmemcpy> std_floats;
    typedef StdArray<double, datatype_pod_canmemcpy> std_doubles;
    typedef StdArray<void*, datatype_pod_canmemcpy> std_pointers;
    typedef StdArray<char, datatype_pod_canmemcpy> std_chararray;
    typedef StdArray<wchar_t, datatype_pod_canmemcpy> std_wchararray;
}
