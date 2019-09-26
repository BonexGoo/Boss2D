#pragma once
#include <boss_map.hpp>
#include <boss_object.hpp>
#include <boss_string.hpp>

namespace BOSS
{
    class Context;
    typedef Array<const Context*, datatype_pod_canmemcpy> CollectedContexts;
    enum CollectOption {CO_Null, CO_GetParent};
    enum ScriptType {ST_Json, ST_Xml};
    enum ScriptOption {SO_OnlyReference, SO_NeedCopy};

    /// @brief 트리형 콘텍스트지원
    class Context
    {
    public:
        /// @brief 자식콘텍스트를 생성하며 접근(키워드식)
        /// @param key : 자식콘텍스트의 이름
        /// @param length : 이름의 길이(-1이면 자동설정)
        /// @return 해당 자식콘텍스트의 인스턴스
        Context& At(chars key, sint32 length = -1);

        /// @brief 자식콘텍스트를 생성하며 접근(배열식)
        /// @param index : 자식콘텍스트의 순번
        /// @return 해당 자식콘텍스트의 인스턴스
        Context& At(sint32 index);

        /// @brief 자식콘텍스트를 마지막에 추가후 접근(배열식)
        /// @return 해당 자식콘텍스트의 인스턴스
        Context& AtAdding();

        /// @brief 자신의 값 설정
        /// @param value : 문자열값
        /// @param length : 길이(-1은 자동측정)
        /// @param need_quotation : 인용부호 필요여부
        void Set(chars value, sint32 length = -1, bool need_quotation = true);

        /// @brief 초기화
        void Clear();

        /// @brief Json소스를 파싱하여 로드(buffer)
        /// @param src : Json소스
        /// @return 성공여부
        bool LoadJson(buffer src);

        /// @brief Json소스를 파싱하여 로드(chars)
        /// @param option : 복사필요여부(src가 곧 지워질 예정이라면 복사필요)
        /// @param src : Json소스
        /// @param length : src의 길이(-1이면 자동설정)
        /// @return 성공여부
        bool LoadJson(ScriptOption option, chars src, sint32 length = -1);

        /// @brief Json소스로 세이브
        /// @param dst : 결과스트링(기존스트링에 추가할 경우)
        /// @return Json소스
        String SaveJson(String dst = String()) const;

        /// @brief Xml소스를 파싱하여 로드(buffer)
        /// @param src : Xml소스
        /// @return 성공여부
        bool LoadXml(buffer src);

        /// @brief Xml소스를 파싱하여 로드(chars)
        /// @param option : 복사필요여부(src가 곧 지워질 예정이라면 복사필요)
        /// @param src : Xml소스
        /// @param length : src의 길이(-1이면 자동설정)
        /// @return 성공여부
        bool LoadXml(ScriptOption option, chars src, sint32 length = -1);

        /// @brief Xml소스로 세이브
        /// @param dst : 결과스트링(기존스트링에 추가할 경우)
        /// @return Xml소스
        String SaveXml(String dst = String()) const;

        /// @brief Bin바이너리를 로드
        /// @param src : Bin바이너리
        /// @return 성공여부
        bool LoadBin(bytes src);

        /// @brief Bin바이너리로 세이브
        /// @param dst : 결과배열(기존배열에 추가할 경우)
        /// @return Bin바이너리
        uint08s SaveBin(uint08s dst = uint08s::Null()) const;

        /// @brief Prm소스를 파싱하여 로드(chars)
        /// @param src : Prm소스
        /// @return 성공여부
        bool LoadPrm(chars src, sint32 length = -1);

        /// @brief 키-밸류 매칭에 의한 수집
        /// @param key : 자식콘텍스트의 이름
        /// @param value : 매칭할 문자열값
        /// @return 수집된 콘텍스트들
        CollectedContexts CollectByMatch(chars key, chars value, CollectOption option = CO_Null) const;

    public:
        /// @brief 자식콘텍스트로 접근(키워드식)
        /// @param key : 자식콘텍스트의 이름
        /// @return 해당 자식콘텍스트의 인스턴스
        const Context& operator()(chars key) const;

        /// @brief 자식콘텍스트로 접근(키워드식/순번방식)
        /// @param order : 자식콘텍스트의 순번
        /// @param getname : 자식콘텍스트의 이름얻기
        /// @return 해당 자식콘텍스트의 인스턴스
        const Context& operator()(sint32 order, chararray* getname = nullptr) const;

        /// @brief 자식콘텍스트로 접근(배열식)
        /// @param index : 자식콘텍스트의 순번
        /// @return 해당 자식콘텍스트의 인스턴스
        const Context& operator[](sint32 index) const;

        /// @brief 자식콘텍스트의 수량얻기(키워드식)
        /// @return 수량값
        sint32 LengthOfNamable() const;

        /// @brief 자식콘텍스트의 수량얻기(배열식)
        /// @return 수량값
        sint32 LengthOfIndexable() const;

        /// @brief 자신의 유효여부
        /// @return 유효여부
        bool IsValid() const;

        /// @brief 자신의 문자열값 얻기
        /// @return 문자열값
        chars GetString() const;

        /// @brief 빠르게 자신의 문자열값 얻기(chars_endless)
        /// @return 문자열값
        chars_endless GetStringFast(sint32& length) const;

        /// @brief 자신의 정수값 얻기
        /// @return 정수값
        const sint64 GetInt() const;

        /// @brief 자신의 소수값 얻기
        /// @return 소수값
        const float GetFloat() const;

        /// @brief 자신의 문자열값 얻기(디폴트처리)
        /// @param value : 디폴트값
        /// @return 문자열값
        chars GetString(chars value) const;

        /// @brief 자신의 정수값 얻기(디폴트처리)
        /// @param value : 디폴트값
        /// @return 정수값
        const sint64 GetInt(const sint64 value) const;

        /// @brief 자신의 소수값 얻기(디폴트처리)
        /// @param value : 디폴트값
        /// @return 소수값
        const float GetFloat(const float value) const;

        /// @brief 자식콘텍스트 현황보고
        void DebugPrint() const;

    public:
        /// @brief 생성자
        Context();

        /// @brief 복사생성자
        /// @param rhs : 복사할 인스턴스
        Context(const Context& rhs);

        /// @brief 이동생성자
        /// @param rhs : 이동할 인스턴스
        Context(Context&& rhs);

        /// @brief 생성자(Bin바이너리를 로드)
        /// @param src : Bin바이너리
        Context(bytes src);

        /// @brief 생성자(스크립트를 파싱)
        /// @param type : 스크립트종류
        /// @param src : 스크립트소스
        Context(ScriptType type, buffer src);

        /// @brief 생성자(스크립트를 파싱)
        /// @param type : 스크립트종류
        /// @param option : 복사필요여부(src가 곧 지워질 예정이라면 복사필요)
        /// @param src : 스크립트소스
        /// @param length : src의 길이(-1이면 자동설정)
        Context(ScriptType type, ScriptOption option, chars src, sint32 length = -1);

        /// @brief 소멸자
        ~Context();

        /// @brief 복사
        /// @param rhs : 복사할 인스턴스
        /// @return 자기 객체
        Context& operator=(const Context& rhs);

        /// @brief 이동
        /// @param rhs : 이동할 인스턴스
        /// @return 자기 객체
        Context& operator=(Context&& rhs);

        /// @brief bool형변환
        /// @return 자신의 유효여부
        operator bool() const;

    private:
        const Context& NullChild() const;
        void SetValueForSource(chars value, sint32 length);
        void ClearCache();
        static chars FindMark(chars value, const char mark);
        static chars SkipBlank(chars value, bool exclude_nullzero);
        static chars SkipBlankReverse(chars value);

    private:
        Context* InitSource(Context* anyparent);
        // Json
        bool LoadJsonCore(chars src);
        void SaveJsonCore(sint32 tab, String name, String& dst, bool indexable, bool lastchild) const;
        static void SaveJsonCoreCB(const MapPath* path, Context* data, payload param);
        // Xml
        bool LoadXmlCore(chars src);
        void SaveXmlCore(sint32 tab, String name, String& dst) const;
        static void SaveXmlCoreCB(const MapPath* path, Context* data, payload param);
        // Bin
        static const String& GetBinHeader();
        bytes LoadBinCore(bytes src);
        void SaveBinCore(uint08s& dst, const String& name = String()) const;
        // Collect
        void CollectCore(const Context* parent, chars key, chars value, const sint32 length, CollectedContexts* result, CollectOption option) const;
        static void CollectCoreCB(const MapPath* path, Context* data, payload param);
        // Debug
        void DebugPrintCore(sint32 tab, String name, bool indexable) const;
        static void DebugPrintCoreCB(const MapPath* path, Context* data, payload param);

    private:
        // 원본연결
        class StringSource
        {
            BOSS_DECLARE_NONCOPYABLE_CLASS(StringSource)
        public:
            StringSource();
            ~StringSource();

        public:
            void InitString(buffer src);
            void InitString(ScriptOption option, chars src, sint32 length);
            chars GetString() const;

        private:
            buffer m_buffer;
            chars m_string;
        };
        Object<StringSource> m_source;

        // 자식연결
        Map<Context> m_namableChild;
        Map<Context> m_indexableChild;

        // 자기데이터
        bool m_valueNeedQuotation;
        chars m_valueOffset;
        sint32 m_valueLength;

        // 캐시데이터
        mutable char* m_parsedString;
        mutable sint64* m_parsedInt;
        mutable float* m_parsedFloat;
    };
    typedef Array<Context> Contexts;
}
