#pragma once
#include <boss_array.hpp>

namespace BOSS
{
    /// @brief 스트링지원
    class String
    {
    public:
        /// @brief 생성자
        String();

        /// @brief 복사생성자
        /// @param rhs : 복사할 인스턴스
        String(const String& rhs);

        /// @brief 이동생성자
        /// @param rhs : 이동할 인스턴스
        String(String&& rhs);

        /// @brief 특수생성자(공유배열로부터)
        /// @param rhs : 공유배열
        String(const chararray& rhs);

        /// @brief 특수생성자(공유ID로부터)
        /// @param rhs : 복사할 인스턴스
        String(id_share_read rhs);

        /// @brief 특수생성자(복제된 공유ID로부터)
        /// @param rhs : 복사할 인스턴스
        String(id_cloned_share_read rhs);

        /// @brief 특수생성자(네이티브 스트링으로부터)
        /// @param rhs : 네이티브 스트링
        /// @param length : 적용할 길이(-1이면 자동설정)
        String(chars rhs, sint32 length = -1);

        /// @brief 특수생성자(캐릭터로부터)
        /// @param rhs : 캐릭터
        String(char rhs);

        /// @brief 소멸자
        ~String();

        /// @brief 복사
        /// @param rhs : 복사할 인스턴스
        /// @return 자기 객체
        String& operator=(const String& rhs);

        /// @brief 이동
        /// @param rhs : 이동할 인스턴스
        /// @return 자기 객체
        String& operator=(String&& rhs);

        /// @brief 복사(chars)
        /// @param rhs : 복사할 스트링
        /// @return 자기 객체
        String& operator=(chars rhs);

        /// @brief 복사(char)
        /// @param rhs : 복사할 캐릭터
        /// @return 자기 객체
        String& operator=(char rhs);

        /// @brief 추가
        /// @param rhs : 추가할 인스턴스
        /// @return 자기 객체
        String& operator+=(const String& rhs);

        /// @brief 추가(chars)
        /// @param rhs : 추가할 스트링
        /// @return 자기 객체
        String& operator+=(chars rhs);

        /// @brief 추가(char)
        /// @param rhs : 추가할 캐릭터
        /// @return 자기 객체
        String& operator+=(char rhs);

        /// @brief 더하기
        /// @param rhs : 더할 인스턴스
        /// @return 새로운 객체
        String operator+(const String& rhs) const;

        /// @brief 더하기(chars)
        /// @param rhs : 더할 스트링
        /// @return 새로운 객체
        String operator+(chars rhs) const;
        friend String operator+(chars lhs, const String& rhs);

        /// @brief 더하기(char)
        /// @param rhs : 더할 캐릭터
        /// @return 새로운 객체
        String operator+(char rhs) const;
        friend String operator+(char lhs, const String& rhs);

        /// @brief 동등 비교
        /// @param rhs : 비교할 인스턴스
        /// @return 같으면 true, 다르면 false
        bool operator==(const String& rhs) const;

        /// @brief 동등 비교(chars)
        /// @param rhs : 비교할 스트링
        /// @return 같으면 true, 다르면 false
        bool operator==(chars rhs) const;
        friend bool operator==(chars lhs, const String& rhs);

        /// @brief 차등 비교
        /// @param rhs : 비교할 인스턴스
        /// @return 다르면 true, 같으면 false
        bool operator!=(const String& rhs) const;

        /// @brief 차등 비교(chars)
        /// @param rhs : 비교할 스트링
        /// @return 다르면 true, 같으면 false
        bool operator!=(chars rhs) const;
        friend bool operator!=(chars lhs, const String& rhs);

        /// @brief 형변환(chars)
        operator chars() const;

        /// @brief 형변환(id_share)
        operator id_share() const;

        /// @brief 형변환(id_cloned_share)
        operator id_cloned_share() const;

        /// @brief 비우기
        void Empty();

        /// @brief 접근
        /// @param index : 인덱스(음수는 역인덱스)
        /// @return 해당 캐릭터
        const char operator[](sint32 index) const;

        /// @brief 텍스트길이 구하기
        /// @return 텍스트길이
        sint32 Length() const;

        /// @brief strncmp식 비교
        /// @param other : 비교할 인스턴스
        /// @param offset : other의 시작위치
        /// @return 비교결과(0-같음)
        sint32 Compare(const String& other, sint32 offset = 0) const;

        /// @brief strnicmp식 비교
        /// @param other : 비교할 인스턴스
        /// @param offset : other의 시작위치
        /// @return 비교결과(0-같음)
        sint32 CompareNoCase(const String& other, sint32 offset = 0) const;

        /// @brief 검색
        /// @param index : 검색할 시작인덱스
        /// @param key : 검색키
        /// @return 검색한 인덱스, 검색실패시 -1
        sint32 Find(sint32 index, const String& key) const;

        /// @brief 캐릭터치환
        /// @param oldword : 찾을 캐릭터
        /// @param newword : 바꿀 캐릭터
        /// @return 자기 객체
        String& Replace(char oldword, char newword);

        /// @brief 텍스트치환
        /// @param oldtext : 찾을 텍스트
        /// @param newtext : 바꿀 텍스트
        /// @return 자기 객체
        String& Replace(chars oldtext, chars newtext);

        /// @brief 후방추가(네이티브 스트링으로부터)
        /// @param other : 네이티브 스트링
        /// @param length : 적용할 길이(-1이면 자동설정)
        /// @return 자기 객체
        String& AddTail(chars other, sint32 length = -1);

        /// @brief 빠른 후방추가(상수 스트링으로부터)
        /// @param other : 상수 스트링
        /// @return 자기 객체
        template<sint32 length>
        String& AddTailFast(const char(&other)[length])
        {
            Memory::Copy(m_words.AtDumpingAdded(length - 1) - 1, other, sizeof(char) * length);
            m_findmap.Clear();
            return *this;
        }

        /// @brief 후방감소
        /// @param length : 감소할 길이
        /// @return 자기 객체
        String& SubTail(sint32 length);

        /// @brief 부분생성(Left)
        /// @param length : 복사할 길이
        /// @return 새로운 객체
        String Left(sint32 length) const;

        /// @brief 부분생성(Right)
        /// @param length : 복사할 길이
        /// @return 새로운 객체
        String Right(sint32 length) const;

        /// @brief 부분생성(Middle)
        /// @param index : 복사할 시작인덱스
        /// @param length : 복사할 길이
        /// @return 새로운 객체
        String Middle(sint32 index, sint32 length) const;

        /// @brief 부분생성(Trim)
        /// @return 새로운 객체
        String Trim() const;

        /// @brief 파일로 저장
        /// @param filename : 파일명
        /// @param bom : BOM코드 삽입
        /// @return 성공여부
        bool ToFile(chars filename, bool bom = false) const;

        /// @brief 어셋으로 저장
        /// @param filename : 파일명
        /// @param bom : BOM코드 삽입
        /// @return 성공여부
        bool ToAsset(chars filename, bool bom = false) const;

        /// @brief URL스트링으로 저장
        /// @param safeword : 알파벳, 숫자이외에 보존할 안전한 문자코드(%와 +는 불가)
        /// @return URL스트링
        String ToUrlString(chars safeword = "-_.!~*'()") const;

        /// @brief HTML스트링으로 저장
        /// @return HTML스트링
        String ToHtmlString() const;

    public:
        /// @brief sprintf식 객체생성
        /// @param text : 문자열
        /// @param ... : 가변인자
        /// @return 객체
        static String Format(chars text, ...);

        /// @brief 파일로드식 객체생성
        /// @param filename : 파일명
        /// @return 객체
        static String FromFile(chars filename);

        /// @brief 어셋로드식 객체생성
        /// @param filename : 파일명
        /// @param assetpath : 우선탐색용 패스
        /// @return 객체
        static String FromAsset(chars filename, id_assetpath_read assetpath = nullptr);

        /// @brief 컨버팅식 객체생성
        /// @param text : 컨버팅할 와이드스트링
        /// @param length : 적용할 길이(-1이면 자동설정)
        /// @return 객체
        static String FromWChars(wchars text, sint32 length = -1);

        /// @brief 컨버팅식 객체생성(URL스트링)
        /// @param text : 컨버팅할 URL스트링
        /// @param length : 적용할 길이(-1이면 자동설정)
        /// @return 객체
        static String FromUrlString(chars text, sint32 length = -1);

        /// @brief 정수식 객체생성(sint32)
        /// @param value : 컨버팅할 정수
        /// @return 객체
        static String FromInteger(const sint32 value);

        /// @brief 정수식 객체생성(sint64)
        /// @param value : 컨버팅할 정수
        /// @return 객체
        static String FromInteger(const sint64 value);

        /// @brief 실수식 객체생성(float)
        /// @param value : 컨버팅할 실수
        /// @return 객체
        static String FromFloat(const float value);

        /// @brief 실수식 객체생성(double)
        /// @param value : 컨버팅할 실수
        /// @return 객체
        static String FromFloat(const double value);

        /// @brief strcmp식 비교
        /// @param text : 원본 스트링
        /// @param other : 비교할 스트링
        /// @param maxlength : 최대길이(-1이면 자동설정)
        /// @return 비교결과(0-같음)
        static sint32 Compare(chars text, chars other, sint32 maxlength = -1);

        /// @brief stricmp식 비교
        /// @param text : 원본 스트링
        /// @param other : 비교할 스트링
        /// @param maxlength : 최대길이(-1이면 자동설정)
        /// @return 비교결과(0-같음)
        static sint32 CompareNoCase(chars text, chars other, sint32 maxlength = -1);

        /// @brief 첫 글자의 길이
        /// @param text : 조사할 스트링
        /// @return 길이(char단위)
        static sint32 GetLengthOfFirstLetter(chars text);

    private:
        static const chararray& NullString();
        const sint32* GetFindMap() const;

    private:
        chararray m_words;
        mutable sint32s m_findmap;
    };
    typedef Array<String> Strings;
}
