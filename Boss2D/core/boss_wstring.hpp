#pragma once
#include <boss_array.hpp>

namespace BOSS
{
    /// @brief 와이드스트링지원
    class WString
    {
    public:
        /// @brief 생성자
        WString();

        /// @brief 복사생성자
        /// @param rhs : 복사할 인스턴스
        WString(const WString& rhs);

        /// @brief 이동생성자
        /// @param rhs : 이동할 인스턴스
        WString(WString&& rhs);

        /// @brief 특수생성자(공유배열로부터)
        /// @param rhs : 공유배열
        WString(const wchararray& rhs);

        /// @brief 특수생성자(공유ID로부터)
        /// @param rhs : 복사할 인스턴스
        WString(id_share_read rhs);

        /// @brief 특수생성자(복제된 공유ID로부터)
        /// @param rhs : 복사할 인스턴스
        WString(id_cloned_share_read rhs);

        /// @brief 특수생성자(네이티브 와이드스트링으로부터)
        /// @param rhs : 네이티브 와이드스트링
        /// @param length : 적용할 길이(-1이면 자동설정)
        WString(wchars rhs, sint32 length = -1);

        /// @brief 특수생성자(와이드캐릭터로부터)
        /// @param rhs : 와이드캐릭터
        WString(wchar_t rhs);

        /// @brief 소멸자
        ~WString();

        /// @brief 복사
        /// @param rhs : 복사할 인스턴스
        /// @return 자기 객체
        WString& operator=(const WString& rhs);

        /// @brief 이동
        /// @param rhs : 이동할 인스턴스
        /// @return 자기 객체
        WString& operator=(WString&& rhs);

        /// @brief 복사(wchars)
        /// @param rhs : 복사할 와이드스트링
        /// @return 자기 객체
        WString& operator=(wchars rhs);

        /// @brief 복사(wchar_t)
        /// @param rhs : 복사할 와이드캐릭터
        /// @return 자기 객체
        WString& operator=(wchar_t rhs);

        /// @brief 추가
        /// @param rhs : 추가할 인스턴스
        /// @return 자기 객체
        WString& operator+=(const WString& rhs);

        /// @brief 추가(wchars)
        /// @param rhs : 추가할 와이드스트링
        /// @return 자기 객체
        WString& operator+=(wchars rhs);

        /// @brief 추가(wchar_t)
        /// @param rhs : 추가할 와이드캐릭터
        /// @return 자기 객체
        WString& operator+=(wchar_t rhs);

        /// @brief 더하기
        /// @param rhs : 더할 인스턴스
        /// @return 새로운 객체
        WString operator+(const WString& rhs) const;

        /// @brief 더하기(wchars)
        /// @param rhs : 더할 와이드스트링
        /// @return 새로운 객체
        WString operator+(wchars rhs) const;
        friend WString operator+(wchars lhs, const WString& rhs);

        /// @brief 더하기(wchar_t)
        /// @param rhs : 더할 와이드캐릭터
        /// @return 새로운 객체
        WString operator+(wchar_t rhs) const;
        friend WString operator+(wchar_t lhs, const WString& rhs);

        /// @brief 동등 비교
        /// @param rhs : 비교할 인스턴스
        /// @return 같으면 true, 다르면 false
        bool operator==(const WString& rhs) const;

        /// @brief 동등 비교(wchars)
        /// @param rhs : 비교할 스트링
        /// @return 같으면 true, 다르면 false
        bool operator==(wchars rhs) const;
        friend bool operator==(wchars lhs, const WString& rhs);

        /// @brief 차등 비교
        /// @param rhs : 비교할 인스턴스
        /// @return 다르면 true, 같으면 false
        bool operator!=(const WString& rhs) const;

        /// @brief 차등 비교(wchars)
        /// @param rhs : 비교할 스트링
        /// @return 다르면 true, 같으면 false
        bool operator!=(wchars rhs) const;
        friend bool operator!=(wchars lhs, const WString& rhs);

        /// @brief 형변환(wchars)
        operator wchars() const;

        /// @brief 형변환(id_share)
        operator id_share() const;

        /// @brief 형변환(id_cloned_share)
        operator id_cloned_share() const;

        /// @brief 비우기
        void Empty();

        /// @brief 접근
        /// @param index : 인덱스(음수는 역인덱스)
        /// @return 해당 와이드캐릭터
        const wchar_t operator[](sint32 index) const;

        /// @brief 텍스트길이 구하기
        /// @return 텍스트길이
        sint32 Length() const;

        /// @brief 후방추가(네이티브 와이드스트링으로부터)
        /// @param other : 네이티브 와이드스트링
        /// @param length : 적용할 길이(-1이면 자동설정)
        /// @return 자기 객체
        WString& AddTail(wchars other, sint32 length = -1);

        /// @brief 후방감소
        /// @param length : 감소할 길이
        /// @return 자기 객체
        WString& SubTail(sint32 length);

        /// @brief CP949형 스트링으로 컨버팅하여 반환
        /// @param urlcode : URL코드방식 여부
        /// @return 생성된 버퍼
        buffer MakeCp949(bool urlcode) const;

    public:
        /// @brief swprintf식 객체생성
        /// @param text : 문자열
        /// @param ... : 가변인자
        /// @return 객체
        static WString Format(wchars text, ...);

        /// @brief 컨버팅식 객체생성
        /// @param text : 컨버팅할 스트링
        /// @param length : 적용할 길이(-1이면 자동설정)
        /// @return 객체
        static WString FromChars(chars text, sint32 length = -1);

        /// @brief 컨버팅식 객체생성(CP949)
        /// @param urlcode : URL코드방식 여부
        /// @param text_cp949 : 컨버팅할 CP949형 스트링
        /// @param length : 적용할 길이(-1이면 자동설정)
        /// @return 객체
        static WString FromCp949(bool urlcode, chars_cp949 text_cp949, sint32 length = -1);

        /// @brief 한글일 경우 조합식으로 병합하여 반환
        /// @param front : 앞 코드
        /// @param rear : 뒷 코드
        /// @return 병합된 결과(병합성공시 스트링사이즈 1, 실패시 2)
        static wchars MergeKorean(wchar_t front, wchar_t rear);

        /// @brief wcscmp식 비교
        /// @param text : 원본 와이드스트링
        /// @param other : 비교할 와이드스트링
        /// @param maxlength : 최대길이(-1이면 자동설정)
        /// @return 비교결과(0-같음)
        static sint32 Compare(wchars text, wchars other, sint32 maxlength = -1);

        /// @brief wcsicmp식 비교
        /// @param text : 원본 와이드스트링
        /// @param other : 비교할 와이드스트링
        /// @param maxlength : 최대길이(-1이면 자동설정)
        /// @return 비교결과(0-같음)
        static sint32 CompareNoCase(wchars text, wchars other, sint32 maxlength = -1);

        /// @brief 첫 글자의 길이
        /// @param text : 조사할 스트링
        /// @return 길이(char단위)
        static sint32 GetLengthOfFirstLetter(wchars text);

    private:
        static const wchararray& NullString();
        wchararray* GetSafedRhs(wchars& rhs);

    private:
        wchararray m_words;
    };
    typedef Array<WString> WStrings;
}
